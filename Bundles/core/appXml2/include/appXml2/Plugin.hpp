/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2015-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __APPXML2_PLUGIN_HPP__
#define __APPXML2_PLUGIN_HPP__

#include "appXml2/config.hpp"

#include <fwRuntime/Plugin.hpp>
#include <fwServices/AppConfigManager2.hpp>

namespace appXml2
{

/**
 * @brief   This class is started when the bundles is loaded.
 */
class APPXML2_CLASS_API Plugin : public ::fwRuntime::Plugin
{
public:
    /// Constructor.
    APPXML2_API Plugin() throw();

    /// Destructor. Do nothing.
    APPXML2_API ~Plugin() throw();

    /// Overrides start method. .
    APPXML2_API void start() throw(::fwRuntime::RuntimeException);

    /// Overrides stop method. Do nothing
    APPXML2_API void stop() throw();

    APPXML2_API void initialize() throw( ::fwRuntime::RuntimeException );

    APPXML2_API void uninitialize() throw();

private:
    std::string m_configurationName;
    std::string m_parametersName;
    ::fwServices::AppConfigManager2::sptr m_appConfigMng;
};

} // namespace appXml2

#endif // __APPXML2_PLUGIN_HPP__
