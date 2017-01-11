/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/Material.hpp"
#include "visuVTKAdaptor/Mesh.hpp"
#include "visuVTKAdaptor/MeshNormals.hpp"

#include "visuVTKAdaptor/Texture.hpp"
#include "visuVTKAdaptor/Transform.hpp"

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
#include <vtkTextureMapToCylinder.h>
#include <vtkTextureMapToPlane.h>
#include <vtkTextureMapToSphere.h>

#include <vtkMath.h>
#include <vtkMatrix4x4.h>
#include <vtkPicker.h>
#include <vtkPlaneCollection.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTransform.h>



fwServicesRegisterMacro( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::Mesh, ::fwData::Mesh );

//-----------------------------------------------------------------------------

namespace visuVTKAdaptor
{

//-----------------------------------------------------------------------------

const ::fwCom::Signals::SignalKeyType Mesh::s_TEXTURE_APPLIED_SIG      = "textureApplied";
const ::fwCom::Slots::SlotKeyType Mesh::s_UPDATE_VISIBILITY_SLOT       = "updateVisibility";
const ::fwCom::Slots::SlotKeyType Mesh::s_UPDATE_POINT_COLORS_SLOT     = "updatepointColors";
const ::fwCom::Slots::SlotKeyType Mesh::s_UPDATE_CELL_COLORS_SLOT      = "updateCellColors";
const ::fwCom::Slots::SlotKeyType Mesh::s_UPDATE_VERTEX_SLOT           = "updateVertex";
const ::fwCom::Slots::SlotKeyType Mesh::s_UPDATE_POINT_NORMALS_SLOT    = "updatePointNormals";
const ::fwCom::Slots::SlotKeyType Mesh::s_UPDATE_CELL_NORMALS_SLOT     = "updateCellNormals";
const ::fwCom::Slots::SlotKeyType Mesh::s_UPDATE_POINT_TEX_COORDS_SLOT = "updatePointTexCoords";
const ::fwCom::Slots::SlotKeyType Mesh::s_UPDATE_CELL_TEX_COORDS_SLOT  = "updateCellTexCoords";
const ::fwCom::Slots::SlotKeyType Mesh::s_SHOW_POINT_COLORS_SLOT       = "showPointColors";
const ::fwCom::Slots::SlotKeyType Mesh::s_SHOW_CELL_COLORS_SLOT        = "showCellColors";
const ::fwCom::Slots::SlotKeyType Mesh::s_HIDE_COLORS_SLOT             = "hideColors";
const ::fwCom::Slots::SlotKeyType Mesh::s_UPDATE_COLOR_MODE_SLOT       = "updateColorMode";

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

    static PlaneShifterCallback* New( vtkPlane* src,  vtkPlane* dst, double factor = 1.)
    {
        return new PlaneShifterCallback( src, dst, factor );
    }

    //--------------------------------------------------------------------------
    PlaneShifterCallback( vtkPlane* src,  vtkPlane* dst, double factor )
        : m_planeSrc(src),
          m_planeDst(dst),
          m_factor(factor)
    {
        m_planeSrc->Register(this);
        m_planeDst->Register(this);
        src->AddObserver(vtkCommand::ModifiedEvent, this);
        this->Execute( 0, vtkCommand::ModifiedEvent, 0);
    }

    void Stop()
    {
        m_planeSrc->RemoveObserver(this);
        m_planeSrc->UnRegister(this);
        m_planeDst->UnRegister(this);
    }


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
                for (int i = 0; i<3; i++)
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

    void Stop()
    {
        this->Clear();
        m_planeCollectionSrc->RemoveObserver(this);
        m_planeCollectionSrc->UnRegister(this);
        m_planeCollectionDst->UnRegister(this);
    }

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

    static PlaneCollectionAdaptorStarter* New(
        ::visuVTKAdaptor::Mesh::sptr service,
        vtkPlaneCollection* src,
        double factor = 1. )
    {
        return new PlaneCollectionAdaptorStarter( service, src, factor );
    }

