#include "UGameManager.h"
#include "UUIManager.h"

// 정적 멤버 변수 초기화
UGameManager* UGameManager::instance = nullptr;
std::mutex UGameManager::mutex;

void UGameManager::GameOver() {
    bIsGameOver = true;
}