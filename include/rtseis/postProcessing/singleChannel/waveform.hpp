#ifndef RTSEIS_POSTPROCESSING_SC_WAVEFORM
#define RTSEIS_POSTPROCESSING_SC_WAVEFORM 1
#include <memory>
#include <vector>
#include <string>
#ifndef RTSEIS_POSTPROCESSING_SC_TAPER
#include "rtseis/postProcessing/singleChannel/taper.hpp"
#endif

// Forward declare filter representations
namespace RTSeis::Utilities::FilterRepresentations
{
class FIR;
class SOS;
class BA;
class ZPK;
} 


namespace RTSeis::PostProcessing::SingleChannel
{
/*!
 * @defgroup rtseis_postprocessing Post-Processing
 * @brief These are higher level algorithms that expedite post-processing
 *        of seismic data. 
 */
//namespace PostProcessing
//{
/*!
 * @defgroup rtseis_postprocessing_sc Single-Channel Processing
 * @brief This section contains single-channel post-processing algorithms.
 * @ingroup rtseis_postprocessing
 */
//namespace SingleChannel
//{

/*!
 * @brief Defines the IIR filter implementation.
 * @ingroup rtseis_postprocessing_sc
 */
enum class IIRFilterImplementation
{
    SOS,    /*!< Apply the filter as a cascade of second order sections.
                 This is numerically more stable than a direct form
                 implementation. */
    DIRECT  /*!< Direct form IIR implementation.  The design and filter
                 application is slightly faster than using second order
                 sections filtering. */
};
/*!
 * @brief Defines the analog prototype from which the IIR filters are designed.
 * @ingroup rtseis_postprocessing_sc
 */
enum class IIRPrototype
{
    BUTTERWORTH, /*!< Butterworth filter design. */
    BESSEL,      /*!< Bessel filter design. */
    CHEBYSHEV1,  /*!< Chebyshev I filter design. */
    CHEBYSHEV2   /*!< Chebyshev II filter design. */
};
/*!
 * @brief Defines the window used in the FIR filter design.
 * @ingroup rtseis_postprocessing_sc 
 */
enum class FIRWindow
{
    HAMMING,     /*!< Hamming window. */
    BARTLETT,    /*!< Bartlett (triangle) window. */
    HANN,        /*!< Hann window. */
    BLACKMAN_OPT /*!< Optimal Blackman window. */
}; 
/*!
 * @brief Defines the filter passband.
 * @ingroup rtseis_postprocessing_sc
 */
enum class Bandtype
{
    LOWPASS,  /*!< Lowpass filter. */
    HIGHPASS, /*!< Highpass filter. */
    BANDPASS, /*!< Bandpass filter. */
    BANDSTOP  /*!< Bandstop (notch) filter. */ 
};
/*!
 * @brief Defines the time series interpolation strategy.
 * @ingroup rtseis_postprocessing_sc
 */
enum class InterpolationMethod
{
    DFT,                     /*!< This resamples a signal in the Fourier domain.
                                  For upsampling, this amounts to zero-stuffing
                                  in the frequency domain.  */ 
    WEIGHTED_AVERAGE_SLOPES, /*!< This uses the weighted-average slopes method
                                  of Wiggins.  While designed for unevenly
                                  sampled data, this can be quite effective at
                                  resampling evenly spaced data.  This is the
                                  algorithm used in SAC.  */
};

/*!
 * @brief Defines the nature of the convolution or correlation
 *        and the consequence with respect to edge effects.
 * @ingroup rtseis_postprocessing_sc
 */
enum class ConvolutionMode
{
    FULL,   /*!< A full discrete convolution or correlation of
                 inputs which will have length \f$ m + n - 1 \f$.
                 Because the signals do not overlap completely
                 at the convolution edges boundary effects can be
                 seen. */
    VALID,  /*!< The output consists only of those elements that
                 do not rely on zero-padding.  The return 
                 convolution or correlation will have length
                 \f$ \max(m,n) - \min(m,n) + 1 \f$.  This will
                 only be computed where the input signals completely
                 overlap so that there will not be edge effects. */
    SAME,  /*!< The output is the same size as the first input
                 and centered with respect to the FULL output.
                 The resulting convolution or correlation will
                 have length \f$ \max(m, n) \f$. */
};
/*!
 * @brief Defines the convolution or correlation implementation.
 * @ingroup rtseis_postprocessing_sc
 */
enum class ConvolutionImplementation
{
    AUTO,   /*!< Let the implementation decide. */
    DIRECT, /*!< Time domain implementation. */
    FFT     /*!< Frequency domain implementation. */
};
/// @class Waveform Waveform "include/rtseis/processing/singleChannel/postProcessing.hpp"
/// @brief This class is to be used for single-channel post-processing
///        applications.
/// @copyright Ben Baker (University of Utah) distributed under the MIT license.
/// @ingroup rtseis_postprocessing_sc
template <class T = double>
class Waveform
{
public:
    /// @name Constructors
    /// @{
    /// @brief Default constructor.  The sampling period defaults to unity.
    Waveform(); //const double dt = 1);
    /// @brief Copy constructor.
    /// @param[in] waveform  The waveform from which to initialize this class.
    Waveform(const Waveform &waveform);
    /// @brief Move constructor.
    /// @param[in,out] waveform  The class from which  to initialize this
    ///                          class.  On exit, waveform's behavior is
    ///                          undefined.
    Waveform(Waveform &&waveform) noexcept;
    /// @brief Constructs a waveform from time series data.  The sampling
    ///        period will be unity.
    /// @param[in] x   Signal from which to construct time series.
    /// @throws std::invalid_argument if x is empty or the sampling period
    ///         is not positive.
    //explicit Waveform(const std::vector<double> &x); //, const double dt = 1);
    /// @brief Constructs a waveform from time series data.
    /// @param[in] dt   Sampling period in seconds.  This must be positive.
    /// @param[in] x    Signal from which to construct time series.
    /// @throws std::invalid_argument if the sampling period is not positive
    ///         or x is empty.
    //Waveform(const double dt, const std::vector<double> &x);
    /// @}

