/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
    fwCoreServiceClassDefinitionsMacro( (ShowDistance)( ::fwGui::IActionSrv) );

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
