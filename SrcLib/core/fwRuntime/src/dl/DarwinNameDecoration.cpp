/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRuntime/dl/DarwinNameDecorator.hpp"



namespace fwRuntime
{

namespace dl
{



const std::string DarwinNameDecorator::getNativePrefix() const throw()
{
	return std::string("lib");
}



const std::string DarwinNameDecorator::getNativeExtension() const throw()
{
	return std::string("dylib");
}



} // namespace dl

} // namespace fwRuntime
