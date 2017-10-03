/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VIDEOCALIBRATION_SMARKERTOPOINT_HPP__
#define __VIDEOCALIBRATION_SMARKERTOPOINT_HPP__

#include "videoCalibration/config.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwServices/IController.hpp>

#include <string>
#include <vector>

namespace videoCalibration
{

/**
 * @brief This service update a pointlist with the center of the marker (from a matrixTL) when the extractMarker slot is
 * called
 * This service can be used to save the displacement of a marker in time. (each point of the pointlist will be a
 * position).
 *
 * @section Slots Slots
 * - \b addPoint() : Add marker position in the pointList. Position is extracted from matrixTL.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
     <service uid="markerToPoint" type="::videoCalibration::SMarkerToPoint">
         <in key="matrixTL" uid="matrixTL" />
         <inout key="pointList" uid="markersPL" />
     </service>
   @endcode
 * @subsection Input Input
 * - \b matrixTL [::arData::MatrixTL]: timeline for tool matrices.
 * @subsection In-Out In-Out
 * - \b pointList [::fwData::Pointlist]: marker position.
 */
class VIDEOCALIBRATION_CLASS_API SMarkerToPoint : public ::fwServices::IController
{
public:
    fwCoreServiceClassDefinitionsMacro((SMarkerToPoint)(::fwServices::IController));

    /**
     * @name Slots API
     * @{
     */
    VIDEOCALIBRATION_API static const ::fwCom::Slots::SlotKeyType s_ADD_POINT_SLOT;
    ///@}

    /// Constructor
    VIDEOCALIBRATION_API SMarkerToPoint() noexcept;

    /// Destructor
    VIDEOCALIBRATION_API ~SMarkerToPoint() noexcept;

protected:

    /// Does nothing
    VIDEOCALIBRATION_API void configuring() override;

    /// Does nothing.
    VIDEOCALIBRATION_API void starting() override;

    /// Does nothing.
    VIDEOCALIBRATION_API void swapping() override;

    /// Does nothing.
    VIDEOCALIBRATION_API void updating() override;

    /// Does nothing.
    VIDEOCALIBRATION_API void stopping() override;

    /// Slot called to extract position from the latest matrix of the MatrixTL and push it in the pointList
    VIDEOCALIBRATION_API void addPoint();
};

} //namespace videoCalibration

#endif //__VIDEOCALIBRATION_SMARKERTOPOINT_HPP__
