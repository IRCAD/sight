/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_LABELEDPOINTLIST_HPP__
#define __VISUVTKADAPTOR_LABELEDPOINTLIST_HPP__

#include "visuVTKAdaptor/config.hpp"

#include <fwData/Color.hpp>

#include <fwRenderVTK/IVtkAdaptorService.hpp>

class vtkCommand;

namespace visuVTKAdaptor
{
/**
 * @brief Adaptor to display a point list
 *
 * @code{.xml}
      <adaptor id="..." class="::visuVTKAdaptor::LabeledPointList" objectId="self">
        <config renderer="default" picker="..." color="#FFFFFF" radius="10"/>
      </adaptor>
     @endcode
 * @subsection Configuration Configuration
 * - \b renderer : defines the renderer to show the point list.
 * - \b picker : defines the picker of the point list.
 * - \b color(#FFFFFF) : color of the points.
 * - \b radius(double) : points radius.
 */
class VISUVTKADAPTOR_CLASS_API LabeledPointList : public ::fwRenderVTK::IVtkAdaptorService
{

public:

    fwCoreServiceClassDefinitionsMacro( (LabeledPointList)(::fwRenderVTK::IVtkAdaptorService) );

    VISUVTKADAPTOR_API LabeledPointList() throw();

    VISUVTKADAPTOR_API virtual ~LabeledPointList() throw();

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect PointList::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect PointList::s_POINT_ADDED_SIG to this::s_UPDATE_SLOT
     * Connect PointList::s_POINT_REMOVED_SIG to this::s_UPDATE_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsType getObjSrvConnections() const;

protected:

    VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);

    VISUVTKADAPTOR_API void doConfigure() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);

    std::list< ::fwRenderVTK::IVtkAdaptorService::sptr > m_subServices;

    vtkCommand* m_rightButtonCommand;

    /// Points color
    ::fwData::Color::sptr m_ptColor;

    /// Points radius
    double m_radius;
};

} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_LABELEDPOINTLIST_HPP__
