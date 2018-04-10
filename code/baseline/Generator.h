#pragma once
#include <memory>
#include <utility>

template<class T>
struct Generator {
    using element_type = T;

    template<class G>
    Generator(G &&g)
        : p(std::make_unique<Impl<G>>(std::forward<G>(g))) {
    }

    operator bool() const { return p->hasNext(); }
    auto next() -> T { return p->next(); }

    struct End {};
    static auto end() -> End { return {}; }

    auto begin() {
        struct Iterator {
            using iterator_category =
                std::input_iterator_tag;
            using difference_type = ptrdiff_t;
            using value_type = T;
            using reference = T const &;
            using pointer = T const *;

            Generator *gen{};
            T val{};

            auto operator*() const -> const T & {
                return val;
            }
            auto operator++() -> Iterator & {
                if (*gen)
                    val = gen->next();
                else
                    gen = nullptr;
                return *this;
            }
            bool operator==(End) const { return !gen; }
            bool operator!=(End) const { return gen; }
        };
        if (*this) return Iterator{this, next()};
        return Iterator{};
    }

private:
    struct Interface {
        virtual bool hasNext() const = 0;
        virtual T next() = 0;
    };
    template<class G>
    struct Impl : Interface {
        Impl(const G &g)
            : g(g) {}
        Impl(G &&g)
            : g(std::move(g)) {}

        bool hasNext() const override { return g; }
        T next() override { return g.next(); }

        G g;
    };

    std::unique_ptr<Interface> p;
};
