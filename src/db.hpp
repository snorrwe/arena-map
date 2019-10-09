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
    struct Node {
        Point point;
        double* ptr;
        size_t size;
        Node(Point point, double* ptr, size_t size)
            : point(point)
            , ptr(ptr)
            , size(size)
        {
        }

        bool operator<(Node const& n) const noexcept { return point < n.point; }
        bool operator==(Node const& n) const noexcept
        {
            return point == n.point;
        }

        bool operator<(Point const& p) const noexcept { return point < p; }
        bool operator==(Point const& p) const noexcept { return point == p; }
    };

    ArenaDb(size_t key_capacity = 500, size_t value_capacity = 30)
        : allocator(key_capacity * value_capacity * sizeof(double) + key_capacity * sizeof(Node))
        , value_capacity(value_capacity)

    {
        _points.reserve(key_capacity);
    }

    Node const* get(Point const p) const noexcept
    {

        auto it = std::lower_bound(_points.begin(), _points.end(), p);
        if (it == _points.end() || it->point != p)
            return nullptr;
        return &*it;
    }

    Node& insert(Point const p)
    {
        auto it = std::lower_bound(_points.begin(), _points.end(), p);
        if (it != _points.end() && it->point == p) {
            return *it;
        }
        double* ptr = (double*)allocator.allocate<double>(value_capacity);
        _points.emplace(it, p, ptr, value_capacity);
        return _points.back();
    }

    void clear()
    {
        allocator.clear();
        _points.clear();
    }

private:
    ArenaAllocator allocator;
    std::vector<Node, TypedArena<Node>> _points { TypedArena<Node> { allocator } };
    size_t value_capacity;
};

