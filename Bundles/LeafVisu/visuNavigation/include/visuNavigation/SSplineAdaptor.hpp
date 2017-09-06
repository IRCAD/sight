/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUNAVIGATION_SSPLINEADAPTOR_HPP__
#define __VISUNAVIGATION_SSPLINEADAPTOR_HPP__

#include "visuNavigation/config.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwData/Point.hpp>

#include <fwRenderVTK/IAdaptor.hpp>

#include <vtkParametricFunctionSource.h>
#include <vtkParametricSpline.h>
#include <vtkPoints.h>
#include <vtkSmartPointer.h>

namespace visuNavigation
{

/**
 * @brief Adaptor representing a spline from a point list.
 *
 * @section Slots Slots
 * - \b addPoint(::fwData::Point::sptr) : Adds the point into the spline
 * - \b removePoint(::fwData::Point::sptr) : Removes the point from the spline
 * - \b updateSpline() : Updates the spline's points
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::visuNavigation::SSplineAdaptor">
       <int key="pointlist" uid="..." />
       <config renderer="default" />
   </service>
   @endcode
 * @subsection Input Input
 * - \b pointlist [::fwData::PointList]: description5.
 * @subsection Configuration Configuration
 * - \b config(mandatory) : contains the adaptor configuration
 *    - \b renderer(mandatory) : renderer where the resection is displayed
 */
class VISUNAVIGATION_CLASS_API SSplineAdaptor : public ::fwRenderVTK::IAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro((SSplineAdaptor)(::fwRenderVTK::IAdaptor));

    /**
     * @name Constructor/Destructor
     * @{ */
    VISUNAVIGATION_API SSplineAdaptor() noexcept;
    VISUNAVIGATION_API virtual ~SSplineAdaptor() noexcept;
    /**  @} */

protected:

    /**
     * @name Overrides.
     * @{ */
    VISUNAVIGATION_API void configuring();
    VISUNAVIGATION_API void starting();
    VISUNAVIGATION_API void updating();
    VISUNAVIGATION_API void stopping();
    /**  @} */

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect PointList::s_MODIFIED_SIG to this::s_UPDATE_SPLINE_SLOT
     * Connect PointList::s_POINT_ADDED_SIG to this::s_ADD_POINT_SLOT
     * Connect PointList::s_POINT_REMOVED_SIG to this::s_REMOVE_POINT_SLOT
     */
    VISUNAVIGATION_API virtual KeyConnectionsMap getAutoConnections() const;

private:

    /**
     * @name Slots
     * @{
     */
    /// Adds the point into the spline
    void addPoint(::fwData::Point::sptr point);

    /// Removes the point from the spline
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

    /// Spline length.
    double m_splineLength;
};

} //namespace visuNavigation

#endif /* __VISUNAVIGATION_SSPLINEADAPTOR_HPP__ */

