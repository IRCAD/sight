/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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
 * @brief   This action allows to show/hide image distances.
 *
 * @section Slots Slots
 * - \b showDistance(bool): slot called to check/uncheck the action when the distance are visible or not.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::uiMeasurement::action::ShowDistance">
       <inout key="image" uid="..." />
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b image [::fwData::Image]: image containing the distances.
 */
class UIMEASUREMENT_CLASS_API ShowDistance : public ::fwGui::IActionSrv
{

public:
    fwCoreServiceMacro(ShowDistance,  ::fwGui::IActionSrv);

    UIMEASUREMENT_API ShowDistance() noexcept;

    UIMEASUREMENT_API virtual ~ShowDistance() noexcept;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_DISTANCE_DISPLAYED_SIG to this::s_SHOW_DISTANCE_SLOT
     */
    UIMEASUREMENT_API virtual KeyConnectionsMap getAutoConnections() const override;
protected:

    void configuring() override;

    void starting() override;

    void updating() override;

    void stopping() override;

    void info(std::ostream& _sstream ) override;

private:

    /// Slot: show/hide distance
    void showDistance(bool isShown);

};

} // namespace action

} // namespace uiMeasurement
