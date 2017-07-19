/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_MATERIAL_HPP__
#define __VISUVTKADAPTOR_MATERIAL_HPP__

#include "visuVTKAdaptor/config.hpp"

#include <fwRenderVTK/IVtkAdaptorService.hpp>



class VISUVTKADAPTOR_CLASS_API vtkProperty;

namespace fwData
{
class Material;
}

namespace visuVTKAdaptor
{


/**
 * @brief Manage material representation of meshes
 */
class VISUVTKADAPTOR_CLASS_API Material : public ::fwRenderVTK::IVtkAdaptorService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (Material)(::fwRenderVTK::IVtkAdaptorService) );

    VISUVTKADAPTOR_API Material() noexcept;

    VISUVTKADAPTOR_API virtual ~Material() noexcept;

    VISUVTKADAPTOR_API void setVtkProperty(vtkProperty *property);

    VISUVTKADAPTOR_API vtkProperty *getVtkProperty() const;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Material::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsType getObjSrvConnections() const;

protected:

    VISUVTKADAPTOR_API void doStart();
    VISUVTKADAPTOR_API void doStop();
    VISUVTKADAPTOR_API void doConfigure();
    VISUVTKADAPTOR_API void doSwap();
    VISUVTKADAPTOR_API void doUpdate();

    void updateMaterial( SPTR(::fwData::Material) material );


    vtkProperty *m_property;
    bool m_manageProperty;
    bool m_lighting;

};




} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_MATERIAL_HPP__
