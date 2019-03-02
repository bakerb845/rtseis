#ifndef RTSEIS_UTILS_MATH_CONVOLVE_HPP
#define RTSEIS_UTILS_MATH_CONVOLVE_HPP 1
#include <complex>
#include <vector>

namespace RTSeis
{
namespace Utilities
{
namespace Math 
{
/*!
 * @defgroup rtseis_utils_math_convolve Convolve
 * @brief Utility functions for convolution and correlation.
 * @copyright Ben Baker distributed under the MIT license.
 * @ingroup rtseis_utils_math
 */
namespace Convolve
{
    /*!
     * @brief Defines the algorithm used to implement the convolution or
     *        correlation.  In general, convolution or correlation can be
     *        implemented in the time or frequency domain.  This allows the
     *        user to choose between circumstances when one domain leads 
     *        to a more efficient implementation than the other.
     * @ingroup rtseis_utils_math_convolve
     */
    enum Implementation
    {
        AUTO = 0,   /*!< Let IPP decide. */
        DIRECT = 1, /*!< Time domain implementation. */
        FFT = 2     /*!< FFT implementation. */
    };
    /*!
     * @brief This defines the nature of the convolution or correlation
     *        and the consequence with respect to edge effects.
     * @ingroup rtseis_utils_math_convolve
     */
    enum Mode
    {
        FULL = 0,   /*!< A full discrete convolution or correlation of
                         inputs which will have length \f$ m + n - 1 \f$.
                         Because the signals do not overlap completely
                         at the convolution edges boundary effects can be
                         seen. */
        VALID = 1,  /*!< The output consists only of those elements that
                         do not rely on zero-padding.  The return 
                         convolution or correlation will have length
                         \f$ \max(m,n) - \min(m,n) + 1 \f$.  This will
                         only be computed where the input signals completely
                         overlap so that there will not be edge effects. */
        SAME  = 2,  /*!< The output is the same size as the first input
                         and centered with respect to the FULL output.
                         The resulting convolution or correlation will
                         have length \f$ \max(m, n) \f$. */
    };

    /*!
     * @brief Computes the convolution \f$ c[k] = \sum_n a[n] b[n-k] \f$.
     * @param[in] a        First array in convolution.  This has length [m].
     * @param[in] b        Second array in convolution.  This has length [n].
     * @param[out] c       The resulting convolution.
     * @param[in] mode     Defines the convolution output.
     * @param[in] implementation  Defines the implementation type.
     * @result 0 indicates success.
     * @ingroup rtseis_utils_math_convolve
     */
    int convolve(const std::vector<double> &a,
                 const std::vector<double> &b,
                 std::vector<double> &c,
                 const Mode mode = Mode::FULL,
                 const Implementation implementation = Implementation::AUTO);
    /*!
     * @brief Computes the correlation \f$ c[k] = \sum_n a[n] b[n+k] \f$.
     * @param[in] a     First array in correlation.  This has length [m].
     * @param[in] b     Second array in correlation.  This has length [n].
     * @param[out] c    The resulting correlation.
     * @param[in] mode  Defines the correlation output.
     * @param[in] implementation  Defines the implementation type.
     * @result 0 indicates success.
     * @ingroup rtseis_utils_math_convolve
     */
    int correlate(const std::vector<double> &a,
                  const std::vector<double> &b,
                  std::vector<double> &c, 
                  const Mode mode = Mode::FULL,
                  const Implementation implementation = Implementation::AUTO);
    /*!
     * @brief Computes the autocorrelation \f$ c[k] = \sum_n a[n] a[n+k] \f$.
     * @param[in] a     Array to autocorrelation.  This has length [m].
     * @param[out] c    The resulting autocorrelation.
     * @param[in] mode  Defines the autocorrelation output.
     * @param[in] implementation  Defines the implementation type.
     * @result 0 indicates success.
     * @ingroup rtseis_utils_math_convolve
     */
    int autocorrelate(const std::vector<double> &a,
                      std::vector<double> &c, 
                      const Mode mode = Convolve::Mode::FULL,
                      const Implementation implementation = Implementation::AUTO);
}; /* End Convolve. */
}; /* End Math. */
}; /* End Utils */
}; /* End RTSeis */
#endif