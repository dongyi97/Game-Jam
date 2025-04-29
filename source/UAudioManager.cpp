#include "UAudioManager.h"
#include <assert.h>
#include <fstream>
#include <vector>
#include <string>
#include <mmreg.h> 
#pragma comment(lib, "xaudio2.lib")

UAudioManager UAudioManager::MainAudioManager;

UAudioManager::UAudioManager() : pXAudio2(nullptr), pMasterVoice(nullptr)
{
}

UAudioManager::~UAudioManager()
{
    Shutdown();
}

HRESULT UAudioManager::Initialize()
{
    HRESULT hr = XAudio2Create(&pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
    if (FAILED(hr)) return hr;

    hr = pXAudio2->CreateMasteringVoice(&pMasterVoice);
    if (FAILED(hr)) return hr;

}

void UAudioManager::Shutdown()
{
    if (pMasterVoice)
    {
        pMasterVoice->DestroyVoice();
        pMasterVoice = nullptr;
    }
    if (pXAudio2)
    {
        pXAudio2->Release();
        pXAudio2 = nullptr;
    }
}

// 파일 전체 내용을 벡터에 읽어오는 헬퍼 함수
static bool ReadFileToVector(const std::wstring& filename, std::vector<char>& data)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file)
        return false;
    file.seekg(0, std::ios::end);
    size_t size = static_cast<size_t>(file.tellg());
    file.seekg(0, std::ios::beg);
    data.resize(size);
    file.read(&data[0], size);
    return true;
}

bool UAudioManager::PlaySound(const std::wstring& filename)
{
    // WAV 파일 전체 데이터를 메모리에 로드합니다.
    auto wavBuffer = std::make_shared<std::vector<char>>();
    if (!ReadFileToVector(filename, *wavBuffer))
        return false;

    audioBuffers.push_back(wavBuffer);

    const char* dataPtr = wavBuffer->data();
    size_t dataSize = wavBuffer->size();

    // "RIFF" 헤더 확인
    if (std::string(dataPtr, 4) != "RIFF")
        return false;
    // "WAVE" 포맷 확인
    if (std::string(dataPtr + 8, 4) != "WAVE")
        return false;

    // fmt 및 data 청크 찾기
    const char* ptr = dataPtr + 12;
    const char* fmtChunk = nullptr;
    UINT32 fmtChunkSize = 0;
    const char* dataChunk = nullptr;
    UINT32 dataChunkSize = 0;

    while (ptr < dataPtr + dataSize)
    {
        std::string chunkName(ptr, 4);
        UINT32 chunkSize = *(UINT32*)(ptr + 4);
        if (chunkName == "fmt ")
        {
            fmtChunk = ptr + 8;
            fmtChunkSize = chunkSize;
        }
        else if (chunkName == "data")
        {
            dataChunk = ptr + 8;
            dataChunkSize = chunkSize;
            // data 청크를 찾으면 종료
            break; 
        }
        ptr += 8 + chunkSize;
    }
    if (!fmtChunk || !dataChunk)
        return false;

    // fmt 청크 데이터를 기반으로 WAVEFORMATEX 구조체를 생성
    WAVEFORMATEX* pwfx = (WAVEFORMATEX*)malloc(fmtChunkSize);
    if (!pwfx)
        return false;
    memcpy(pwfx, fmtChunk, fmtChunkSize);

    // XAUDIO2_BUFFER 설정
    XAUDIO2_BUFFER buffer = { 0 };
    buffer.Flags = XAUDIO2_END_OF_STREAM;
    buffer.AudioBytes = dataChunkSize;
    buffer.pAudioData = reinterpret_cast<const BYTE*>(dataChunk);

    // 소스 보이스 생성
    IXAudio2SourceVoice* pSourceVoice = nullptr;
    HRESULT hr = pXAudio2->CreateSourceVoice(&pSourceVoice, pwfx);
    if (FAILED(hr))
    {
        free(pwfx);
        return false;
    }

    // 버퍼 제출 및 소스 보이스 시작
    hr = pSourceVoice->SubmitSourceBuffer(&buffer);
    if (FAILED(hr))
    {
        pSourceVoice->DestroyVoice();
        free(pwfx);
        return false;
    }
    hr = pSourceVoice->Start(0);
    if (FAILED(hr))
    {
        pSourceVoice->DestroyVoice();
        free(pwfx);
        return false;
    }

    // 소스 보이스는 재생 후 자체 관리하도록 두고, WAV 파일 포맷 구조체는 즉시 해제
    free(pwfx);

    return true;
}
