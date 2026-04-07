#pragma once
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>
#include <deque>
#include <functional>
#include <mutex>
#include "../globals.h"

// НЕНАДОДЯДЯБЛЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯ

using CheckTargetAttackableHandler = bool(*)(void* attacker, void* target, bool checkBackstage, bool& outResult);

inline std::vector<CheckTargetAttackableHandler>& GetCheckTargetAttackableHandlers()
{
    static std::vector<CheckTargetAttackableHandler> handlers;
    return handlers;
}

inline void RegisterCheckTargetAttackable(CheckTargetAttackableHandler fn)
{
    GetCheckTargetAttackableHandlers().push_back(fn);
}

#define _(name) \
    static bool name(void* attacker, void* target, bool checkBackstage, bool& outResult); \
    static int name##_reg = (RegisterCheckTargetAttackable(&name), 0); \
    static bool name(void* attacker, void* target, bool checkBackstage, bool& outResult)


using UpdateHandler = void(*)(void* __this);

inline std::vector<UpdateHandler>& GetUpdateHandlers()
{
    static std::vector<UpdateHandler> handlers;
    return handlers;
}

inline void RegisterUpdate(UpdateHandler fn)
{
    GetUpdateHandlers().push_back(fn);
}

#define UPDATE_HOOK(name) \
      static void name(void* __this); \
      static int name##_reg = (RegisterUpdate(&name), 0); \
      static void name(void* __this)



using UnityThreadTask = std::function<void()>;

inline std::deque<UnityThreadTask>& GetUnityThreadQueue()
{
    static std::deque<UnityThreadTask> queue;
    return queue;
}

inline std::mutex& GetUnityThreadQueueMutex()
{
    static std::mutex mutex;
    return mutex;
}

inline void RunOnUnityThread(UnityThreadTask fn)
{
    if (!fn)return;
    std::lock_guard<std::mutex> lock(GetUnityThreadQueueMutex());
    GetUnityThreadQueue().push_back(std::move(fn));
}

inline void DrainUnityThreadQueue()
{
    std::deque<UnityThreadTask> local;
    {
        std::lock_guard<std::mutex> lock(GetUnityThreadQueueMutex());
        if (GetUnityThreadQueue().empty())
            return;
        local.swap(GetUnityThreadQueue());
    }
    for (auto& task : local)
    {
        if (task)task();
    }
}

template <typename Fn>
struct HookChain;

template <typename Ret, typename... Args>
struct HookChain<Ret(*)(Args...)>
{
    using Fn = Ret(*)(Args...);
    using RetType = Ret;
    using Handler = Ret(*)();

    struct Frame
    {
        Fn orig{};
        std::vector<Handler>* handlers{};
        size_t index{};
        std::tuple<Args...> args;
    };

    static void Install(Handler fn)
    {
        Handlers().push_back(fn);
    }

    static Ret Dispatch(Fn orig, Args... args)
    {
        Frame frame{ orig, &Handlers(), 0u, std::tuple<Args...>(args...) };
        Frames().push_back(&frame);
        if constexpr (std::is_void_v<Ret>) {
            CallNext();
            Frames().pop_back();
            return;
        } else {
            Ret out = CallNext();
            Frames().pop_back();
            return out;
        }
    }

    static Ret CallNext()
    {
        Frame* frame = CurrentFrame();
        if (!frame) {
            if constexpr (std::is_void_v<Ret>)
                return;
            else
                return Ret{};
        }

        if (frame->index < frame->handlers->size())
        {
            Handler fn = (*frame->handlers)[frame->index++];
            return fn();
        }

        if (frame->orig)
            return std::apply(frame->orig, frame->args);

        if constexpr (std::is_void_v<Ret>)
            return;
        else
            return Ret{};
    }

    template <size_t I>
    static auto& Arg()
    {
        Frame* frame = CurrentFrame();
        return std::get<I>(frame->args);
    }

private:
    static std::vector<Handler>& Handlers()
    {
        static std::vector<Handler> handlers;
        return handlers;
    }

    static std::vector<Frame*>& Frames()
    {
        static thread_local std::vector<Frame*> stack;
        return stack;
    }

    static Frame* CurrentFrame()
    {
        auto& stack = Frames();
        return stack.empty() ? nullptr : stack.back();
    }
};

#define HOOK_CHAIN_DECLARE(name, ret, ...) \
    using name##_Fn = ret (*)(__VA_ARGS__); \
    HookChain<name##_Fn>& name##_Chain(); \
    extern ret (*name##_Orig)(__VA_ARGS__); \
    ret name##_Hook(__VA_ARGS__);

#define HOOK_CHAIN_DEFINE(name, ret, params, ...) \
    HookChain<name##_Fn>& name##_Chain() { static HookChain<name##_Fn> chain; return chain; } \
    ret (*name##_Orig) params = nullptr; \
    ret name##_Hook params { return name##_Chain().Dispatch(name##_Orig, __VA_ARGS__); }

#define HOOK_INSTALL(name, handler) \
    static HookChain<name##_Fn>::RetType handler(); \
    static int handler##_reg = (name##_Chain().Install(&handler), 0); \
    static HookChain<name##_Fn>::RetType handler()

#define HOOK_RET_ORIG(name) \
    auto ret_orig = []() -> decltype(auto) { return HookChain<name##_Fn>::CallNext(); }

#define HOOK_ARG(name, index) \
    HookChain<name##_Fn>::template Arg<index>()


#define GAME_HOOK(offset, ret, name, ...) HOOK_CHAIN_DECLARE(name, ret, __VA_ARGS__)
#include "../core/GameHooks.def"
#undef GAME_HOOK