    /// @name Operators
    /// @{
    /// @brief Copy assignment operator.
    /// @param[in,out] waveform  The waveform to copy to this.
    /// @result A deep copy of the input waveform.
    Waveform& operator=(const Waveform &waveform);
    /// @brief Move assignment operator.
    /// @param[in,out] waveform  The waveform whose memory will be moved to
    ///                          this.  On exit, waveform's behavior is
    ///                          undefined.
    /// @result The memory from waveform moved to this. 
    Waveform& operator=(Waveform &&waveform) noexcept;
    /// @}

    /// @name Destructors
    /// @{
    /// @brief Default destructor.
    ~Waveform();
    /// @}

    /// @name Data
    /// @{
    /// @brief Sets a signal on waveform class.
    /// @param[in] x   The signal to set.
    /// @throws std::invalid_argument if x is empty.
    void setData(const std::vector<T> &x);
    /// @brief Sets a waveform on the module.
    /// @param[in] n   The number of points in the signal.
    /// @param[in] x   The signal to set.  This is an array of dimension [n].
    /// @throws std::invalid_argument if x is null or n is less than 1.
    void setData(size_t n, const T x[]);
    /// @brief Sets a pointer to the input data on the module.
    /// @param[in] n   The number of points in the signal.
    /// @param[in] x   The signal to set.  This is a unique pointer to an
    ///                array of dimension [n].  This class will own this
    ///                reference until it is released or the class goes out
    ///                of scope.
    /// @note This class will own x's reference until it is released
    ///       with \c releaseDataPointer().
    /// @sa \c releaseDataPointer()
    void setDataPointer(size_t n, const T *x);
    /// @brief Releases the data pointer back to the owner.
    /// @note This will reset the number of data points to 0. 
    /// @sa \c setDataPointer()
    void releaseDataPointer() noexcept;
    /// @result The processed waveform data.
    std::vector<T> getData() const;
    /// @brief Gets the prcoessed waveform data.
    /// @param[in] nwork  Max number of points allocated to y.
    /// @param[out] y     The output time series.  This has dimension [nwork]
    ///                   however only the first \c getOutputLength() samples
    ///                   are accessed.
    /// @throws std::invalid_argument if y is NULL or nwork is too small.
    void getData(size_t nwork, T *y[]) const;
    /// @result The length of the output signal, y.
    size_t getOutputLength() const;
    /// @}