    //--------------------------------------------------------------------------
    PlaneCollectionAdaptorStarter( ::visuVTKAdaptor::Mesh::sptr service,
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

    void Stop()
    {
        m_planeCollectionSrc->RemoveObserver(this);
        m_planeCollectionSrc->UnRegister(this);
        this->Clear();
    }

    void Clear()
    {
        for( ::visuVTKAdaptor::Mesh::wptr adaptor :  m_meshServices )
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

    virtual void Execute( vtkObject* caller, unsigned long eventId, void* data)
    {
        ::visuVTKAdaptor::Mesh::sptr service;

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


                ::fwRenderVTK::IVtkAdaptorService::sptr meshService =
                    ::fwServices::add< ::fwRenderVTK::IVtkAdaptorService > (
                        service->getObject(),
                        "::visuVTKAdaptor::Mesh" );

                ::visuVTKAdaptor::Mesh::sptr meshAdaptor = Mesh::dynamicCast(meshService);

                meshAdaptor->setRenderService ( service->getRenderService()  );
                meshAdaptor->setRenderId      ( service->getRenderId()       );
                meshAdaptor->setPickerId      ( service->getPickerId()       );
                meshAdaptor->setMaterial      ( service->getMaterial()       );
                meshAdaptor->setVtkClippingPlanes( newCollection );
                meshAdaptor->setAutoRender( service->getAutoRender() );

                meshAdaptor->start();
                meshAdaptor->updateVisibility ( service->getVisibility()     );

                newPlane->Delete();

                m_planeCollections.push_back(newCollection);
                m_meshServices.push_back(meshAdaptor);
            }

            bool hasItems = !m_meshServices.empty();
            service->setActorPropertyToUnclippedMaterial( hasItems );
        }
        else if ( eventId == vtkCommand::UserEvent )
        {
            for( ::visuVTKAdaptor::Mesh::wptr adaptor :  m_meshServices )
            {
                if (!adaptor.expired())
                {
                    adaptor.lock()->updateVisibility(*(bool*)data);
                }
            }
        }
    }

protected:

    ::visuVTKAdaptor::Mesh::wptr m_service;

    vtkPlaneCollection* m_planeCollectionSrc;
    ::fwData::Mesh::sptr m_mesh;

    std::vector< ::visuVTKAdaptor::Mesh::wptr > m_meshServices;

    std::vector< PlaneShifterCallback* > m_planeCallbacks;
    std::vector< vtkPlaneCollection* >   m_planeCollections;

    double m_factor;
};


//------------------------------------------------------------------------------

Mesh::Mesh() throw() :
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

    m_sigTextureApplied = TextureAppliedSignalType::New();
    ::fwCom::HasSignals::m_signals(s_TEXTURE_APPLIED_SIG, m_sigTextureApplied);

    m_slotUpdateVisibility     = ::fwCom::newSlot(&Mesh::updateVisibility, this);
    m_slotUpdatePointColors    = ::fwCom::newSlot(&Mesh::updatePointColors, this);
    m_slotUpdateCellColors     = ::fwCom::newSlot(&Mesh::updateCellColors, this);
    m_slotUpdateVertex         = ::fwCom::newSlot(&Mesh::updateVertex, this);
    m_slotUpdatePointNormals   = ::fwCom::newSlot(&Mesh::updatePointNormals, this);
    m_slotUpdateCellNormals    = ::fwCom::newSlot(&Mesh::updateCellNormals, this);
    m_slotUpdatePointTexCoords = ::fwCom::newSlot(&Mesh::updatePointTexCoords, this);
    m_slotUpdateCellTexCoords  = ::fwCom::newSlot(&Mesh::updateCellTexCoords, this);
    m_slotShowPointColors      = ::fwCom::newSlot(&Mesh::showPointColors, this);
    m_slotShowCellColors       = ::fwCom::newSlot(&Mesh::showCellColors, this);
    m_slotHideColors           = ::fwCom::newSlot(&Mesh::hideColors, this);
    m_slotUpdateColorMode      = ::fwCom::newSlot(&Mesh::updateColorMode, this);

    ::fwCom::HasSlots::m_slots(s_UPDATE_VISIBILITY_SLOT, m_slotUpdateVisibility)
        (s_UPDATE_POINT_COLORS_SLOT, m_slotUpdatePointColors)
        (s_UPDATE_CELL_COLORS_SLOT, m_slotUpdateCellColors)
        (s_UPDATE_VERTEX_SLOT, m_slotUpdateVertex)
        (s_UPDATE_POINT_NORMALS_SLOT, m_slotUpdatePointNormals)
        (s_UPDATE_CELL_NORMALS_SLOT, m_slotUpdateCellNormals)
        (s_UPDATE_POINT_TEX_COORDS_SLOT, m_slotUpdatePointTexCoords)
        (s_UPDATE_CELL_TEX_COORDS_SLOT, m_slotUpdateCellTexCoords)
        (s_SHOW_POINT_COLORS_SLOT, m_slotShowPointColors)
        (s_SHOW_CELL_COLORS_SLOT, m_slotShowCellColors)
        (s_HIDE_COLORS_SLOT, m_slotHideColors)
        (s_UPDATE_COLOR_MODE_SLOT, m_slotUpdateColorMode)
    ;

    ::fwCom::HasSlots::m_slots.setWorker( m_associatedWorker );
}

