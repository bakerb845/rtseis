#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <ipps.h>
#define RTSEIS_LOGGING 1
#include "rtseis/private/throw.hpp"
#include "rtseis/utilities/filterImplementations/downsample.hpp"
#include "rtseis/log.h"

using namespace RTSeis::Utilities::FilterImplementations;

class Downsample::DownsampleImpl
{
public:
    /// Default constructor
    DownsampleImpl() = default;

    /// Copy constructor
    DownsampleImpl(const DownsampleImpl &downsample)
    {
        *this = downsample;
    }
    /// (Deep) copy operator
    DownsampleImpl& operator=(const DownsampleImpl &downsample)
    {
        if (&downsample == this){return *this;}
        phase0_ = downsample.phase0_;
        downFactor_ = downsample.downFactor_;
        phase_ = downsample.phase_;
        mode_ = downsample.mode_;
        precision_ = downsample.precision_;
        linit_ = downsample.linit_; 
        return *this;
    }
    /// Default destructor
    ~DownsampleImpl() = default;
    /// Clears memory and resets the module
    void clear()
    {
        phase0_ = 0;
        downFactor_ = 0;
        phase_ = 0;
        mode_ = RTSeis::ProcessingMode::POST_PROCESSING;
        precision_ = RTSeis::Precision::DOUBLE;
        linit_ = false;
    }
    //--------------------------------------------------------------------//
    /// Initialize the filter
    int initialize(const int downFactor,
                   const RTSeis::ProcessingMode mode,
                   const RTSeis::Precision precision)
    {
        clear();
        downFactor_ = downFactor;
        phase0_ = 0;
        phase_ = 0;
        mode_ = mode;
        precision_ = precision;
        linit_ = true;
        return 0;
    }
    /// Determines if the module is initialized
    bool isInitialized() const
    {
        return linit_;
    }
    /// Estimates the space required to hold the downsampled signal
    int estimateSpace(const int n) const
    {
        int phase = 0;
        if (mode_ ==  RTSeis::ProcessingMode::REAL_TIME){phase = phase_;}
        int pDstLen = (n + downFactor_ - 1 - phase)/downFactor_;
        return pDstLen;  
    }
    /// Gets the downsampling factor
    int getDownsampleFactor() const
    {
        return downFactor_;
    }
    /// Sets the initial conditions
    int setInitialConditions(const int phase)
    {
        resetInitialConditions();
        if (phase < 0 || phase > downFactor_ - 1) 
        {
            return -1;
        }
        phase0_ = phase;
        phase_  = phase;
        return 0;
    }
    /// Resets the initial conditions
    int resetInitialConditions()
    {
        phase_ = phase0_;
        return 0;
    }
    /// Apply the downsampler
    int apply(const int nx, const double x[],
              int *nyDown, double y[])
    {
        *nyDown = 0;
        if (nx == 0){return 0;} 
        // There's really nothing to do
        if (downFactor_ == 1)
        {
            ippsCopy_64f(x, y, nx);
            *nyDown = nx; 
            return 0;
        }
        // Apply downsampler
        int pDstLen = estimateSpace(nx);
        int phase = 0;
        int pEst = pDstLen;
        if (mode_ == RTSeis::ProcessingMode::REAL_TIME){phase = phase_;}
        IppStatus status = ippsSampleDown_64f(x, nx, y, &pEst,
                                              downFactor_, &phase);
        if (status != ippStsNoErr)
        {
            RTSEIS_ERRMSG("%s", "Failed to downsample signal");
            return -1; 
        }
        // TODO - this is hack'ish.  I think I need to try cacheing vectors
        // that are too small to downsample then hit them on the next
        // iteration. 
        // Weird unhandled exception
        if (pEst < 0 || pEst >= nx){pEst = pDstLen;}
        *nyDown = pEst;
        if (mode_ == RTSeis::ProcessingMode::REAL_TIME){phase_ = phase;}
        return 0;
    }
    /// Apply the downsampler
    int apply(const int nx, const float x[],
              int *nyDown, float y[])
    {
        *nyDown = 0;
        if (nx == 0){return 0;}
        // There's really nothing to do
        if (downFactor_ == 1)
        {
            ippsCopy_32f(x, y, nx);
            *nyDown = nx;
            return 0;
        }
        // Apply downsampler
        int pDstLen = estimateSpace(nx);
        int phase = 0;
        int pEst = pDstLen;
        if (mode_ == RTSeis::ProcessingMode::REAL_TIME){phase = phase_;}
        IppStatus status = ippsSampleDown_32f(x, nx, y, &pEst,
                                              downFactor_, &phase);
        if (status != ippStsNoErr)
        {
            RTSEIS_ERRMSG("%s", "Failed to downsample signal");
            return -1;
        }
        // Weird unhandled exception
        if (pEst < 0 || pEst >= nx){pEst = pDstLen;}
        *nyDown = pEst;
        if (mode_ == RTSeis::ProcessingMode::REAL_TIME){phase_ = phase;}
        return 0;
    }
private:
    /// Initial conditions for phase.
    int phase0_ = 0;
    /// Downsampling factor.
    int downFactor_ = 0;
    /// The phase.
    int phase_ = 0;
    /// By default the module does post-procesing.
    RTSeis::ProcessingMode mode_ = RTSeis::ProcessingMode::POST_PROCESSING;
    /// The default module implementation.
    RTSeis::Precision precision_ = RTSeis::Precision::DOUBLE;
    /// Flag indicating the module is initialized.
    bool linit_ = false; 
};

