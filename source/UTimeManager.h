#ifndef UTIMEMANGER_H
#define UTIMEMANGER_H

#include <functional>
#include <vector>
#include <utility>
#include <algorithm>
#include <memory>

class UTimeManager
{
public:
    // 싱글톤 인스턴스 접근자
    static UTimeManager& Get()
    {
        static UTimeManager instance;
        return instance;
    }

    // 함수 타입 정의
    using TimerCallback = std::function<void()>;
    using TimerID = uint32_t;

    struct TimerTask
    {
        TimerCallback Callback;
        double ExecutionTime;
        bool bIsCompleted = false;
        void* Owner = nullptr;
        TimerID ID = 0;

        TimerTask(TimerCallback callback, double executionTime, void* owner, TimerID id)
            : Callback(callback), ExecutionTime(executionTime), Owner(owner), ID(id) {
        }
    };

    // 초기화 함수
    static void Init()
    {
        Get().InitInternal();
    }

    // 델타 타임 추가 함수
    static void AddDeltaTime(double deltaTime)
    {
        Get().AddDeltaTimeInternal(deltaTime);
    }

    // 지연 함수 실행 예약
    static TimerID Invoke(const TimerCallback& action, float duration, void* owner = nullptr)
    {
        return Get().InvokeInternal(action, duration, owner);
    }

    // 반복 함수 실행 예약
    static TimerID InvokeRepeating(const TimerCallback& action, float delay, float repeatRate, void* owner = nullptr)
    {
        return Get().InvokeRepeatingInternal(action, delay, repeatRate, owner);
    }

    // 특정 소유자의 모든 타이머 취소
    static void CancelTimers(void* owner)
    {
        Get().CancelTimersInternal(owner);
    }

    // 특정 ID의 타이머 취소
    static void CancelTimer(TimerID id)
    {
        Get().CancelTimerInternal(id);
    }

private:
    // 생성자, 복사 생성자 등은 private으로 설정 (싱글톤 패턴)
    UTimeManager() = default;
    UTimeManager(const UTimeManager&) = delete;
    UTimeManager& operator=(const UTimeManager&) = delete;

    double totalTime = 0.0;
    std::vector<TimerTask> TimerTasks;
    TimerID nextTimerID = 1;

    // 내부 구현 함수들
    void InitInternal()
    {
        totalTime = 0.0;
        TimerTasks.clear();
        nextTimerID = 1;
    }

    void AddDeltaTimeInternal(double deltaTime)
    {
        totalTime += deltaTime;
        UpdateTimers();
    }

    TimerID InvokeInternal(const TimerCallback& action, float duration, void* owner)
    {
        double executionTime = totalTime + duration;
        TimerID id = nextTimerID++;
        TimerTasks.emplace_back(action, executionTime, owner, id);
        return id;
    }

    TimerID InvokeRepeatingInternal(const TimerCallback& action, float delay, float repeatRate, void* owner)
    {
        double firstExecutionTime = totalTime + delay;
        TimerID id = nextTimerID++;

        // 재귀적으로 자신을 다시 예약하는 래퍼 함수
        auto repeatingAction = std::make_shared<std::function<void()>>();

        *repeatingAction = [this, action, repeatRate, repeatingAction, owner, id]() {
            // 원래 액션 실행
            action();

            // 다음 실행 예약 (같은 ID 사용)
            double nextExecutionTime = totalTime + repeatRate;
            TimerTasks.emplace_back([repeatingAction]() { (*repeatingAction)(); },
                nextExecutionTime, owner, id);
            };

        // 첫 번째 실행 예약
        TimerTasks.emplace_back([repeatingAction]() { (*repeatingAction)(); },
            firstExecutionTime, owner, id);
        return id;
    }

    void CancelTimersInternal(void* owner)
    {
        if (!owner) return;

        for (auto& task : TimerTasks)
        {
            if (task.Owner == owner)
            {
                task.bIsCompleted = true;
            }
        }
    }

    void CancelTimerInternal(TimerID id)
    {
        for (auto& task : TimerTasks)
        {
            if (task.ID == id)
            {
                task.bIsCompleted = true;
            }
        }
    }

    void UpdateTimers()
    {
        // 실행할 타이머 작업을 확인하고 실행
        for (auto& task : TimerTasks)
        {
            if (!task.bIsCompleted && totalTime >= task.ExecutionTime)
            {
                try {
                    task.Callback();
                }
                catch (const std::exception& e) {
                    // 예외 처리 로깅
                    // std::cerr << "Timer callback exception: " << e.what() << std::endl;
                }
                task.bIsCompleted = true;
            }
        }

        // 완료된 작업 제거
        TimerTasks.erase(
            std::remove_if(TimerTasks.begin(), TimerTasks.end(),
                [](const TimerTask& task) { return task.bIsCompleted; }),
            TimerTasks.end());
    }
};

#endif