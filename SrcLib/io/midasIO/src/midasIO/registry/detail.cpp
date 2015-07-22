/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/util/LazyInstantiator.hpp>

#include "midasIO/registry/detail.hpp"


namespace midasIO
{

namespace registry
{

struct ConfigurationRegistryInstantiatorTag {};

SPTR(ConfigurationFactoryType) getConfigurationRegistry()
{
    typedef ::fwCore::util::LazyInstantiator< ConfigurationFactoryType, ConfigurationRegistryInstantiatorTag >
        InstantiatorType;
    return InstantiatorType::getInstance();
}

struct ResponseHandlerRegistryInstantiatorTag {};

SPTR(ResponseHandlerFactoryType) getResponseHandlerRegistry()
{
    typedef ::fwCore::util::LazyInstantiator< ResponseHandlerFactoryType, ResponseHandlerRegistryInstantiatorTag >
        InstantiatorType;
    return InstantiatorType::getInstance();
}

} // namespace registry

} // namespace midasIO