    /*! @name Convolution and Correlation
     * @{
     */
    /*!
     * @brief Computes the convolution \f$ x \ast s \f$ where the
     *        convolution sum is defined by
     *        \f$ y[k] = \sum_n x[n] s[n-k] \f$.
     * @param[in] s     The signal to convolve with x.
     * @param[in] mode  Defines the convolution output.
     * @param[in] implementation  Defines the implementation type.
     * @throws std::invalid_argument if s is empty or there is no data.
     */
    void convolve(const std::vector<T> &s,
         const ConvolutionMode mode = ConvolutionMode::FULL,
         const ConvolutionImplementation implementation = ConvolutionImplementation::AUTO);
    /*! 
     * @brief Computes the correlation \f$ x \star s \f$ where the
     *        correlation sum is defined by
     *        \f$ y[k] = \sum_n x[n] s[n+k] \f$.
     * @param[in] s     The signal to correlate with x.
     * @param[in] mode  Defines the correlation output.
     * @param[in] implementation  Defines the implementation type.
     * @throws std::invalid_argument if s is empty or there is no data.
     */
    void correlate(const std::vector<T> &s,
         const ConvolutionMode mode = ConvolutionMode::FULL,
         const ConvolutionImplementation implementation = ConvolutionImplementation::AUTO);
    /*! 
     * @brief Computes the autocorrelation \f$ x \star x \f$.
     * @param[in] mode  Defines the correlation output.
     * @param[in] implementation  Defines the implementation type.
     * @throws std::invalid_argument if there is no data.
     */
    void autocorrelate(const ConvolutionMode mode = ConvolutionMode::FULL,
         const ConvolutionImplementation implementation = ConvolutionImplementation::AUTO);
    /*! @} */

    /*! @name Demeaning and Detrending
     * @{
     */
    /*!
     * @brief Removes the mean from the data.
     * @throws std::runtime_error if there is no data.
     *
     * @snippet testing/postProcessing/singleChannel.cpp ppSCDemeanExample
     */
    void demean();
    /*!
     * @brief Removes a best fitting line \f$ \hat{y} = a x + b \f$
     *        from the data by first computing \f$ a \f$ and \f$ b \f$
     *        then computing \f$ y - (a x + b) \f$.
     * @throws std::runtime_error if there is no data.
     *
     * @snippet testing/postProcessing/singleChannel.cpp ppSCDetrendExample
     */
    void detrend();
    /*! @} */

