#pragma once
#include "arena.hpp"
#include "point.hpp"
#include <algorithm>
#include <map>
#include <numeric>
#include <vector>

class NaiveDb final
{
public:
    using Map = std::map<Point, std::vector<double>>;

    /**
     * Returns nullptr is p is not in the database
     */
    std::vector<double> const* get(Point p) const noexcept
    {
        auto it = data.find(p);
        if (it != data.end())
        {
            return &it->second;
        }
        return nullptr;
    }

    void insert(Point const key, std::vector<double> const value)
    {
        data.insert(std::make_pair(key, std::move(value)));
    }

    void clear()
    {
        data.clear();
    }

    Map::iterator begin()
    {
        return data.begin();
    }
    Map::iterator end()
    {
        return data.end();
    }

private:
    Map data;
};

/// Vector with a fix capacity
/// Ctor takes a pointer to a buffer of size capacity
/// !!Important!! this object does not manage memory.
/// Be sure not to leak the buffer passed to the vector!
template <typename T>
class FixedLenView final
{
    T* ptr;
    size_t capacity;
    size_t _size = 0;

public:
    FixedLenView(T* ptr, size_t capacity) : ptr(ptr), capacity(capacity)
    {
    }

    FixedLenView(FixedLenView&& v) : ptr(v.ptr), capacity(v.capacity), _size(v._size)
    {
        v.ptr = nullptr;
        v.capacity = 0;
        v._size = 0;
    }

    FixedLenView& operator=(FixedLenView&& v)
    {
        ptr = v.ptr;
        capacity = v.capacity;
        _size = v._size;
        v.ptr = nullptr;
        v.capacity = 0;
        v._size = 0;
        return *this;
    }

    ~FixedLenView()
    {
        for (size_t i = 0; i < _size; ++i)
            ptr[i].~T();
    }

    T const& at(size_t const index) const
    {
        assert(index < _size);
        return ptr[index];
    }

    T& at(size_t const index)
    {
        assert(index < _size);
        return ptr[index];
    }

    T const& operator[](size_t const index) const
    {
        return at(index);
    }

    T& operator[](size_t const index)
    {
        return at(index);
    }

    T& push_back(T item)
    {
        assert(_size < capacity);
        ptr[_size] = std::move(item);
        ++_size;
        return ptr[_size - 1];
    }

    T& insert(size_t index, T item)
    {
        assert(_size < capacity);
        assert(index <= _size);
        for (size_t i = _size; i > index; --i)
        {
            std::swap(ptr[i], ptr[i - 1]);
        }
        ptr[index] = std::move(item);
        ++_size;
        return ptr[_size - 1];
    }

    void clear()
    {
        _size = 0;
    }

    T* begin()
    {
        return ptr;
    }

    T* end()
    {
        return ptr + _size;
    }
    T const* begin() const
    {
        return ptr;
    }

    T const* end() const
    {
        return ptr + _size;
    }

    T& back()
    {
        return ptr[_size - 1];
    }

    T const& back() const
    {
        return ptr[_size - 1];
    }

    size_t size() const
    {
        return _size;
    }
};

template <typename T1, typename T2>
class JoinIterator
{
    T1* a;
    T2* b;

public:
    JoinIterator(T1* a, T2* b) : a(a), b(b)
    {
    }

    JoinIterator(JoinIterator const&) = default;
    JoinIterator& operator=(JoinIterator const&) = default;

    T1& first()
    {
        return *a;
    }
    T2& second()
    {
        return *b;
    }

    bool operator==(JoinIterator<T1, T2> const& other) const
    {
        return a == other.a && b == other.b;
    }

    bool operator!=(JoinIterator<T1, T2> const& other) const
    {
        return !(*this == other);
    }

    JoinIterator& operator++()
    {
        ++a;
        ++b;
        return *this;
    }

    JoinIterator operator++(int)
    {
        auto* a = this->a++;
        auto* b = this->b++;
        return JoinIterator{a, b};
    }

    std::pair<T1&, T2&> operator*()
    {
        return std::make_pair(*a, *b);
    }

    JoinIterator<T1, T2>* operator->()
    {
        return this;
    }
};

class ArenaDb final
{
public:
    using VecValues = FixedLenView<double>;
    using VecKeys = FixedLenView<Point>;

    explicit ArenaDb() = delete;
    explicit ArenaDb(size_t key_capacity, size_t value_capacity = 30)
        : value_capacity{value_capacity}
        , allocator{}
        , keys{allocator.allocate<Point>(key_capacity), key_capacity}
        , values{allocator.allocate<VecValues>(key_capacity), key_capacity}
    {
    }

    JoinIterator<Point, VecValues> begin()
    {
        return JoinIterator<Point, VecValues>{keys.begin(), values.begin()};
    }

    JoinIterator<Point, VecValues> end()
    {
        return JoinIterator<Point, VecValues>{keys.end(), values.end()};
    }

    VecValues const* get(Point const p) const noexcept
    {
        size_t const ind = find(p);
        if (ind == size)
            return nullptr;
        return &values.at(ind);
    }

    // Inserting the same key twice is UB!
    VecValues* insert(Point const p)
    {
        keys.push_back(p);
        values.push_back(VecValues{allocator.allocate<double>(value_capacity), value_capacity});
        ++size;

        return &values.back();
    }

    void clear()
    {
        keys.clear();
        values.clear();
    }

    void sort()
    {
        if (sorted == size)
            return;
        sort_impl(0, size);
        sorted = size;
    }

private:
    size_t find(Point const p) const noexcept
    {
        auto const* const begin = keys.begin();
        auto const* end = begin + sorted;
        auto const* it = std::lower_bound(begin, end, p);
        if (it == end || *it != p)
        {
            end = keys.end();
            it = std::find(begin + sorted, end, p);
        }
        return it - begin;
    }

    // quicksort
    void sort_impl(size_t begin, size_t end)
    {
        if (begin == end)
            return;
        size_t pivot = partition(begin, end);
        sort_impl(begin, pivot);
        // skip the pivot
        sort_impl(pivot + 1, end);
    }

    size_t partition(size_t begin, size_t end)
    {
        using std::swap;
        --end;
        size_t pivot = end;
        size_t i = begin;

        for (size_t j = begin; j != end; ++j)
        {
            if (keys.at(j) < keys.at(pivot))
            {
                swap(keys.at(i), keys.at(j));
                swap(values.at(i), values.at(j));
                ++i;
            }
        }
        swap(keys.at(i), keys.at(pivot));
        swap(values.at(i), values.at(pivot));
        return i;
    }

    size_t sorted = 0;
    size_t size = 0;
    size_t value_capacity;
    ArenaAllocator allocator;

    VecKeys keys;
    FixedLenView<VecValues> values;
};

