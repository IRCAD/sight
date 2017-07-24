/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_RESECTION_HPP__
#define __VISUVTKADAPTOR_RESECTION_HPP__

#include "visuVTKAdaptor/config.hpp"

#include <fwRenderVTK/IAdaptor.hpp>

namespace visuVTKAdaptor
{

class VISUVTKADAPTOR_CLASS_API Resection : public ::fwRenderVTK::IAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro( (Resection)(::fwRenderVTK::IAdaptor) );

    VISUVTKADAPTOR_API Resection() noexcept;

    VISUVTKADAPTOR_API virtual ~Resection() noexcept;

    //------------------------------------------------------------------------------

    void setClippingPlanes(::fwRenderVTK::SRender::VtkObjectIdType id)
    {
        m_clippingPlanes = id;
    }

    //------------------------------------------------------------------------------

    void setSharpEdgeAngle(double angle)
    {
        m_sharpEdgeAngle = angle;
    }

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect mesh::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect mesh::s_VISIBILITY_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect mesh::s_RECONSTRUCTION_ADDED_SIG to this::s_UPDATE_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsType getObjSrvConnections() const;

protected:
    VISUVTKADAPTOR_API void doStart();
    VISUVTKADAPTOR_API void doConfigure();
    VISUVTKADAPTOR_API void doSwap();
    // redraw all (stop then restart sub services)
    VISUVTKADAPTOR_API void doUpdate();
    VISUVTKADAPTOR_API void doStop();

private:
    ::fwRenderVTK::SRender::VtkObjectIdType m_clippingPlanes;
    double m_sharpEdgeAngle;
    bool m_autoResetCamera;
};

} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_RESECTION_HPP__