    /*! @name Finite Impulse Response Filtering
     * @{
     */
    /*!
     * @brief Applies the digital FIR filter to the time series.
     * @param[in] fir  The digital FIR filter to apply to the signal.
     * @param[in] lremovePhase  If true, then this attempts to remove the 
     *                          the phase distortion by applying the
     *                          filter both forwards and backwards.  This is
     *                          done because FIR filters are not required to
     *                          have linear phase responses (i.e., a constant
     *                          group delay).
     * @throws std::invalid_argument if the filter is invalid.
     * @note It is the responsibility of the user to ensure that the
     *       signal sampling rate and the sampling rate used in the digital
     *       filter design are compatible.
     */
    void firFilter(const Utilities::FilterRepresentations::FIR &fir,
                   bool lremovePhase=false);
    /*! 
     * @brief Lowpass filters a signal using an FIR filter.
     * @param[in] ntaps   The number of filter taps.  This must be at least 4.
     *                    Moreover, if the phase shift is to be removed then
     *                    ntaps must be odd.  If it is not odd then ntaps will
     *                    bet set to ntaps + 1.
     * @param[in] fc      The critical frequency in Hz.  This must be between
     *                    0 and and the Nyquist frequency.  The latter can be
     *                    obtained from \c getNyquistFrequency().
     * @param[in] window  The window using the FIR filter design. 
     * @param[in] lremovePhase  If true then the phase shift is to be removed.
     *                          The filters designed have linear group delays
     *                          hence, the first ntaps/2 data points will not
     *                          be saved to the output signal.
     * @throws std::invalid_argument if the number of taps or critical frequency
     *         is invalid.
     */
    void firLowpassFilter(int ntaps, double fc, 
                          const FIRWindow window,
                          bool lremovePhase=false);
    /*! 
     * @brief Highpass filters a signal using an FIR filter.
     * @param[in] ntaps   The number of filter taps.
     * @param[in] fc      The critical frequency in Hz.
     * @param[in] window  The window using the FIR filter design. 
     * @param[in] lremovePhase  If true then the phase shift is to be removed.
     * @throws std::invalid_argument if the number of taps or critical frequency
     *         is invalid.
     * @sa firLowpassFilter()
     */
    void firHighpassFilter(int ntaps, double fc,
                           const FIRWindow window,
                           bool lremovePhase=false);
    /*! 
     * @brief Bandpass filters a signal using an FIR filter.
     * @param[in] ntaps   The number of filter taps.
     * @param[in] fc      The critical frequencies in Hz.  fc.first is the low
     *                    corner and fc.second is the high corner.
     *                    Both critical frequencies must be greater than 0 and
     *                    less than the Nyquist frequency.  Additionally,
     *                    fc.second must be greater than fc.first.  
     * @param[in] window  The window using the FIR filter design. 
     * @param[in] lremovePhase  If true then the phase shift is to be removed.
     * @throws std::invalid_argument if the number of taps or critical frequency
     *         is invalid.
     * @sa firLowpassFilter()
     */
    void firBandpassFilter(int ntaps, const std::pair<double,double> fc,
                           const FIRWindow window,
                           const bool lremovePhase=false);
    /*! 
     * @brief Bandstop (notch) filters a signal using an FIR filter.
     * @param[in] ntaps   The number of filter taps.
     * @param[in] fc      The critical frequencies in Hz.  fc.first is the low
     *                    corner and fc.second is the high corner.
     *                    Both critical frequencies must be greater than 0 and
     *                    less than the Nyquist frequency.  Additionally,
     *                    fc.second must be greater than fc.first.  
     * @param[in] window  The window using the FIR filter design.
     * @param[in] lremovePhase  If true then the phase shift is to be removed.
     * @throws std::invalid_argument if the number of taps or critical frequency
     *         is invalid.
     * @sa firBandpassFilter()
     */
    void firBandstopFilter(int ntaps, const std::pair<double,double> fc,
                           const FIRWindow window,
                           bool lremovePhase=false);
    /*! @} */

