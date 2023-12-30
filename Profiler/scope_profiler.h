//
// Created by ZZK on 2023/9/19.
//

#pragma once

#include <chrono>
#include <iostream>
#include <vector>
#include <iomanip>
#include <string_view>
#include <map>
#include <set>

class ScopeProfiler
{
public:
    using ClockType = std::chrono::steady_clock;

    struct Record
    {
        const char *tag;
        int64_t us;
    };

private:
    ClockType::time_point beg;
    ClockType::time_point end;
    const char *tag;

    ScopeProfiler(const char *tag_, ClockType::time_point beg_);
    void on_destroy(ClockType::time_point end_);

public:
    ScopeProfiler(const char *tag_) : ScopeProfiler(tag_, ClockType::now()) {}
    ~ScopeProfiler() { on_destroy(ClockType::now()); }

    static std::vector<Record> const &get_records() { return recorder.records; }
    static void print_log(std::ostream &out = std::clog);

    struct Recorder
    {
        std::vector<Record> records;
        ~Recorder();
    };

    inline thread_local static Recorder recorder = {};
};

ScopeProfiler::ScopeProfiler(const char *tag_, ClockType::time_point beg_)
    : tag(tag_), beg(beg_)
{

}

void ScopeProfiler::on_destroy(ClockType::time_point end_)
{
    auto diff = end_ - beg;
    int64_t us = std::chrono::duration_cast<std::chrono::microseconds>(diff).count();
    recorder.records.push_back({ tag, us });
}

void ScopeProfiler::print_log(std::ostream &out)
{
    if (recorder.records.empty())
    {
        return;
    }

    struct Statistic
    {
        int64_t max_us = 0;
        int64_t min_us = 0;
        int64_t total_us = 0;
        int64_t count_rec = 0;
        const char *tag = nullptr;
    };

    std::map<std::string_view, Statistic> stats;
    for (auto const &[tag, us] : recorder.records)
    {
        auto &stat = stats[tag];
        stat.total_us += us;
        stat.max_us = std::max(stat.max_us, us);
        stat.min_us = !stat.count_rec ? us : std::min(stat.min_us, us);
        stat.count_rec++;
        stat.tag = tag;
    }

    struct StatisticCompare
    {
        using value_type = std::pair<std::string_view, Statistic>;
        bool operator()(value_type const &lhs, value_type const &rhs) const
        {
            return lhs.second.total_us > rhs.second.total_us;
        }
    };

    std::multiset<std::pair<std::string_view, Statistic>, StatisticCompare> sorted_stats(stats.begin(), stats.end());

    auto dump = [&out] (int64_t val, int64_t w) {
        int64_t tpwv = 1;
        for (int64_t i = 0; i < w - 1; ++i) tpwv *= 10;
        if (val > tpwv)
        {
            if (val / 1000 > tpwv / 10)
            {
                out << std::setw(w - 1) << val / 1000000 << 'M';
            } else
            {
                out << std::setw(w - 1) << val / 1000 << 'K';
            }
        } else
        {
            out << std::setw(w) << val;
        }
    };

    out << "   avg   |   min   |   max   |  total  | cnt | tag\n";

    for (auto const &[tag, stat] : sorted_stats)
    {
        dump(stat.total_us / stat.count_rec, 9); out << '|';
        dump(stat.min_us, 9); out << '|';
        dump(stat.max_us, 9); out << '|';
        dump(stat.total_us, 9); out << '|';
        dump(stat.count_rec, 5); out << '|';
        out << ' ' << tag << '\n';
    }
}

ScopeProfiler::Recorder::~Recorder()
{
    ScopeProfiler::print_log();
}

#if defined(__GNUC__) || defined(__clang__)
#define DefScopeProfiler ::ScopeProfiler _scopeProfiler(__PRETTY_FUNCTION__);
#elif defined(_MSC_VER)
#define DefScopeProfiler ::ScopeProfiler _scopeProfiler(__FUNCSIG__);
#else
#define DefScopeProfiler ::ScopeProfiler _scopeProfiler(__func__);
#endif
#define NamedScopeProfiler(name) ::ScopeProfiler _scopeProfiler_##name(#name);
#define RepeatScopeProfiler(times) for (size_t _spTime = 0; _spTime < times; _spTime++)

template <class T>
static
#if defined(__GNUC__) || defined(__clang__)
__attribute__((noinline))
#elif defined(_MSC_VER)
__declspec(noinline)
#endif
void do_not_optimize(T volatile const &t) {}





























