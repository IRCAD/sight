/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/SMesh.hpp"

#include "visuVTKAdaptor/SMaterial.hpp"
#include "visuVTKAdaptor/SMeshNormals.hpp"
#include "visuVTKAdaptor/STexture.hpp"
#include "visuVTKAdaptor/STransform.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Material.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/mt/ObjectReadLock.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>

#include <fwVtkIO/helper/Mesh.hpp>
#include <fwVtkIO/vtk.hpp>

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCommand.h>
#include <vtkDepthSortPolyData.h>
#include <vtkMath.h>
#include <vtkMatrix4x4.h>
#include <vtkPicker.h>
#include <vtkPlaneCollection.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTextureMapToCylinder.h>
#include <vtkTextureMapToPlane.h>
#include <vtkTextureMapToSphere.h>
#include <vtkTransform.h>

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::SMesh);

//-----------------------------------------------------------------------------

namespace visuVTKAdaptor
{

//-----------------------------------------------------------------------------

const ::fwCom::Signals::SignalKeyType SMesh::s_TEXTURE_APPLIED_SIG = "textureApplied";

const ::fwCom::Slots::SlotKeyType SMesh::s_UPDATE_VISIBILITY_SLOT       = "updateVisibility";
const ::fwCom::Slots::SlotKeyType SMesh::s_UPDATE_POINT_COLORS_SLOT     = "updatePointColors";
const ::fwCom::Slots::SlotKeyType SMesh::s_UPDATE_CELL_COLORS_SLOT      = "updateCellColors";
const ::fwCom::Slots::SlotKeyType SMesh::s_UPDATE_VERTEX_SLOT           = "updateVertex";
const ::fwCom::Slots::SlotKeyType SMesh::s_UPDATE_POINT_NORMALS_SLOT    = "updatePointNormals";
const ::fwCom::Slots::SlotKeyType SMesh::s_UPDATE_CELL_NORMALS_SLOT     = "updateCellNormals";
const ::fwCom::Slots::SlotKeyType SMesh::s_UPDATE_POINT_TEX_COORDS_SLOT = "updatePointTexCoords";
const ::fwCom::Slots::SlotKeyType SMesh::s_UPDATE_CELL_TEX_COORDS_SLOT  = "updateCellTexCoords";
const ::fwCom::Slots::SlotKeyType SMesh::s_SHOW_POINT_COLORS_SLOT       = "showPointColors";
const ::fwCom::Slots::SlotKeyType SMesh::s_SHOW_CELL_COLORS_SLOT        = "showCellColors";
const ::fwCom::Slots::SlotKeyType SMesh::s_HIDE_COLORS_SLOT             = "hideColors";
const ::fwCom::Slots::SlotKeyType SMesh::s_UPDATE_COLOR_MODE_SLOT       = "updateColorMode";
const ::fwCom::Slots::SlotKeyType SMesh::s_UPDATE_NORMAL_MODE_SLOT      = "updateNormalMode";

const ::fwServices::IService::KeyType SMesh::s_MESH_INPUT = "mesh";

//-----------------------------------------------------------------------------

class MeshVtkCommand : public vtkCommand
{
public:
    virtual void Stop() = 0;
};

//------------------------------------------------------------------------------

class PlaneShifterCallback : public MeshVtkCommand
{
public:

    //------------------------------------------------------------------------------

    static PlaneShifterCallback* New( vtkPlane* src,  vtkPlane* dst, double factor = 1.)
    {
        return new PlaneShifterCallback( src, dst, factor );
    }

    //--------------------------------------------------------------------------
    PlaneShifterCallback( vtkPlane* src,  vtkPlane* dst, double factor ) :
        m_planeSrc(src),
        m_planeDst(dst),
        m_factor(factor)
    {
        m_planeSrc->Register(this);
        m_planeDst->Register(this);
        src->AddObserver(vtkCommand::ModifiedEvent, this);
        this->Execute( 0, vtkCommand::ModifiedEvent, 0);
    }

    //------------------------------------------------------------------------------

    void Stop()
    {
        m_planeSrc->RemoveObserver(this);
        m_planeSrc->UnRegister(this);
        m_planeDst->UnRegister(this);
    }

    //------------------------------------------------------------------------------

