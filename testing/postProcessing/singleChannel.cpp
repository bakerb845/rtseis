#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <ipps.h>
#define RTSEIS_LOGGING 1
#include "rtseis/log.h"
#include "rtseis/postProcessing/singleChannel/waveform.hpp"

const std::string dataDir = "data/";
const std::string taperSolns100FileName = dataDir + "taper100.all.txt";
const std::string taperSolns101FileName = dataDir + "taper101.all.txt";

using namespace RTSeis::PostProcessing;

int testDemean(void);
int testDetrend(void);
int testTaper(void);

int main(void)
{
    rtseis_utils_verbosity_setLoggingLevel(RTSEIS_SHOW_ALL);
    int ierr;
    ierr = testDemean();
    if (ierr != EXIT_SUCCESS)
    { 
        RTSEIS_ERRMSG("%s", "Failed demean test");
        return EXIT_FAILURE;
    } 
    RTSEIS_INFOMSG("%s", "Passed demean test");

    ierr = testDetrend();
    if (ierr != EXIT_SUCCESS)
    {
        RTSEIS_ERRMSG("%s", "Failed detrend test");
        return EXIT_FAILURE;
    } 
    RTSEIS_INFOMSG("%s", "Passed detrend test");

    ierr = testTaper();
    if (ierr != EXIT_SUCCESS)
    {
        RTSEIS_ERRMSG("%s", "Failed taper test");
        return EXIT_FAILURE;
    }
    RTSEIS_INFOMSG("%s", "Passed window test");
    return EXIT_SUCCESS; 
}


