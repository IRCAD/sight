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
 * @class SFixedPointRegistration
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
 *
 */
class VIDEOCALIBRATION_CLASS_API SFixedPointRegistration : public ::fwServices::IController
{
public:
    fwCoreServiceClassDefinitionsMacro((SFixedPointRegistration)(::fwServices::IController));

    /// Constructor
    VIDEOCALIBRATION_API SFixedPointRegistration() throw ();

    /// Destructor
    VIDEOCALIBRATION_API ~SFixedPointRegistration() throw ();

protected:

    /**
     * @brief Configures the service
     *
     * @verbatim
        <service uid="..." impl="::videoCalibration::SFixedPointRegistration" autoConnect="no">
             <pointList>pointListUid</pointList>
        </service>
       @endverbatim
     * - \b pointList : uid of the data pointList (marker positions)
     */
    VIDEOCALIBRATION_API void configuring() throw (fwTools::Failed);

    /// Does nothing.
    VIDEOCALIBRATION_API void starting() throw (fwTools::Failed);

    /// Does nothing.
    VIDEOCALIBRATION_API void swapping() throw (fwTools::Failed);

    ///Compute the center of the sphere
    VIDEOCALIBRATION_API void updating() throw (fwTools::Failed);

    /// Does nothing.
    VIDEOCALIBRATION_API void stopping() throw (fwTools::Failed);

private:

    ::fwData::TransformationMatrix3D::sptr m_calibrationMatrix;///< (Output) Matrix of Calibration (marker/tool)
    ::fwData::PointList::sptr m_pointList;             ///< (Input) the ::fwData::PointList
    std::string m_pointListUID;                           ///< UID of the fwData::PointList


};

} //namespace videoCalibration



#endif //__VIDEOCALIBRATION_SFIXEDPOINTREGISTRATION_HPP__