    virtual void Execute( vtkObject* caller, unsigned long eventId, void* )
    {
        if (eventId == vtkCommand::ModifiedEvent)
        {
            double n[3];
            double o[3];

            m_planeSrc->GetNormal(n);
            m_planeSrc->GetOrigin(o);

            if (m_factor < 0)
            {
                for (int i = 0; i < 3; i++)
                {
                    n[i] = -n[i];
                }
            }

            m_planeDst->SetNormal( n );
            m_planeDst->SetOrigin( o );
            m_planeDst->Push( m_factor );
            m_planeDst->Modified();
        }

    }

protected:
    vtkPlane* m_planeSrc;
    vtkPlane* m_planeDst;
    double m_factor;
};

class PlaneCollectionShifterCallback : public MeshVtkCommand
{
public:

    //------------------------------------------------------------------------------

    static PlaneCollectionShifterCallback* New(
        vtkPlaneCollection* src,
        vtkPlaneCollection* dst,
        double factor = 1. )
    {
        return new PlaneCollectionShifterCallback( src, dst, factor );
    }

    //--------------------------------------------------------------------------
    PlaneCollectionShifterCallback( vtkPlaneCollection* src,
                                    vtkPlaneCollection* dst,
                                    double factor) :
        m_planeCollectionSrc(src),
        m_planeCollectionDst(dst),
        m_factor(factor)
    {
        m_planeCollectionSrc->Register(this);
        m_planeCollectionDst->Register(this);
        m_planeCollectionSrc->AddObserver(vtkCommand::ModifiedEvent, this);

        this->Execute( 0, vtkCommand::ModifiedEvent, 0);
    }

    //------------------------------------------------------------------------------

    void Stop()
    {
        this->Clear();
        m_planeCollectionSrc->RemoveObserver(this);
        m_planeCollectionSrc->UnRegister(this);
        m_planeCollectionDst->UnRegister(this);
    }

    //------------------------------------------------------------------------------

    void Clear()
    {
        m_planeCollectionDst->RemoveAllItems();

        for( PlaneShifterCallback* psc :  m_planeCallbacks )
        {
            psc->Stop();
            psc->Delete();
            psc = 0;
        }
        m_planeCallbacks.clear();
    }

    //------------------------------------------------------------------------------

    virtual void Execute( vtkObject* caller, unsigned long eventId, void* )
    {
        if (eventId == vtkCommand::ModifiedEvent)
        {
            this->Clear();

            vtkPlane* plane = NULL;
            for (  m_planeCollectionSrc->InitTraversal();
                   (plane = m_planeCollectionSrc->GetNextItem());
                   )
            {
                vtkPlane* newPlane = vtkPlane::New();
                m_planeCollectionDst->AddItem(newPlane);
                m_planeCallbacks.push_back(PlaneShifterCallback::New(plane, newPlane, m_factor));
                newPlane->Modified();
                newPlane->Delete();
            }
        }
    }

protected:
    vtkPlaneCollection* m_planeCollectionSrc;
    vtkPlaneCollection* m_planeCollectionDst;

    std::vector< PlaneShifterCallback* > m_planeCallbacks;
    double m_factor;
};

class PlaneCollectionAdaptorStarter : public MeshVtkCommand
{
public:

    //------------------------------------------------------------------------------

    static PlaneCollectionAdaptorStarter* New(
        ::visuVTKAdaptor::SMesh::sptr service,
        vtkPlaneCollection* src,
        double factor = 1. )
    {
        return new PlaneCollectionAdaptorStarter( service, src, factor );
    }

    //--------------------------------------------------------------------------
    PlaneCollectionAdaptorStarter( ::visuVTKAdaptor::SMesh::sptr service,
                                   vtkPlaneCollection* src,
                                   double factor) :
        m_service(service),
        m_planeCollectionSrc(src),
        m_factor(factor)
    {
        m_planeCollectionSrc->Register(this);
        m_planeCollectionSrc->AddObserver(vtkCommand::ModifiedEvent, this);
        this->Execute( 0, vtkCommand::ModifiedEvent, 0);
    }

    //------------------------------------------------------------------------------

    void Stop()
    {
        m_planeCollectionSrc->RemoveObserver(this);
        m_planeCollectionSrc->UnRegister(this);
        this->Clear();
    }

    //------------------------------------------------------------------------------

