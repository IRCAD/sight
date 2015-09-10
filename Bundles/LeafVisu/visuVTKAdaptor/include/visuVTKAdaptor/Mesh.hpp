/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_MESH_HPP__
#define __VISUVTKADAPTOR_MESH_HPP__


#include <fwRenderVTK/IVtkAdaptorService.hpp>

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

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

namespace fwData
{
class Mesh;
class Material;
}

namespace visuVTKAdaptor
{

class Transform;

class VISUVTKADAPTOR_CLASS_API MeshVtkCommand;

/**
 * @brief Display a mesh on the generic scene
 */
class VISUVTKADAPTOR_CLASS_API Mesh : public ::fwRenderVTK::IVtkAdaptorService
{

public:
    fwCoreServiceClassDefinitionsMacro ( (Mesh)(::fwRenderVTK::IVtkAdaptorService) );
    /** @} */

    VISUVTKADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_VISIBILITY_SLOT;
    typedef ::fwCom::Slot<void (bool)> UpdateVisibilitySlotType;

    VISUVTKADAPTOR_API Mesh() throw();
    VISUVTKADAPTOR_API virtual ~Mesh() throw();

    /**
     * @name Signals API
     * @{
     */
    typedef ::fwCom::Signal< void ( SPTR(::fwData::Material) ) > TextureAppliedSignalType;
    VISUVTKADAPTOR_API static const ::fwCom::Signals::SignalKeyType s_TEXTURE_APPLIED_SIG;
    /** @} */

    /**
     * @name Slots API
     * @{
     */
    VISUVTKADAPTOR_API SPTR(::fwData::Material) getMaterial();
    VISUVTKADAPTOR_API void setMaterial(SPTR(::fwData::Material) material);

    VISUVTKADAPTOR_API SPTR(::fwData::Material) getUnclippedMaterial();
    VISUVTKADAPTOR_API void setUnclippedPartMaterial(SPTR(::fwData::Material) material);

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
    VISUVTKADAPTOR_API void doReceive   (::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed);


    vtkActor *newActor();
    void buildPipeline();

    void updateMesh ( SPTR(::fwData::Mesh) mesh );

    void setServiceOnMaterial(
        ::fwRenderVTK::IVtkAdaptorService::sptr &srv,
        SPTR(::fwData::Material) material
        );

    void removePlaneCollectionShifterCommand();
    void removeServicesStarterCommand();
    void createServicesStarterCommand();

    void createTransformService();
    void createNormalsService();
    void removeNormalsService();

    bool m_showClippedPart;
    bool m_autoResetCamera;

    vtkPolyData        *m_polyData;
    vtkPolyDataMapper  *m_mapper;
    vtkActor           *m_actor;
    vtkCommand         *m_depthSortCommand;

    vtkPlaneCollection *m_clippingPlanes;
    ::fwRenderVTK::VtkRenderService::VtkObjectIdType m_clippingPlanesId;

    MeshVtkCommand *m_planeCollectionShifterCallback;
    MeshVtkCommand *m_servicesStarterCallback;

    SPTR(::fwData::Material)       m_material;
    SPTR(::fwData::Material)       m_unclippedPartMaterial;

    ::fwRenderVTK::IVtkAdaptorService::wptr m_materialService;
    ::fwRenderVTK::IVtkAdaptorService::wptr m_unclippedPartMaterialService;
    ::fwRenderVTK::IVtkAdaptorService::wptr m_normalsService;

    vtkTransform* m_transform;
    WPTR(::visuVTKAdaptor::Transform) m_transformService;

    /**
     * @brief Type for generation of texture coordinates
     */
    enum UvGenType
    {
        NONE,
        SPHERE,
        CYLINDER,
        PLANE,
    };

    UvGenType m_uvgen;
    std::string m_textureAdaptorUID;

    /// Signal to emit when a texture must be applied on a material.
    TextureAppliedSignalType::sptr m_sigTextureApplied;

private:

    UpdateVisibilitySlotType::sptr m_slotUpdateVisibility; ///< slot used to change the mesh visibility

public:

    vtkActor * getActor()
    {
        return m_actor;
    }

};

} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_MESH_HPP__
