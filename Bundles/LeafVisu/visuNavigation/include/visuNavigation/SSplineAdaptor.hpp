/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUNAVIGATION_SSPLINEADAPTOR_HPP__
#define __VISUNAVIGATION_SSPLINEADAPTOR_HPP__

#include "visuNavigation/config.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwData/Point.hpp>

#include <fwRenderVTK/IVtkAdaptorService.hpp>

#include <vtkParametricFunctionSource.h>
#include <vtkParametricSpline.h>
#include <vtkPoints.h>
#include <vtkSmartPointer.h>


namespace visuNavigation
{

/**
 * @brief Adaptor representing a spline from a point list.
 * @class SSplineAdaptor
 */
class VISUNAVIGATION_CLASS_API SSplineAdaptor : public ::fwRenderVTK::IVtkAdaptorService
{

public:

    fwCoreServiceClassDefinitionsMacro((SSplineAdaptor)(::fwRenderVTK::IVtkAdaptorService));

    /**
     * @name Constructor/Destructor
     * @{ */
    VISUNAVIGATION_API SSplineAdaptor() noexcept;
    VISUNAVIGATION_API virtual ~SSplineAdaptor() noexcept;
    /**  @} */

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect PointList::s_MODIFIED_SIG to this::s_UPDATE_SPLINE_SLOT
     * Connect PointList::s_POINT_ADDED_SIG to this::s_ADD_POINT_SLOT
     * Connect PointList::s_POINT_REMOVED_SIG to this::s_REMOVE_POINT_SLOT
     */
    VISUNAVIGATION_API virtual KeyConnectionsType getObjSrvConnections() const;

protected:

    /**
     * @name Overrides.
     * @{ */
    VISUNAVIGATION_API void doStart();
    VISUNAVIGATION_API void doStop();
    void doSwap()
    {
    }
    VISUNAVIGATION_API void doUpdate();
    VISUNAVIGATION_API void doConfigure();
    /**  @} */

private:

    /**
     * @name Slots
     * @{
     */
    /// Adds a point into the spline
    void addPoint(::fwData::Point::sptr point);

    /// Removes a point from the spline
    void removePoint(::fwData::Point::sptr point);

    /// Updates the spline's points
    void updateSpline();
    /**
     * @}
     */

    /// Spline as a VTK object.
    vtkSmartPointer<vtkParametricSpline> m_parametricSpline;

    /// Spline points.
    vtkSmartPointer<vtkPoints> m_vtkpoints;

    /// Number of points.
    size_t m_numberOfPoints;

    /// Spline length.
    double m_splineLength;
};

} //namespace visuNavigation

#endif /* __VISUNAVIGATION_SSPLINEADAPTOR_HPP__ */

