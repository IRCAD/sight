/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VISUVTKADAPTOR_TRIANGULARMESH_HPP_
#define _VISUVTKADAPTOR_TRIANGULARMESH_HPP_


#include <fwRenderVTK/IVtkAdaptorService.hpp>

#include "visuVTKAdaptor/config.hpp"
#include "visuVTKAdaptor/MeshFactory.hpp"

class vtkCommand;
class vtkDepthSortPolyData;
class vtkAlgorithmOutput;
class vtkPolyDataNormals;
class vtkPlaneCollection;
class vtkActorCollection;
class vtkActor;
class vtkPolyData;

namespace visuVTKAdaptor
{


class VISUVTKADAPTOR_CLASS_API TriangularMeshVtkCommand ;

class VISUVTKADAPTOR_CLASS_API TriangularMesh: public ::fwRenderVTK::IVtkAdaptorService
{

public:
    fwCoreServiceClassDefinitionsMacro ( (TriangularMesh)(::fwRenderVTK::IVtkAdaptorService) ) ;

    VISUVTKADAPTOR_API TriangularMesh() throw();
    VISUVTKADAPTOR_API virtual ~TriangularMesh() throw();

    VISUVTKADAPTOR_API ::fwData::Material::sptr getMaterial();
    VISUVTKADAPTOR_API void setMaterial(::fwData::Material::sptr material);

    VISUVTKADAPTOR_API ::fwData::Material::sptr getUnclippedMaterial();
    VISUVTKADAPTOR_API void setUnclippedPartMaterial(::fwData::Material::sptr material);


    VISUVTKADAPTOR_API void setSharpEdgeAngle  ( double angle );
    VISUVTKADAPTOR_API double getSharpEdgeAngle( );

    VISUVTKADAPTOR_API void setShowClippedPart ( bool show );
    VISUVTKADAPTOR_API void setClippingPlanesId( ::fwRenderVTK::VtkRenderService::VtkObjectIdType id );

    VISUVTKADAPTOR_API void setVtkClippingPlanes               ( vtkPlaneCollection *planes );
    VISUVTKADAPTOR_API void setMapperInput                     ( vtkAlgorithmOutput *input  );
    VISUVTKADAPTOR_API vtkAlgorithmOutput *getMapperInput      ( );
    VISUVTKADAPTOR_API void setActorPropertyToUnclippedMaterial( bool opt );

    VISUVTKADAPTOR_API void updateVisibility ( bool isVisible );
    VISUVTKADAPTOR_API bool getVisibility();

    VISUVTKADAPTOR_API void updateOptionsMode();

#ifndef USE_DEPTH_PEELING // replacement for depth peeling
    VISUVTKADAPTOR_API vtkDepthSortPolyData * getDepthSort();
    VISUVTKADAPTOR_API void updateDepthSort               ();
    VISUVTKADAPTOR_API void removeDepthSortCommand        ();
#endif


protected:

   VISUVTKADAPTOR_API void doStart    () throw(fwTools::Failed);
   VISUVTKADAPTOR_API void configuring() throw(fwTools::Failed);
   VISUVTKADAPTOR_API void doStop     () throw(fwTools::Failed);
   VISUVTKADAPTOR_API void doSwap   () throw(fwTools::Failed);
   VISUVTKADAPTOR_API void doUpdate   () throw(fwTools::Failed);
   VISUVTKADAPTOR_API void doUpdate   (::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed);


    vtkActor *newActor();
    void buildPipeline();


    void updateTriangularMesh ( ::fwData::TriangularMesh::sptr mesh );
    void updateMaterial       ( ::fwData::Material::sptr material   );

    void setServiceOnMaterial(
        ::fwRenderVTK::IVtkAdaptorService::sptr &srv,
        ::fwData::Material::sptr material
        );

    void removePlaneCollectionShifterCommand();
    void removeServicesStarterCommand();
    void createServicesStarterCommand();

    void createNormalsService();
    void removeNormalsService();




    bool   m_showClippedPart;
    double m_sharpEdgeAngle;
    bool   m_manageMapperInput;
    bool   m_autoResetCamera;

    vtkAlgorithmOutput *m_mapperInput;
    vtkPolyDataNormals *m_normals;
    vtkActor           *m_actor;
    vtkCommand         *m_depthSortCommand;

    vtkPlaneCollection *m_clippingPlanes;
    ::fwRenderVTK::VtkRenderService::VtkObjectIdType m_clippingPlanesId;

    TriangularMeshVtkCommand *m_planeCollectionShifterCallback;
    TriangularMeshVtkCommand *m_servicesStarterCallback;



    ::fwData::TriangularMesh::wptr m_triangularMesh;
    ::fwData::Material::sptr       m_material;
    ::fwData::Material::sptr       m_unclippedPartMaterial;

    ::fwRenderVTK::IVtkAdaptorService::wptr m_materialService;
    ::fwRenderVTK::IVtkAdaptorService::wptr m_unclippedPartMaterialService;
    ::fwRenderVTK::IVtkAdaptorService::wptr m_normalsService;


#ifndef USE_DEPTH_PEELING // replacement for depth peeling
    bool hasAlpha();

    vtkDepthSortPolyData *m_depthSort;
    bool m_hasAlpha;
#endif
};




} //namespace visuVTKAdaptor

#endif // _VISUVTKADAPTOR_TRIANGULARMESH_HPP_