    void Clear()
    {
        for( ::visuVTKAdaptor::SMesh::wptr adaptor :  m_meshServices )
        {
            if (!adaptor.expired())
            {
                adaptor.lock()->stop();
                ::fwServices::OSR::unregisterService(adaptor.lock());
            }
        }
        m_meshServices.clear();

        for( PlaneShifterCallback* psc :  m_planeCallbacks )
        {
            psc->Stop();
            psc->Delete();
            psc = 0;
        }
        m_planeCallbacks.clear();

        for( vtkPlaneCollection* planeCol :  m_planeCollections )
        {
            planeCol->Delete();
        }
        m_planeCollections.clear();

        if (!m_service.expired())
        {
            m_service.lock()->setActorPropertyToUnclippedMaterial( false );
        }
    }

    //------------------------------------------------------------------------------

    virtual void Execute( vtkObject* caller, unsigned long eventId, void* data)
    {
        ::visuVTKAdaptor::SMesh::sptr service;

        if (m_service.expired())
        {
            this->Stop();
            this->Delete();
            return;
        }

        if (eventId == vtkCommand::ModifiedEvent)
        {
            service = m_service.lock();

            this->Clear();

            vtkPlane* plane = NULL;
            for (  m_planeCollectionSrc->InitTraversal();
                   (plane = m_planeCollectionSrc->GetNextItem());
                   )
            {
                vtkPlane* newPlane = vtkPlane::New();
                m_planeCallbacks.push_back(PlaneShifterCallback::New(plane, newPlane, m_factor));

                vtkPlaneCollection* newCollection = vtkPlaneCollection::New();
                newCollection->AddItem(newPlane);

                ::fwServices::registry::ServiceFactory::sptr srvFactory =
                    ::fwServices::registry::ServiceFactory::getDefault();

                ::fwServices::IService::sptr srv = srvFactory->create("::visuVTKAdaptor::SMesh");
                SLM_ASSERT("Service of type '::visuVTKAdaptor::SMesh' cannot be instantiated.", srv);

                ::fwRenderVTK::IAdaptor::sptr meshService = ::fwRenderVTK::IAdaptor::dynamicCast(srv);
                SLM_ASSERT("Service of type '::visuVTKAdaptor::SMesh' is not an adaptor", meshService);

                ::fwServices::OSR::registerServiceInput(service->getInput< ::fwData::Mesh >(SMesh::s_MESH_INPUT),
                                                        SMesh::s_MESH_INPUT, srv);

                ::visuVTKAdaptor::SMesh::sptr meshAdaptor = SMesh::dynamicCast(meshService);

                meshAdaptor->setRenderService( service->getRenderService()  );
                meshAdaptor->setRendererId( service->getRendererId()       );
                meshAdaptor->setPickerId( service->getPickerId()       );
                meshAdaptor->setMaterial( service->getMaterial()       );
                meshAdaptor->setVtkClippingPlanes( newCollection );
                meshAdaptor->setAutoRender( service->getAutoRender() );

                meshAdaptor->start();
                meshAdaptor->updateVisibility( service->getVisibility()     );

                newPlane->Delete();

                m_planeCollections.push_back(newCollection);
                m_meshServices.push_back(meshAdaptor);
            }

            bool hasItems = !m_meshServices.empty();
            service->setActorPropertyToUnclippedMaterial( hasItems );
        }
        else if ( eventId == vtkCommand::UserEvent )
        {
            for( ::visuVTKAdaptor::SMesh::wptr adaptor :  m_meshServices )
            {
                if (!adaptor.expired())
                {
                    adaptor.lock()->updateVisibility(*(bool*)data);
                }
            }
        }
    }

protected:

    ::visuVTKAdaptor::SMesh::wptr m_service;

    vtkPlaneCollection* m_planeCollectionSrc;
    ::fwData::Mesh::sptr m_mesh;

    std::vector< ::visuVTKAdaptor::SMesh::wptr > m_meshServices;

    std::vector< PlaneShifterCallback* > m_planeCallbacks;
    std::vector< vtkPlaneCollection* >   m_planeCollections;

