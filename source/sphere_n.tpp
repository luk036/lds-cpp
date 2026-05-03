#include "lds/sphere_n.hpp"

#include <cmath>
#include <cstdint>
#include <memory>
#include <mutex>
// #include <numbers>
#include <stdexcept>
#include <unordered_map>
#include <vector>

namespace lds {

    std::vector<double> linspace(double start, double stop, std::unsigned long num) {
        if (num == 1) {
            return {start};
        }

        std::vector<double> result;
        result.reserve(num);

        double step = (stop - start) / static_cast<double>(num - 1);
        for (std::unsigned long i = 0; i < num; ++i) {
            result.emplace_back(start + (static_cast<double>(i) * step));
        }

        return result;
    }

    double simple_interp(double x_value, std::span<const double> x_points,
                         std::span<const double> y_points) {
        if (x_points.empty() || y_points.empty() || x_points.size() != y_points.size()) {
            throw std::invalid_argument("x_points and y_points must be non-empty and same size");
        }

        if (x_value <= x_points[0]) {
            return y_points[0];
        }
        if (x_value >= x_points.back()) {
            return y_points.back();
        }

        for (std::unsigned long i = 0; i < x_points.size() - 1; ++i) {
            if (x_points[i] <= x_value && x_value <= x_points[i + 1]) {
                // Linear interpolation
                double t = (x_value - x_points[i]) / (x_points[i + 1] - x_points[i]);
                return y_points[i] + t * (y_points[i + 1] - y_points[i]);
            }
        }

        return y_points.back();  // fallback
    }

    // Precomputed tables (similar to Python version)
    namespace {
        const std::vector<double> X = linspace(0.0, PI, 300);

        std::vector<double> compute_neg_cosine() {
            std::vector<double> result;
            result.reserve(X.size());
            for (double x : X) {
                result.emplace_back(-std::cos(x));
            }
            return result;
        }

        std::vector<double> compute_sine() {
            std::vector<double> result;
            result.reserve(X.size());
            for (double x : X) {
                result.emplace_back(std::sin(x));
            }
            return result;
        }

        std::vector<double> compute_f2(const std::vector<double>& neg_cosine,
                                       const std::vector<double>& sine) {
            std::vector<double> result;
            result.reserve(X.size());
            for (std::unsigned long i = 0; i < X.size(); ++i) {
                result.emplace_back((X[i] + neg_cosine[i] * sine[i]) / 2.0);
            }
            return result;
        }

        const std::vector<double> NEG_COSINE = compute_neg_cosine();
        const std::vector<double> SINE = compute_sine();
        const std::vector<double> F2 = compute_f2(NEG_COSINE, SINE);
    }  // namespace

    // Recursive helper function for get_tp
    static std::vector<double> get_tp_recursive(int n) {
        if (n == 0) {
            return X;
        }
        if (n == 1) {
            return NEG_COSINE;
        }

        std::vector<double> tp_minus2 = get_tp_recursive(n - 2);
        std::vector<double> result;
        result.reserve(tp_minus2.size());

        for (std::unsigned long i = 0; i < tp_minus2.size(); ++i) {
            double value = ((n - 1) * tp_minus2[i] + NEG_COSINE[i] * std::pow(SINE[i], n - 1)) / n;
            result.emplace_back(value);
        }

        return result;
    }

    std::vector<double> get_tp(int n) {
        if (n < 0) {
            throw std::invalid_argument("n must be non-negative");
        }

        static std::unordered_map<int, std::vector<double>> tp_cache;
        static std::mutex tp_cache_mutex;

        std::lock_guard<std::mutex> lock(tp_cache_mutex);

        auto it = tp_cache.find(n);
        if (it != tp_cache.end()) {
            return it->second;
        }

        auto result = get_tp_recursive(n);
        tp_cache[n] = result;
        return result;
    }

    Sphere3::Sphere3(std::span<const std::uint64_t> base)
        : vdc_(base[0]), sphere2_(base[1], base[2]) {
        if (base.size() < 3) {
            throw std::invalid_argument("Sphere3 requires at least 3 bases");
        }
    }

    std::vector<double> Sphere3::pop() {
        std::lock_guard<std::mutex> lock(mutex_);
        double ti = HALF_PI * vdc_.pop();  // map to [t0, tm-1]
        double xi = simple_interp(ti, F2, X);
        double cosxi = std::cos(xi);
        double sinxi = std::sin(xi);

        auto sphere2_point = sphere2_.pop();
        std::vector<double> result;
        result.reserve(4);
        result.emplace_back(sinxi * sphere2_point[0]);
        result.emplace_back(sinxi * sphere2_point[1]);
        result.emplace_back(sinxi * sphere2_point[2]);
        result.emplace_back(cosxi);

        return result;
    }

    void Sphere3::reseed(std::uint64_t seed) {
        std::lock_guard<std::mutex> lock(mutex_);
        vdc_.reseed(seed);
        sphere2_.reseed(seed);
    }

    // SphereWrapper implementation
    SphereWrapper::SphereWrapper(std::span<const std::uint64_t> base) : sphere_(base[0], base[1]) {}

    std::vector<double> SphereWrapper::pop() {
        std::lock_guard<std::mutex> lock(mutex_);
        auto arr = sphere_.pop();
        std::vector<double> result(arr.begin(), arr.end());
        return result;
    }

    void SphereWrapper::reseed(std::uint64_t seed) {
        std::lock_guard<std::mutex> lock(mutex_);
        sphere_.reseed(seed);
    }

    SphereN::SphereN(std::span<const std::uint64_t> base)
        : vdc_(base[0]), n_(static_cast<int>(base.size()) - 1) {
        if (n_ < 2) {
            throw std::invalid_argument("SphereN requires at least 3 bases (n >= 2)");
        }

        if (n_ == 2) {
            // Create a SphereWrapper object
            std::vector<std::uint64_t> sphere_base = {base[1], base[2]};
            s_gen_ = std::make_unique<SphereWrapper>(sphere_base);
        } else {
            std::vector<std::uint64_t> sub_base(base.begin() + 1, base.end());
            s_gen_ = std::make_unique<SphereN>(sub_base);
        }

        auto tp = get_tp(n_);
        range_ = tp.back() - tp.front();
    }
    std::vector<double> SphereN::pop() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (n_ == 2) {
            double ti = HALF_PI * vdc_.pop();  // map to [t0, tm-1]
            double xi = simple_interp(ti, F2, X);
            double cosxi = std::cos(xi);
            double sinxi = std::sin(xi);

            auto sub_point = s_gen_->pop();
            std::vector<double> result;
            result.reserve(sub_point.size() + 1);

            for (double s : sub_point) {
                result.emplace_back(sinxi * s);
            }
            result.emplace_back(cosxi);

            return result;
        }

        double vd = vdc_.pop();
        auto tp = get_tp(n_);
        double ti = tp.front() + range_ * vd;  // map to [t0, tm-1]
        double xi = simple_interp(ti, tp, X);
        double sinphi = std::sin(xi);

        auto sub_point = s_gen_->pop();
        std::vector<double> result;
        result.reserve(sub_point.size() + 1);

        for (double s : sub_point) {
            result.emplace_back(s * sinphi);
        }
        result.emplace_back(std::cos(xi));

        return result;
    }

    void SphereN::reseed(std::uint64_t seed) {
        std::lock_guard<std::mutex> lock(mutex_);
        vdc_.reseed(seed);
        s_gen_->reseed(seed);
    }

}  // namespace lds