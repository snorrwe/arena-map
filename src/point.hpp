#pragma once

struct Point {
    int x, y;

    bool operator<(Point const& p) const noexcept
    {
        return x < p.x || (x == p.x && y < p.y);
    }

    bool operator==(Point const& p) const noexcept
    {
        return x == p.x && y == p.y;
    }

    bool operator!=(Point const& p) const noexcept
    {
        return x != p.x || y != p.y;
    }
};

