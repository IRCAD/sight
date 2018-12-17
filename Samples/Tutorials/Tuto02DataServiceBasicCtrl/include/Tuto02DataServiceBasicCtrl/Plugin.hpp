/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

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