int testDemean(void)
{
    // Do two passes
    for (int j=0; j<2; j++)
    {
    // Create a reference signal from which to remove the mean
    std::vector<double> x;
    int npts = 5000 + j;
    x.resize(npts);
    double xmean = 5.1 + static_cast<double> (j);
    std::fill(x.begin(), x.end(), xmean);
    std::vector<double> y;
//! [ppSCDemeanExample]
    RTSeis::PostProcessing::SingleChannel::Waveform waveform;
    try
    {
        waveform.setData(x);    // Set waveform data to demean
        waveform.demean();      // Demean the data
        waveform.getData(y);    // Get the demeaned data in a vector, y
    }
    catch (std::invalid_argument &ia)
    {
        fprintf(stderr, "Demean failed %s\n", ia.what()); 
        return EXIT_FAILURE;
    }
//! [ppSCDemeanExample]
    // Verify
    if (y.size() != x.size())
    {
        RTSEIS_ERRMSG("%s", "Inconsistent sizes");
        return EXIT_FAILURE;
    }
    double maxAbs = 0;
    ippsMaxAbs_64f(y.data(), npts, &maxAbs);
    if (maxAbs > 1.e-13)
    {
        RTSEIS_ERRMSG("Demean failed %e", maxAbs);
    }
    } // Loop on passes

    return EXIT_SUCCESS;
}
//============================================================================//
int testDetrend(void)
{
    for (int j=0; j<2; j++)
    {
    std::vector<double> x;
    std::vector<double> y;
    int npts = 50001 + j;
    x.resize(npts);
    #pragma omp simd
    for (int i=0; i<npts; i++){x[i] = 1.1 + 0.3*static_cast<double> (i);}
//! [ppSCDetrendExample]
    RTSeis::PostProcessing::SingleChannel::Waveform waveform;
    try 
    {
        waveform.setData(x);    // Set the data to detrend
        waveform.detrend();     // Detrend
        waveform.getData(y);    // Get the detrended data in a vector, y
    }   
    catch (std::invalid_argument &ia)
    {
        fprintf(stderr, "Detrend failed %s\n", ia.what()); 
        return EXIT_FAILURE;
    }   
//! [ppSCDetrendExample]
    double maxAbs = 0;
    ippsMaxAbs_64f(y.data(), npts, &maxAbs);
    // ~50,000 points loses about 5 digits which is sensible 
    if (maxAbs > 1.e-9)
    {
        RTSEIS_ERRMSG("Demean failed %e", maxAbs);
        return EXIT_FAILURE;
    }
    }; // Loop on npts
    return EXIT_SUCCESS;
}
//============================================================================//
int testTaper(void)
{
    // Load the reference solutions
    std::vector<double> yHamming100Ref(100);
    std::vector<double> yHanning100Ref(100);
    std::vector<double> ySine100Ref(100);
    std::vector<double> yHamming101Ref(101);
    std::vector<double> yHanning101Ref(101);
    std::vector<double> ySine101Ref(101);
    std::string line100, line101;
    std::ifstream taper100File(taperSolns100FileName);
    std::ifstream taper101File(taperSolns101FileName);
    for (int i=0; i<100; i++)
    {
        if (!std::getline(taper100File, line100))
        {
            RTSEIS_ERRMSG("Premature end of file %s\n",
                          taperSolns100FileName.c_str());
            return EXIT_FAILURE;
        }
        std::sscanf(line100.c_str(), "%lf, %lf, %lf\n",
                    &yHamming100Ref[i], &yHanning100Ref[i], &ySine100Ref[i]);
        if (!std::getline(taper101File, line101))
        {
            RTSEIS_ERRMSG("Premature end of file %s\n",
                          taperSolns101FileName.c_str());
            return EXIT_FAILURE;
        }
        std::sscanf(line101.c_str(), "%lf, %lf, %lf\n",
                    &yHamming101Ref[i], &yHanning101Ref[i], &ySine101Ref[i]);
    }
    if (!std::getline(taper101File, line101))
    {
        RTSEIS_ERRMSG("Premature end of file %s\n",
                      taperSolns101FileName.c_str());
        return EXIT_FAILURE;
    }
    std::sscanf(line101.c_str(), "%lf, %lf, %lf\n",
                &yHamming101Ref[100], &yHanning101Ref[100], &ySine101Ref[100]);

    taper100File.close();
    taper101File.close();

    namespace SC = RTSeis::PostProcessing::SingleChannel;
    RTSeis::PostProcessing::SingleChannel::Waveform waveform;
    std::vector<double> x100(100, 1);
    std::vector<double> yHamming100;
    std::vector<double> yHann100;
    std::vector<double> ySine100;
    try
    {
        double pct = 100*(0.2*2); // SAC to RTSeis; 20 pct of signal
        waveform.setData(x100);
        waveform.taper(pct, SC::TaperParameters::HAMMING);
        waveform.getData(yHamming100);

        pct = 100*(0.1*2); // SAC to RTSeis; 20 pct of signal
        waveform.setData(x100);
        waveform.taper(pct, SC::TaperParameters::HANN);
        waveform.getData(yHann100);

        pct = 100*(0.3*2); // SAC to RTSeis; 60 pct of signal
        waveform.setData(x100);
        waveform.taper(pct, SC::TaperParameters::SINE);
        waveform.getData(ySine100); 
    }
    catch (const std::invalid_argument &ia)
    {
        RTSEIS_ERRMSG("Taper 100 failed %s", ia.what());
        return EXIT_FAILURE;
    }
    // Compare
    for (int i=0; i<ySine100.size(); i++)
    {
        if (std::abs(yHamming100[i] - yHamming100Ref[i]) > 1.e-6)
        {
            RTSEIS_ERRMSG("Sine 100 failed %d,%lf,%lf", 
                          i, yHamming100[i], yHamming100Ref[i])
            return EXIT_FAILURE;
        }
        if (std::abs(yHann100[i] - yHanning100Ref[i]) > 1.e-6)
        {
            RTSEIS_ERRMSG("Sine 100 failed %d,%lf,%lf", 
                          i, yHann100[i], yHanning100Ref[i])
            return EXIT_FAILURE;
        }
        if (std::abs(ySine100[i] - ySine100Ref[i]) > 1.e-6)
        {
            RTSEIS_ERRMSG("Sine 100 failed %d,%lf,%lf", 
                          i, ySine100[i], ySine100Ref[i])
            return EXIT_FAILURE;
        }
    }
    // Repeat for 101 points
    std::vector<double> x101(101, 1); 
    std::vector<double> yHamming101;
    std::vector<double> yHann101;
    std::vector<double> ySine101;
    try 
    {   
        double pct = 100*(0.05*2); // SAC to RTSeis; 10 pct of signal
        waveform.setData(x101);
        waveform.taper(pct, SC::TaperParameters::HAMMING);
        waveform.getData(yHamming101);

        pct = 100*(0.1*2); // SAC to RTSeis; 20 pct of signal
        waveform.setData(x101);
        waveform.taper(pct, SC::TaperParameters::HANN);
        waveform.getData(yHann101);

        pct = 100*(0.15*2); // SAC to RTSeis; 30 pct of signal
        waveform.setData(x101);
        waveform.taper(pct, SC::TaperParameters::SINE);
        waveform.getData(ySine101); 
    }   
    catch (const std::invalid_argument &ia)
    {   
        RTSEIS_ERRMSG("Taper 101 failed %s", ia.what());
        return EXIT_FAILURE;
    }
    for (int i=0; i<101; i++)
    {
        if (std::abs(yHamming101[i] - yHamming101Ref[i]) > 1.e-6)
        {
            RTSEIS_ERRMSG("Sine 101 failed %d,%lf,%lf", 
                          i, yHamming101[i], yHamming101Ref[i])
            return EXIT_FAILURE;
        }
        if (std::abs(yHann101[i] - yHanning101Ref[i]) > 1.e-6)
        {
            RTSEIS_ERRMSG("Sine 101 failed %d,%lf,%lf", 
                          i, yHann101[i], yHanning101Ref[i])
            return EXIT_FAILURE;
        }   
        if (std::abs(ySine101[i] - ySine101Ref[i]) > 1.e-6)
        {   
            RTSEIS_ERRMSG("Sine 101 failed %d,%lf,%lf", 
                          i, ySine101[i], ySine101Ref[i])
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}
