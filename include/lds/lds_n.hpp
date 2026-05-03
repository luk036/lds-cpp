#pragma once

#include <array>
#include <cstddef>
#include <memory>
#include <vector>

#include <lds/lds.hpp>

namespace lds {

    class VdCorputBase {
      public:
        virtual ~VdCorputBase() = default;
        virtual auto pop() -> double = 0;
        virtual auto peek() -> double = 0;
        virtual auto skip(unsigned long n) -> void = 0;
        virtual auto reseed(const unsigned long& seed) -> void = 0;
        virtual auto get_index() const -> unsigned long = 0;
    };

    template <unsigned long Base>
    class VdCorputWrap : public VdCorputBase {
        VdCorput<Base> vdc;

      public:
        constexpr VdCorputWrap() : vdc() {}
        constexpr auto pop() -> double override { return this->vdc.pop(); }
        constexpr auto peek() -> double override { return this->vdc.peek(); }
        constexpr auto skip(unsigned long n) -> void override { this->vdc.skip(n); }
        constexpr auto reseed(const unsigned long& seed) -> void override { this->vdc.reseed(seed); }
        [[nodiscard]] constexpr auto get_index() const -> unsigned long override { return this->vdc.get_index(); }
    };

    class VdCorputDynamic : public VdCorputBase {
        unsigned long base_;
        unsigned long count = 0;

      public:
        explicit VdCorputDynamic(unsigned long base) : base_(base) {}

        static auto vdc(unsigned long cnt, unsigned long base) -> double {
            auto reslt = 0.0;
            auto denom = 1.0;
            auto count = cnt;
            while (count != 0) {
                const auto remainder = count % base;
                count /= base;
                denom *= static_cast<double>(base);
                reslt += static_cast<double>(remainder) / denom;
            }
            return reslt;
        }

        auto pop() -> double override {
            ++this->count;
            return vdc(this->count - 1, this->base_);
        }

        auto peek() -> double override {
            return vdc(this->count, this->base_);
        }

        auto skip(unsigned long n) -> void override { this->count += n; }

        auto reseed(const unsigned long& seed) -> void override { this->count = seed; }

        auto get_index() const -> unsigned long override { return this->count; }
    };

    template <std::size_t N>
    class HaltonN {
        std::array<std::unique_ptr<VdCorputBase>, N> vdcs;

      public:
        constexpr HaltonN(const std::array<unsigned long, N>& bases) : vdcs() {
            for (std::size_t i = 0; i < N; ++i) {
                this->vdcs[i] = create_vdc(bases[i]);
            }
        }

        constexpr auto pop() -> std::array<double, N> {
            std::array<double, N> result;
            for (std::size_t i = 0; i < N; ++i) {
                result[i] = this->vdcs[i]->pop();
            }
            return result;
        }

        [[nodiscard]] constexpr auto peek() -> std::array<double, N> {
            std::array<double, N> result;
            for (std::size_t i = 0; i < N; ++i) {
                result[i] = this->vdcs[i]->peek();
            }
            return result;
        }

        [[nodiscard]] constexpr auto batch(unsigned long n) -> std::vector<std::array<double, N>> {
            std::vector<std::array<double, N>> result;
            result.reserve(n);
            for (unsigned long i = 0; i < n; ++i) {
                result.emplace_back(this->pop());
            }
            return result;
        }

        constexpr auto skip(unsigned long n) -> void {
            for (auto& vdc : this->vdcs) {
                vdc->skip(n);
            }
        }

        constexpr auto reseed(const unsigned long& seed) -> void {
            for (auto& vdc : this->vdcs) {
                vdc->reseed(seed);
            }
        }

        [[nodiscard]] constexpr auto get_index() const -> unsigned long {
            return this->vdcs[0]->get_index();
        }

      private:
        static auto create_vdc(unsigned long base) -> std::unique_ptr<VdCorputBase> {
            switch (base) {
                case 2:
                    return std::make_unique<VdCorputWrap<2>>();
                case 3:
                    return std::make_unique<VdCorputWrap<3>>();
                case 5:
                    return std::make_unique<VdCorputWrap<5>>();
                case 7:
                    return std::make_unique<VdCorputWrap<7>>();
                case 11:
                    return std::make_unique<VdCorputWrap<11>>();
                case 13:
                    return std::make_unique<VdCorputWrap<13>>();
                case 17:
                    return std::make_unique<VdCorputWrap<17>>();
                case 19:
                    return std::make_unique<VdCorputWrap<19>>();
                case 23:
                    return std::make_unique<VdCorputWrap<23>>();
                case 29:
                    return std::make_unique<VdCorputWrap<29>>();
                case 31:
                    return std::make_unique<VdCorputWrap<31>>();
                default:
                    return std::make_unique<VdCorputDynamic>(base);
            }
        }
    };

}  // namespace lds