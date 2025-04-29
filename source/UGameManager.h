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
    // �̱��� �ν��Ͻ��� ������ ���� ����
    static UGameManager* instance;

    // ��Ƽ������ ȯ�濡���� ������ �ʱ�ȭ�� ���� ���ؽ�
    static std::mutex mutex;

    // ���ҽ� ������ ���� ���� ��� ����
    int resourceCounter;

    // �����ڸ� private���� �����Ͽ� �ܺο��� ���� ��ü ���� ����
    UGameManager() : resourceCounter(0) {
        std::cout << "SingletonManager ������" << std::endl;
    }

    // ���� �����ڿ� ���� �����ڸ� �����Ͽ� ���� ����
    UGameManager(const UGameManager&) = delete;
    UGameManager& operator=(const UGameManager&) = delete;

public:
    // �Ҹ���
    ~UGameManager() {
        std::cout << "SingletonManager �Ҹ��" << std::endl;
    }

    // �̱��� �ν��Ͻ��� �����ϱ� ���� ���� �޼���
    static UGameManager* getInstance() {
        // ���� üŷ �� ����(Double-Checked Locking Pattern)���� ȿ���� ���
        if (instance == nullptr) {
            std::lock_guard<std::mutex> lock(mutex);
            if (instance == nullptr) {
                instance = new UGameManager();
            }
        }
        return instance;
    }

    // �̱��� �ν��Ͻ� ����
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