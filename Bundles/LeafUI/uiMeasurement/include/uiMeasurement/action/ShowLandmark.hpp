/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIMEASUREMENT_ACTION_SHOWLANDMARK_HPP__
#define __UIMEASUREMENT_ACTION_SHOWLANDMARK_HPP__

#include "uiMeasurement/config.hpp"

#include <fwData/Image.hpp>

#include <fwGui/IActionSrv.hpp>

namespace uiMeasurement
{
namespace action
{

/**
 * @brief   This action allows to show/hide image landmarks.
 */
class UIMEASUREMENT_CLASS_API ShowLandmark : public ::fwGui::IActionSrv
{

public:
    fwCoreServiceClassDefinitionsMacro ( (ShowLandmark)( ::fwGui::IActionSrv) );

    UIMEASUREMENT_API ShowLandmark() noexcept;

    UIMEASUREMENT_API virtual ~ShowLandmark() noexcept;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_LANDMARK_DISPLAYED_SIG to this::s_SHOW_LANDMARK_SLOT
     */
    UIMEASUREMENT_API virtual KeyConnectionsType getObjSrvConnections() const;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_LANDMARK_DISPLAYED_SIG to this::s_SHOW_LANDMARK_SLOT
     */
    UIMEASUREMENT_API virtual ::fwServices::IService::KeyConnectionsMap getAutoConnections() const;
protected:

    void configuring();

    void starting();

    void updating();

    void stopping();

    UIMEASUREMENT_API void swapping();
    UIMEASUREMENT_API void info(std::ostream &_sstream );

private:

    /// Slot: show/hide landmark
    void showLandmark(bool isShown);

};

} // namespace action

} // namespace uiMeasurement

#endif // __UIMEASUREMENT_ACTION_SHOWLANDMARK_HPP__
