#pragma once

/** @file lds_n.hpp
 *  @brief N-dimensional Halton sequence generator with runtime polymorphism.
 */

#include <array>
#include <cstddef>
#include <lds/lds.hpp>
#include <memory>

namespace lds {

    /**
     * @brief Abstract base class for polymorphic van der Corput sequence generators.
     *
     * Provides a common interface for both compile-time (template) and
     * runtime-dispatch van der Corput generators used in N-dimensional sequences.
     *
     * @note Strategy/Abstract interface pattern: this abstract class defines the
     * runtime-polymorphic interface (pop, peek, skip, reseed, get_index) through
     * which interchangeable generator strategies are invoked. The caller holds a
     * VdCorputBase pointer and dispatches virtually, so the concrete strategy can
     * be swapped at runtime (e.g. compile-time VdCorputWrap vs runtime
     * VdCorputDynamic) without changing the client code.
     */
    class VdCorputBase {
      public:
        virtual ~VdCorputBase() = default;
        /** Ensure an explicit default constructor is available for derived classes */
        VdCorputBase() = default;

        // Non-copyable and non-movable: polymorphic base should not be
        // accidentally copied or moved (prevents slicing and ownership issues).
        VdCorputBase(const VdCorputBase&) = delete;
        VdCorputBase& operator=(const VdCorputBase&) = delete;
        VdCorputBase(VdCorputBase&&) = delete;
        VdCorputBase& operator=(VdCorputBase&&) = delete;

        /** @brief Generate the next value in the sequence. */
        virtual auto pop() -> double = 0;

        /** @brief Peek at the next value without advancing state. */
        virtual auto peek() -> double = 0;

        /** @brief Skip n values in the sequence. */
        virtual auto skip(unsigned long n) -> void = 0;

        /** @brief Reset the generator to a specific seed value. */
        virtual auto reseed(const unsigned long& seed) -> void = 0;

        /** @brief Get the current index in the sequence. */
        virtual auto get_index() const -> unsigned long = 0;
    };

    /**
     * @brief Compile-time-polymorphic wrapper around VdCorput<Base>.
     * @tparam Base The numeric base for the van der Corput sequence.
     *
     * @note Adapter pattern: this class adapts the compile-time-polymorphic
     * VdCorput<Base> template behind the runtime interface VdCorputBase, bridging
     * template-based and runtime dispatch. The `vdc` member holds the adaptee and
     * every VdCorputBase method is forwarded to it, translating the compile-time
     * template parameter Base into an object whose behavior is selected at runtime
     * via virtual dispatch.
     */
    template <unsigned long Base> class VdCorputWrap : public VdCorputBase {
        VdCorput<Base> vdc;

      public:
        /** @brief Construct a VdCorputWrap with precomputed reverse powers of Base. */
        constexpr VdCorputWrap() : vdc() {}
        constexpr auto pop() -> double override { return this->vdc.pop(); }
        constexpr auto peek() -> double override { return this->vdc.peek(); }
        constexpr auto skip(unsigned long n) -> void override { this->vdc.skip(n); }
        constexpr auto reseed(const unsigned long& seed) -> void override {
            this->vdc.reseed(seed);
        }
        [[nodiscard]] constexpr auto get_index() const -> unsigned long override {
            return this->vdc.get_index();
        }
    };

    /**
     * @brief Runtime-polymorphic van der Corput generator with dynamic base.
     *
     * Unlike VdCorputWrap (which requires a compile-time base), this class
     * accepts the base as a constructor argument, enabling fully dynamic dispatch.
     */
    class VdCorputDynamic : public VdCorputBase {
        unsigned long base_;
        unsigned long count = 0;
        std::array<double, 64> rev_lst_{};

        /**
         * @brief Internal implementation of the van der Corput computation.
         *
         * Reverse powers of the base are precomputed once in the constructor
         * for fast lookup during pop()/peek(). The array is fixed at 64 entries,
         * sufficient for double precision (53 mantissa bits). Delegates the
         * digit/weight summation to the shared core in lds::detail.
         *
         * @param[in] cnt The sequence index to compute.
         * @return The van der Corput value for index cnt.
         */
        auto pop_impl(unsigned long cnt) -> double {
            return detail::vdc_digit_sum<double>(cnt, this->base_, this->rev_lst_);
        }

      public:
        /**
         * @brief Construct a VdCorputDynamic with a given base.
         * @param[in] base The numeric base for the sequence.
         */
        explicit VdCorputDynamic(unsigned long base) : base_(base) {
            double reverse = 1.0;
            for (auto& v : rev_lst_) {
                reverse /= static_cast<double>(base_);
                v = reverse;
            }
        }

        auto pop() -> double override {
            ++this->count;
            return pop_impl(this->count);
        }

        auto peek() -> double override { return pop_impl(this->count + 1); }

        auto skip(unsigned long n) -> void override { this->count += n; }

        auto reseed(const unsigned long& seed) -> void override { this->count = seed; }

        auto get_index() const -> unsigned long override { return this->count; }
    };

