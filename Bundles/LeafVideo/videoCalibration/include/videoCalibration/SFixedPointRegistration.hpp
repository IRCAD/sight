/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VIDEOCALIBRATION_SFIXEDPOINTREGISTRATION_HPP__
#define __VIDEOCALIBRATION_SFIXEDPOINTREGISTRATION_HPP__

#include "videoCalibration/config.hpp"

#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwServices/IController.hpp>

#include <vector>
#include <string>

namespace videoCalibration
{

/**
 * @brief This service is used to registrate a marker and the tip of a surgical tool by using the fixed point method.
 * Basicaly the service compute the center of a 3D sphere (at least a half sphere).
 * The sphere is represented by a set of points (::fwData::PointList).
 *
 * Here is a brief description of how to calibrate a marker and a tool:
 * The tool tip need to be immovable and we rotate the tool arround the tip point.
 * The rotation arround the tip will describe a half sphere, so we can compute an estimation of the vector T between
 * the center of the marker and the tip of the tool.
 * See ::arlCore::PointList::sphereCenterEstimation for more explanations.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
     <service uid="toolRegistration" type="::videoCalibration::SFixedPointRegistration">
         <in key="pointList" uid="markersPL" />
         <inout key="matrix" uid="toolCalibration" />
     </service>
   @endcode
 * @subsection Input Input
 * - \b pointList [::fwData::Pointlist]: marker position.
 * @subsection In-Out In-Out
 * - \b matrix [::fwData::TransformationMatrix3D]: tool calibration matrix.
 */
class VIDEOCALIBRATION_CLASS_API SFixedPointRegistration : public ::fwServices::IController
{
public:
    fwCoreServiceClassDefinitionsMacro((SFixedPointRegistration)(::fwServices::IController));

    /// Constructor
    VIDEOCALIBRATION_API SFixedPointRegistration() noexcept;

    /// Destructor
    VIDEOCALIBRATION_API ~SFixedPointRegistration() noexcept;

protected:

    /// Does nothing
    VIDEOCALIBRATION_API void configuring();

    /// Does nothing.
    VIDEOCALIBRATION_API void starting();

    /// Does nothing.
    VIDEOCALIBRATION_API void swapping();

    ///Compute the center of the sphere
    VIDEOCALIBRATION_API void updating();

    /// Does nothing.
    VIDEOCALIBRATION_API void stopping();

};

} //namespace videoCalibration



#endif //__VIDEOCALIBRATION_SFIXEDPOINTREGISTRATION_HPP__
