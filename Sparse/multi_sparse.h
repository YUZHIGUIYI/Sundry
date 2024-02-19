//
// Created by ZZK on 2024/2/19.
//

#pragma once

#include <unordered_map>
#include <memory>
#include <tuple>
#include <array>
#include <map>

namespace experimental
{
    template <int Bshift, typename Node>
    struct DenseBlock
    {
    public:
        static constexpr bool is_place = false;
        static constexpr int num_depth = 0;

        static constexpr int bit_shift = Bshift;
        static constexpr int B = 1 << Bshift;
        static constexpr int Bmask = B - 1;

    private:
        Node m_data[B][B];

    public:
        Node* fetch(int x, int y)
        {
            return &m_data[x & Bmask][y & Bmask];
        }

        Node* touch(int x, int y)
        {
            return &m_data[x & Bmask][y & Bmask];
        }

        template <typename Func>
        void foreach(const Func &func)
        {
            for (int x = 0; x < B; ++x)
            {
                for (int y = 0; y < B; ++y)
                {
                    func(x, y, &m_data[x][y]);
                }
            }
        }
    };

    template <int Bshift, typename Node>
    struct PointerBlock
    {
    public:
        static constexpr bool is_place = false;
        static constexpr int num_depth = Node::num_depth + 1;

        static constexpr int bit_shift = Bshift;
        static constexpr int B = 1 << Bshift;
        static constexpr int Bmask = B - 1;

    private:
        std::unique_ptr<Node> m_data[B][B];

    public:
        Node* fetch(int x, int y)
        {
            return m_data[x & Bmask][y & Bmask].get();
        }

        Node* touch(int x, int y)
        {
            auto& block = m_data[x & Bmask][y & Bmask];
            if (!block)
            {
                block = std::make_unique<Node>();
            }
            return block.get();
        }

        template <typename Func>
        void foreach(const Func &func)
        {
            for (int x = 0; x < B; ++x)
            {
                for (int y = 0; y < B; ++y)
                {
                    auto ptr = m_data[x][y].get();
                    if (ptr)
                    {
                        func(x, y, ptr);
                    }
                }
            }
        }
    };

    template <typename Node>
    struct HashBlock
    {
    public:
        static constexpr bool is_place = false;
        static constexpr int bit_shift = 0;
        static constexpr int num_depth = Node::num_depth + 1;

        struct MyHash
        {
            size_t operator()(const std::tuple<int, int> &key) const
            {
                auto const &[x, y] = key;
                return (x * 2718281828) ^ (y * 3141592653);
            }
        };

    private:
        std::unordered_map<std::tuple<int, int>, Node, MyHash> m_data;

    public:
        Node* fetch(int x, int y)
        {
            auto key = std::make_tuple(x, y);
            if (m_data.contains(key))
            {
                return &m_data[key];
            }
            return nullptr;
        }

        Node* touch(int x, int y)
        {
            auto key = std::make_tuple(x, y);
            if (!m_data.contains(key))
            {
                return &m_data.try_emplace(key).first->second;
            }
            return &m_data[key];
        }

        template <typename Func>
        void foreach(const Func &func)
        {
            for (auto& [key, block] : m_data)
            {
                auto& [x, y] = key;
                func(x, y, &block);
            }
        }
    };

    template <typename T>
    struct PlaceData
    {
    public:
        static constexpr bool is_place = true;

    private:
        T m_value;

    public:
        T read()
        {
            return m_value;
        }

        void write(T value)
        {
            m_value = value;
        }

        template <typename Func>
        void visit(const Func &func)
        {
            func(m_value);
        }
    };

    template <typename T, typename Layout>
    struct RootGrid
    {
        Layout m_root;

        template <typename Node>
        static T _read(Node &node, int x, int y)
        {
            if constexpr (Node::is_place)
            {
                return node.read();
            } else
            {
                auto *child = node.fetch(x >> Node::bit_shift, y >> Node::bit_shift);
                if (!child)
                {
                    return T{};
                }
                return _read(*child, x, y);
            }
        }

        T read(int x, int y)
        {
            return _read(m_root, x, y);
        }

        struct WriteAccessor
        {
            Layout& m_root;
            mutable std::array<std::map<std::tuple<int, int>, void *>, Layout::num_depth> cached_array;

            template <int CurrDepth, typename Node>
            void _write(Node &node, int x, int y, T value) const
            {
                if constexpr (Node::is_place)
                {
                    node.write(value);
                } else
                {
                    auto child = [&] {
                        if constexpr (CurrDepth < Layout::num_depth)
                        {
                            auto& cache = cached_array[CurrDepth];
                            auto it = cache.find({ x >> Node::bit_shift, y >> Node::bit_shift });
                            if (it != cache.end())
                            {
                                return decltype(node.touch(0, 0))(it->second);
                            }
                        }
                        auto* child = node.touch(x >> Node::bit_shift, y >> Node::bit_shift);
                        if constexpr (CurrDepth < Layout::num_depth)
                        {
                            auto& cache = cached_array[CurrDepth];
                            cache.try_emplace({ x >> Node::bit_shift, y >> Node::bit_shift }, child);
                        }
                        return child;
                    } ();
                    return _write<CurrDepth + 1>(*child, x, y, value);
                }
            }

            void write(int x, int y, T value) const
            {
                _write<0>(m_root, x, y, value);
            }
        };

        WriteAccessor write_access()
        {
            return { m_root };
        }

        template <typename Node>
        static void _write(Node &node, int x, int y, T value)
        {
            if constexpr (Node::is_place)
            {
                node.write(value);
            } else
            {
                auto* child = node.touch(x >> Node::bit_shift, y >> Node::bit_shift);
                _write(*child, x, y, value);
            }
        }

        void write(int x, int y, T value)
        {
            _write(m_root, x, y, value);
        }

        template <typename Node, typename Func>
        static void _foreach(Node &node, int x, int y, const Func &func)
        {
            if constexpr (Node::is_place)
            {
                node.visit([&] (T &value) {
                    func(x, y, value);
                });
            } else
            {
                return node.foreach([&] (int x, int y, auto *child) {
                    _foreach(*child, x, y, func);
                });
            }
        }

        template <typename Func>
        void foreach(const Func &func)
        {
            _foreach(m_root, 0, 0, func);
        }
    };
}






















