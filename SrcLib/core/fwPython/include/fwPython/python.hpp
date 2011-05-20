#ifndef _FWPYTHON_PYTHON_HPP_
#define _FWPYTHON_PYTHON_HPP_


#include <string>
#include <vector>

#include "fwPython/config.hpp"


namespace fwPython
{

FWPYTHON_API void initialize();
FWPYTHON_API bool isInitialized();
FWPYTHON_API void finalize();

FWPYTHON_API void setHome(std::string path);
FWPYTHON_API void addPath(std::string path);

FWPYTHON_API char* getHome();
FWPYTHON_API std::vector<std::string> getPath();

} //namespace fwPython

#endif //_FWPYTHON_PYTHON_HPP_
