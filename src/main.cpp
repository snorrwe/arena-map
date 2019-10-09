#include <celero/Celero.h>

#include <iostream>

#include <random>

#ifndef WIN32
#include <cmath>
#include <cstdlib>
#endif

#include "arena.hpp"
#include "db.hpp"

CELERO_MAIN

std::random_device RandomDevice;
std::uniform_int_distribution<int> UniformDistribution(0, 255);

struct DbFixture : public celero::TestFixture {
    size_t num_keys, num_values = 30;

    virtual std::vector<celero::TestFixture::ExperimentValue>
    getExperimentValues() const override
    {
        std::vector<celero::TestFixture::ExperimentValue> problemSpace;

        const int64_t totalNumberOfTests = 5;

        for (int64_t i = 0; i < totalNumberOfTests; i++) {
            problemSpace.emplace_back(int64_t(1 << (8 + i)));
        }

        return problemSpace;
    }

    virtual void setUp(
        const celero::TestFixture::ExperimentValue& experimentValue) override
    {
        num_keys = experimentValue.Value;
    }
};

struct NaiveMapFixture : public DbFixture {
    NaiveDb db;
};

struct ArenaFixture : public DbFixture {
};

BASELINE_F(Init, NaiveMap, NaiveMapFixture, 0, 256)
{
    for (int i = 0; i < num_keys; ++i) {
        auto p = Point { rand(), rand() };
        std::vector<double> v;
        v.reserve(30);
        for (int j = 0; j < num_values; ++j) {
            v.emplace_back(rand());
        }
        db.insert(p, std::move(v));
    }
    db.clear();
}

BENCHMARK_F(Init, Arena, ArenaFixture, 0, 256)
{
    ArenaDb db { num_keys, num_values };

    for (int i = 0; i < num_keys; ++i) {
        auto p = Point { rand(), rand() };
        auto& data = db.insert(p);
        for (int j = 0; j < num_values; ++j) {
            data.ptr[j] = rand();
        }
    }
    celero::DoNotOptimizeAway(db);
}

struct NaiveMapFindFixture : public celero::TestFixture {
    std::vector<Point> keys;
    NaiveDb db;

    size_t num_keys, num_values = 30;

    virtual std::vector<celero::TestFixture::ExperimentValue>
    getExperimentValues() const override
    {
        std::vector<celero::TestFixture::ExperimentValue> problemSpace;

        const int64_t totalNumberOfTests = 5;

        for (int64_t i = 0; i < totalNumberOfTests; i++) {
            problemSpace.emplace_back(int64_t(1 << (8 + i)));
        }

        return problemSpace;
    }

    virtual void setUp(
        const celero::TestFixture::ExperimentValue& experimentValue) override
    {
        num_keys = experimentValue.Value;
        db.clear();
        for (int i = 0; i < num_keys; ++i) {
            auto p = Point { rand(), rand() };
            std::vector<double> v;
            v.reserve(30);
            for (int j = 0; j < num_values; ++j) {
                v.emplace_back(rand());
            }
            db.insert(p, std::move(v));
            keys.emplace_back(p);
        }
    }
};

struct ArenaMapFindFixture : public celero::TestFixture {
    std::unique_ptr<ArenaDb> db;
    std::vector<Point> keys;

    size_t num_keys, num_values = 30;

    virtual std::vector<celero::TestFixture::ExperimentValue>
    getExperimentValues() const override
    {
        std::vector<celero::TestFixture::ExperimentValue> problemSpace;

        const int64_t totalNumberOfTests = 5;

        for (int64_t i = 0; i < totalNumberOfTests; i++) {
            problemSpace.emplace_back(int64_t(1 << (8 + i)));
        }

        return problemSpace;
    }

    virtual void setUp(
        const celero::TestFixture::ExperimentValue& experimentValue) override
    {
        num_keys = experimentValue.Value;
        db.reset(
            new ArenaDb {
                num_keys, num_values });
        for (int i = 0; i < num_keys; ++i) {
            auto p = Point { rand(), rand() };
            keys.emplace_back(p);
            auto& data = db->insert(p);
            for (int j = 0; j < num_values; ++j) {
                data.ptr[j] = rand();
            }
        }
    }
};

BASELINE_F(Find, NaiveMap, NaiveMapFindFixture, 0, 256)
{
    for (auto const& k : keys) {
        auto* v = db.get(k);
        celero::DoNotOptimizeAway(v);
    }
}

BENCHMARK_F(Find, Arena, ArenaMapFindFixture, 0, 256)
{
    for (auto const& k : keys) {
        auto* v = db->get(k);
        celero::DoNotOptimizeAway(v);
    }
}

