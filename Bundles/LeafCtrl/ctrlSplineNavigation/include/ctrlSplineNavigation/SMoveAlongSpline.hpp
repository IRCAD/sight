/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __CTRLSPLINENAVIGATION_SMOVEALONGSPLINE_HPP__
#define __CTRLSPLINENAVIGATION_SMOVEALONGSPLINE_HPP__

#include "ctrlSplineNavigation/config.hpp"

#include <fwData/PointList.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwServices/IController.hpp>

#include <vtkParametricSpline.h>
#include <vtkPoints.h>
#include <vtkSmartPointer.h>

namespace ctrlSplineNavigation
{

/**
 * @brief  This service SMoveAlongSpline enables to move the camera along a spline.
 *  @code{.xml}
        <service type="::ctrlSplineNavigation::SJumpToPointController">
            <in key="points" uid="..."/>
        </service>
   @endcode
 * @subsection Input Input
 * - \b matrix [::fwData::PointList]: spline used to move the camera.
 */
class CTRLSPLINENAVIGATION_CLASS_API SMoveAlongSpline : public ::fwServices::IController
{

public:

    fwCoreServiceClassDefinitionsMacro((SMoveAlongSpline)(::fwServices::IController));

    /**
     * @name Constructor/Destructor
     * @{ */
    CTRLSPLINENAVIGATION_API SMoveAlongSpline() noexcept;
    CTRLSPLINENAVIGATION_API virtual ~SMoveAlongSpline() noexcept;
    /**  @} */

    /**
     * @name Signal keys.
     * @{ */
    CTRLSPLINENAVIGATION_API static const ::fwCom::Signals::SignalKeyType s_POINT_CHANGED_SIG;
    CTRLSPLINENAVIGATION_API static const ::fwCom::Signals::SignalKeyType s_LENGTH_CHANGED_SIG;
    /**  @} */

    /**
     * @name Signal types.
     * @{ */
    typedef ::fwCom::Signal< void (::fwData::TransformationMatrix3D::sptr ) > PointChangedSignalType;
    typedef ::fwCom::Signal<void (double)> SplineLengthChangedSignalType;
    /**  @} */

    /**
     * @name Slot keys.
     * @{ */
    CTRLSPLINENAVIGATION_API static const ::fwCom::Slots::SlotKeyType s_CHANGE_SLIDER_VALUE_SLOT;
    CTRLSPLINENAVIGATION_API static const ::fwCom::Slots::SlotKeyType s_CHANGE_SELECTED_POINT_SLOT;
    CTRLSPLINENAVIGATION_API static const ::fwCom::Slots::SlotKeyType s_CHANGE_ANGLE_SLOT;
    CTRLSPLINENAVIGATION_API static const ::fwCom::Slots::SlotKeyType s_ADD_POINT_SLOT;
    CTRLSPLINENAVIGATION_API static const ::fwCom::Slots::SlotKeyType s_REMOVE_POINT_SLOT;
    CTRLSPLINENAVIGATION_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_SPLINE_SLOT;
    /**  @} */

    /**
     * @name Slot types.
     * @{ */
    typedef ::fwCom::Slot< void (double) > ChangeSliderValueSlotType;
    typedef ::fwCom::Slot< void (::fwData::Point::sptr) > ChangeSelectedPointSlotType;
    typedef ::fwCom::Slot<void (double)> ChangeAngleSlotType;
    /**  @} */

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect PointList::s_MODIFIED_SIG to this::s_UPDATE_SPLINE_SLOT
     * Connect PointList::s_POINT_ADDED_SIG to this::s_ADD_POINT_SLOT
     * Connect PointList::s_POINT_REMOVED_SIG to this::s_REMOVE_POINT_SLOT
     */
    CTRLSPLINENAVIGATION_API virtual KeyConnectionsType getObjSrvConnections() const override;

protected:

    /**
     * @name Overrides.
     * @{ */
    virtual void starting() override;
    virtual void stopping() override;
    virtual void configuring() override;
    virtual void updating() override;
    /**  @} */

    /**
     * @brief Moves the camera to the next point of the spline according to given slider position.
     *
     * @param sliderPosition slider position matching a point on the spline
     */
    void moveToPoint(double sliderPosition);

    /**
     * @brief Moves the camera to the given point.
     *
     * @param point point to move the camera
     */
    void moveToSelectedPoint (::fwData::Point::sptr point);

    /**
     * @brief Computes the transformation matrix of the camera.
     */
    void computeTransformationMatrix(double position);

    /**
     * @brief Sets the orientation of the camera axes.
     */
    void setCameraRotation(double angle);

    /// Slot: Adds a point into the spline
    void addPoint(::fwData::Point::sptr point);

    /// Slot: Removes a point from the spline
    void removePoint(::fwData::Point::sptr point);

    /// Slot: Updates the spline's points
    void updateSpline();

    /// Signal emitted when the position on the spline changes
    PointChangedSignalType::sptr m_sigPointChanged;

    /// Signal emitted when the spline length changes
    SplineLengthChangedSignalType::sptr m_sigSplineLengthChanged;

private:

    /// Spline as VTK object
    vtkSmartPointer<vtkParametricSpline> m_parametricSpline;

    /// Spline points
    vtkSmartPointer<vtkPoints> m_vtkPoints;

    double m_currentSliderPosition;
    double m_previousSliderPosition;
    size_t m_nbSplinePoints;
    double m_previousyVector[3];
    double m_splineLength;
    double m_angle;
};

} // namespace ctrlSplineNavigation

#endif /* __CTRLSPLINENAVIGATION_SMOVEALONGSPLINE_HPP__ */

