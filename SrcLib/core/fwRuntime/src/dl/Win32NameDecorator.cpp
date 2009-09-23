/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include "fwRuntime/dl/Win32NameDecorator.hpp"



namespace fwRuntime
{

namespace dl
{



const std::string Win32NameDecorator::getNativeExtension() const throw()
{
	return std::string("dll");
}



} // namespace dl

} // namespace fwRuntime
