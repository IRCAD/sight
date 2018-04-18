/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "ctrlSplineNavigation/config.hpp"

#include <fwData/TransformationMatrix3D.hpp>

#include <fwServices/IController.hpp>

#include <fwTools/Failed.hpp>

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

    fwCoreServiceClassDefinitionsMacro( (SJumpToPointController)(::fwServices::IController) );

    /// Constructor.
    CTRLSPLINENAVIGATION_API SJumpToPointController() noexcept;

    /// Destructor.
    CTRLSPLINENAVIGATION_API virtual ~SJumpToPointController() noexcept;

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
    virtual void starting() override;
    virtual void stopping() override;
    virtual void updating() override;
    virtual void configuring() override;
    /**  @} */

    /**
     * @brief Jumps to given point.
     *
     * @param matrix ::fwData::TransformationMatrix3D new point associated to this service
     */
    void jumpToViewPoint(::fwData::TransformationMatrix3D::sptr matrix);
};

} // ctrlSplineNavigation
