//
// Created by ZZK on 2023/9/19.
//

#pragma once

#include <chrono>
#include <iostream>

#define TICK(x) auto bench_##x = std::chrono::steady_clock::now();
#define TOCK(x) std::cerr << #x ": " << std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now() - bench_##x).count(); std::cerr << "ms\n";
