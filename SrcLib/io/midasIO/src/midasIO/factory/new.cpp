/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "midasIO/factory/new.hpp"

#include "midasIO/IConfiguration.hpp"

namespace midasIO
{

namespace configurationFactory
{

SPTR(::midasIO::IConfiguration) New( const ::midasIO::registry::KeyType & classname )
{
    return ::midasIO::registry::getConfigurationRegistry()->create(classname);
}

} // namespace configurationFactory

namespace responseHandlerFactory
{

SPTR(::midasIO::IResponseHandler) New( const ::midasIO::registry::KeyType & classname )
{
    return ::midasIO::registry::getResponseHandlerRegistry()->create(classname);
}

} // namespace responseHandlerFactory

} // namespace midasIO

