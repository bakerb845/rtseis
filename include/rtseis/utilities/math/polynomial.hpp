#ifndef RTSEIS_UTILS_MATH_POLYNOMIAL_HPP
#define RTSEIS_UTILS_MATH_POLYNOMIAL_HPP 1
#include <complex>
#include <vector>
namespace RTSeis::Utilities::Math::Polynomial
{
/// @name Polynomial Root Finding
/// @{
/// @brief Computes the roots of a polynomial:
///        \f[
///             q(x) = c_0 x^p + c_1 x^{p-1} + \cdots + c_{p+1}
///        \f]
///        where \f$ p \f$ is the polynomial order
///        and \f$ coeffs[0] = c_0 \f$.
/// @param[in] coeffs   The coefficients of the polynomial whose order 
///                     is defined above.  Note, the coeffs[0] cannot be
///                     0 and coeffs must have length at least 1.
/// @result The roots of the polynomial.  This has dimension [coeffs.size() - 1].
/// @throws std::invalid_argument if coeffs is invalid.
/// @ingroup rtseis_utils_math_polynomial
std::vector<std::complex<double>> roots(const std::vector<double> &coeffs);
/// @}

/// @name Polynomial Generation From Roots
/// @{
/// @brief Returns a polynomial whose roots are given by p.
/// @param[in] p    The polynomial roots.  
/// @result The polynomial coefficients corresponding to the
///         roots of the given polynomial.  This has dimension
///         [p.size()+1] and is ordered so that the last coefficient
///         is the constant term and the first coefficient scales
///        the highest order polynomial.
/// @ingroup rtseis_utils_math_polynomial
template<typename U>
std::vector<std::complex<U>>
poly(const std::vector<std::complex<U>> &p) noexcept;
/// @copydoc poly()
/// @ingroup rtseis_utils_math_polynomial
template<typename U>
std::vector<U> poly(const std::vector<U> &p) noexcept;
/// @}

/// @name Polynomial Evaluation
/// @{
/// @brief Evaluates the polynomial
///        \f[
///            p(x) = p_{n_{order}}
///                 + x p_{n_{order}-1}
///                 + \cdots
///                 + x^{n_{order}} p_0
///        \f]
///        at points \f$ x_j, j=1,2,...,n_x \f$.
/// @param[in] p    The polynomial coefficients ordered such that the
///                 highest order coefficient comes first.  This has
///                 dimension [order+1].  Note, p.size() must be positive.
/// @param[in] x    The points at which to evaluate the polynomial.  This
///                 has dimension [x.size()].
/// @result \f$ y = p(x) \f$ evaluated at each \f$ x_i \f$.  This has dimension
///         [x.size()].
/// @throws std::invalid_argument if p is empty.
/// @ingroup rtseis_utils_math_polynomial 
template<typename U>
std::vector<U> polyval(const std::vector<U> &p, const std::vector<U> &x);
/// @copydoc polyval()
/// @ingroup rtseis_utils_math_polynomial
template<typename U>
std::vector<std::complex<U>>
polyval(const std::vector<std::complex<U>> &p,
        const std::vector<std::complex<U>> &x);
/// @}
}
#endif
