/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VISUVTKADAPTOR_MESH_HPP_
#define _VISUVTKADAPTOR_MESH_HPP_


#include <fwRenderVTK/IVtkAdaptorService.hpp>

#include "visuVTKAdaptor/config.hpp"

class vtkCommand;
class vtkDepthSortPolyData;
class vtkAlgorithm;
class vtkAlgorithmOutput;
class vtkPolyDataNormals;
class vtkPlaneCollection;
class vtkActorCollection;
class vtkActor;
class vtkPolyData;
class vtkPolyDataMapper;
class vtkTransform;

namespace visuVTKAdaptor
{

class Transform;

class VISUVTKADAPTOR_CLASS_API MeshVtkCommand ;

class VISUVTKADAPTOR_CLASS_API Mesh: public ::fwRenderVTK::IVtkAdaptorService
{

public:
    fwCoreServiceClassDefinitionsMacro ( (Mesh)(::fwRenderVTK::IVtkAdaptorService) ) ;

    VISUVTKADAPTOR_API Mesh() throw();
    VISUVTKADAPTOR_API virtual ~Mesh() throw();

    VISUVTKADAPTOR_API ::fwData::Material::sptr getMaterial();
    VISUVTKADAPTOR_API void setMaterial(::fwData::Material::sptr material);

    VISUVTKADAPTOR_API ::fwData::Material::sptr getUnclippedMaterial();
    VISUVTKADAPTOR_API void setUnclippedPartMaterial(::fwData::Material::sptr material);

    VISUVTKADAPTOR_API void setShowClippedPart ( bool show );
    VISUVTKADAPTOR_API void setClippingPlanesId( ::fwRenderVTK::VtkRenderService::VtkObjectIdType id );

    VISUVTKADAPTOR_API void setVtkClippingPlanes               ( vtkPlaneCollection *planes );
    VISUVTKADAPTOR_API void setActorPropertyToUnclippedMaterial( bool opt );

    /// Active/Inactive automatic reset on camera. By default =true.
    VISUVTKADAPTOR_API void setAutoResetCamera(bool autoResetCamera);

    VISUVTKADAPTOR_API void updateVisibility ( bool isVisible );
    VISUVTKADAPTOR_API bool getVisibility();

    VISUVTKADAPTOR_API void updateOptionsMode();

protected:

   VISUVTKADAPTOR_API void doStart    () throw(fwTools::Failed);
   VISUVTKADAPTOR_API void configuring() throw(fwTools::Failed);
   VISUVTKADAPTOR_API void doStop     () throw(fwTools::Failed);
   VISUVTKADAPTOR_API void doSwap   () throw(fwTools::Failed);
   VISUVTKADAPTOR_API void doUpdate   () throw(fwTools::Failed);
   VISUVTKADAPTOR_API void doUpdate   (::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed);


    vtkActor *newActor();
    void buildPipeline();

    void updateMesh ( ::fwData::Mesh::sptr mesh );

    void setServiceOnMaterial(
        ::fwRenderVTK::IVtkAdaptorService::sptr &srv,
        ::fwData::Material::sptr material
        );

    void removePlaneCollectionShifterCommand();
    void removeServicesStarterCommand();
    void createServicesStarterCommand();

    void createTransformService();
    void createNormalsService();
    void removeNormalsService();

    bool   m_showClippedPart;
    bool   m_autoResetCamera;

    vtkPolyData        *m_polyData;
    vtkPolyDataMapper  *m_mapper;
    vtkActor           *m_actor;
    vtkCommand         *m_depthSortCommand;

    vtkPlaneCollection *m_clippingPlanes;
    ::fwRenderVTK::VtkRenderService::VtkObjectIdType m_clippingPlanesId;

    MeshVtkCommand *m_planeCollectionShifterCallback;
    MeshVtkCommand *m_servicesStarterCallback;

    ::fwData::Material::sptr       m_material;
    ::fwData::Material::sptr       m_unclippedPartMaterial;

    ::fwRenderVTK::IVtkAdaptorService::wptr m_materialService;
    ::fwRenderVTK::IVtkAdaptorService::wptr m_unclippedPartMaterialService;
    ::fwRenderVTK::IVtkAdaptorService::wptr m_normalsService;

    vtkTransform* m_transform;
    WPTR(::visuVTKAdaptor::Transform) m_transformService;

public :

   VISUVTKADAPTOR_API vtkActor * getActor(){ return m_actor; }

};

} //namespace visuVTKAdaptor

#endif // _VISUVTKADAPTOR_MESH_HPP_
