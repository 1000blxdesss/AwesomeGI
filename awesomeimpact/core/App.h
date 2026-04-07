#pragma once

#include <Windows.h>
#include <atomic>
#include <mutex>


class App
{
public:
    explicit App(HMODULE module);

    void Bootstrap();
    void Run();

private:
    void Init();
    void TryInitRenderHook();
    void Shutdown();
    void Update();
    void InitFeatures();

    HMODULE module_ { nullptr };
    std::atomic<bool> running_{ true };
    std::atomic<bool> bootstrapped_{ false };
    std::atomic<bool> renderHookBound_{ false };
    std::atomic<bool> shutdownDone_{ false };
    std::once_flag initOnce_{};

};

void SetGlobalAppInstance(App* app);
void TickGlobalAppFromGameUpdate();
int ConsumeGlobalMouseWheelSteps();