//============================================================================//

Downsample::Downsample() :
    pDownsample_(std::make_unique<DownsampleImpl>())
{
}

Downsample::~Downsample() = default;

Downsample::Downsample(const Downsample &downsample)
{
    *this = downsample;
}

Downsample::Downsample(Downsample &&downsample) noexcept
{
    *this = std::move(downsample); 
}

Downsample& Downsample::operator=(const Downsample &downsample)
{
    if (&downsample == this){return *this;}
    pDownsample_ = std::make_unique<DownsampleImpl> (*downsample.pDownsample_);
    //pDownsample_ = std::unique_ptr<DownsampleImpl>
    //               (new DownsampleImpl(*downsample.pDownsample_));
    return *this;
}

Downsample& Downsample::operator=(Downsample &&downsample) noexcept
{
    if (&downsample == this){return *this;}
    pDownsample_ = std::move(downsample.pDownsample_);
    return *this; 
}

void Downsample::initialize(const int downFactor,
                            const RTSeis::ProcessingMode mode,
                            const RTSeis::Precision precision)
{
    clear();
    if (downFactor < 1)
    {
        RTSEIS_THROW_IA("Downsampling factor=%d must be positive", downFactor);
    }
    pDownsample_->initialize(downFactor, mode, precision);
}

void Downsample::clear()
{
    if (pDownsample_){pDownsample_->clear();}
}

void Downsample::setInitialConditions(const int phase)
{
    if (!isInitialized()){RTSEIS_THROW_RTE("%s", "Downsampler not initialized");}
    int downFactor = pDownsample_->getDownsampleFactor();
    if (phase < 0 || phase > downFactor - 1)
    {
        RTSEIS_THROW_IA("phase=%d must be in range[0,%d]", phase, downFactor-1);
    }
    pDownsample_->setInitialConditions(phase);
}

void Downsample::resetInitialConditions()
{
    if (!isInitialized())
    {
        RTSEIS_THROW_RTE("%s", "Downsampler not initialized");
    }
    pDownsample_->resetInitialConditions();
}

void Downsample::apply(const int nx, const double x[], const int ny,
                       int *nyDown, double y[])
{
    *nyDown = 0;
    if (nx <= 0){return;} // Nothing to do
    if (!isInitialized())
    {
        RTSEIS_THROW_RTE("%s", "Downsampler not intitialized");
    }
    int pDstLen = pDownsample_->estimateSpace(nx); 
#ifdef DEBUG
    assert(pDstLen >= 0);
#endif
    if (ny < pDstLen)
    {
        RTSEIS_ERRMSG("ny=%d must be at least length=%d", ny, pDstLen);
    }
    if (x == nullptr || y == nullptr)
    {
        if (x == nullptr){RTSEIS_THROW_IA("%s", "Error x is NULL");}
        RTSEIS_THROW_IA("%s", "Error y is NULL");
    }
    int ierr = pDownsample_->apply(nx, x, nyDown, y);
#ifdef DEBUG
    assert(ierr == 0);
#endif
    if (ierr != 0)
    {
        RTSEIS_THROW_RTE("%s", "Failed to apply downsampler");
    }
}

void Downsample::apply(const int nx, const float x[], const int ny, 
                       int *nyDown, float y[])
{
    *nyDown = 0;
    if (nx <= 0){return;} // Nothing to do
    if (!isInitialized())
    {
        RTSEIS_THROW_RTE("%s", "Downsampler not intitialized");
    }
    int pDstLen = estimateSpace(nx); 
#ifdef DEBUG
    assert(pDstLen >= 0);
#endif
    if (ny < pDstLen)
    {
        RTSEIS_THROW_IA("ny=%d must be at least length=%d", ny, pDstLen);
    }
    if (x == nullptr || y == nullptr)
    {
        if (x == nullptr){RTSEIS_THROW_IA("%s", "Error x is NULL");}
        RTSEIS_THROW_IA("%s", "Error y is NULL");
    }
    int ierr = pDownsample_->apply(nx, x, nyDown, y);
#ifdef DEBUG
    assert(ierr == 0);
#endif
    if (ierr != 0)
    {
        RTSEIS_THROW_IA("%s", "Failed to apply downsampler");
        return;
    }
}

bool Downsample::isInitialized() const noexcept
{
    return pDownsample_->isInitialized();
}

int Downsample::estimateSpace(const int n) const
{
    if (!isInitialized()){RTSEIS_THROW_RTE("%s", "Class not initialized");}
    if (n < 0){RTSEIS_THROW_IA("n=%d cannot be negative", n);}
    return pDownsample_->estimateSpace(n);
}
 
int Downsample::getDownsampleFactor() const noexcept
{
    return pDownsample_->getDownsampleFactor();
}
