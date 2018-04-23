/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "Tuto02DataServiceBasicCtrl/config.hpp"

#include <fwData/Image.hpp>

#include <fwRuntime/Plugin.hpp>

#include <fwServices/IService.hpp>

namespace Tuto02DataServiceBasicCtrl
{

/**
 * @brief   This class is started when the bundles is loaded.
 */
class TUTO02DATASERVICEBASICCTRL_CLASS_API Plugin : public ::fwRuntime::Plugin
{
public:
    /// Constructor.
    TUTO02DATASERVICEBASICCTRL_API Plugin() noexcept;

    /// Destructor. Do nothing.
    TUTO02DATASERVICEBASICCTRL_API ~Plugin() noexcept;

    /// Overrides start method. .
    TUTO02DATASERVICEBASICCTRL_API void start();

    /// Overrides stop method. Do nothing
    TUTO02DATASERVICEBASICCTRL_API void stop() noexcept;

    TUTO02DATASERVICEBASICCTRL_API void initialize();

    TUTO02DATASERVICEBASICCTRL_API void uninitialize() noexcept;

private:
    ::fwData::Image::sptr m_image;

    ::fwServices::IService::sptr m_frameSrv;
    ::fwServices::IService::sptr m_renderSrv;
    ::fwServices::IService::sptr m_readerSrv;
};

} // namespace Tuto02DataServiceBasicCtrl
