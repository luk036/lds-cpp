#pragma once

#include "low_discr_seq.hpp"
#include <memory> // import unqiue_ptr
#include <variant>

namespace lds
{

/** Generate Sphere-3 Halton sequence */
class sphere3
{
  private:
    vdcorput _vdc;
    sphere _sphere2;

  public:
    /**
     * @brief Construct a new sphere3 object
     *
     * @param base
     */
    constexpr explicit sphere3(gsl::span<const unsigned> base) noexcept
        : _vdc(base[0])
        , _sphere2(base.subspan(1, 2))
    {
    }

    /**
     * @brief
     *
     * @return std::vector<double>
     */
    auto operator()() -> std::vector<double>;

    constexpr auto reseed(unsigned seed) noexcept -> void
    {
        this->_vdc.reseed(seed);
        this->_sphere2.reseed(seed);
    }
};


/** Generate using cylindrical coordinate method */
class cylin_n
{
  private:
    vdcorput _vdc;
    std::variant<std::unique_ptr<cylin_n>, std::unique_ptr<circle>> _Cgen;

  public:
    /**
     * @brief Construct a new cylin n object
     *
     * @param n dimension
     * @param base sequence base
     */
    cylin_n(gsl::span<const unsigned> base);

    /**
     * @brief
     *
     * @return std::vector<double>
     */
    auto operator()() -> std::vector<double>;
};


/** Generate Sphere-3 Halton sequence */
class sphere_n
{
  private:
    vdcorput _vdc;
    unsigned long _n;
    std::variant<std::unique_ptr<sphere_n>, std::unique_ptr<sphere>> _Sgen;
    double _range_t;
    double _t0;

  public:
    /**
     * @brief Construct a new sphere n object
     *
     * @param n dimension
     * @param base sequence base
     */
    sphere_n(gsl::span<const unsigned> base);

    /**
     * @brief
     *
     * @return std::vector<double>
     */
    auto operator()() -> std::vector<double>;
};


} // namespace
