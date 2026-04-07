#pragma once

#include <utility>

template <typename T>
class ComPtr
{
public:
    ComPtr() = default;
    explicit ComPtr(T* ptr) : ptr_(ptr) {}
    ComPtr(const ComPtr&) = delete;
    ComPtr& operator=(const ComPtr&) = delete;

    ComPtr(ComPtr&& other) noexcept : ptr_(other.ptr_)
    {
        other.ptr_ = nullptr;
    }
    ComPtr& operator=(ComPtr&& other) noexcept
    {
        if (this != &other) {
            Reset();
            ptr_ = other.ptr_;
            other.ptr_ = nullptr;
        }
        return *this;
    }

    ~ComPtr()
    {
        Reset();
    }

    T* Get() const { return ptr_; }
    T** GetAddressOf() { return &ptr_; }
    T* const* GetAddressOf() const { return &ptr_; }

    T* operator->() const { return ptr_; }
    explicit operator bool() const { return ptr_ != nullptr; }

    void Reset()
    {
        if (ptr_) {
            ptr_->Release();
            ptr_ = nullptr;
        }
    }

    void Attach(T* ptr)
    {
        Reset();
        ptr_ = ptr;
    }

    T* Detach()
    {
        T* out = ptr_;
        ptr_ = nullptr;
        return out;
    }

private:
    T* ptr_ = nullptr;
};
