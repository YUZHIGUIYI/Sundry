//
// Created by ZZK on 2024/2/18.
//

#pragma once

#include <tuple>
#include <unordered_map>
#include <memory>

namespace experimental
{
    struct Grid
    {
        struct MyHash
        {
            size_t operator()(std::tuple<int, int> const &key) const
            {
                auto const &[x, y] = key;
                return (x * 2718281828) ^ (y * 3141592653);
            }
        };

        static constexpr int Bshift = 8;
        static constexpr int B = 1 << Bshift;
        static constexpr int Bmask = B - 1;

        static constexpr int B1shift = 6;
        static constexpr int B1 = 1 << B1shift;
        static constexpr int B1mask = B1 - 1;

        struct Block
        {
            char m_block[B][B];
        };

        std::unique_ptr<Block> m_data[B1][B1];

        [[nodiscard]] char read(int x, int y) const
        {
            auto& block = m_data[(x >> Bshift) & B1mask][(y >> Bshift) & B1mask];
            if (!block) return 0;
            return block->m_block[x & Bmask][y & Bmask];
        }

        void write(int x, int y, char value)
        {
            auto& block = m_data[(x >> Bshift) & B1mask][(y >> Bshift) & B1mask];
            if (!block)
            {
                block = std::make_unique<Block>();
            }
            block->m_block[x & Bmask][y & Bmask] = value;
        }

        template <typename Func>
        void foreach(const Func &func)
        {
            for (int x1 = 0; x1 < B1; ++x1)
            {
                for (int y1 = 0; y1 < B1; ++y1)
                {
                    auto const& block = m_data[x1 & B1mask][y1 & B1mask];
                    if (!block) continue;

                    int xb = x1 << B1shift;
                    int yb = y1 << B1shift;
                    for (int dx = 0; dx < B; ++dx)
                    {
                        for (int dy = 0; dy < B; ++dy)
                        {
                            func(xb | dx, yb | dy, block->m_block[dx][dy]);
                        }
                    }
                }
            }
        }
    };
}






































