/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRuntime/ExecutableFactory.hpp"



namespace fwRuntime
{



ExecutableFactory::ExecutableFactory( const std::string & type )
:   m_type( type )
{}



ExecutableFactory::~ExecutableFactory()
{}



const std::string ExecutableFactory::getType() const
{
    return m_type;
}




} // namespace fwRuntime
