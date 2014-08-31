#include <boost/python.hpp>
#include "DmpAlgBgoPed.h"

BOOST_PYTHON_MODULE(libDmpAlgBgoPed){
  using namespace boost::python;

  class_<DmpAlgBgoPed,boost::noncopyable,bases<DmpVAlg> >("DmpAlgBgoPed",init<>());
}

