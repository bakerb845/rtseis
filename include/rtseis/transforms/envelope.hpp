#ifndef RTSEIS_TRANSFORMS_ENVELOPE_HPP
#define RTSEIS_TRANSFORMS_ENVELOPE_HPP 1
#include <memory>
namespace RTSeis::Transforms
{
/// @class Envelope envelope.hpp "include/rtseis/utilities/transforms/envelope.hpp"
/// @brief Computes the envelope of a signal using.  This is for post-processing
///        only and works by computing the Hilbert transform to obtain
///        the analytic signal.  Then, by taking the absolute value of the 
///        analytic signal, the envelope is obtained.
/// @ingroup rtseis_utils_transforms
/// @sa Hilbert
template<class T = double>
class Envelope
{
public:
    /// @name Constructors
    /// @{
    /// @brief Default constructor.
    Envelope(); 
    /// @brief Copy constructor.
    /// @param[in] envelope  Class from which to initialize.
    Envelope(const Envelope &envelope);
    /// @brief Move constructor.
    /// @param[in,out] envelope  Class from which this class is initialized.
    ///                          On exit envelope's behavior is undefined.
    Envelope(Envelope &&envelope) noexcept;
    /// @}

    /// @name Operators
    /// @{
    /// @brief Copy assignment operator.
    /// @param[in] envelope  Envelope class to copy.
    /// @result A deep copy of the input envelope class.
    Envelope& operator=(const Envelope &envelope);
    /// @brief Move assignment operator.
    /// @param[in,out] envelope  On exit this class is moved.  
    ///                          On entry envelope's behavior is undefined.
    /// @result envelope has been moved to this class.
    Envelope& operator=(Envelope &&envelope) noexcept;
    /// @}

    /*! @name Destructors
     */
    /*!
     * @brief Destructor.
     */
    ~Envelope();
    /*!
     * @brief Resets the module and releases all memory.
     */
    void clear() noexcept;
    /*! @} */

    /// @result True indicates that the class is initialized.
    [[nodiscard]] bool isInitialized() const noexcept;
    /// @result The length of the envelope.
    /// @throws std::runtime_error if the class is not initialized.
    [[nodiscard]] int getTransformLength() const;

    /// @brief Initializes the envelope.
    /// @param[in] n   The number of samples in the signal whose envelope is
    ///                to be computed.
    /// @throws std::invalid_argument if n is not positive.
    void initialize(int n);
    /// @brief Computes the envelope of the signal.
    /// @param[in] n   The number of samples in the signal to transform.
    ///                This cmust equal \c getTransformLength().
    /// @param[in] x   The signal to transform.  This is an array whose 
    ///                dimension is [n].
    /// @param[out] yupper  The upper envelope of x.  This is an array whose
    ///                     dimension is [n].
    /// @throws std::invalid_argument if n is incorrect or x or yupper are NULL.
    /// @throws std::runtime_error if the class is not initialized.
    void transform(int n, const T x[], T *yupper[]);
    /// @brief Computes the envelope of the signal.
    /// @param[in] n   The number of samples in the signal to transform.
    ///                This cmust equal \c getTransformLength().
    /// @param[in] x   The signal to transform.  This is an array whose 
    ///                dimension is [n].
    /// @param[out] yupper  The upper envelope of x.  This is an array whose
    ///                     dimension is [n].
    /// @param[out] ylower  The lower envelope of x.  This is an array whose
    ///                     dimension is [n].
    /// @throws std::invalid_argument if n is incorrect or x or yupper are NULL.
    /// @throws std::runtime_error if the class is not initialized.
    void transform(int n, const T x[], T *yupper[], T *ylower[]);
private:
    class EnvelopeImpl;
    std::unique_ptr<EnvelopeImpl> pImpl;
};
}
#endif
