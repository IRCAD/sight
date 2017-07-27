/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_POINTLABEL_HPP__
#define __VISUVTKADAPTOR_POINTLABEL_HPP__

#include "visuVTKAdaptor/config.hpp"
#include "visuVTKAdaptor/SText.hpp"

#include <fwRenderVTK/IAdaptor.hpp>

#include <string>

class VISUVTKADAPTOR_CLASS_API vtkTextActor;

namespace visuVTKAdaptor
{

class VISUVTKADAPTOR_CLASS_API PointLabel : public SText
{

public:
    fwCoreServiceClassDefinitionsMacro( (PointLabel)(::fwRenderVTK::IAdaptor) );

    VISUVTKADAPTOR_API PointLabel();

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Point::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsType getObjSrvConnections() const;

protected:

    VISUVTKADAPTOR_API void doUpdate();
    VISUVTKADAPTOR_API void doSwap();

    void starting();
    void stopping();
};

} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_POINTLABEL_HPP__

