/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __CTRLCOMPUTECPR_SCOMPUTECPR3D_HPP__
#define __CTRLCOMPUTECPR_SCOMPUTECPR3D_HPP__

#include "ctrlComputeCPR/config.hpp"

#include <cpr/ComputeCPRFunctor.hxx>

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwData/Mesh.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwServices/IController.hpp>

#include <fwTools/Failed.hpp>

namespace ctrlComputeCPR
{

/**
 * @brief Computes a 3D curved planar reformation (CPR).
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::ctrlComputeCPR::SComputeCPR3D">
            <inout key="mesh" uid="..." />
            <in key="spline" uid="..." />
            <in key="image" uid="..." />
        </service>
   @endcode
 * @subsection Input Input
 * - \b spline [::fwData::PointList]: PointList used to generate spline.
 * - \b image [::fwData::Image]: Image used to color points in mesh.
 * @subsection In-Out In-Out
 * - \b mesh [::fwData::Mesh]: mesh where curved planar reformation is generated.
 */
class CTRLCOMPUTECPR_CLASS_API SComputeCPR3D : public ::fwServices::IController
{

public:

    fwCoreServiceClassDefinitionsMacro( (SComputeCPR3D)(::fwServices::IController) );

    /// Constructor.
    CTRLCOMPUTECPR_API SComputeCPR3D() throw();

    /// Destructor.
    CTRLCOMPUTECPR_API virtual ~SComputeCPR3D() throw();

    /**
     * @name Slot keys.
     * @{ */
    CTRLCOMPUTECPR_API static const ::fwCom::Slots::SlotKeyType s_CHANGE_HEIGHT_SLOT;
    CTRLCOMPUTECPR_API static const ::fwCom::Slots::SlotKeyType s_CHANGE_SPACING_SLOT;
    CTRLCOMPUTECPR_API static const ::fwCom::Slots::SlotKeyType s_CHANGE_ANGLE_SLOT;
    CTRLCOMPUTECPR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_SPLINE_SLOT;
    /**  @} */

    /**
     * @name Typedefs : slots.
     * @{ */
    typedef ::fwCom::Slot <void (double)> ChangeHeightSlotType;
    typedef ::fwCom::Slot <void (double)> ChangeSpacingSlotType;
    typedef ::fwCom::Slot <void (double)> ChangeAngleSlotType;
    /**  @} */

protected:

    /**
     * @name Typedefs.
     * @{ */
    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;
    typedef signed short ImagePixelType;
    /**  @} */

    /**
     * @name Overrides
     * @{ */
    virtual void starting() throw(::fwTools::Failed);
    virtual void stopping() throw(::fwTools::Failed);
    virtual void updating() throw(::fwTools::Failed);
    /**  @} */

    /**
     * @brief Overrides IService::configuring().
     * @throw fwTools::Failed
     */
    virtual void configuring() throw(fwTools::Failed);

    /// This method computes the mesh 3D of the CPR.
    void computeMesh();

    /// Set height of CPR according to the value set in the QSpinBox.
    void setHeight(double height);

    /// Set spacing of CPR according to the value set in the QSpinBox.
    void setSpacing(double height);

    /// Set rotation of CPR according to the slider value.
    void setNormalRotation(double angle);

    /// Slot: Updates the spline's points
    void updateSpline();

private:

    /// Number of spline points.
    size_t m_nbSplinePoints;

    /**
     * @name CPR properties.
     * @{ */
    double m_height;
    double m_spacing;
    double m_angle;
    /**  @} */
};

} // namespace ctrlComputeCPR

#endif /* __CTRLCOMPUTECPR_SCOMPUTECPR3D_HPP__ */