    double m_factor;
};

//------------------------------------------------------------------------------

SMesh::SMesh() noexcept :
    m_showClippedPart(false),
    m_autoResetCamera(true),
    m_polyData(nullptr),
    m_mapper(vtkPolyDataMapper::New()),
    m_actor(nullptr),
    m_clippingPlanes(nullptr),
    m_planeCollectionShifterCallback(nullptr),
    m_servicesStarterCallback(nullptr),
    m_transform(vtkTransform::New()),
    m_uvgen(NONE)
{
    m_unclippedPartMaterial = ::fwData::Material::New();
    m_material              = ::fwData::Material::New();
    m_unclippedPartMaterial->diffuse()->setRGBA("#aaaaff44");
    m_clippingPlanesId = "";

    m_sigTextureApplied = newSignal<TextureAppliedSignalType>(s_TEXTURE_APPLIED_SIG);

    newSlot(s_UPDATE_VISIBILITY_SLOT, &SMesh::updateVisibility, this);
    newSlot(s_UPDATE_POINT_COLORS_SLOT, &SMesh::updatePointColors, this);
    newSlot(s_UPDATE_CELL_COLORS_SLOT, &SMesh::updateCellColors, this);
    newSlot(s_UPDATE_VERTEX_SLOT, &SMesh::updateVertex, this);
    newSlot(s_UPDATE_POINT_NORMALS_SLOT, &SMesh::updatePointNormals, this);
    newSlot(s_UPDATE_CELL_NORMALS_SLOT, &SMesh::updateCellNormals, this);
    newSlot(s_UPDATE_POINT_TEX_COORDS_SLOT, &SMesh::updatePointTexCoords, this);
    newSlot(s_UPDATE_CELL_TEX_COORDS_SLOT, &SMesh::updateCellTexCoords, this);
    newSlot(s_SHOW_POINT_COLORS_SLOT, &SMesh::showPointColors, this);
    newSlot(s_SHOW_CELL_COLORS_SLOT, &SMesh::showCellColors, this);
    newSlot(s_HIDE_COLORS_SLOT, &SMesh::hideColors, this);
    newSlot(s_UPDATE_COLOR_MODE_SLOT, &SMesh::updateColorMode, this);
    newSlot(s_UPDATE_NORMAL_MODE_SLOT, &SMesh::updateNormalMode, this);
}

//------------------------------------------------------------------------------

SMesh::~SMesh() noexcept
{
    m_clippingPlanes = nullptr;

    m_mapper->Delete();
    m_mapper = nullptr;

    m_transform->Delete();
    m_transform = nullptr;

    if(m_actor)
    {
        m_actor->Delete();
        m_actor = nullptr;
    }

    if (m_polyData)
    {
        m_polyData->Delete();
        m_polyData = nullptr;
    }
}

//------------------------------------------------------------------------------

void SMesh::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("service.config.<xmlattr>");

    const std::string color          = config.get<std::string>("color", "#ffffffff");
    const std::string unclippedColor = config.get<std::string>("unclippedcolor", "#aaaaff44");

    m_material->diffuse()->setRGBA(color);

    m_unclippedPartMaterial->diffuse()->setRGBA(unclippedColor);

    const std::string autoresetcamera = config.get<std::string>("autoresetcamera", "yes");
    SLM_ASSERT("'autoresetcamera' must be 'yes' or 'no'", autoresetcamera == "yes" || autoresetcamera == "no");
    m_autoResetCamera = (autoresetcamera == "yes");

    const std::string uvGen = config.get<std::string>("uvgen", "none");

    if(uvGen == "none")
    {
        m_uvgen = NONE;
    }
    else if(uvGen == "sphere")
    {
        m_uvgen = SPHERE;
    }
    else if(uvGen == "cylinder")
    {
        m_uvgen = CYLINDER;
    }
    else if(uvGen == "plane")
    {
        m_uvgen = PLANE;
    }
    else
    {
        SLM_FATAL("'uvgen' value must be 'none', 'sphere', 'cylinder' or 'plane', actual: " + uvGen);
    }

    if (config.count("texture"))
    {
        SLM_FATAL("'texture' is deprecated, you need to connect manually the SMesh::textureApplied signal to the "
                  "STexture::applySTexture slot.");
    }

    if (config.count("shadingMode"))
    {
        const std::string shading                         = config.get<std::string>("shadingMode");
        const ::fwData::Material::ShadingType shadingMode = (shading == "ambient") ? ::fwData::Material::AMBIENT :
                                                            (shading == "flat") ? ::fwData::Material::FLAT :
                                                            (shading == "gouraud") ? ::fwData::Material::GOURAUD :
                                                            ::fwData::Material::PHONG;
        m_material->setShadingMode(shadingMode);
    }
}

//------------------------------------------------------------------------------

