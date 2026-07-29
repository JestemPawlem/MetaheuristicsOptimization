#include <benchmark/benchmark.h>
#include <cmath>
#include <numbers>
#include <vector>

#include <mh/utils/math.h>


static void BM_math_std_cos_two_pi(benchmark::State& state)
{
    constexpr std::size_t kMask = 1023;
    constexpr float two_pi = 2.0f * std::numbers::pi_v<float>;

    std::vector<float> inputs(kMask + 1);
    for (std::size_t i = 0; i <= kMask; ++i)
        inputs[i] = static_cast<float>(i) * 0.001f;

    std::size_t idx = 0;
    for (auto _ : state)
    {
        float res = std::cos(inputs[idx & kMask] * two_pi);
        benchmark::DoNotOptimize(res);
        benchmark::ClobberMemory();
        ++idx;
    }
}

BENCHMARK(BM_math_std_cos_two_pi);


static void BM_math_mh_cos_two_pi(benchmark::State& state)
{
    constexpr std::size_t kMask = 1023;
    std::vector<float> inputs(kMask + 1);
    for (std::size_t i = 0; i <= kMask; ++i)
        inputs[i] = static_cast<float>(i) * 0.001f;

    std::size_t idx = 0;
    for (auto _ : state)
    {
        float res = mh::utils::math::cos_two_pi(inputs[idx & kMask]);
        benchmark::DoNotOptimize(res);
        benchmark::ClobberMemory();
        ++idx;
    }
}

BENCHMARK(BM_math_mh_cos_two_pi);