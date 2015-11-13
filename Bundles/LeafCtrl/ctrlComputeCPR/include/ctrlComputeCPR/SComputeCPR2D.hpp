/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __CTRLCOMPUTECPR_SCOMPUTECPR2D_HPP__
#define __CTRLCOMPUTECPR_SCOMPUTECPR2D_HPP__

#include "ctrlComputeCPR/config.hpp"

#include <cpr/ComputeCPRFunctor.hxx>

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwData/Image.hpp>
#include <fwData/Point.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwServices/IController.hpp>

#include <fwTools/Failed.hpp>
#include <fwTools/Type.hpp>

#include <string>

namespace fwServices
{
class ObjectMsg;
}

namespace fwData
{
class PointList;
}

namespace ctrlComputeCPR
{

/**
 * @brief Computes a 2D curved planar reformation (CPR).
 * @class SComputeCPR2D
 */
class CTRLCOMPUTECPR_CLASS_API SComputeCPR2D : public ::fwServices::IController
{

public:

    fwCoreServiceClassDefinitionsMacro ( (SComputeCPR2D)(::fwServices::IController) );

    /// Constructor.
    CTRLCOMPUTECPR_API SComputeCPR2D() throw();

    /// Destructor.
    CTRLCOMPUTECPR_API virtual ~SComputeCPR2D() throw();

    /**
     * @name Slot keys.
     * @{ */
    CTRLCOMPUTECPR_API static const ::fwCom::Slots::SlotKeyType s_CHANGE_HEIGHT_SLOT;
    CTRLCOMPUTECPR_API static const ::fwCom::Slots::SlotKeyType s_CHANGE_SPACING_SLOT;
    CTRLCOMPUTECPR_API static const ::fwCom::Slots::SlotKeyType s_CHANGE_ANGLE_SLOT;
    CTRLCOMPUTECPR_API static const ::fwCom::Slots::SlotKeyType s_SELECT_POINT_SLOT;
    /**  @} */

    /**
     * @name Typedefs : slots.
     * @{ */
    typedef ::fwCom::Slot <void (double)> ChangeHeightSlotType;
    typedef ::fwCom::Slot <void (double)> ChangeSpacingSlotType;
    typedef ::fwCom::Slot <void (double)> ChangeAngleSlotType;
    typedef ::fwCom::Slot <void (int)>     SelectPointSlotType;
    /**  @} */

protected:

    /**
     * @name Typedefs
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
     * Configuration example :
       @verbatim
       <config>
        <splinePoints uid="..." />  <!-- ::fwData::PointList containing spline points            -->
        <sourceImage uid="..." />   <!-- source image (::fwData::Image)                          -->
        <visuPoints uid="..." />    <!-- ::fwData::PointList containing spline points to display -->
       </config>
       @endverbatim
     *
     * @throw fwTools::Failed
     */
    virtual void configuring() throw(fwTools::Failed);

    /// Computes the image of CPR.
    void computeCPRImage();

    /**
     * @brief Sets height of CPR according to the value set in the QSpinBox.
     *
     * @param height CPR height
     */
    void setHeight(double height);

    /**
     * @brief Set spacing of CPR according to the value set in the QSpinBox.
     *
     * @param height CPR spacing
     */
    void setSpacing(double height);

    /**
     * @brief Set rotation of CPR according to the slider value.
     *
     * @param angle CPR rotation angle
     */
    void setNormalRotation(double angle);

    /**
     * @brief Fills visualize point list to visualize spline points on the CPR image.
     *
     * @param indexSelectedPoint index of point to add in the associated point list
     */
    void fillVisualizePointList(int indexSelectedPoint);

    /**
     * @brief Adds point to the visualize point list
     *
     * @param ::fwData::PointList source points
     * @param indexSelectedPoint index of point
     * @param ::fwData::PointList target point list
     */
    void addPointToVisualizePointList(
        const SPTR(::fwData::PointList)& pointList,
        const int indexSelectedPoint,
        const SPTR(::fwData::PointList)& visualizePointList);

    /// Clears the visualization point list.
    void clearVisualizePointList();

    /// Adds a point into the spline
    void addPoint(::fwData::Point::sptr point);

    /// Removes a point from the spline
    void removePoint(::fwData::Point::sptr point);

    /// Updates the spline's points
    void updateSpline();

private:

    /// Slot onHeightChange
    ChangeHeightSlotType::sptr m_slotChangeHeight;

    /// Slot onSpacingChange
    ChangeSpacingSlotType::sptr m_slotChangeSpacing;

    /// Slot onAngleChange
    ChangeAngleSlotType::sptr m_slotChangeAngle;

    // Slot onNewPointSelect
    SelectPointSlotType::sptr m_slotSelectPoint;

    /**
     * @name UIDs
     * @{ */
    std::string m_splinePointsUID;
    std::string m_sourceImageUID;
    std::string m_visuPointsUID;
    /**  @} */

    /**
     * @brief Index of selected point in associated point list
     * @see m_splinePointsUID
     */
    int m_selectedPointIndex;

    /// Number of spline points.
    int m_nbSplinePoints;

    /**
     * @name CPR properties.
     * @{ */
    double m_height;
    double m_spacing;
    double m_angle;
    /**  @} */
};

} // namespace ctrlComputeCPR

#endif /* __CTRLCOMPUTECPR_SCOMPUTECPR2D_HPP__ */
