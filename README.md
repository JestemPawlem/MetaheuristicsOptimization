# Metaheuristics Optimization Library (C++23)

A high-performance C++23 header-only framework designed for automated hyperparameter tuning and benchmarking of metaheuristic algorithms on mathematical test functions (Rastrigin, Rosenbrock, etc.).

## Performance First
Grid searches over hyperparameter spaces can quickly turn into multi-day runs. To compress execution times from days to hours, this library employs low-level optimizations:

* **Data-Oriented Design (DOD):** Structures and memory layouts are optimized for cache locality.
* **Zero Allocation Policy:** Avoids heap allocations in critical loops (`std::array` are preferred since population sizes and iterations are known at compile time).
* **Compile-Time Grid Generation:** Hyperparameter grids and evaluation schemes are evaluated using `constexpr` mechanics where possible.
* **Fast Math & PRNGs:** Custom fast math approximations (`mh::utils::math`) and lightweight, thread-local pseudo-random number generators replace heavy standard library equivalents.
* **Multi-threaded Benchmarking:** Parallel execution across hyperparameter configurations.

## Evaluation & Metrics
To account for the non-deterministic nature of metaheuristics, each hyperparameter setup is evaluated over multiple independent runs. The benchmarking code records and exports:
* **Trimmed Mean (Top 80%):** Robust central value calculation that excludes extreme stochastic outliers.
* **Execution Time**
* **Best Fitness Found**
* **Standard Deviation**

Results are exported to CSV for rapid post-processing, visualization, and statistical analysis via Python scripts (`scripts/`).

## Project Structure
* `src/include/mh/` – Header-only optimization engine, algorithms, and math utilities.
* `src/experiments/` – Multi-threaded grid search execution binaries.
* `src/benchmarks/` – Microbenchmarks using Google Benchmark to validate math speedups.
* `scripts/` – Python notebooks for hyperparameter visual analysis.