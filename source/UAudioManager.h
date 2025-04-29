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

    // XAudio2 ������ ������ ���̽� �ʱ�ȭ
    HRESULT Initialize();

    // ���� ����
    void Shutdown();

    // �־��� WAV ������ ���
    bool PlaySound(const std::wstring& filename);

private:
    IXAudio2* pXAudio2;
    IXAudio2MasteringVoice* pMasterVoice;

    //std::shared_ptr<VoiceCallback*> callback;

    std::vector<std::shared_ptr<std::vector<char>>> audioBuffers;
};
