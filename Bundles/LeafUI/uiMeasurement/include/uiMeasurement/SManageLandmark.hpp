/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIMEASUREMENT_SMANAGELANDMARK_HPP__
#define __UIMEASUREMENT_SMANAGELANDMARK_HPP__

#include "uiMeasurement/config.hpp"

#include <fwCom/Slot.hpp>

#include <fwDataTools/PickingInfo.hpp>

#include <fwServices/IController.hpp>

namespace uiMeasurement
{
/**
 * @brief   SManageLandmark adds a point into a pointlist on ctrl-left click.
 *
 * @section Slots Slots
 * - \b createLandmark(::fwDataTools::PickingInfo): add a point into a pointlist on ctrl-left click using PickingInfo.
 * - \b clearPointlist(): Clear all points from pointlist
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service uid="..." type="::uiMeasurement::SManageLandmark" >
       <inout key="landmarks" uid="..." />
   </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b landmarks [::fwData::PointList]: the pointlist containing the added points.
 */
class UIMEASUREMENT_CLASS_API SManageLandmark : public ::fwServices::IController
{
public:

    fwCoreServiceClassDefinitionsMacro((SManageLandmark)(::fwServices::IController));

    /**
     * @brief Constructor.
     */
    UIMEASUREMENT_API SManageLandmark() throw ();

    /**
     * @brief Destructor.
     */
    virtual ~SManageLandmark() throw ()
    {
    }

    /**
     * @name Slot API
     * @{
     */
    UIMEASUREMENT_API static const ::fwCom::Slots::SlotKeyType s_CREATE_LANDMARK_SLOT;
    UIMEASUREMENT_API static const ::fwCom::Slots::SlotKeyType s_CLEAR_SLOT;
    ///@}

protected:

    /**
     * @brief This method is used to configure the service.
     */
    UIMEASUREMENT_API void configuring() throw (fwTools::Failed);

    /// This method is used to initialize the service.
    UIMEASUREMENT_API void starting() throw (fwTools::Failed);

    /// Does nothing.
    UIMEASUREMENT_API void stopping() throw (fwTools::Failed);

    /// Does nothing.
    UIMEASUREMENT_API void updating() throw (fwTools::Failed);

private:

    /// SLot: Each time a point is created, adds an ID label to the point.
    void createLandmark(::fwDataTools::PickingInfo info);

    /// Slot: Clear the pointlist
    void clearPointlist();

    size_t m_counter; ///< Counter used as point ID.

};

} //namespace uiMeasurement

#endif  // __UIMEASUREMENT_SMANAGELANDMARK_HPP__