//------------------------------------------------------------------------------

Mesh::~Mesh() throw()
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

void Mesh::doConfigure() throw(fwTools::Failed)
{
    assert(m_configuration->getName() == "config");

    std::string color          = m_configuration->getAttributeValue("color");
    std::string unclippedColor = m_configuration->getAttributeValue("unclippedcolor");

    m_material->diffuse()->setRGBA(color.empty() ? "#ffffffff" : color );

    m_unclippedPartMaterial->diffuse()->setRGBA(unclippedColor.empty() ? "#aaaaff44" : unclippedColor );

    if (m_configuration->hasAttribute("autoresetcamera") )
    {
        std::string autoresetcamera = m_configuration->getAttributeValue("autoresetcamera");
        m_autoResetCamera = (autoresetcamera == "yes");
    }

    if(m_configuration->hasAttribute("uvgen"))
    {
        std::string uvGen = m_configuration->getAttributeValue ("uvgen");
        if(uvGen == "sphere")
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
    }

    if ( m_configuration->hasAttribute("texture") )
    {
        m_textureAdaptorUID = m_configuration->getAttributeValue("texture");
    }
}

//------------------------------------------------------------------------------

void Mesh::doUpdate() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    ::fwData::Mesh::sptr mesh = this->getObject < ::fwData::Mesh >();
    this->updateMesh( mesh );
}

//------------------------------------------------------------------------------

void Mesh::doStart() throw(fwTools::Failed)
{
    if(!m_textureAdaptorUID.empty())
    {
        ::fwRenderVTK::SRender::sptr renderService      = this->getRenderService();
        ::fwRenderVTK::IVtkAdaptorService::sptr adaptor = renderService->getAdaptor(m_textureAdaptorUID);
        ::visuVTKAdaptor::Texture::sptr textureAdaptor  = ::visuVTKAdaptor::Texture::dynamicCast(adaptor);

        SLM_ASSERT("textureAdaptor is NULL", textureAdaptor);
        m_connections.connect(this->getSptr(), s_TEXTURE_APPLIED_SIG, textureAdaptor,
                              ::visuVTKAdaptor::Texture::s_APPLY_TEXTURE_SLOT);
    }

    this->buildPipeline();
    m_transformService.lock()->start();
}

//------------------------------------------------------------------------------

void Mesh::doStop() throw(fwTools::Failed)
{
    m_transformService.lock()->stop();
    ::fwServices::OSR::unregisterService(m_transformService.lock());

    this->removeAllPropFromRenderer();
    if (this->getPicker())
    {
        this->removeFromPicker(m_actor);
    }

    removeNormalsService();
    removePlaneCollectionShifterCommand();
    removeServicesStarterCommand();

    this->unregisterServices();

    m_connections.disconnect();
}

//------------------------------------------------------------------------------

