#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ipps.h>
#define RTSEIS_LOGGING 1
#include "rtseis/log.h"
#include "rtseis/modules/demean.hpp"

using namespace RTSeis::Modules;

class Demean::DemeanImpl
{
    public:
        /// Default constructor
        DemeanImpl(void)
        {
            return;
        }
        /// Copy constructor
        DemeanImpl(const DemeanImpl &demean)
        {
            *this = demean;
        }
        /// Copy operator
        DemeanImpl& operator=(const DemeanImpl &demean)
        {
            if (&demean == this){return *this;}
            parms_ = demean.parms_;
            mean_ = demean.mean_;
            linit_ = demean.linit_;
            return *this;
        }
        /// Destructor
        ~DemeanImpl(void)
        {
            clear();
            return;
        }
        /// Resets the module
        void clear(void)
        {
            parms_.clear();
            mean_ = 0;
            linit_ = true; // Module always ready to roll 
            return;
        }
        /// Sets the parameters
        void setParameters(const DemeanParameters &parameters)
        {
            parms_ = parameters;
            return;
        }
        /// Removes mean from data
        int demean(const int nx, const double x[], double y[])
        {
            mean_ = 0;
            if (nx <= 0){return 0;}
            ippsMean_64f(x, nx, &mean_); // Compute mean of input 
            ippsSubC_64f(x, mean_, y, nx); // y - mean(x)
            return 0;
        }
        /// Removes mean from data
        int demean(const int nx, const float x[], float y[])
        {
            mean_ = 0;
            if (nx <= 0){return 0;} 
            float mean32;
            ippsMean_32f(x, nx, &mean32, ippAlgHintAccurate);
            mean_ = static_cast<double> (mean32);
            ippsSubC_32f(x, mean32, y, nx); // y - mean(x)
            return 0;
        }
        /// Utility to set the mean
        void setMean(const double mean)
        {
            mean_ = mean;
        }
    private: 
        DemeanParameters parms_;
        double mean_ = 0;
        bool linit_ = true; // This module is always ready to roll
};

DemeanParameters::DemeanParameters(const RTSeis::Precision precision) :
    precision_(precision),
    mode_(RTSeis::ProcessingMode::POST_PROCESSING),
    linit_(true)
{
    return;
}

DemeanParameters::DemeanParameters(const DemeanParameters &parameters)
{
    *this = parameters;
    return;
}

DemeanParameters&
    DemeanParameters::operator=(const DemeanParameters &parameters)
{
    if (&parameters == this){return *this;}
    precision_ = parameters.precision_;
    mode_ = parameters.mode_;
    linit_ = parameters.linit_;
    return *this;
}

DemeanParameters::~DemeanParameters(void)
{
    clear();
    return;
}

void DemeanParameters::clear(void)
{
    precision_ = defaultPrecision_;
    mode_ = RTSeis::ProcessingMode::POST_PROCESSING;
    linit_ = true; // Demeaning is always ready to roll
    return;
}

RTSeis::Precision DemeanParameters::getPrecision(void) const
{
    return precision_;
}

RTSeis::ProcessingMode DemeanParameters::getProcessingMode(void) const
{
    return mode_;
}

bool DemeanParameters::isInitialized(void) const
{
    return linit_;
}
//============================================================================//

Demean::Demean(void) :
    pDemean_(new DemeanImpl())
{
    return;
}

Demean::Demean(const Demean &demean)
{
    *this = demean;
    return;
}

Demean& Demean::operator=(const Demean &demean)
{
    if (&demean == this){return *this;}
    if (pDemean_){pDemean_->clear();}
    pDemean_ = std::unique_ptr<DemeanImpl> (new DemeanImpl(*demean.pDemean_));
    return *this;
}

Demean::~Demean(void)
{
    clear();
    return;
}

void Demean::clear(void)
{
    pDemean_->clear();
    return;
}

int Demean::setParameters(const DemeanParameters &parameters)
{
    clear();
    if (!parameters.isInitialized())
    {
        RTSEIS_ERRMSG("%s", "Invalid parameters");
        return -1;
    }
    pDemean_->setParameters(parameters);
    return 0;
}

int Demean::demean(const int nx, const double x[], double y[])
{
    pDemean_->setMean(0);
    if (nx <= 0){return 0;}
    if (x == nullptr || y == nullptr)
    {
        if (x == nullptr){RTSEIS_ERRMSG("%s", "x is null");}
        if (y == nullptr){RTSEIS_ERRMSG("%s", "y is null");}
    }
    pDemean_->demean(nx, x, y);
    return 0;
}

int Demean::demean(const int nx, const float x[], float y[])
{
    pDemean_->setMean(0);
    if (nx <= 0){return 0;} // Nothing to do
    if (x == nullptr || y == nullptr)
    {
        if (x == nullptr){RTSEIS_ERRMSG("%s", "x is null");}
        if (y == nullptr){RTSEIS_ERRMSG("%s", "y is null");}
    }
    pDemean_->demean(nx, x, y);
    return 0;
}

