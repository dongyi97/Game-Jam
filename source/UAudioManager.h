#pragma once
#include <xaudio2.h>
#include <string>
#include <wrl/client.h>
#include <vector>
#include <memory>
class UAudioManager
{
public:
	static UAudioManager MainAudioManager;

    UAudioManager();
    ~UAudioManager();

    // XAudio2 엔진과 마스터 보이스 초기화
    HRESULT Initialize();

    // 엔진 정리
    void Shutdown();

    // 주어진 WAV 파일을 재생
    bool PlaySound(const std::wstring& filename);

private:
    IXAudio2* pXAudio2;
    IXAudio2MasteringVoice* pMasterVoice;

    //std::shared_ptr<VoiceCallback*> callback;

    std::vector<std::shared_ptr<std::vector<char>>> audioBuffers;
};
