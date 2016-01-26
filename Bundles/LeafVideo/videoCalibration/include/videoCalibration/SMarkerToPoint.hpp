/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VIDEOCALIBRATION_SMARKERTOPOINT_HPP__
#define __VIDEOCALIBRATION_SMARKERTOPOINT_HPP__

#include "videoCalibration/config.hpp"

#include <fwData/Image.hpp>
#include <fwData/Point.hpp>
#include <fwData/TransformationMatrix3D.hpp>
#include <extData/MatrixTL.hpp>

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwServices/IController.hpp>

#include <vector>
#include <string>

namespace videoCalibration
{

/**
 * @class SMarkerToPoint
 * @brief This service update a pointlist with the center of the marker (from a matrixTL) when the extractMarker slot is called
 * This service can be used to save the displacement of a marker in time. (each point of the pointlist will be a position).
 */
class VIDEOCALIBRATION_CLASS_API SMarkerToPoint : public ::fwServices::IController
{
public:
    fwCoreServiceClassDefinitionsMacro((SMarkerToPoint)(::fwServices::IController));

    /**
     * @name Slots API
     * @{
     */
    VIDEOCALIBRATION_API static const ::fwCom::Slots::SlotKeyType s_ADD_MARKER_SLOT;
    ///@}


    /// Constructor
    VIDEOCALIBRATION_API SMarkerToPoint() throw ();

    /// Destructor
    VIDEOCALIBRATION_API ~SMarkerToPoint() throw ();

protected:

    /**
     * @brief Configures the service
     *
     * @code{.xml}
        <service uid="..." impl="::videoCalibration::SMarkerToPoint" autoConnect="no">
             <matrixTL>matrixTLUid</matrixTL>
        </service>
       @endcode
     * - \b matrixTL : key of the matrix timeline (marker position)
     */
    VIDEOCALIBRATION_API void configuring() throw (fwTools::Failed);

    /// Does nothing.
    VIDEOCALIBRATION_API void starting() throw (fwTools::Failed);

    /// Does nothing.
    VIDEOCALIBRATION_API void swapping() throw (fwTools::Failed);

    /// Does nothing.
    VIDEOCALIBRATION_API void updating() throw (fwTools::Failed);

    /// Does nothing.
    VIDEOCALIBRATION_API void stopping() throw (fwTools::Failed);

    /**
     * @brief Slot called to extract position from the latest matrix of the MatrixTL
     */
    VIDEOCALIBRATION_API void findPosition();

private:

    std::string m_matrixTLUid; ///< matrixTL UID
    ::extData::MatrixTL::sptr m_matrixTL;///< matrixTL

};

} //namespace videoCalibration



#endif //__VIDEOCALIBRATION_SMARKERTOPOINT_HPP__