    /*! @name Second Order Section (Biquadratic) Filtering
     * @{
     * @note SOS filter application can be numerically more robust than
     *       its direct form counterpart and should therefore be preferred
     *       whenever possible.
     */
    /*! 
     * @brief Applies the digital IIR filter represented as cascaded second
     *        order sections to the time series.
     * @param[in] sos  The digital IIR filter stored in second order sections.
     * @param[in] lzeroPhase  If true then the phase shift is removed by
     *                        filtering the signal in both directions.  This
     *                        effectively squares the magnitude of the filter
     *                        response while conveniently annihilating
     *                        the nonlinear phase response.
     * @throws std::invalid_argument if the filter is invalid.
     * @note It is the responsibility of the user to ensure that the
     *       signal sampling rate and the sampling rate used in the digital
     *       filter design are compatible.
     */
    void sosFilter(const Utilities::FilterRepresentations::SOS &sos,
                   bool lzeroPhase=false);
    /*! 
     * @brief Lowpass filters a signal using an IIR filter specified as a series
     *        of second order sections.
     * @param[in] order   The filter order which equals the number of npoles.
     *                    This must be positive.
     * @param[in] fc      The critical frequency in Hz.  This must be between
     *                    0 and and the Nyquist frequency.  The latter can be
     *                    obtained from \c getNyquistFrequency().
     * @param[in] prototype  The analog prototype from which to design the
     *                       the digital lowpass filter.
     * @param[in] ripple  For Chebyshev I filters this controls the maximum
     *                    attenuation in the passband and is measured in dB.
     *                    For Chebyshev II filters this controls the minimum
     *                    attenuation in the stopband and is measured in dB.
     *                    For both Chebyshev I and II filters this must be
     *                    positive.  For Butterworth and Bessel filters this
     *                    parameter is ignored.
     * @param[in] lzeroPhase  If true then the phase shift is removed by
     *                        filtering the signal in both directions.  This
     *                        effectively squares the magnitude of the filter
     *                        response while conveniently annihilating
     *                        the nonlinear phase response.
     * @throws std::invalid_argument if the order isn't positive or the ripple
     *         isn't positive for Chebyshev I or Chebyshev II filter design.
     *
     * @snippet testing/postProcessing/singleChannel.cpp ppSCSOSLowpassExample
     *
     */
    void sosLowpassFilter(int order, double fc, 
                          const IIRPrototype prototype,
                          const double ripple,
                          bool lzeroPhase=false);
    /*!
     * @brief Highpass filters a signal using an IIR filter.
     * @param[in] order   The filter order which equals the number of npoles.
     * @param[in] fc      The critical frequency in Hz.
     * @param[in] prototype  The analog prototype from which to design the
     *                       the digital lowpass filter.
     * @param[in] ripple  Controls the ripple size in Chebyshev I and 
     *                    Chebyshev II design.
     * @param[in] lzeroPhase  If true then the phase shift is removed.
     * @throws std::invalid_argument if the order isn't positive or the ripple
     *         isn't positive for Chebyshev I or Chebyshev II filter design.
     * @sa sosLowpassFilter()
     */
    void sosHighpassFilter(int order, double fc,
                           const IIRPrototype prototype,
                           const double ripple,
                           bool lzeroPhase=false);
    /*! 
     * @brief Bandpass filters a signal using an IIR filter.
     * @param[in] order   The filter order which equals the number of npoles.
     * @param[in] fc      The critical frequencies in Hz.  fc.first is the low
     *                    corner and fc.second is the high corner.
     *                    Both critical frequencies must be greater than 0 and
     *                    less than the Nyquist frequency.  Additionally,
     *                    fc.second must be greater than fc.first.
     * @param[in] prototype  The analog prototype from which to design the
     *                       the digital lowpass filter.
     * @param[in] ripple  Controls the ripple size in Chebyshev I and 
     *                    Chebyshev II design.
     * @param[in] lzeroPhase  If true then the phase shift is removed.
     * @throws std::invalid_argument if the order isn't positive or the ripple
     *         isn't positive for Chebyshev I or Chebyshev II filter design.
     * @sa sosLowpassFilter()
     */
    void sosBandpassFilter(int order, const std::pair<double,double> fc,
                           const IIRPrototype prototype,
                           double ripple,
                           bool lzeroPhase=false);
    /*! 
     * @brief Bandstop filters a signal using an IIR filter.
     * @param[in] order   The filter order which equals the number of npoles.
     * @param[in] fc      The critical frequencies in Hz.
     * @param[in] prototype  The analog prototype from which to design the
     *                       the digital lowpass filter.
     * @param[in] ripple  Controls the ripple size in Chebyshev I and 
     *                    Chebyshev II design.
     * @param[in] lzeroPhase  If true then the phase shift is removed.
     * @throws std::invalid_argument if the order isn't positive or the ripple
     *         isn't positive for Chebyshev I or Chebyshev II filter design.
     * @sa sosLowpassFilter()
     * @sa sosBandpassFilter()
     */
    void sosBandstopFilter(int order, const std::pair<double,double> fc,
                           const IIRPrototype prototype,
                           double ripple,
                           bool lzeroPhase=false);
    /*! @} */

