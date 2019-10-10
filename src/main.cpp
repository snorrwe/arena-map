#include <celero/Celero.h>

#include <iostream>

#include <random>

#ifndef WIN32
#include <cmath>
#include <cstdlib>
#endif

#include "arena.hpp"
#include "db.hpp"
#include "point.hpp"

CELERO_MAIN

std::vector<celero::TestFixture::ExperimentValue> problemSpace {
    32, 1 << 8, 400, 1 << 9, 1 << 10, 1 << 11,
};

struct DbFixture : public celero::TestFixture
{
    size_t num_keys, num_values = 30;

    virtual std::vector<celero::TestFixture::ExperimentValue> getExperimentValues() const override
    {
        return problemSpace;
    }

    virtual void setUp(const celero::TestFixture::ExperimentValue& experimentValue) override
    {
        num_keys = experimentValue.Value;
    }
};

struct NaiveMapFixture : public DbFixture
{
    NaiveDb db;
};

struct ArenaFixture : public DbFixture
{
};

BASELINE_F(Init, NaiveMap, NaiveMapFixture, 0, 256)
{
    for (int i = 0; i < num_keys; ++i)
    {
        auto p = Point {rand(), rand()};
        std::vector<double> v;
        v.reserve(30);
        for (int j = 0; j < num_values; ++j)
        {
            v.emplace_back(rand());
        }
        db.insert(p, std::move(v));
    }
    db.clear();
}

BENCHMARK_F(Init, Arena, ArenaFixture, 0, 256)
{
    ArenaDb db {num_keys, num_values};

    for (int i = 0; i < num_keys; ++i)
    {
        auto p = Point {rand(), rand()};
        auto* data = db.insert(p);
        celero::DoNotOptimizeAway(db);
        for (int j = 0; j < num_values; ++j)
        {
            data->push_back(rand());
        }
        celero::DoNotOptimizeAway(data);
    }
    celero::DoNotOptimizeAway(db);
}

struct NaiveMapFindFixture : public celero::TestFixture
{
    std::vector<Point> keys;
    NaiveDb db;

    size_t num_keys, num_values = 30;

    virtual std::vector<celero::TestFixture::ExperimentValue> getExperimentValues() const override
    {
        return problemSpace;
    }

    virtual void setUp(const celero::TestFixture::ExperimentValue& experimentValue) override
    {
        num_keys = experimentValue.Value;
        db.clear();
        for (int i = 0; i < num_keys; ++i)
        {
            auto p = Point {rand(), rand()};
            std::vector<double> v;
            v.reserve(30);
            for (int j = 0; j < num_values; ++j)
            {
                v.emplace_back(rand());
            }
            db.insert(p, std::move(v));
            keys.emplace_back(p);
        }
    }
};

struct ArenaMapFindFixture : public celero::TestFixture
{
    std::unique_ptr<ArenaDb> db;
    std::vector<Point> keys;

    size_t num_keys, num_values = 30;

    virtual std::vector<celero::TestFixture::ExperimentValue> getExperimentValues() const override
    {
        return problemSpace;
    }

    virtual void setUp(const celero::TestFixture::ExperimentValue& experimentValue) override
    {
        num_keys = experimentValue.Value;
        db.reset(new ArenaDb {num_keys, num_values});
        for (int i = 0; i < num_keys; ++i)
        {
            auto p = Point {rand(), rand()};
            keys.emplace_back(p);
            auto* data = db->insert(p);
            for (int j = 0; j < num_values; ++j)
            {
                data->push_back(rand());
            }
        }
    }
};

BASELINE_F(Find, NaiveMap, NaiveMapFindFixture, 0, 256)
{
    for (auto const& k : keys)
    {
        auto* v = db.get(k);
        celero::DoNotOptimizeAway(v);
    }
}

BENCHMARK_F(Find, Arena, ArenaMapFindFixture, 0, 256)
{
    for (auto const& k : keys)
    {
        auto* v = db->get(k);
        celero::DoNotOptimizeAway(v);
    }
}

BENCHMARK_F(Find, ArenaSorted, ArenaMapFindFixture, 0, 256)
{
    db->sort();
    for (auto const& k : keys)
    {
        auto* v = db->get(k);
        celero::DoNotOptimizeAway(v);
    }
}

BASELINE_F(InsertAndFind, NaiveMap, DbFixture, 0, 64)
{
    NaiveDb db;

    auto const insert = [&]() {
        const auto p = Point {rand(), rand()};
        std::vector<double> v;
        v.reserve(30);
        for (int j = 0; j < num_values; ++j)
        {
            v.emplace_back(rand());
        }
        db.insert(p, std::move(v));
        return p;
    };

    std::vector<Point> keys;
    for (int i = 0; i < num_keys; ++i)
    {
        auto key = insert();
        keys.emplace_back(key);
    }
    for (auto& k : keys)
    {
        auto* v = db.get(k);
        celero::DoNotOptimizeAway(v);
    }

    celero::DoNotOptimizeAway(db);
}

BENCHMARK_F(InsertAndFind, ArenaSorting, DbFixture, 0, 64)
{
    ArenaDb db {num_keys};

    auto const insert = [&]() {
        const auto p = Point {rand(), rand()};
        auto* v = db.insert(p);
        for (int j = 0; j < num_values; ++j)
        {
            v->push_back(rand());
        }
        return p;
    };

    std::vector<Point> keys;
    for (int i = 0; i + 1 < num_keys; ++i)
    {
        auto key = insert();
        keys.emplace_back(key);
    }
    db.sort();
    for (auto& k : keys)
    {
        auto* v = db.get(k);
        celero::DoNotOptimizeAway(v);
    }

    celero::DoNotOptimizeAway(keys);
    celero::DoNotOptimizeAway(db);
}

