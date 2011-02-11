#ifndef _FWPYTHON_PYTHON_HPP_
#define _FWPYTHON_PYTHON_HPP_


#include <string>
#include "fwPython/config.hpp"


namespace fwPython
{

FWPYTHON_API void initialize();
FWPYTHON_API bool isInitialized();
FWPYTHON_API void finalize();

} //namespace fwPython

#endif //_FWPYTHON_PYTHON_HPP_
