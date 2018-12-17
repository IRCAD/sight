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

#include "uiMeasurement/config.hpp"

#include <fwData/Image.hpp>

#include <fwGui/IActionSrv.hpp>

namespace uiMeasurement
{
namespace action
{

/**
 * @brief   This action allows to show/hide image landmarks.
 *
 * @deprecated This service is deprecated and will be removed.
 */
class UIMEASUREMENT_CLASS_API ShowLandmark : public ::fwGui::IActionSrv
{

public:
    fwCoreServiceClassDefinitionsMacro( (ShowLandmark)( ::fwGui::IActionSrv) );

    UIMEASUREMENT_API ShowLandmark() noexcept;

    UIMEASUREMENT_API virtual ~ShowLandmark() noexcept;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_LANDMARK_DISPLAYED_SIG to this::s_SHOW_LANDMARK_SLOT
     */
    UIMEASUREMENT_API virtual ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;
protected:

    void configuring() override;

    void starting() override;

    void updating() override;

    void stopping() override;

    UIMEASUREMENT_API void swapping() override;
    UIMEASUREMENT_API void info(std::ostream& _sstream ) override;

private:

    /// Slot: show/hide landmark
    void showLandmark(bool isShown);

};

} // namespace action

} // namespace uiMeasurement
