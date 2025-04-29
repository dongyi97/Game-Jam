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

// ���� ��ü ������ ���Ϳ� �о���� ���� �Լ�
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
    // WAV ���� ��ü �����͸� �޸𸮿� �ε��մϴ�.
    auto wavBuffer = std::make_shared<std::vector<char>>();
    if (!ReadFileToVector(filename, *wavBuffer))
        return false;

    audioBuffers.push_back(wavBuffer);

    const char* dataPtr = wavBuffer->data();
    size_t dataSize = wavBuffer->size();

    // "RIFF" ��� Ȯ��
    if (std::string(dataPtr, 4) != "RIFF")
        return false;
    // "WAVE" ���� Ȯ��
    if (std::string(dataPtr + 8, 4) != "WAVE")
        return false;

    // fmt �� data ûũ ã��
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
            // data ûũ�� ã���� ����
            break; 
        }
        ptr += 8 + chunkSize;
    }
    if (!fmtChunk || !dataChunk)
        return false;

    // fmt ûũ �����͸� ������� WAVEFORMATEX ����ü�� ����
    WAVEFORMATEX* pwfx = (WAVEFORMATEX*)malloc(fmtChunkSize);
    if (!pwfx)
        return false;
    memcpy(pwfx, fmtChunk, fmtChunkSize);

    // XAUDIO2_BUFFER ����
    XAUDIO2_BUFFER buffer = { 0 };
    buffer.Flags = XAUDIO2_END_OF_STREAM;
    buffer.AudioBytes = dataChunkSize;
    buffer.pAudioData = reinterpret_cast<const BYTE*>(dataChunk);

    // �ҽ� ���̽� ����
    IXAudio2SourceVoice* pSourceVoice = nullptr;
    HRESULT hr = pXAudio2->CreateSourceVoice(&pSourceVoice, pwfx);
    if (FAILED(hr))
    {
        free(pwfx);
        return false;
    }

    // ���� ���� �� �ҽ� ���̽� ����
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

    // �ҽ� ���̽��� ��� �� ��ü �����ϵ��� �ΰ�, WAV ���� ���� ����ü�� ��� ����
    free(pwfx);

    return true;
}
