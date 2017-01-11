/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
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

namespace fwData
{
class PointList;
}

namespace ctrlComputeCPR
{

/**
 * @brief Computes a 2D curved planar reformation (CPR).
 *  @code{.xml}
        <service type="::ctrlComputeCPR::SComputeCPR2D">
            <inout key="image" uid="..." />
            <inout key="points" uid="..." />
            <in key="spline" uid="..." />
            <in key="sourceImage" uid="..." />
        </service>
   @endcode
 * @subsection Input Input
 * - \b sourceImage [::fwData::Image]: Image used to generate cpr image.
 * - \b spline [::fwData::PointList]: PointList used to generate spline.
 * @subsection In-Out In-Out
 * - \b points [::fwData::PointList]: PointList used to visualize points on the cpr image.
 * - \b image [::fwData::Image]: data where cpr image is generated.
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
    CTRLCOMPUTECPR_API static const ::fwCom::Slots::SlotKeyType s_ADD_POINT_SLOT;
    CTRLCOMPUTECPR_API static const ::fwCom::Slots::SlotKeyType s_REMOVE_POINT_SLOT;
    CTRLCOMPUTECPR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_SPLINE_SLOT;
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
    void fillVisualizePointList(size_t indexSelectedPoint);

    /**
     * @brief Adds point to the visualize point list
     *
     * @param pointList ::fwData::PointList source points
     * @param indexSelectedPoint index of point
     * @param visualizePointList  ::fwData::PointList target point list
     */
    void addPointToVisualizePointList(
        const CSPTR(::fwData::PointList)& pointList,
        size_t indexSelectedPoint,
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

    /**
     * @brief Index of selected point in associated point list
     * @see m_splinePointsUID
     */
    size_t m_selectedPointIndex;

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

#endif /* __CTRLCOMPUTECPR_SCOMPUTECPR2D_HPP__ */
