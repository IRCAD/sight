/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/ClassRegistrar.hpp>

#include "fwData/String.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwData::String, ::fwData::String );
namespace fwData
{
String::String(std::string _value) : m_value( _value )
{
}

String::~String()
{
}

std::string & String::value()
{
	return m_value;
}

const std::string & String::value() const
{
	return m_value;
}

} // namespace fwData
