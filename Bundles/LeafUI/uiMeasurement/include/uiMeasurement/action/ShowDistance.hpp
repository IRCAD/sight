/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIMEASUREMENT_ACTION_SHOWDISTANCE_HPP__
#define __UIMEASUREMENT_ACTION_SHOWDISTANCE_HPP__

#include "uiMeasurement/config.hpp"

#include <fwData/Image.hpp>

#include <fwGui/IActionSrv.hpp>

namespace uiMeasurement
{
namespace action
{

/**
 * @brief   This action allows to show/hide image distances.
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
    UIMEASUREMENT_API virtual KeyConnectionsMap getAutoConnections() const;
protected:

    void swapping();

    void configuring();

    void starting();

    void updating();

    void stopping();

    void info(std::ostream& _sstream );

private:

    /// Slot: show/hide distance
    void showDistance(bool isShown);

};

} // namespace action

} // namespace uiMeasurement

#endif // __UIMEASUREMENT_ACTION_SHOWDISTANCE_HPP__
