/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __MIDASIO_REGISTRY_DETAIL_HPP__
#define __MIDASIO_REGISTRY_DETAIL_HPP__

#include <string>

#include <fwCore/macros.hpp>
#include <fwCore/util/FactoryRegistry.hpp>

#include "midasIO/config.hpp"

namespace midasIO
{

class IConfiguration;
class IResponseHandler;

namespace registry
{

typedef std::string KeyType;

typedef ::fwCore::util::FactoryRegistry< SPTR(::midasIO::IConfiguration) (), KeyType >   ConfigurationFactoryType;
typedef ::fwCore::util::FactoryRegistry< SPTR(::midasIO::IResponseHandler) (), KeyType > ResponseHandlerFactoryType;

MIDASIO_API SPTR(ConfigurationFactoryType) getConfigurationRegistry();
MIDASIO_API SPTR(ResponseHandlerFactoryType) getResponseHandlerRegistry();

} // namespace registry

} // namespace midasIO

#endif /* __MIDASIO_REGISTRY_DETAIL_HPP__ */

