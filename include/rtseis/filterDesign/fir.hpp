#ifndef RTSEIS_FILTERDESIGN_FIR_HPP
#define RTSEIS_FILTERDESIGN_FIR_HPP 1
#include <utility>
#include "rtseis/filterDesign/enums.hpp"
namespace RTSeis::FilterRepresentations
{
class FIR;
}
namespace RTSeis::FilterDesign::FIR
{
/// @brief Designs an FIR lowpass filter using the window method.
/// @param[in] order   Order of filter.  The number of taps is order + 1.
///                    This must be at least 4.
/// @param[in] r       The normalized cutoff frequency where 1 is
///                    the Nyquist.
/// @param[in] window  FIR window design.  The default is a Hamming window.
/// @result The lowpass filter corresponding to the design parameters.
/// @throws std::invalid_argument if any arguments are incorrect.
/// @ingroup rtseis_filterdesign_fir
[[nodiscard]] RTSeis::FilterRepresentations::FIR
FIR1Lowpass(int order, double r,
            FIRWindow window = FIRWindow::HAMMING);
/// @brief Designs an FIR highpass filter using the window method.
/// @param[in] order   Order of filter.  The number of taps is order + 1.
///                    This must be at least 4.
/// @param[in] r       The normalized cutoff frequency where 1 is
///                    the Nyquist.
/// @param[in] window  FIR window design.  The default is a Hamming window.
/// @result The highpass filter corresponding to the design parameters.
/// @throws std::invalid_argument if any arguments are incorrect.
/// @ingroup rtseis_filterdesign_fir
[[nodiscard]] RTSeis::FilterRepresentations::FIR
FIR1Highpass(int order, double r,
             FIRWindow window = FIRWindow::HAMMING);
/// @brief Designs an FIR bandpass filter using the window method.
/// @param[in] order   Order of filter.  The number of taps is order + 1.
///                    This must be at least 4.
/// @param[in] r       Normalized low cutoff frequency and high cutoff
///                    frequencies where 1 is the Nyquist.  Here,
///                    r.first is the low cutoff and r.second is the
///                    high cutoff.
/// @param[in] window  FIR window design.  The default is a Hamming window.
/// @result The bandpass filter corresponding to the design parameters.
/// @throws std::invalid_argument if any arguments are incorrect.
/// @ingroup rtseis_filterdesign_fir
[[nodiscard]] RTSeis::FilterRepresentations::FIR
FIR1Bandpass(int order, const std::pair<double,double> &r,
             FIRWindow window = FIRWindow::HAMMING);
/// @brief Designs an FIR bandstop (notch) filter using the window method.
/// @param[in] order   Order of filter.  The number of taps is order + 1.
///                    This must be at least 4.
/// @param[in] r       Normalized low cutoff frequency and high cutoff
///                    frequencies where 1 is the Nyquist.  Here,
///                    r.first is the low cutoff and r.second is the
///                    high cutoff.
/// @param[in] window  FIR window design.  The default is a Hamming window.
/// @result The bandstop filter corresponding to the design parameters.
/// @throws std::invalid_argument if any arguments are incorrect.
/// @ingroup rtseis_filterdesign_fir
[[nodiscard]] RTSeis::FilterRepresentations::FIR
FIR1Bandstop(int order, const std::pair<double,double> &r,
             FIRWindow window = FIRWindow::HAMMING);
/// @brief Designs an FIR Hilbert transform using a Kaiser window.
///@param[in] order  Order of the filter.  The number of taps is order + 1.
///                  If order is even then the real FIR filter will have one
///                  non-zero coefficient and every other imaginary coefficient
///                  will be non-zero.  This Type III filter is computationally
///                  advantageous.
///                  If order is odd then then neither the real nor imaginary
///                  FIR filters will be sparse.  However, the filter amplitude
///                  near the Nyquist frequency, \f$ \pi \f$, is is constant.
///                  This Type IV filter will yield a better approximation to
///                  the Hilbert transform.
/// @param[in] beta  The \f$ \beta \f$ used in the Kaiser window.
/// @result result.first() are the real coefficients of the FIR Hilbert 
///         transformer while result.second() are the imaginary coefficients
///         of the Hilbert transformer.  Both the real and imaginary filters
///         have order + 1 filter taps. 
/// @throws std::invalid_argument If order is negative or if \f$ \beta \f$
///         is too large.
/// @ingroup rtseis_filterdesign_fir
[[nodiscard]] 
std::pair<RTSeis::FilterRepresentations::FIR, RTSeis::FilterRepresentations::FIR>
HilbertTransformer(int order, double beta = 8);
}
#endif