    /*! @name Infinite Impulse Response Filtering
     * @{
     */
    /*! 
     * @brief Applies the digital IIR filter using a direct form implementation
     *        to the time series.
     * @param[in] ba   The digital IIR filter stored as feed-forward and
     *                 feed-back coefficients.
     * @param[in] lzeroPhase  If true, then this removes the phase distortion
     *                        by applying the filter to both the time forward
     *                        and time reversed signal.
     * @note For higher-order filters this can be less numerically stable than
     *        an SOS implementation.
     * @throws std::invalid_argument if the filter is invalid.
     */
    void iirFilter(const Utilities::FilterRepresentations::BA &ba,
                   bool lzeroPhase=false);
    /*!
     * @brief Lowpass filters a signal using an IIR direct form filter.
     * @param[in] order   The filter order which equals the number of npoles.
     *                    This must be positive.
     * @param[in] fc      The critical frequency in Hz.  This must be between
     *                    0 and and the Nyquist frequency.  The latter can be
     *                    obtained from \c getNyquistFrequency().
     * @param[in] prototype  The analog prototype from which to design the
     *                       the digital lowpass filter.
     * @param[in] ripple  Controls the ripple size in Chebyshev I and 
     *                    Chebyshev II design.
     * @param[in] lzeroPhase  If true then the phase shift is removed.
     * @throws std::invalid_argument if the order isn't positive or the ripple
     *         isn't positive for Chebyshev I or Chebyshev II filter design.
     * @sa sosLowpassFilter()
     */
    void iirLowpassFilter(int order, double fc,
                          const IIRPrototype prototype,
                          double ripple,
                          bool lzeroPhase=false);
    /*!
     * @brief Highpass filters a signal using an IIR filter.
     * @param[in] order   The filter order which equals the number of npoles.
     * @param[in] fc      The critical frequency in Hz.
     * @param[in] prototype  The analog prototype from which to design the
     *                       the digital lowpass filter.
     * @param[in] ripple  Controls the ripple size in Chebyshev I and 
     *                    Chebyshev II design.
     * @param[in] lzeroPhase  If true then the phase shift is removed.
     * @throws std::invalid_argument if the order isn't positive or the ripple
     *         isn't positive for Chebyshev I or Chebyshev II filter design.
     * @sa sosLowpassFilter()
     */
    void iirHighpassFilter(int order, double fc,
                           const IIRPrototype prototype,
                           double ripple,
                           bool lzeroPhase=false);
    /*! 
     * @brief Bandpass filters a signal using an IIR filter.
     * @param[in] order   The filter order which equals the number of npoles.
     * @param[in] fc      The critical frequencies in Hz.
     * @param[in] prototype  The analog prototype from which to design the
     *                       the digital lowpass filter.
     * @param[in] ripple  Controls the ripple size in Chebyshev I and 
     *                    Chebyshev II design.
     * @param[in] lzeroPhase  If true then the phase shift is removed.
     * @throws std::invalid_argument if the order isn't positive or the ripple
     *         isn't positive for Chebyshev I or Chebyshev II filter design.
     * @sa iirLowpassFilter()
     * @sa iirBandpassFilter()
     */
    void iirBandpassFilter(int order, const std::pair<double,double> fc,
                           const IIRPrototype prototype,
                           double ripple,
                           bool lzeroPhase=false);
    /*! 
     * @brief Bandstop filters a signal using an IIR filter.
     * @param[in] order   The filter order which equals the number of npoles.
     * @param[in] fc      The critical frequencies in Hz.
     * @param[in] prototype  The analog prototype from which to design the
     *                       the digital lowpass filter.
     * @param[in] ripple  Controls the ripple size in Chebyshev I and 
     *                    Chebyshev II design.
     * @param[in] lzeroPhase  If true then the phase shift is removed.
     * @throws std::invalid_argument if the order isn't positive or the ripple
     *         isn't positive for Chebyshev I or Chebyshev II filter design.
     * @sa sosLowpassFilter()
     * @sa sosBandpassFilter()
     */
    void iirBandstopFilter(int order, const std::pair<double,double> fc,
                           const IIRPrototype prototype,
                           double ripple,
                           bool lzeroPhase=false);
    /*! @} */

