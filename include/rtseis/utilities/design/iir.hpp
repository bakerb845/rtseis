#ifndef RTSEIS_UTILS_DESIGN_IIR_HPP
#define RTSEIS_UTILS_DESIGN_IIR_HPP 1
#include "rtseis/utilities/design/enums.hpp"

namespace RTSeis
{
namespace Utilities
{
namespace FilterRepresentations
{
class BA;
class SOS;
class ZPK;
};
namespace FilterDesign
{

namespace IIR
{

/*!
 * @brief Convenience function to design a digital or analog filter from
 *        an analog prototype.
 * @param[in] n        Order of filter to design.
 * @param[in] W        A scalar or length-2 array defining the critical
 *                     frequencies.  For digital filters, these are
 *                     normalized frequencies in the range [0,1] where 1
 *                     is the Nyquist frequency in pi radians/sample
 *                     (thereby making Wn half-cycles per sample.)   For
 *                     analog filters, Wn is the angular frequency in rad/s.
 * @param[in] rp       For Chebyshev I filters this specifies the maximum
 *                     ripple in the passband in decibels and must be positive.
 * @param[in] rs       For Chebyshev II filters this specifies the minimum
 *                     attenutation in the stop band in decibels and must
 *                     be positive.
 * @param[in] btype    The type of filter, e.g., lowpass, highpass,
 *                     bandpass, or bandstop.
 * @param[in] ftype    The IIR filter protoytpe, e.g., Butterworth, Bessel,
 *                     Chebyshev1, or Chebyshev2.
 * @param[in] ldigital  Identifies the filter as a digital or analog filter.
 * @result The IIR filter corresponding to the given design represented
 *         as the numerator and denominator coefficients of a real-valued
 *         transfer function.
 * @throws std::invalid_argument if any of the arguments are invalid.
 * @ingroup rtseis_utils_design_iir
 */
FilterRepresentations::BA
designBAIIRFilter(const int n, const double *W,
                  const double rp, const double rs,
                  const Bandtype btype,
                  const IIRPrototype ftype,
                  IIRFilterDomain ldigital = IIRFilterDomain::DIGITAL);
/*!
 * @brief Convenience function to design a digital or analog filter from
 *        an analog prototype.
 * @param[in] n        Order of filter to design.
 * @param[in] W        A scalar or length-2 array defining the critical
 *                     frequencies.  For digital filters, these are
 *                     normalized frequencies in the range [0,1] where 1
 *                     is the Nyquist frequency in pi radians/sample
 *                     (thereby making Wn half-cycles per sample.)   For
 *                     analog filters, Wn is the angular frequency in rad/s.
 * @param[in] rp       For Chebyshev I filters this specifies the maximum
 *                     ripple in the passband in decibels and must be positive.
 * @param[in] rs       For Chebyshev II filters this specifies the minimum
 *                     attenutation in the stop band in decibels and must
 *                     be positive.
 * @param[in] btype    The type of filter, e.g., lowpass, highpass,
 *                     bandpass, or bandstop.
 * @param[in] ftype    The IIR filter protoytpe, e.g., Butterworth, Bessel,
 *                     Chebyshev1, or Chebyshev2.
 * @param[in] ldigital  Identifies the filter as a digital or analog filter.
 * @result The IIR filter corresponding to the given design represented
 *         as zeros, poles, and a gain.
 * @throws std::invalid_argument if any of the arguments are invalid.
 * @ingroup rtseis_utils_design_iir
 */
FilterRepresentations::ZPK
designZPKIIRFilter(const int n, const double *W,
                   const double rp, const double rs,
                   const Bandtype btype,
                   const IIRPrototype ftype,
                   const IIRFilterDomain ldigital = IIRFilterDomain::DIGITAL);
/*!
 * @brief Convenience function to design a digital or analog filter from
 *        an analog prototype.
 * @param[in] n        Order of filter to design.
 * @param[in] W        A scalar or length-2 array defining the critical
 *                     frequencies.  For digital filters, these are
 *                     normalized frequencies in the range [0,1] where 1
 *                     is the Nyquist frequency in pi radians/sample
 *                     (thereby making Wn half-cycles per sample.)   For
 *                     analog filters, Wn is the angular frequency in rad/s.
 * @param[in] rp       For Chebyshev I filters this specifies the maximum
 *                     ripple in the passband in decibels and must be positive.
 * @param[in] rs       For Chebyshev II filters this specifies the minimum
 *                     attenutation in the stop band in decibels and must
 *                     be positive.
 * @param[in] btype    The type of filter, e.g., lowpass, highpass,
 *                     bandpass, or bandstop.
 * @param[in] ftype    The IIR filter protoytpe, e.g., Butterworth, Bessel,
 *                     Chebyshev1, or Chebyshev2.
 * @param[in] ldigital Identifies the filter as a digital or analog filter.
 * @param[in] pairing  Defines the pairing strategy.
 * @result The IIR filter corresponding to the given design represented
 *         as a series of cascaded second order sections.
 * @throws std::invalid_argument if any of the arguments are invalid.
 * @ingroup rtseis_utils_design_iir
 */
FilterRepresentations::SOS
designSOSIIRFilter(const int n, const double *W,
                   const double rp, const double rs,
                   const Bandtype btype,
                   const IIRPrototype ftype,
                   const IIRFilterDomain ldigital = IIRFilterDomain::DIGITAL,
                   const SOSPairing pairing = SOSPairing::NEAREST);
/*!
 * @brief Convert a filter specified as zeros, poles, and a gain to
 *        a filter consisting of cascaded second order sections.
 * @param[in] zpk      ZPK filter to convert to second order sections.
 * @param[in] pairing  The pairing strategy.
 * @result The corresponding IIR filter stored as a cascade of second order
 *         sections.
 * @throws std::invalid_argument if the number of poles in zpk is not
 *         equal to the number of zeros or there are no poles or zeros
 *         zpk.
 * @ingroup rtseis_utils_design_iir
 */
FilterRepresentations::SOS
zpk2sos(const FilterRepresentations::ZPK &zpk,
        const SOSPairing pairing = SOSPairing::NEAREST);
/*!
 * @brief Computes the pole-zero representation from a polynomial
 *        transfer function.
 * @param[in] ba    The transfer function to convert to zeros, poles, and
 *                  gain.
 * @result The IIR filter stored represented in terms of zeros, poles, and
 *         a gain.
 * @throws std::invalid_argument if there are no numerator or denominator
 *         coefficients or the first numerator or denominator coefficient
 *         is 0.
 * @ingroup rtseis_utils_design_iir
 */
FilterRepresentations::ZPK
tf2zpk(const FilterRepresentations::BA &ba);
/*! 
 * @brief Computes the polynomial transfer function from a pole-zero
 *        representation.
 * @param[in] zpk  Input transfer function specified in terms of zeros,
 *                 poles, and gain.
 * @result The corresponding IIR filter represented as real-valued
 *         numerator and denominator coefficients.
 * @ingroup rtseis_utils_design_iir
 */
FilterRepresentations::BA
zpk2tf(const FilterRepresentations::ZPK &zpk) noexcept;
/*!
 * @brief Converts a lowpass filter prototype to a different cutoff
 *        frequency.
 * @param[in] zpkIn  Input lowpass filter prototype to convert.
 * @param[in] w0     Desired cutoff frequency (rad/s).  This must be >= 0.
 * @result The corresponding lowpass filter.
 * @throws std::invalid_argument if zpkIn is empty or w0 is invalid.
 * @ingroup rtseis_utils_design_iir
 */
FilterRepresentations::ZPK
zpklp2lp(const FilterRepresentations::ZPK &zpkIn, const double w0);
/*!
 * @brief Converts a lowpass filter prototype to a highpss filter.
 * @param[in] zpkIn    Input lowpass filter prototype to convert.
 * @param[in] w0       Desired cutoff frequency (rad/s).  This must be >= 0.
 * @result The corresponding highpass filter.
 * @throws std::invalid_argument if zpkIn is empty or w0 is invalid.
 * @ingroup rtseis_utils_design_iir
 */
FilterRepresentations::ZPK
zpklp2hp(const FilterRepresentations::ZPK &zpkIn, const double w0);
/*!
 * @brief Transforms a lowpass filter prototype to a bandpass filter.
 *        The passband width is defined by [w0,w1] = [w0,w0+bw] in rad/s.
 * @param[in] zpkIn  Input lowpass filter prototype to convert.
 * @param[in] w0     Desired cutoff frequency in rad/s.  This must be >= 0.
 * @param[in] bw     The desired passband width in rad/s.  This must be
 *                   positive.
 * @result The corresponding bandpass filter.
 * @throws std::invalid_argument if w0 or bw is invalid or zpkIn is empty.
 * @ingroup rtseis_utils_design_iir
 */
FilterRepresentations::ZPK
zpklp2bp(const FilterRepresentations::ZPK &zpkIn,
         const double w0, const double bw);
/*!
 * @brief Transforms a lowpass filter prototype to a bandstop filter.
 *        The stopband width is defined by [w0,w1] = [w0,w0+bw] in rad/s.
 * @param[in] zpkIn  Input lowpass filter prototype to convert.
 * @param[in] w0     Desired cutoff frequency in rad/s.  This must be >= 0.
 * @param[in] bw     The desired stopband width in rad/s.  This must be >= 0.
 * @result The corresponding bandstop filter.
 * @throws std::invalid_argument if w0 or bw is invalid or zpkIn is empty.
 * @ingroup rtseis_utils_design_iir
 */
FilterRepresentations::ZPK
zpklp2bs(const FilterRepresentations::ZPK &zpkIn,
         const double w0, const double bw);
/*!
 * @brief Converts an analog filter to a digital filter using the bilinear
 *        transform.  This works by transforming a set of poles and zeros
 *        from the s-plane to the digital z-plane using Tustin's method,
 *        which substitutes \f$ s = \frac{z-1}{z+1} \f$ thereby maintaining
 *        the shape of the frequency response.
 *
 * @param[in] zpk  The analog zeros, poles, and gain to transform to the
 *                 z-plane.
 * @param[in] fs   The sampling rate in Hz.  Note, that pre-warping is
 *                 performed in this function.
 * @result The bilinear-transformed variant of zpk.
 * @throws std::invalid_argument if the number of zeros exceeds the number of
 *         poles in zpk.
 * @ingroup rtseis_utils_design_iir
 */
FilterRepresentations::ZPK
zpkbilinear(const FilterRepresentations::ZPK zpk, const double fs);

}; /* End IIR */

}; /* End FilterDesign */
}; /* End Utils */

}; /* End RTseis */

#endif
