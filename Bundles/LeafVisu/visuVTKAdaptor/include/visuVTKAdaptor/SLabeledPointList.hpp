/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_SLABELEDPOINTLIST_HPP__
#define __VISUVTKADAPTOR_SLABELEDPOINTLIST_HPP__

#include "visuVTKAdaptor/config.hpp"

#include <fwData/Color.hpp>

#include <fwRenderVTK/IAdaptor.hpp>

class vtkCommand;

namespace visuVTKAdaptor
{
/**
 * @brief Adaptor to display a labeled point list
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
      <service uid="..." type="::visuVTKAdaptor::SLabeledPointList" autoConnect="yes">
        <inout key="pointList" uid="..." />
        <config renderer="default" picker="..." color="#FFFFFF" radius="10" interaction="on"/>
      </service>
     @endcode
 * @subsection In-Out In-Out
 * - \b pointList [::fwData::PointList]: labeled point list to display (each point should have a label as field).
 *
 * @subsection Configuration Configuration
 * - \b renderer : defines the renderer to show the labeled point list.
 * - \b picker : defines the picker of the labeled point list.
 * - \b color(#FFFFFF) : color of the points.
 * - \b radius(double) : points radius.
 * - \b interaction (optional, default: on): if "on" interactions are enabled
 */
class VISUVTKADAPTOR_CLASS_API SLabeledPointList : public ::fwRenderVTK::IAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro( (SLabeledPointList)(::fwRenderVTK::IAdaptor) );

    VISUVTKADAPTOR_API SLabeledPointList() noexcept;

    VISUVTKADAPTOR_API virtual ~SLabeledPointList() noexcept;

protected:

    VISUVTKADAPTOR_API void configuring() override;
    VISUVTKADAPTOR_API void starting() override;
    VISUVTKADAPTOR_API void updating() override;
    VISUVTKADAPTOR_API void stopping() override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect PointList::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect PointList::s_POINT_ADDED_SIG to this::s_UPDATE_SLOT
     * Connect PointList::s_POINT_REMOVED_SIG to this::s_UPDATE_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsMap getAutoConnections() const override;

    vtkCommand* m_rightButtonCommand;

    /// Points color
    ::fwData::Color::sptr m_ptColor;

    /// Points radius
    double m_radius;

    /// If true, the interactions are enabled
    bool m_interaction;
};

} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_SLABELEDPOINTLIST_HPP__