    /*! @name Normalization
     * @{
     */
    /*!
     * @brief Rescales the data from current data range to the target range.
     * @param[in] targetRange  The target range where targetRange.first is the
     *                         minimum of the desired range and 
     *                         targetRange.second is the maximum of the desired
     *                         range.
     * @throws std::runtime_error if there is less than 2 data points or 
     *         all samples in the signal are identical.
     */
    void normalizeMinMax(const std::pair<double, double> targetRange = std::make_pair<double, double> (0, 1));
    /*!
     * @brief Sets the data to the +1 or -1 depending on the sign of its
     *        floating point representation.
     * @note +0 and -0 are mapped to +1 and -1 respectively.
     */
    void normalizeSignBit();
    /*!
     * @brief Applies z-score normalization, i.e., standardizes with the 
     *        transform
     *        \f[
     *            y = \frac{x - \mu}{\sigma}
     *        \f]
     * @note If there is only one data point in the signal then that data
     *       point will be set to 0.
     */
    void normalizeZScore();
    /*! @} */

    /*! @name Tapering and Cutting
     * @{
     */
    /*!
     * @brief Tapers the ends of a signal.
     * @param[in] pct  The percentage of the signal to which the taper will
     *                 be applied.  For example, 5 percent indicates that
     *                 the first 2.5 and final 2.5 percent of the signal
     *                 will be tapered.  This must be in the range (0,100). 
     * @param[in] window  Defines the window function used to generate
     *                    the taper.
     * @note The SAC convention would require a fraction in the range (0,0.5).
     *       Hence, to convert from SAC to RTSeis one would do something like
     *       pct = 100*(2*fraction).
     * @throw std::invalid_argument if the parameters are invalid.
     */
    void taper(double pct = 5,
               const TaperParameters::Type window = TaperParameters::Type::HAMMING);
    /*! @} */

    /// @name Resampling
    /// @{
    /// @brief Downsamples a signal.
    /// @param[in] nq  The downsampling factor.  Every (nq - 1)'th sample will
    ///                be retained.  This must be positive.
    /// @throws std::invalid_argument if nq is negative.
    void downsample(int nq);
    /// @brief Decimates a signal.
    /// @param[in] nq  The downsampling factor.  Every (nq - 1)'th sample will
    ///                be retained.  This must be at least 2.  Moreover, for 
    ///                downsampling factors greater than 13 it may be better
    ///                to break this operation into decimation stages.
    /// @param[in] nfir   The length of the FIR filter.  Note, that if this even
    ///                   then it will be increased by one sample to better 
    ///                   correct the FIR filter's group delay.  This must at
    ///                   least length 5.
    /// @throws std::invalid_argument if any arguments are incorrect.
    /// @note This will design a Hamming window-based filter whose cutoff
    ///        frequency is 1/nq.
    void decimate(int nq, int nfir);
    /*!
     * @brief Resamples a signal using the Fourier transform.
     * @param[in] newSamplingPeriod  The desired sampling period.
     * @param[in] method             The interpolation method.
     * @throws std::invalid_argument if newSamplingPeriod is not positive or
     *         method is not known.
     * @note While upsampling is a fairly safe operation, the user must take
     *       care to lowpass the signal prior to calling this as a downsampler.
     */
    void interpolate(double newSamplingPeriod,
                     InterpolationMethod method = InterpolationMethod::DFT);
    /// @}

    /// @name Envelope 
    /// @{
    /// @brief Computes the envelope of the signal using an FIR Hilbert
    ///        transformer.
    /// @param[in] nfir  The number of FIR coefficients in the Hilbert
    ///                  transformer.  This must be positive.
    /// @throws std::invalid_argument if nfir is not positive.
    void firEnvelope(int nfir);
    /// @brief Computes the envelope of the signal by computing the absolute
    ///        value of the analytic signal.
    void envelope();
    /// @}

    /// @name Utilities
    /// @{
    /// @brief Sets the sampling period.
    /// @param[in] dt  The signal sampling period in seconds.
    ///                This must be positive.
    /// @throws std::invalid_argument if dt is not positive.
    void setSamplingPeriod(double dt);
    /// @result The signal sampling period in seconds.
    double getSamplingPeriod() const noexcept;
    /// @result The Nyquist freuqency in Hz.
    double getNyquistFrequency() const noexcept;
    /// @}
private:
    class WaveformImpl;
    std::unique_ptr<WaveformImpl> pImpl;
}; // end waveform
} // End RTSeis
#endif
