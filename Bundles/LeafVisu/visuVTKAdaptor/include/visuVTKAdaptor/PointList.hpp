/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_POINTLIST_HPP__
#define __VISUVTKADAPTOR_POINTLIST_HPP__

#ifndef ANDROID

#include "visuVTKAdaptor/config.hpp"
#include "visuVTKAdaptor/MeshFactory.hpp"
#include "visuVTKAdaptor/Point.hpp"

#include <fwData/Color.hpp>
#include <fwData/Point.hpp>

#include <fwRenderVTK/IAdaptor.hpp>

#include <set>
#include <vector>

namespace visuVTKAdaptor
{

/**
 * @brief Adaptor to display a point list.
 *
 * @section Slots Slots
 * - addPoint(::fwData::Point::sptr) : add point in the list.
 * - updateSpline() : Updates the spline's points.
 *
 * @code{.xml}
      <adaptor id="..." class="::visuVTKAdaptor::PointList" objectId="self">
        <config renderer="default" picker="..." color="#FFFFFF" radius="10" interaction="on"/>
      </adaptor>
     @endcode
 * @subsection Configuration Configuration
 * - \b renderer : defines the renderer to show the point list.
 * - \b picker : defines the picker of the point list.
 * - \b color(#FFFFFF) : color of the point.
 * - \b radius(double) : point radius.
 * - \b interaction (optional, default: on): if "on" interactions are enabled
 */

class VISUVTKADAPTOR_CLASS_API PointList : public ::fwRenderVTK::IAdaptor
{

public:
    typedef std::vector< WPTR(::fwData::Point) > WeakPointListType;
    typedef std::set< WPTR(::fwData::Point) > WeakPointSetType;

    fwCoreServiceClassDefinitionsMacro( (PointList)(::fwRenderVTK::IAdaptor) );

    VISUVTKADAPTOR_API PointList() noexcept;

    VISUVTKADAPTOR_API virtual ~PointList() noexcept;

    VISUVTKADAPTOR_API void setRadius(const double);

    VISUVTKADAPTOR_API void setColor(const fwData::Color::sptr);

    VISUVTKADAPTOR_API void setInteraction(const bool);

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect PointList::s_MODIFIED_SIG to this::s_UPDATE_SPLINE_SLOT
     * Connect PointList::s_POINT_ADDED_SIG to this::s_ADD_POINT_SLOT
     * Connect PointList::s_POINT_REMOVED_SIG to this::s_REMOVE_POINT_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsType getObjSrvConnections() const;

protected:

    VISUVTKADAPTOR_API void doStart();
    VISUVTKADAPTOR_API void doStop();

    VISUVTKADAPTOR_API void doConfigure();
    VISUVTKADAPTOR_API void doSwap();
    VISUVTKADAPTOR_API void doUpdate();

    VISUVTKADAPTOR_API void createServices(WeakPointListType& wPtList);
    VISUVTKADAPTOR_API WeakPointListType getWeakPointList();
    VISUVTKADAPTOR_API WeakPointListType getNewPoints();

    WeakPointListType m_oldWeakPointList;
    WeakPointListType m_weakPointList;

private:
    /**
     * @name Slots
     * @{
     */
    /// Adds a point into the spline
    void addPoint(::fwData::Point::sptr point);

    /// Updates the spline's points
    void updateSpline();
    /**
     * @}
     */

    /// Points color
    ::fwData::Color::sptr m_ptColor;

    /// Points radius
    double m_radius;

    /// If true, the interactions are enabled
    bool m_interaction;
};

} //namespace visuVTKAdaptor

#endif // ANDROID

#endif // __VISUVTKADAPTOR_POINTLIST_HPP__
