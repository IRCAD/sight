/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __CTRLSPLINENAVIGATION_SMOVEALONGSPLINE_HPP__
#define __CTRLSPLINENAVIGATION_SMOVEALONGSPLINE_HPP__

#include <fwServices/IController.hpp>

#include <fwData/PointList.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <vtkParametricSpline.h>
#include <vtkPoints.h>
#include <vtkSmartPointer.h>


#include "ctrlSplineNavigation/config.hpp"

namespace ctrlSplineNavigation
{

/**
 * @class  SMoveAlongSpline
 * @brief  This service SMoveAlongSpline enables to move the camera along a spline.
 */
class CTRLSPLINENAVIGATION_CLASS_API SMoveAlongSpline : public ::fwServices::IController
{

public:

    fwCoreServiceClassDefinitionsMacro((SMoveAlongSpline)(::fwServices::IController));

    /**
     * @name Constructor/Destructor
     * @{ */
    CTRLSPLINENAVIGATION_API SMoveAlongSpline() throw();
    CTRLSPLINENAVIGATION_API virtual ~SMoveAlongSpline() throw();
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
    /**  @} */

    /**
     * @name Slot types.
     * @{ */
    typedef ::fwCom::Slot< void (double) > ChangeSliderValueSlotType;
    typedef ::fwCom::Slot< void (::fwData::Point::sptr) > ChangeSelectedPointSlotType;
    typedef ::fwCom::Slot<void (double)> ChangeAngleSlotType;
    /**  @} */

protected:

    /**
     * @name Overrides.
     * @{ */
    virtual void starting() throw(::fwTools::Failed);
    virtual void stopping() throw(::fwTools::Failed);
    virtual void configuring() throw(::fwTools::Failed);
    virtual void updating() throw(::fwTools::Failed);
    virtual void receiving(::fwServices::ObjectMsg::csptr _msg) throw(::fwTools::Failed);
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

    /// Signal emitted when the position on the spline changes
    PointChangedSignalType::sptr m_sigPointChanged;

    /// Signal emitted when the spline length changes
    SplineLengthChangedSignalType::sptr m_sigSplineLengthChanged;

private:

    /// Changes camera rotation angle.
    ChangeAngleSlotType::sptr m_slotChangeAngle;

    /// Retrieves the current slider value.
    ChangeSliderValueSlotType::sptr m_slotChangeSliderValue;

    /// Retrieves the selected spline point.
    ChangeSelectedPointSlotType::sptr m_slotChangeSelectedPoint;

    /// Spline as VTK object
    vtkSmartPointer<vtkParametricSpline> m_parametricSpline;

    /// Spline points
    vtkSmartPointer<vtkPoints> m_vtkPoints;

    double m_currentSliderPosition;
    double m_previousSliderPosition;
    int m_nbSplinePoints;
    double m_previousyVector[3];
    double m_splineLength;
    double m_angle;
};

} // namespace ctrlSplineNavigation

#endif /* __CTRLSPLINENAVIGATION_SMOVEALONGSPLINE_HPP__ */