void SMesh::updating()
{
    ::fwData::Mesh::csptr mesh = this->getInput < ::fwData::Mesh >(s_MESH_INPUT);
    SLM_ASSERT("Missing mesh", mesh);
    this->updateMesh( mesh );
    this->requestRender();
}

//------------------------------------------------------------------------------

void SMesh::starting()
{
    this->initialize();

    this->buildPipeline();

    if (!m_transformService.expired())
    {
        m_transformService.lock()->start();
    }
    this->requestRender();
}

//------------------------------------------------------------------------------

void SMesh::stopping()
{
    if (!m_transformService.expired())
    {
        m_transformService.lock()->stop();
        ::fwServices::OSR::unregisterService(m_transformService.lock());
    }

    this->removeAllPropFromRenderer();
    if (this->getPicker())
    {
        this->removeFromPicker(m_actor);
    }

    this->removeNormalsService();
    this->removePlaneCollectionShifterCommand();
    this->removeServicesStarterCommand();

    this->unregisterServices();

    m_connections.disconnect();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SMesh::createTransformService()
{
    ::fwData::Mesh::csptr mesh = this->getInput < ::fwData::Mesh >(s_MESH_INPUT);
    SLM_ASSERT("Missing mesh", mesh);

    if(!this->getTransformId().empty())
    {
        vtkTransform* t = m_renderService.lock()->getOrAddVtkTransform(m_transformId);
        m_transform->Concatenate(t);
    }

    ::fwData::TransformationMatrix3D::sptr fieldTransform = mesh->getField< ::fwData::TransformationMatrix3D >(
        "TransformMatrix");

    if (fieldTransform)
    {
        vtkTransform* vtkFieldTransform = vtkTransform::New();
        vtkFieldTransform->Identity();

        // create the srv configuration for objects auto-connection
        IService::Config srvConfig;
        IAdaptor::sptr transformService =
            this->createSubAdaptor( "::visuVTKAdaptor::STransform", srvConfig);
        this->registerServiceInOut(fieldTransform, STransform::s_TM3D_INOUT, transformService, true, srvConfig);
        m_transformService = ::visuVTKAdaptor::STransform::dynamicCast(transformService);

        transformService->setConfiguration(srvConfig);
        transformService->setRenderService( this->getRenderService()  );
        transformService->setRendererId( this->getRendererId()       );
        transformService->setAutoRender( this->getAutoRender()     );

        m_transformService.lock()->setTransform(vtkFieldTransform);
        m_transform->Concatenate(vtkFieldTransform);
        vtkFieldTransform->Delete();
    }

    m_actor->SetUserTransform(m_transform);
}

//------------------------------------------------------------------------------

::fwData::Material::sptr SMesh::getMaterial() const
{
    return m_material;
}

//------------------------------------------------------------------------------

::fwData::Material::sptr SMesh::getUnclippedMaterial() const
{
    return m_unclippedPartMaterial;
}

//------------------------------------------------------------------------------

void SMesh::setActorPropertyToUnclippedMaterial(bool opt)
{
    ::visuVTKAdaptor::SMaterial::sptr mat;

    if (opt)
    {
        SLM_ASSERT("Material service expired", !m_unclippedPartMaterialService.expired());
        mat = ::visuVTKAdaptor::SMaterial::dynamicCast(m_unclippedPartMaterialService.lock());
    }
    else
    {
        SLM_ASSERT("Material service expired", !m_materialService.expired());
        mat = ::visuVTKAdaptor::SMaterial::dynamicCast(m_materialService.lock());
    }

    SLM_ASSERT("Invalid Material Adaptor", mat);

    m_actor->SetProperty( mat->getVtkProperty() );
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void SMesh::setShowClippedPart(bool show)
{
    m_showClippedPart = show;
}

//------------------------------------------------------------------------------

void SMesh::setClippingPlanesId(::fwRenderVTK::SRender::VtkObjectIdType id)
{
    m_clippingPlanesId = id;
}

//------------------------------------------------------------------------------

void SMesh::setServiceOnMaterial(::fwRenderVTK::IAdaptor::sptr& srv, ::fwData::Material::sptr material)
{
    if (!srv)
    {
        // create the srv configuration for objects auto-connection
        IService::Config srvConfig;
        srv = this->createSubAdaptor("::visuVTKAdaptor::SMaterial", srvConfig);
        this->registerServiceInput(material, SMaterial::s_MATERIAL_INPUT, srv, true, srvConfig);

        srv->setConfiguration(srvConfig);
        srv->setRenderService(this->getRenderService());
        srv->setAutoRender( this->getAutoRender() );
        srv->start();
        srv->update();
    }
}

//------------------------------------------------------------------------------

void SMesh::setMaterial(::fwData::Material::sptr material)
{
    m_material = material;
}

//------------------------------------------------------------------------------

void SMesh::setUnclippedPartMaterial(::fwData::Material::sptr material)
{
    m_unclippedPartMaterial = material;
}

//------------------------------------------------------------------------------

void SMesh::updateOptionsMode()
{
    if (m_material->getOptionsMode() == ::fwData::Material::NORMALS)
    {
        this->createNormalsService();
    }
    else
    {
        this->removeNormalsService();
    }
}

//------------------------------------------------------------------------------

void SMesh::createNormalsService()
{
    if ( m_normalsService.expired() )
    {
        ::fwData::Mesh::csptr mesh = this->getInput < ::fwData::Mesh >(s_MESH_INPUT);
        SLM_ASSERT("Missing mesh", mesh);

        // create the srv configuration for objects auto-connection
        IService::Config srvConfig;
        ::fwRenderVTK::IAdaptor::sptr service = this->createSubAdaptor("::visuVTKAdaptor::SMeshNormals", srvConfig);
        this->registerServiceInput(mesh, SMeshNormals::s_MESH_INPUT, service, true, srvConfig);

        service->setConfiguration(srvConfig);
        service->setRenderService( this->getRenderService() );
        service->setRendererId( this->getRendererId()      );
        service->setPickerId( this->getPickerId()      );
        service->setAutoRender( this->getAutoRender()    );
        ::visuVTKAdaptor::SMeshNormals::dynamicCast(service)->setPolyData( m_polyData );
        service->start();

        m_normalsService = service;
    }
}

//------------------------------------------------------------------------------

void SMesh::removeNormalsService()
{
    if ( !m_normalsService.expired() )
    {
        this->unregisterService(m_normalsService.lock());
        m_normalsService.reset();
    }
}

//------------------------------------------------------------------------------

void SMesh::buildPipeline()
{
    ::fwData::Mesh::csptr mesh = this->getInput < ::fwData::Mesh >(s_MESH_INPUT);
    SLM_ASSERT("Missing mesh", mesh);

    if (!m_clippingPlanesId.empty())
    {
        vtkObject* o               = this->getVtkObject(m_clippingPlanesId);
        vtkPlaneCollection* planes = vtkPlaneCollection::SafeDownCast(o);
        this->setVtkClippingPlanes( planes );
    }

    ::fwRenderVTK::IAdaptor::sptr materialService;
    ::fwRenderVTK::IAdaptor::sptr unclippedPartMaterialService;

    this->setServiceOnMaterial(materialService, m_material);
    this->setServiceOnMaterial(unclippedPartMaterialService, m_unclippedPartMaterial);

    m_materialService              = materialService;
    m_unclippedPartMaterialService = unclippedPartMaterialService;

    if (!m_actor)
    {
        m_actor = this->newActor();
        this->createTransformService();

        this->addToRenderer(m_actor);
        if (this->getPicker())
        {
            this->addToPicker(m_actor);
        }
    }

    this->updateMesh( mesh );
    this->updateOptionsMode();

    setActorPropertyToUnclippedMaterial(false);

    removeServicesStarterCommand();

    if( m_clippingPlanes && m_showClippedPart )
    {
        createServicesStarterCommand();
    }
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void SMesh::updateMesh( ::fwData::Mesh::csptr mesh )
{
    if (m_polyData)
    {
        m_polyData->Delete();
        m_polyData = 0;
    }
    m_polyData = vtkPolyData::New();

    {
        ::fwData::mt::ObjectReadLock lock(mesh);
        ::fwVtkIO::helper::Mesh::toVTKMesh(mesh, m_polyData);
    }

    if(m_uvgen == SPHERE)
    {
        vtkSmartPointer<vtkTextureMapToSphere> texCoord = vtkSmartPointer<vtkTextureMapToSphere>::New();
        texCoord->SetInputData(m_polyData);
        m_mapper->SetInputConnection(texCoord->GetOutputPort());
    }
    else if(m_uvgen == CYLINDER)
    {
        vtkSmartPointer<vtkTextureMapToCylinder> texCoord = vtkSmartPointer<vtkTextureMapToCylinder>::New();
        texCoord->SetInputData(m_polyData);
        m_mapper->SetInputConnection(texCoord->GetOutputPort());
    }
    else if(m_uvgen == PLANE)
    {
        vtkSmartPointer<vtkTextureMapToPlane> texCoord = vtkSmartPointer<vtkTextureMapToPlane>::New();
        texCoord->SetInputData(m_polyData);
        m_mapper->SetInputConnection(texCoord->GetOutputPort());
    }
    else
    {
        m_mapper->SetInputData(m_polyData);
    }

    ::fwData::Material::sptr material = this->getMaterial();
    SLM_ASSERT("Missing material", material);
    m_sigTextureApplied->asyncEmit(material);

    if (m_autoResetCamera)
    {
        this->getRenderer()->ResetCamera();
    }
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

vtkActor* SMesh::newActor()
{
    vtkActor* actor = vtkActor::New();

    m_mapper->SetInputData(m_polyData);

    if (m_clippingPlanes)
    {
        vtkPlaneCollection* newClippingPlanes = vtkPlaneCollection::New();

        removePlaneCollectionShifterCommand();

        m_planeCollectionShifterCallback =
            PlaneCollectionShifterCallback::New(m_clippingPlanes, newClippingPlanes, 2.);

        m_mapper->SetClippingPlanes(newClippingPlanes);
        newClippingPlanes->Delete();
    }

    actor->SetMapper(m_mapper);
    this->setVtkPipelineModified();
    return actor;
}

//------------------------------------------------------------------------------

void SMesh::updateVisibility( bool isVisible)
{
    if (m_actor)
    {
        m_actor->SetVisibility( isVisible );
    }

    if (m_servicesStarterCallback)
    {
        m_servicesStarterCallback->Execute(0, vtkCommand::UserEvent, &isVisible);
    }
    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

bool SMesh::getVisibility() const
{
    bool visible = false;
    if (m_actor)
    {
        visible = m_actor->GetVisibility() == 1;
    }
    return visible;
}

//------------------------------------------------------------------------------

void SMesh::setVtkClippingPlanes(vtkPlaneCollection* planes)
{
    m_clippingPlanes = planes;
}

//------------------------------------------------------------------------------

void SMesh::removePlaneCollectionShifterCommand()
{
    if (m_planeCollectionShifterCallback)
    {
        m_planeCollectionShifterCallback->Stop();
        m_planeCollectionShifterCallback->Delete();
        m_planeCollectionShifterCallback = 0;
    }
}

//------------------------------------------------------------------------------

void SMesh::createServicesStarterCommand()
{
    if(!m_servicesStarterCallback)
    {
        ::visuVTKAdaptor::SMesh::sptr srv =
            ::visuVTKAdaptor::SMesh::dynamicCast(
                this->getSptr()
                );
        m_servicesStarterCallback = PlaneCollectionAdaptorStarter::New( srv, m_clippingPlanes, -1. );
    }
}

//------------------------------------------------------------------------------

void SMesh::removeServicesStarterCommand()
{
    if(m_servicesStarterCallback)
    {
        m_servicesStarterCallback->Stop();
        m_servicesStarterCallback->Delete();
        m_servicesStarterCallback = 0;
    }
}

//------------------------------------------------------------------------------

void SMesh::setAutoResetCamera(bool autoResetCamera)
{
    m_autoResetCamera = autoResetCamera;
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SMesh::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push( s_MESH_INPUT, ::fwData::Mesh::s_MODIFIED_SIG, s_UPDATE_SLOT );
    connections.push( s_MESH_INPUT, ::fwData::Mesh::s_POINT_COLORS_MODIFIED_SIG, s_UPDATE_POINT_COLORS_SLOT );
    connections.push( s_MESH_INPUT, ::fwData::Mesh::s_CELL_COLORS_MODIFIED_SIG, s_UPDATE_CELL_COLORS_SLOT);
    connections.push( s_MESH_INPUT, ::fwData::Mesh::s_VERTEX_MODIFIED_SIG, s_UPDATE_VERTEX_SLOT);
    connections.push( s_MESH_INPUT, ::fwData::Mesh::s_POINT_NORMALS_MODIFIED_SIG, s_UPDATE_POINT_NORMALS_SLOT);
    connections.push( s_MESH_INPUT, ::fwData::Mesh::s_CELL_NORMALS_MODIFIED_SIG,  s_UPDATE_CELL_NORMALS_SLOT);
    connections.push( s_MESH_INPUT, ::fwData::Mesh::s_POINT_TEX_COORDS_MODIFIED_SIG, s_UPDATE_POINT_TEX_COORDS_SLOT);
    connections.push( s_MESH_INPUT, ::fwData::Mesh::s_CELL_TEX_COORDS_MODIFIED_SIG, s_UPDATE_CELL_TEX_COORDS_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

void SMesh::updatePointColors()
{
    ::fwData::Mesh::csptr mesh = this->getInput < ::fwData::Mesh >(s_MESH_INPUT);
    SLM_ASSERT("Missing mesh", mesh);
    SLM_ASSERT("m_polyData not instanced", m_polyData);

    ::fwVtkIO::helper::Mesh::updatePolyDataPointColor(m_polyData, mesh);
    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SMesh::updateCellColors()
{
    ::fwData::Mesh::csptr mesh = this->getInput < ::fwData::Mesh >(s_MESH_INPUT);
    SLM_ASSERT("Missing mesh", mesh);
    SLM_ASSERT("m_polyData not instanced", m_polyData);

    ::fwVtkIO::helper::Mesh::updatePolyDataCellColor(m_polyData, mesh);
    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SMesh::updateVertex()
{
    ::fwData::Mesh::csptr mesh = this->getInput < ::fwData::Mesh >(s_MESH_INPUT);
    SLM_ASSERT("Missing mesh", mesh);
    SLM_ASSERT("m_polyData not instanced", m_polyData);

    ::fwVtkIO::helper::Mesh::updatePolyDataPoints(m_polyData, mesh);

    if (m_autoResetCamera)
    {
        this->getRenderer()->ResetCamera();
    }
    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SMesh::updatePointNormals()
{
    ::fwData::Mesh::csptr mesh = this->getInput < ::fwData::Mesh >(s_MESH_INPUT);
    SLM_ASSERT("Missing mesh", mesh);
    ::fwVtkIO::helper::Mesh::updatePolyDataPointNormals(m_polyData, mesh);
    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SMesh::updateCellNormals()
{
    ::fwData::Mesh::csptr mesh = this->getInput < ::fwData::Mesh >(s_MESH_INPUT);
    SLM_ASSERT("Missing mesh", mesh);
    ::fwVtkIO::helper::Mesh::updatePolyDataCellNormals(m_polyData, mesh);
    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SMesh::updatePointTexCoords()
{
    ::fwData::Mesh::csptr mesh = this->getInput < ::fwData::Mesh >(s_MESH_INPUT);
    SLM_ASSERT("Missing mesh", mesh);
    ::fwVtkIO::helper::Mesh::updatePolyDataPointTexCoords(m_polyData, mesh);
    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SMesh::updateCellTexCoords()
{
    ::fwData::Mesh::csptr mesh = this->getInput < ::fwData::Mesh >(s_MESH_INPUT);
    SLM_ASSERT("Missing mesh", mesh);
    ::fwVtkIO::helper::Mesh::updatePolyDataCellTexCoords(m_polyData, mesh);
    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SMesh::showPointColors()
{
    m_mapper->ScalarVisibilityOn();
    m_mapper->SetScalarModeToUsePointData();
    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SMesh::showCellColors()
{
    m_mapper->ScalarVisibilityOn();
    m_mapper->SetScalarModeToUseCellData();
    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SMesh::hideColors()
{
    m_mapper->ScalarVisibilityOff();
    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SMesh::updateColorMode(std::uint8_t mode)
{
    switch (mode)
    {
        case 0:
        {
            this->hideColors();
            break;
        }
        case 1:
        {
            this->showPointColors();
            break;
        }
        case 2:
        {
            this->showCellColors();
            break;
        }
        default:
        {
            OSLM_ERROR("mode " << mode << " is not allowed");
            break;
        }
    }
}

//------------------------------------------------------------------------------

void SMesh::updateNormalMode(std::uint8_t mode)
{
    this->updateOptionsMode();
    if (!m_normalsService.expired())
    {
        ::visuVTKAdaptor::SMeshNormals::sptr normalsAdaptor =
            ::visuVTKAdaptor::SMeshNormals::dynamicCast(m_normalsService.lock());
        normalsAdaptor->updateNormalMode(mode);
    }
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor

