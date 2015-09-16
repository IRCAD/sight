/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2004-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSERVICES_HELPER_CONFIG_HPP__
#define __FWSERVICES_HELPER_CONFIG_HPP__

#include <fwCore/base.hpp>

#include <fwServices/config.hpp>

namespace fwRuntime
{
struct ConfigurationElement;
}

namespace fwServices
{

namespace helper
{

class SigSlotConnection;

/**
 * @brief Provides configuration element parsing tools.
 */
class Config
{

public:
    /**
     * @brief Parses "<connect>" tags from given configuration to connect signals and slots using given helper.
     *
     * @param ::fwRuntime::ConfigurationElement configuration element containing "<connect>" tags
     * @param SigSlotConnection helper to connect signals and slots
     * @param obj optional object used to retrieve signal if uid is not defined
     */
    FWSERVICES_API static void createConnections(
        CSPTR(::fwRuntime::ConfigurationElement) cfg,
        SPTR(::fwServices::helper::SigSlotConnection) helper,
        const SPTR(::fwTools::Object)& obj = SPTR(::fwTools::Object)());
};

} // namespace helper
} // namespace fwServices

#endif // __FWSERVICES_HELPER_CONFIG_HPP__

