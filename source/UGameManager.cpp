#include "UGameManager.h"
#include "UUIManager.h"

// ���� ��� ���� �ʱ�ȭ
UGameManager* UGameManager::instance = nullptr;
std::mutex UGameManager::mutex;

void UGameManager::GameOver() {
    bIsGameOver = true;
}