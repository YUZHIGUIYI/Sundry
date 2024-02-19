//
// Created by ZZK on 2024/2/18.
//

#include "multi_sparse.h"
#include <format>
#include <iostream>
#include <string>
#include <chrono>

#define TICK(x) auto bench_##x = std::chrono::steady_clock::now();
#define TOCK(x) std::cerr << std::format("{}: {} ms\n", #x, std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now() - bench_##x).count());

int main()
{
    using namespace experimental;
    using Grid = RootGrid<float, HashBlock<PointerBlock<4, DenseBlock<5, PlaceData<float>>>>>;

    constexpr int N = 512 * 512;

    TICK(BenchMain)

    auto* grid = new Grid{};
    auto wa_grid = grid->write_access();
    float px = -100.0f, py = 100.0f;
    float vx  = 0.2f, vy = -0.6f;
    for (int step = 0; step < N; ++step)
    {
        px += vx;
        py += vy;
        int x = (int)std::floor(px);
        int y = (int)std::floor(py);
        wa_grid.write(x, y, 1.0f);
    }

    grid->foreach([&grid, &wa_grid] (int x, int y, float& value) {
        wa_grid.write(x, y, -4.0f * grid->read(x, y)
                    + grid->read(x + 1, y)
                    + grid->read(x, y + 1)
                    + grid->read(x - 1, y)
                    + grid->read(x, y - 1));
    });

    TOCK(BenchMain)
}













