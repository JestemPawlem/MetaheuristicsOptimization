#include <benchmark/benchmark.h>
#include <cmath>
#include <vector>

#include <mh/utils/math.h>


static void BM_math_std_sqrt(benchmark::State& state)
{
    constexpr std::size_t kMask = 1023;
    std::vector<float> inputs(kMask + 1);
    for (std::size_t i = 0; i <= kMask; ++i)
        inputs[i] = static_cast<float>(i) * 0.1f;

    std::size_t idx = 0;
    for (auto _ : state)
    {
        float res = std::sqrt(inputs[idx & kMask]);
        benchmark::DoNotOptimize(res);
        benchmark::ClobberMemory();
        ++idx;
    }
}

BENCHMARK(BM_math_std_sqrt);