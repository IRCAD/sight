/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include "fwRuntime/dl/PosixNameDecorator.hpp"



namespace fwRuntime
{

namespace dl
{



const std::string PosixNameDecorator::getNativePrefix() const throw()
{
	return std::string("lib");
}



const std::string PosixNameDecorator::getNativeExtension() const throw()
{
	return std::string("so");
}



} // namespace dl

} // namespace fwRuntime
