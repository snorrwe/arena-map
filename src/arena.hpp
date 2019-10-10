#pragma once
#include <cassert>
#include <memory>

/// Allocator that allocates memory once via malloc and frees all memory on
/// destruction
class ArenaAllocator final {
    char* _start;
    char* _end;
    char* _next;

public:
    ArenaAllocator(ArenaAllocator const&) = delete;
    ArenaAllocator& operator=(ArenaAllocator const&) = delete;

    ArenaAllocator(size_t capacity) noexcept
        : _start(new char[capacity])
        , _end(_start + capacity)
        , _next(_start)
    {
    }

    ~ArenaAllocator() { delete[] _start; }

    ArenaAllocator(ArenaAllocator&& a) noexcept
        : _start(a._start)
        , _end(a._end)
        , _next(a._next)
    {
        a._start = nullptr;
        a._end = nullptr;
        a._next = nullptr;
    }

    ArenaAllocator& operator=(ArenaAllocator&& a) noexcept
    {
        _start = a._start;
        _end = a._end;
        _next = a._next;
        a._start = nullptr;
        a._end = nullptr;
        a._next = nullptr;
        return *this;
    }

    /// Allocate space for n items of type T
    /// Throw std::bad_alloc if the Allocator is out of memory
    template <typename T>
    T* allocate(const size_t n)
    {
        const size_t delta = sizeof(T) * n;
        if (_next + delta > _end)
            throw std::bad_alloc {};
        auto* ptr = (T*)_next;
        _next += delta;
        return ptr;
    }

    /// Reset this allocator
    /// Note that this does not free any memory, but new items will override the
    /// old ones! Using any pointer obtained before clearing is undefined
    /// behaviour
    void clear() noexcept { _next = _start; }

    bool operator==(ArenaAllocator const& other) const noexcept
    {
        return _start == other._start;
    }

    bool operator!=(ArenaAllocator const& other) const noexcept
    {
        return _start != other._start;
    }
};

/// Typed Arena Allocator to be used in container templates
/// Takes an ArenaAllocator as a backend
template <typename T>
class TypedArena {
public:
    using Self = ArenaAllocator;
    using pointer = T*;
    using const_pointer = T const*;
    using void_pointer = void*;
    using const_void_pointer = void const*;
    using value_type = T;

    TypedArena(ArenaAllocator& arena)
        : _arena(arena)
    {
    }

    T* allocate(const size_t n)
    {
        auto* ptr = _arena.allocate<T>(n);
        return ptr;
    }

    // Make the allocator usable in stl containers
    void deallocate(void* _p, size_t _n) noexcept
    {
        // nope
    }

    bool operator==(TypedArena<T> const& other) const noexcept
    {
        return _arena == other._arena;
    }

    bool operator!=(TypedArena<T> const& other) const noexcept
    {
        return !(*this == other);
    }

private:
    ArenaAllocator& _arena;
};

