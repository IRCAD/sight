/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __CTRLSPLINENAVIGATION_SJUMPTOPOINTCONTROLLER_HPP__
#define __CTRLSPLINENAVIGATION_SJUMPTOPOINTCONTROLLER_HPP__

#include <fwTools/Failed.hpp>

#include <fwServices/IController.hpp>

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/TransformationMatrix3D.hpp>

#include "ctrlSplineNavigation/config.hpp"

namespace ctrlSplineNavigation
{

/**
 * @brief Moves from associated point to another.
 *  @code{.xml}
        <service type="::ctrlSplineNavigation::SJumpToPointController">
            <inout key="matrix" uid="..."/>
        </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b matrix [::fwData::TransformationMatrix3D]: matrix to modify.
 */
class CTRLSPLINENAVIGATION_CLASS_API SJumpToPointController : public ::fwServices::IController
{

public:

    fwCoreServiceClassDefinitionsMacro ( (SJumpToPointController)(::fwServices::IController) );

    /// Constructor.
    CTRLSPLINENAVIGATION_API SJumpToPointController() throw();

    /// Destructor.
    CTRLSPLINENAVIGATION_API virtual ~SJumpToPointController() throw();

    /**
     * @name Slots.
     * @{ */
    typedef ::fwCom::Slot< void (::fwData::TransformationMatrix3D::sptr) > ChangeDirectTargetSlotType;
    CTRLSPLINENAVIGATION_API static const ::fwCom::Slots::SlotKeyType s_CHANGE_DIRECT_TARGET_SLOT;
    /**  @} */

protected:
    /**
     * @name Overrides.
     * @{ */
    virtual void starting() throw(::fwTools::Failed);
    virtual void stopping() throw(::fwTools::Failed);
    virtual void updating() throw(::fwTools::Failed);
    virtual void configuring() throw(fwTools::Failed);
    /**  @} */

    /**
     * @brief Jumps to given point.
     *
     * @param matrix ::fwData::TransformationMatrix3D new point associated to this service
     */
    void jumpToViewPoint(::fwData::TransformationMatrix3D::sptr matrix);
};

} // ctrlSplineNavigation

#endif /*__CTRLSPLINENAVIGATION_SJUMPTOPOINTCONTROLLER_HPP__*/

