// Singleton.h
#ifndef UGAME_MANAGER_H
#define UGAME_MANAGER_H

#include <iostream>
#include <memory>
#include <mutex>

class UGameManager {
private:
    int score = 0;
    bool bIsGameOver = false;

private:
    // 싱글톤 인스턴스를 저장할 정적 변수
    static UGameManager* instance;

    // 멀티스레딩 환경에서의 안전한 초기화를 위한 뮤텍스
    static std::mutex mutex;

    // 리소스 관리를 위한 예시 멤버 변수
    int resourceCounter;

    // 생성자를 private으로 선언하여 외부에서 직접 객체 생성 방지
    UGameManager() : resourceCounter(0) {
        std::cout << "SingletonManager 생성됨" << std::endl;
    }

    // 복사 생성자와 대입 연산자를 삭제하여 복제 방지
    UGameManager(const UGameManager&) = delete;
    UGameManager& operator=(const UGameManager&) = delete;

public:
    // 소멸자
    ~UGameManager() {
        std::cout << "SingletonManager 소멸됨" << std::endl;
    }

    // 싱글톤 인스턴스에 접근하기 위한 정적 메서드
    static UGameManager* getInstance() {
        // 더블 체킹 락 패턴(Double-Checked Locking Pattern)으로 효율성 향상
        if (instance == nullptr) {
            std::lock_guard<std::mutex> lock(mutex);
            if (instance == nullptr) {
                instance = new UGameManager();
            }
        }
        return instance;
    }

    // 싱글톤 인스턴스 해제
    static void releaseInstance() {
        std::lock_guard<std::mutex> lock(mutex);
        if (instance != nullptr) {
            delete instance;
            instance = nullptr;
        }
    }

    int GetScore() { return score; }
    void AddScore(int value) { score += value; }

    bool IsGameOver() { return bIsGameOver; }
    void GameOver();
    void Restart() {
        bIsGameOver = false;
        ResetScore();
    }

private:
    void ResetScore() { score = 0; }
};

#endif // SINGLETON_MANAGER_H