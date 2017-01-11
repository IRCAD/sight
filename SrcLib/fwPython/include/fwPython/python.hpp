/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWPYTHON_PYTHON_HPP__
#define __FWPYTHON_PYTHON_HPP__


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

#endif //__FWPYTHON_PYTHON_HPP__
