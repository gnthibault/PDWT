// STL
#include <cassert>

// Pybind11
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>

// Local
#include "filters.h"
#include "Wavelet1D.h"

namespace py = pybind11;

/**
 * One may be interested in taking a look at Wavelet1D.cpp in the src
 * directory in order to instanciate the chosen wavelet transform with
 * an existing c++ version
 */

template<typename T>
class Wavelet1DWrapper {
 public:
  Wavelet1DWrapper(int nbLevel=1, bool doCycleSpinning=false,
      const std::string &name="Daub4") : m_nbLevel(nbLevel), 
      m_doCycleSpinning(doCycleSpinning), m_name(name) {}

  void Initialize(py::array_t<T> image) {
    auto buffer = image.request();
    if (buffer.ndim != 1) {
      throw std::runtime_error("Wavelet1DWrapper::Initialize : "
        "Number of dimensions must be one");
    }
    T* ptr = static_cast<T *>(buffer.ptr);
    size_t size = buffer.size;

    if (m_name=="Daub2") {
      m_pWavelet = std::make_unique<Daub2_1D<T>>(
        ptr,size,1,1,m_doCycleSpinning,m_name,m_nbLevel);
    } else if (m_name=="dtwAnto97QSHIFT6") {
      m_pWavelet = std::make_unique<dtwAnto97QSHIFT6_1D<T>>(
        ptr,size,1,1,m_doCycleSpinning,m_name,m_nbLevel);
    } else {
      throw std::runtime_error("Wavelet1DWrapper::Initialize : "
        "Unsupported wavelet type");
    }
  }
  void forward(py::array_t<T> image) {
    if (m_pWavelet->forward()<0) {
      throw std::runtime_error("Wavelet1DWrapper::forward : "
        "Runtime error");
    }
  }
  void backward(py::array_t<T> image) {
    if (m_pWavelet->backward()<0) {
      throw std::runtime_error("Wavelet1DWrapper::backward : "
        "Runtime error");
    }
  }
  void inverse(py::array_t<T> image) {
    if (m_pWavelet->inverse()<0) {
      throw std::runtime_error("Wavelet1DWrapper::inverse : "
        "Runtime error");
    }
  }
  void get_image(py::array_t<T> image) const {
    auto buffer = image.request();
    if (buffer.ndim != 1) {
      throw std::runtime_error("Wavelet1DWrapper::get_image : "
        "Number of dimensions must be one");
    }
    T* ptr = static_cast<T *>(buffer.ptr);
    if (m_pWavelet->get_image(ptr)<0) {
      throw std::runtime_error("Wavelet1DWrapper::get_image : "
        "Runtime error");
    }
  }
  void set_image(py::array_t<T> image) {
    auto buffer = image.request();
    if (buffer.ndim != 1) {
      throw std::runtime_error("Wavelet1DWrapper::set_image : "
        "Number of dimensions must be one");
    }
    T* ptr = static_cast<T *>(buffer.ptr);
    if (m_pWavelet->set_image(ptr)<0) {
      throw std::runtime_error("Wavelet1DWrapper::set_image : "
        "Runtime error");
    }
  }
 protected:
  int m_nbLevel;
  bool m_doCycleSpinning;
  std::string m_name;
  std::unique_ptr<WaveletWrapper<T>> m_pWavelet;
};

PYBIND11_MODULE(pyPDWT, m) {
  m.doc() = "pyPDWT : pybind11 wavelet binding";
  py::class_<Wavelet1DWrapper<float>> Wavelet1D(m, "Wavelet1D",py::dynamic_attr());
  Wavelet1D
    .def(py::init<int,bool,const std::string &>())
    .def("Initialize", &Wavelet1DWrapper<float>::Initialize)
    .def("forward", &Wavelet1DWrapper<float>::forward)
    .def("backward", &Wavelet1DWrapper<float>::backward)
    .def("inverse", &Wavelet1DWrapper<float>::inverse)
    .def("get_image", &Wavelet1DWrapper<float>::get_image)
    .def("set_image", &Wavelet1DWrapper<float>::set_image);
}