void Mesh::doSwap() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    m_transformService.lock()->stop();
    ::fwServices::OSR::unregisterService(m_transformService.lock());


    ::fwRenderVTK::IVtkAdaptorService::sptr materialService              = m_materialService.lock();
    ::fwRenderVTK::IVtkAdaptorService::sptr unclippedPartMaterialService = m_unclippedPartMaterialService.lock();

    this->setServiceOnMaterial(materialService, m_material);
    this->setServiceOnMaterial(unclippedPartMaterialService, m_unclippedPartMaterial);

    m_materialService              = materialService;
    m_unclippedPartMaterialService = unclippedPartMaterialService;

    ::fwData::Mesh::sptr mesh = this->getObject < ::fwData::Mesh >();

    this->createTransformService();
    m_transformService.lock()->start();
    this->updateMesh( mesh );
}

//------------------------------------------------------------------------------

void Mesh::createTransformService()
{
    ::fwData::Mesh::sptr mesh = this->getObject < ::fwData::Mesh >();

    if(!this->getTransformId().empty())
    {
        vtkTransform* t = m_renderService.lock()->getOrAddVtkTransform(m_transformId);
        m_transform->Concatenate(t);
    }

    ::fwData::TransformationMatrix3D::sptr fieldTransform;
    fieldTransform = mesh->setDefaultField("TransformMatrix", ::fwData::TransformationMatrix3D::New());

    vtkTransform* vtkFieldTransform = vtkTransform::New();
    vtkFieldTransform->Identity();
    m_transformService = ::visuVTKAdaptor::Transform::dynamicCast(
        ::fwServices::add< ::fwRenderVTK::IVtkAdaptorService > (
            fieldTransform,
            "::visuVTKAdaptor::Transform"
            )
        );
    SLM_ASSERT("Transform service NULL", m_transformService.lock());
    ::visuVTKAdaptor::Transform::sptr transformService = m_transformService.lock();

    transformService->setRenderService ( this->getRenderService()  );
    transformService->setRenderId      ( this->getRenderId()       );
    transformService->setAutoRender    ( this->getAutoRender()     );

    transformService->setTransform(vtkFieldTransform);
    m_transform->Concatenate(vtkFieldTransform);
    vtkFieldTransform->Delete();

    m_actor->SetUserTransform(m_transform);
}

//------------------------------------------------------------------------------

::fwData::Material::sptr Mesh::getMaterial() const
{
    return m_material;
}

//------------------------------------------------------------------------------

::fwData::Material::sptr Mesh::getUnclippedMaterial() const
{
    return m_unclippedPartMaterial;
}

//------------------------------------------------------------------------------