    /**
     * @brief N-dimensional Halton sequence generator with runtime-polymorphic bases.
     *
     * Uses VdCorputBase pointers to support arbitrary prime bases at runtime,
     * dispatching to the optimal compile-time (VdCorputWrap) or runtime
     * (VdCorputDynamic) implementation depending on the base value.
     *
     * @tparam N Number of dimensions.
     */
    template <std::size_t N> class HaltonN {
        std::array<std::unique_ptr<VdCorputBase>, N> vdcs;

      public:
        /**
         * @brief Construct an N-dimensional Halton generator.
         * @param[in] bases Array of N base values (one per dimension).
         */
        constexpr HaltonN(const std::array<unsigned long, N>& bases) : vdcs() {
            for (std::size_t i = 0; i < N; ++i) {
                this->vdcs[i] = create_vdc(bases[i]);
            }
        }

        /**
         * @brief Generate the next N-dimensional Halton point.
         *
         * @f[
         *     H(n) = (\phi_{b_1}(n), \phi_{b_2}(n), \dots, \phi_{b_N}(n))
         * @f]
         *
         * @dot
         *   digraph halton_n_flow {
         *     rankdir=LR;
         *     bgcolor="transparent";
         *     node [shape=box, style=filled, fillcolor="#d4e6f1"];
         *     n [label="Index n", fillcolor="#a9cce3"];
         *     dim1 [label="VdC b_1", fillcolor="#d4e6f1"];
         *     dim2 [label="VdC b_2", fillcolor="#d4e6f1"];
         *     dimN [label="VdC b_N", fillcolor="#d4e6f1"];
         *     result [label="Halton N-D\npoint", fillcolor="#7fb3d8"];
         *     n -> dim1;
         *     n -> dim2;
         *     n -> dimN;
         *     dim1 -> result [label="phi_{b1}(n)"];
         *     dim2 -> result [label="phi_{b2}(n)"];
         *     dimN -> result [label="phi_{bN}(n)"];
         *   }
         * @enddot
         *
         * @return Array of N double values, one per dimension.
         */
        constexpr auto pop() -> std::array<double, N> {
            std::array<double, N> result;
            for (std::size_t i = 0; i < N; ++i) {
                result[i] = this->vdcs[i]->pop();
            }
            return result;
        }

        /**
         * @brief Peek at the next point without advancing state.
         * @return Array of N double values, one per dimension.
         */
        [[nodiscard]] constexpr auto peek() -> std::array<double, N> {
            std::array<double, N> result;
            for (std::size_t i = 0; i < N; ++i) {
                result[i] = this->vdcs[i]->peek();
            }
            return result;
        }

        /**
         * @brief Skip n values in the sequence.
         * @param[in] n Number of values to skip.
         */
        constexpr auto skip(unsigned long n) -> void {
            for (auto& vdc : this->vdcs) {
                vdc->skip(n);
            }
        }

        /**
         * @brief Reset all dimension generators to a specific seed.
         * @param[in] seed The seed value to reset to.
         */
        constexpr auto reseed(const unsigned long& seed) -> void {
            for (auto& vdc : this->vdcs) {
                vdc->reseed(seed);
            }
        }

        /**
         * @brief Get the current index from the first dimension generator.
         * @return Current sequence index.
         */
        [[nodiscard]] constexpr auto get_index() const -> unsigned long {
            return this->vdcs[0]->get_index();
        }

      private:
        using Creator = std::unique_ptr<VdCorputBase> (*)();

        /// @brief Factory registry entry: maps a numeric base to its compile-time creator.
        struct VdcFactory {
            unsigned long base;
            Creator create;
        };

        template <unsigned long Base> static auto make_wrap() -> std::unique_ptr<VdCorputBase> {
            return std::make_unique<VdCorputWrap<Base>>();
        }

        /// @brief Build the constexpr factory registry from a pack of compile-time bases.
        template <unsigned long... Bases> static constexpr auto factory_table()
            -> std::array<VdcFactory, sizeof...(Bases)> {
            return {VdcFactory{Bases, &make_wrap<Bases>}...};
        }

        /// Compile-time-specialized bases (first 11 primes); any other base is dynamic.
        static constexpr auto VDC_FACTORIES
            = factory_table<2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31>();

        /**
         * @brief Create a runtime-polymorphic van der Corput generator for a base.
         * @param[in] base The numeric base for the sequence.
         * @return A unique_ptr to the VdCorputBase implementation selected for base.
         *
         * @note Factory Method pattern: returns a VdCorputBase (unique_ptr) selecting
         * the implementation at runtime — compile-time VdCorputWrap<N> for small
         * prime bases, VdCorputDynamic otherwise. The registry maps each
         * compile-time-specialized base to its creator, hiding the construction
         * logic from the caller, so HaltonN clients receive an interchangeable
         * strategy without knowing which concrete class was created.
         */
        static auto create_vdc(unsigned long base) -> std::unique_ptr<VdCorputBase> {
            for (const auto& factory : VDC_FACTORIES) {
                if (factory.base == base) {
                    return factory.create();
                }
            }
            return std::make_unique<VdCorputDynamic>(base);
        }
    };

    // Compile-time contract checks: the runtime family also satisfies the protocol concept,
    // so template and polymorphic generators are interchangeable in generic code.
    static_assert(SequenceGenerator<VdCorputBase, double>);
    static_assert(SequenceGenerator<HaltonN<3>, std::array<double, 3>>);

}  // namespace lds