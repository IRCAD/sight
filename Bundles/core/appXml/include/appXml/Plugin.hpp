/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __APPXML_PLUGIN_HPP__
#define __APPXML_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>
#include <fwServices/AppConfigManager.hpp>

#include "appXml/config.hpp"

namespace appXml
{

/**
 * @brief   This class is started when the bundles is loaded.
 */
class APPXML_CLASS_API Plugin : public ::fwRuntime::Plugin
{
public:
    /// Constructor.
    APPXML_API Plugin() throw();

    /// Destructor. Do nothing.
    APPXML_API ~Plugin() throw();

    /// Overrides start method. .
    APPXML_API void start() throw(::fwRuntime::RuntimeException);

    /// Overrides stop method. Do nothing
    APPXML_API void stop() throw();

    APPXML_API void initialize() throw( ::fwRuntime::RuntimeException );

    APPXML_API void uninitialize() throw();

private:
    std::string m_configurationName;
    std::string m_parametersName;
    ::fwServices::AppConfigManager::sptr m_appConfigMng;
};

} // namespace appXml

#endif // __APPXML_PLUGIN_HPP__
