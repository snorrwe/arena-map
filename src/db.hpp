#pragma once
#include "arena.hpp"
#include "point.hpp"
#include <algorithm>
#include <map>
#include <vector>

class NaiveDb final {
    std::map<Point, std::vector<double>> data;

public:
    /**
     * Returns nullptr is p is not in the database
     */
    std::vector<double> const* get(Point p) const noexcept
    {
        auto it = data.find(p);
        if (it != data.end()) {
            return &it->second;
        }
        return nullptr;
    }

    void insert(Point const key, std::vector<double> const value)
    {
        data.insert(std::make_pair(key, std::move(value)));
    }

    void clear() { data.clear(); }
};

class ArenaDb final {
public:
    // TODO: make this a container
    struct FixedLenVector {
        double* ptr;
        size_t capacity;
        size_t size = 0;
        FixedLenVector(double* ptr, size_t capacity)
            : ptr(ptr)
            , capacity(capacity)
        {
        }
    };

    ArenaDb(size_t key_capacity = 500, size_t value_capacity = 30)
        : value_capacity(value_capacity)
        , allocator(2*key_capacity * value_capacity * sizeof(double) + key_capacity * sizeof(FixedLenVector))

    {
    }

    FixedLenVector const* get(Point const p) const noexcept
    {
        auto it = _points.find(p);
        if (it == _points.end())
            return nullptr;
        return &it->second;
    }

    FixedLenVector* insert(Point const p)
    {
        double* ptr = (double*)allocator.allocate<double>(value_capacity);
        auto result = _points.insert(std::make_pair(p, FixedLenVector { ptr, value_capacity }));
        if (!result.second)
            return nullptr;
        return &result.first->second;
    }

    void clear()
    {
        allocator.clear();
        _points.clear();
    }

private:
    using TAllocator = TypedArena<std::pair<const Point, FixedLenVector>>;

    size_t value_capacity;
    ArenaAllocator allocator;
    std::map<Point, FixedLenVector, std::less<Point>,
        TAllocator>
        _points { TAllocator { allocator } };
};

