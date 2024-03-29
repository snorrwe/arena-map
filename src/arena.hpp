#pragma once
#include <cassert>
#include <memory>

constexpr size_t DEFAULT_PAGE_SIZE = 4096;

/// Allocator that frees all its memory on destruction
class ArenaAllocator final
{
    char* _start;
    char* _end;
    char* _next;

    // ArenaAllocator is a linked list
    ArenaAllocator* _next_arena = nullptr;

public:
    ArenaAllocator(ArenaAllocator const&) = delete;
    ArenaAllocator& operator=(ArenaAllocator const&) = delete;

    explicit ArenaAllocator(size_t capacity = DEFAULT_PAGE_SIZE)
        : _start(new char[capacity]), _end(_start + capacity), _next(_start)
    {
    }

    ~ArenaAllocator()
    {
        delete[] _start;
        delete _next_arena;
    }

    ArenaAllocator(ArenaAllocator&& a) noexcept
        : _start(a._start), _end(a._end), _next(a._next), _next_arena(a._next_arena)
    {
        a._start = nullptr;
        a._end = nullptr;
        a._next = nullptr;
        a._next_arena = nullptr;
    }

    ArenaAllocator& operator=(ArenaAllocator&& a) noexcept
    {
        _start = a._start;
        _end = a._end;
        _next = a._next;
        _next_arena = a._next_arena;
        a._start = nullptr;
        a._end = nullptr;
        a._next = nullptr;
        a._next_arena = nullptr;
        return *this;
    }

    size_t remaining() const noexcept
    {
        if (_next > _end)
            return 0;
        return _end - _next;
    }

    /// Allocate space for n items of type T
    /// Throw std::bad_alloc if the Allocator is out of memory
    template <typename T>
    T* allocate(const size_t n)
    {
        return allocate_from<T>(this, n);
    }

    // Make the allocator usable in stl containers
    void deallocate(void* _p, size_t _n) noexcept
    {
        // nope
    }

    /// Reset this allocator
    /// Note that this does not free any memory, but new items will override the
    /// old ones! Using any pointer obtained before clearing is undefined
    /// behaviour
    void clear() noexcept
    {
        _next = _start;
        if (_next_arena)
            _next_arena->clear();
    }

    bool operator==(ArenaAllocator const& other) const noexcept
    {
        return _start == other._start;
    }

    bool operator!=(ArenaAllocator const& other) const noexcept
    {
        return _start != other._start;
    }

private:
    template <typename T>
    T* allocate_from(ArenaAllocator* arena, const size_t n)
    {
        assert(arena != nullptr);
        const size_t delta = sizeof(T) * n;
        ArenaAllocator* last = nullptr;
        do
        {
            if (delta <= arena->remaining())
            {
                T* ptr = (T*)arena->_next;
                arena->_next += delta;
                return ptr;
            }
            last = arena;
            arena = arena->_next_arena;
        } while (arena != nullptr);
        const size_t capacity = std::max({size_t(_end - _start), delta});
        last->_next_arena = new ArenaAllocator{capacity};
        return allocate_from<T>(last->_next_arena, n);
    }
};

/// Typed Arena Allocator to be used in container templates
/// Takes an ArenaAllocator as a backend
template <typename T>
class TypedArena final
{
    ArenaAllocator* _arena;

public:
    using pointer = T*;
    using const_pointer = T const*;
    using void_pointer = void*;
    using const_void_pointer = void const*;
    using value_type = T;

    explicit TypedArena(ArenaAllocator& arena) : _arena(&arena)
    {
    }

    TypedArena() = delete;

    TypedArena(TypedArena<T> const&) = default;
    TypedArena& operator=(TypedArena<T> const&) = default;
    TypedArena(TypedArena<T>&&) = default;
    TypedArena& operator=(TypedArena<T>&&) = default;
    ~TypedArena() = default;

    T* allocate(const size_t n)
    {
        T* ptr = nullptr;
        if (_arena)
            ptr = _arena->allocate<T>(n);
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

    operator ArenaAllocator&() const
    {
        return *_arena;
    }
};

