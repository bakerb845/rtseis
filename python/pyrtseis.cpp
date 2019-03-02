#include <vector>
#include <memory>
#include <exception>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include <pybind11/complex.h>
#include "include/rtseis/postProcessing/singleChannel/waveform.hpp"
#include "include/rtseis/postProcessing/singleChannel/taper.hpp"

namespace py = pybind11;

namespace PostProcessing
{
class Waveform
{
    public:
        Waveform(void) :
           waveform_(new RTSeis::PostProcessing::SingleChannel::Waveform())
        {
            return;
        }
        ~Waveform(void){return;}
        /// Convolve
        void (py::array_t<double> s, const std::string &smode)
        {
            RTSeis::Utilities::Math::Convolve::Mode mode;
            py::buffer_info sbuf = s.request();
            int len = sbuf.size;
            const double *sptr = (double *) (sbuf.ptr);
            if (sptr == nullptr)
            {
                throw std::runtime_error("s is null");
            }
            RTSeis::Utilities::Math::Convolve::Mode mode = RTSeis::Utilities::Math::Convolve::Mode::FULL:
            if (mode == "full")
            {
                mode = RTSeis::Utilities::Math::Convolve::Mode::FULL;
            }
            else if (mode == "valid")
            {
                mode = RTSeis::Utilities::Math::Convolve::Mode::VALID;
            }
            else if (mode == "same")
            {
                mode = RTSeis::Utilities::Math::Convolve::Mode::SAME;
            }
            else
            {
                throw std::invalid_argument("Invalid mode"); 
            }
            //waveform_->convolve(s, mode);
        }
        /// Remove mean
        void demean(void)
        {
            try
            {
                waveform_->demean();
            }
            catch (const std::invalid_argument& ia) 
            {
                fprintf(stderr, "%s\n", ia.what());
                throw std::invalid_argument("Demean failed");
            }
            return;
        }
        /// Remove trend
        void detrend(void)
        {
            try
            {
                waveform_->detrend();
            }
            catch (const std::invalid_argument& ia)
            {
                fprintf(stderr, "%s\n", ia.what());
                throw std::invalid_argument("Detrend failed");
            }
            return;
        }
        /// Taper
        void taper(const double pct, const std::string &taperName)
        {
            if (pct < 0 || pct > 100)
            {
                throw std::invalid_argument("Invalid percentage");
            }
            // Classify the wavefor type
            RTSeis::PostProcessing::SingleChannel::TaperParameters::Type window
                = RTSeis::PostProcessing::SingleChannel::TaperParameters::Type::HAMMING;
            if (taperName == "hamming")
            {
                window = RTSeis::PostProcessing::SingleChannel::TaperParameters::Type::HAMMING; 
            }
            else if (taperName == "hann" || taperName == "hanning")
            {
                window = RTSeis::PostProcessing::SingleChannel::TaperParameters::Type::HANN;
            }
            else if (taperName == "blackman")
            {
                window = RTSeis::PostProcessing::SingleChannel::TaperParameters::Type::BLACKMAN;
            }
            else if (taperName == "bartlett" || taperName == "triangle")
            {
                window = RTSeis::PostProcessing::SingleChannel::TaperParameters::Type::BARTLETT;
            }
            else if (taperName == "sine")
            {
                window = RTSeis::PostProcessing::SingleChannel::TaperParameters::Type::SINE;
            }
            else
            {
                throw std::invalid_argument("Unknown taper: " + taperName);
            }
            // Taper
            try
            {
                waveform_->taper(pct, window);
            }
            catch (const std::invalid_argument& ia) 
            {
                fprintf(stderr, "%s\n", ia.what());
                throw std::invalid_argument("Taper failed");
            }
            return;
        }
        /// Sets the data
        void setData(py::array_t<double> x)
        {
            py::buffer_info xbuf = x.request();
            int len = xbuf.size;
            const double *xptr = (double *) (xbuf.ptr);
            if (xptr == nullptr)
            {
                throw std::runtime_error("x is null");
            }
            waveform_->setData(len, xptr);
            return;
        }
        /// Gets the data
        py::array_t<double> getData(void)
        {
            std::vector<double> y;
            waveform_->getData(y);
            return py::array(y.size(), y.data());
        }
        bool isInitialized(void) const
        {
            return true;//waveform_->isInitialized();
        }
    private:
        std::unique_ptr<RTSeis::PostProcessing::SingleChannel::Waveform> waveform_;
}; // End waveform

}; // End post-processing


PYBIND11_MODULE(libpyrtseis, modules)
{
    //------------------------------------------------------------------------//
    //                         PostProcessing Group                           //
    //------------------------------------------------------------------------// 
    py::module m = modules.def_submodule("PostProcessing");
    m.doc() = "Utilities for post-processing waveforms";

    py::class_<PostProcessing::Waveform> singleChannelWaveform(m, "Waveform");
    //singleChannelWaveform
    singleChannelWaveform.def(py::init<>());
    singleChannelWaveform.doc() = "Single channel waveform post-processing";
    singleChannelWaveform.def("setData", &PostProcessing::Waveform::setData,
                              "Sets the signal to process on the class");
    singleChannelWaveform.def("getData", &PostProcessing::Waveform::getData,
                              "Gets the filtered data as a NumPy array");
    singleChannelWaveform.def("demean",  &PostProcessing::Waveform::demean,
                              "Removes the mean from the time series");
    singleChannelWaveform.def("detrend", &PostProcessing::Waveform::detrend,
                              "Removes the trend from the time series");
    singleChannelWaveform.def("taper",   &PostProcessing::Waveform::taper,
                              "Tapers the ends of a signal",
                              py::arg("pct") = 5,
                              py::arg("type") = "hamming");
    singleChannelWaveform.def("isInitialized", &PostProcessing::Waveform::isInitialized,
                              "Checks if the class is initialized");

    //m.doc() = "pybind11 example plugin"; // optional module docstring

    //m.def("add", &add, "A function which adds two numbers");

}