void Mesh::setActorPropertyToUnclippedMaterial(bool opt)
{
    ::visuVTKAdaptor::Material::sptr mat;

    if (opt)
    {
        SLM_ASSERT("Material service expired", !m_unclippedPartMaterialService.expired());
        mat = ::visuVTKAdaptor::Material::dynamicCast(m_unclippedPartMaterialService.lock());
    }
    else
    {
        SLM_ASSERT("Material service expired", !m_materialService.expired());
        mat = ::visuVTKAdaptor::Material::dynamicCast(m_materialService.lock());
    }

    SLM_ASSERT("Invalid Material Adaptor", mat);

    m_actor->SetProperty( mat->getVtkProperty() );
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void Mesh::setShowClippedPart(bool show)
{
    m_showClippedPart = show;
}

//------------------------------------------------------------------------------

void Mesh::setClippingPlanesId(::fwRenderVTK::SRender::VtkObjectIdType id)
{
    m_clippingPlanesId = id;
}

//------------------------------------------------------------------------------

void Mesh::setServiceOnMaterial(::fwRenderVTK::IVtkAdaptorService::sptr& srv, ::fwData::Material::sptr material)
{
    if (!srv)
    {
        srv = ::fwServices::add< ::fwRenderVTK::IVtkAdaptorService > (material, "::visuVTKAdaptor::Material");
        SLM_ASSERT("srv not instanced", srv);

        srv->setRenderService(this->getRenderService());
        srv->setAutoRender( this->getAutoRender() );
        srv->start();
        srv->update();
        this->registerService(srv);
    }
    else if (srv->getObject< ::fwData::Material >() != material)
    {
        srv->swap(material);
    }
}

//------------------------------------------------------------------------------

void Mesh::setMaterial(::fwData::Material::sptr material)
{
    m_material = material;
}

//------------------------------------------------------------------------------

void Mesh::setUnclippedPartMaterial(::fwData::Material::sptr material)
{
    m_unclippedPartMaterial = material;
}

//------------------------------------------------------------------------------

void Mesh::updateOptionsMode()
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

void Mesh::createNormalsService()
{
    if ( m_normalsService.expired() )
    {
        ::fwData::Mesh::sptr mesh = this->getObject < ::fwData::Mesh >();

        ::fwRenderVTK::IVtkAdaptorService::sptr service =
            ::fwServices::add< ::fwRenderVTK::IVtkAdaptorService >(
                mesh,
                "::visuVTKAdaptor::MeshNormals"
                );
        SLM_ASSERT("service not instanced", service);

        service->setRenderService( this->getRenderService() );
        service->setRenderId     ( this->getRenderId()      );
        service->setPickerId     ( this->getPickerId()      );
        service->setAutoRender   ( this->getAutoRender()    );
        ::visuVTKAdaptor::MeshNormals::dynamicCast(service)->setPolyData( m_polyData );
        service->start();

        m_normalsService = service;
    }
}

//------------------------------------------------------------------------------

void Mesh::removeNormalsService()
{
    if ( !m_normalsService.expired() )
    {
        m_normalsService.lock()->stop();
        ::fwServices::OSR::unregisterService(m_normalsService.lock());
        m_normalsService.reset();
    }
}

//------------------------------------------------------------------------------

void Mesh::buildPipeline()
{
    ::fwData::Mesh::sptr mesh = this->getObject < ::fwData::Mesh >();

    if (!m_clippingPlanesId.empty())
    {
        vtkObject* o               = this->getVtkObject(m_clippingPlanesId);
        vtkPlaneCollection* planes = vtkPlaneCollection::SafeDownCast(o);
        this->setVtkClippingPlanes( planes );
    }

    ::fwRenderVTK::IVtkAdaptorService::sptr materialService;
    ::fwRenderVTK::IVtkAdaptorService::sptr unclippedPartMaterialService;

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

void Mesh::updateMesh( ::fwData::Mesh::sptr mesh )
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

    if(!m_textureAdaptorUID.empty())
    {
        ::fwData::Material::sptr material = this->getMaterial();
        SLM_ASSERT("Missing material", material);

        m_sigTextureApplied->asyncEmit(material);
    }

    if (m_autoResetCamera)
    {
        this->getRenderer()->ResetCamera();
    }
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

vtkActor* Mesh::newActor()
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

void Mesh::updateVisibility( bool isVisible)
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

bool Mesh::getVisibility() const
{
    bool visible = false;
    if (m_actor)
    {
        visible = m_actor->GetVisibility() == 1;
    }
    return visible;
}

//------------------------------------------------------------------------------

void Mesh::setVtkClippingPlanes(vtkPlaneCollection* planes)
{
    m_clippingPlanes = planes;
}

//------------------------------------------------------------------------------

void Mesh::removePlaneCollectionShifterCommand()
{
    if (m_planeCollectionShifterCallback)
    {
        m_planeCollectionShifterCallback->Stop();
        m_planeCollectionShifterCallback->Delete();
        m_planeCollectionShifterCallback = 0;
    }
}

//------------------------------------------------------------------------------

void Mesh::createServicesStarterCommand()
{
    if(!m_servicesStarterCallback)
    {
        ::visuVTKAdaptor::Mesh::sptr srv =
            ::visuVTKAdaptor::Mesh::dynamicCast(
                this->getSptr()
                );
        m_servicesStarterCallback = PlaneCollectionAdaptorStarter::New( srv, m_clippingPlanes, -1. );
    }
}

//------------------------------------------------------------------------------

void Mesh::removeServicesStarterCommand()
{
    if(m_servicesStarterCallback)
    {
        m_servicesStarterCallback->Stop();
        m_servicesStarterCallback->Delete();
        m_servicesStarterCallback = 0;
    }
}

//------------------------------------------------------------------------------

void Mesh::setAutoResetCamera(bool autoResetCamera)
{
    m_autoResetCamera = autoResetCamera;
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsType Mesh::getObjSrvConnections() const
{
    KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwData::Mesh::s_MODIFIED_SIG, s_UPDATE_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Mesh::s_POINT_COLORS_MODIFIED_SIG, s_UPDATE_POINT_COLORS_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Mesh::s_CELL_COLORS_MODIFIED_SIG, s_UPDATE_CELL_COLORS_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Mesh::s_VERTEX_MODIFIED_SIG,
                                           s_UPDATE_VERTEX_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Mesh::s_POINT_NORMALS_MODIFIED_SIG,
                                           s_UPDATE_POINT_NORMALS_SLOT) );
    connections.push_back( std::make_pair( ::fwData::Mesh::s_CELL_NORMALS_MODIFIED_SIG,
                                           s_UPDATE_CELL_NORMALS_SLOT) );
    connections.push_back( std::make_pair( ::fwData::Mesh::s_POINT_TEX_COORDS_MODIFIED_SIG,
                                           s_UPDATE_POINT_TEX_COORDS_SLOT) );
    connections.push_back( std::make_pair( ::fwData::Mesh::s_CELL_TEX_COORDS_MODIFIED_SIG,
                                           s_UPDATE_CELL_TEX_COORDS_SLOT) );

    return connections;
}

//------------------------------------------------------------------------------

void Mesh::updatePointColors()
{
    ::fwData::Mesh::sptr mesh = this->getObject < ::fwData::Mesh >();
    SLM_ASSERT("m_polyData not instanced", m_polyData);

    ::fwVtkIO::helper::Mesh::updatePolyDataPointColor(m_polyData, mesh);
    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void Mesh::updateCellColors()
{
    ::fwData::Mesh::sptr mesh = this->getObject < ::fwData::Mesh >();
    SLM_ASSERT("m_polyData not instanced", m_polyData);

    ::fwVtkIO::helper::Mesh::updatePolyDataCellColor(m_polyData, mesh);
    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void Mesh::updateVertex()
{
    ::fwData::Mesh::sptr mesh = this->getObject < ::fwData::Mesh >();
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

void Mesh::updatePointNormals()
{
    ::fwData::Mesh::sptr mesh = this->getObject < ::fwData::Mesh >();
    ::fwVtkIO::helper::Mesh::updatePolyDataPointNormals(m_polyData, mesh);
    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void Mesh::updateCellNormals()
{
    ::fwData::Mesh::sptr mesh = this->getObject < ::fwData::Mesh >();
    ::fwVtkIO::helper::Mesh::updatePolyDataCellNormals(m_polyData, mesh);
    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void Mesh::updatePointTexCoords()
{
    ::fwData::Mesh::sptr mesh = this->getObject < ::fwData::Mesh >();
    ::fwVtkIO::helper::Mesh::updatePolyDataPointTexCoords(m_polyData, mesh);
    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void Mesh::updateCellTexCoords()
{
    ::fwData::Mesh::sptr mesh = this->getObject < ::fwData::Mesh >();
    ::fwVtkIO::helper::Mesh::updatePolyDataCellTexCoords(m_polyData, mesh);
    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void Mesh::showPointColors()
{
    m_mapper->ScalarVisibilityOn();
    m_mapper->SetScalarModeToUsePointData();
    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void Mesh::showCellColors()
{
    m_mapper->ScalarVisibilityOn();
    m_mapper->SetScalarModeToUseCellData();
    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void Mesh::hideColors()
{
    m_mapper->ScalarVisibilityOff();
    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void Mesh::updateColorMode(std::uint8_t mode)
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

void Mesh::updateNormalMode(std::uint8_t mode)
{
    this->updateOptionsMode();
    if (!m_normalsService.expired())
    {
        ::visuVTKAdaptor::MeshNormals::sptr normalsAdaptor =
            ::visuVTKAdaptor::MeshNormals::dynamicCast(m_normalsService.lock());
        normalsAdaptor->updateNormalMode(mode);
    }
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor




