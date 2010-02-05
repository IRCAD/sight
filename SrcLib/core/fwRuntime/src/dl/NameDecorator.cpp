/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRuntime/dl/NameDecorator.hpp"



namespace fwRuntime
{

namespace dl
{



const std::string NameDecorator::getNativeName( const std::string & abstractName ) const throw()
{
	return getNativePrefix() + abstractName + ".*?" + getNativeSuffix() + "." + getNativeExtension();
}



const std::string NameDecorator::getNativePrefix() const throw()
{
	return std::string();
}



const std::string NameDecorator::getNativeSuffix() const throw()
{
#ifdef DEBUG
	return std::string( "D" );
#else
	return std::string( "[^D]" );
#endif
}



const std::string NameDecorator::getNativeExtension() const throw()
{
	return std::string();
}



} // namespace dl

} // namespace fwRuntime
