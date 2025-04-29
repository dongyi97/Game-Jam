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
    // �̱��� �ν��Ͻ� ������
    static UTimeManager& Get()
    {
        static UTimeManager instance;
        return instance;
    }

    // �Լ� Ÿ�� ����
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

    // �ʱ�ȭ �Լ�
    static void Init()
    {
        Get().InitInternal();
    }

    // ��Ÿ Ÿ�� �߰� �Լ�
    static void AddDeltaTime(double deltaTime)
    {
        Get().AddDeltaTimeInternal(deltaTime);
    }

    // ���� �Լ� ���� ����
    static TimerID Invoke(const TimerCallback& action, float duration, void* owner = nullptr)
    {
        return Get().InvokeInternal(action, duration, owner);
    }

    // �ݺ� �Լ� ���� ����
    static TimerID InvokeRepeating(const TimerCallback& action, float delay, float repeatRate, void* owner = nullptr)
    {
        return Get().InvokeRepeatingInternal(action, delay, repeatRate, owner);
    }

    // Ư�� �������� ��� Ÿ�̸� ���
    static void CancelTimers(void* owner)
    {
        Get().CancelTimersInternal(owner);
    }

    // Ư�� ID�� Ÿ�̸� ���
    static void CancelTimer(TimerID id)
    {
        Get().CancelTimerInternal(id);
    }

private:
    // ������, ���� ������ ���� private���� ���� (�̱��� ����)
    UTimeManager() = default;
    UTimeManager(const UTimeManager&) = delete;
    UTimeManager& operator=(const UTimeManager&) = delete;

    double totalTime = 0.0;
    std::vector<TimerTask> TimerTasks;
    TimerID nextTimerID = 1;

    // ���� ���� �Լ���
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

        // ��������� �ڽ��� �ٽ� �����ϴ� ���� �Լ�
        auto repeatingAction = std::make_shared<std::function<void()>>();

        *repeatingAction = [this, action, repeatRate, repeatingAction, owner, id]() {
            // ���� �׼� ����
            action();

            // ���� ���� ���� (���� ID ���)
            double nextExecutionTime = totalTime + repeatRate;
            TimerTasks.emplace_back([repeatingAction]() { (*repeatingAction)(); },
                nextExecutionTime, owner, id);
            };

        // ù ��° ���� ����
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
        // ������ Ÿ�̸� �۾��� Ȯ���ϰ� ����
        for (auto& task : TimerTasks)
        {
            if (!task.bIsCompleted && totalTime >= task.ExecutionTime)
            {
                try {
                    task.Callback();
                }
                catch (const std::exception& e) {
                    // ���� ó�� �α�
                    // std::cerr << "Timer callback exception: " << e.what() << std::endl;
                }
                task.bIsCompleted = true;
            }
        }

        // �Ϸ�� �۾� ����
        TimerTasks.erase(
            std::remove_if(TimerTasks.begin(), TimerTasks.end(),
                [](const TimerTask& task) { return task.bIsCompleted; }),
            TimerTasks.end());
    }
};

#endif