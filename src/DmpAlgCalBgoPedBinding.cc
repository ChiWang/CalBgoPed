/*
 *  $Id: DmpAlgCalBgoPedBinding.cc, 2014-09-03 11:22:08 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 03/09/2014
*/

#include <boost/python.hpp>
#include "DmpAlgBgoPed.h"

BOOST_PYTHON_MODULE(libDmpAlgBgoPed){
  using namespace boost::python;

  class_<DmpAlgBgoPed,boost::noncopyable,bases<DmpVAlg> >("DmpAlgBgoPed",init<>());
}

