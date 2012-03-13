/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwTools/fwID.hpp>

#include <fwData/Material.hpp>
#include <fwData/Mesh.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/Factory.hpp>

#include <fwComEd/MaterialMsg.hpp>
#include <fwComEd/MeshMsg.hpp>

#include <vtkIO/vtk.hpp>
#include <vtkIO/helper/Mesh.hpp>

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
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTransform.h>

#include "visuVTKAdaptor/Material.hpp"
#include "visuVTKAdaptor/MeshNormals.hpp"

#include "visuVTKAdaptor/Transform.hpp"
#include "visuVTKAdaptor/Mesh.hpp"


REGISTER_SERVICE( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::Mesh, ::fwData::Mesh ) ;

namespace visuVTKAdaptor
{


class MeshVtkCommand : public vtkCommand
{
public:
    virtual void Stop() = 0;
};


//------------------------------------------------------------------------------

class PlaneShifterCallback : public MeshVtkCommand
{
    public:

    static PlaneShifterCallback *New( vtkPlane *src,  vtkPlane *dst, double factor = 1.)
    { return new PlaneShifterCallback( src, dst, factor ); }

    //--------------------------------------------------------------------------
    PlaneShifterCallback( vtkPlane *src,  vtkPlane *dst, double factor )
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


    virtual void Execute( vtkObject *caller, unsigned long eventId, void * )
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
        vtkPlane *m_planeSrc;
        vtkPlane *m_planeDst;
        double    m_factor;
};



class PlaneCollectionShifterCallback : public MeshVtkCommand
{
    public:

    static PlaneCollectionShifterCallback *New(
            vtkPlaneCollection *src,
            vtkPlaneCollection *dst,
            double factor = 1. )
    { return new PlaneCollectionShifterCallback( src, dst, factor ); }

    //--------------------------------------------------------------------------
    PlaneCollectionShifterCallback(
            vtkPlaneCollection *src,
            vtkPlaneCollection *dst,
            double factor)
    : m_planeCollectionSrc(src),
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

        BOOST_FOREACH( PlaneShifterCallback *psc, m_planeCallbacks )
        {
            psc->Stop();
            psc->Delete();
            psc=0;
        }
        m_planeCallbacks.clear();
    }

    virtual void Execute( vtkObject *caller, unsigned long eventId, void * )
    {
        if (eventId == vtkCommand::ModifiedEvent)
        {
            this->Clear();

            vtkPlane *plane = NULL;
            for (  m_planeCollectionSrc->InitTraversal();
                  (plane=m_planeCollectionSrc->GetNextItem());
                )
            {
                vtkPlane *newPlane = vtkPlane::New();
                m_planeCollectionDst->AddItem(newPlane);
                m_planeCallbacks.push_back(PlaneShifterCallback::New(plane, newPlane, m_factor));
                newPlane->Modified();
                newPlane->Delete();
            }
        }
    }

    protected:
        vtkPlaneCollection *m_planeCollectionSrc;
        vtkPlaneCollection *m_planeCollectionDst;

        std::vector< PlaneShifterCallback* > m_planeCallbacks;
        double    m_factor;
};



class PlaneCollectionAdaptorStarter : public MeshVtkCommand
{
    public:

    static PlaneCollectionAdaptorStarter *New(
            ::visuVTKAdaptor::Mesh::sptr service,
            vtkPlaneCollection *src,
            double factor = 1. )
    {
        return new PlaneCollectionAdaptorStarter( service, src, factor );
    }

    //--------------------------------------------------------------------------
    PlaneCollectionAdaptorStarter(
            ::visuVTKAdaptor::Mesh::sptr service,
            vtkPlaneCollection *src,
            double factor)
    : m_service(service),
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
        BOOST_FOREACH( ::visuVTKAdaptor::Mesh::wptr adaptor, m_meshServices )
        {
            if (!adaptor.expired())
            {
                adaptor.lock()->stop();
                ::fwServices::OSR::unregisterService(adaptor.lock());
            }
        }
        m_meshServices.clear();

        BOOST_FOREACH( PlaneShifterCallback *psc, m_planeCallbacks )
        {
            psc->Stop();
            psc->Delete();
            psc=0;
        }
        m_planeCallbacks.clear();

        BOOST_FOREACH( vtkPlaneCollection *planeCol, m_planeCollections )
        {
            planeCol->Delete();
        }
        m_planeCollections.clear();

        if (!m_service.expired())
        {
            m_service.lock()->setActorPropertyToUnclippedMaterial( false );
        }
    }

    virtual void Execute( vtkObject *caller, unsigned long eventId, void * data)
    {
        ::visuVTKAdaptor::Mesh::sptr service;

        if (m_service.expired())
        {
            this->Stop();
            this->Delete();
            return ;
        }

        if (eventId == vtkCommand::ModifiedEvent)
        {
            service = m_service.lock();

            this->Clear();

            vtkPlane *plane = NULL;
            for (  m_planeCollectionSrc->InitTraversal();
                    (plane = m_planeCollectionSrc->GetNextItem());
                )
            {
                vtkPlane *newPlane = vtkPlane::New();
                m_planeCallbacks.push_back(PlaneShifterCallback::New(plane, newPlane, m_factor));

                vtkPlaneCollection *newCollection = vtkPlaneCollection::New();
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

                meshAdaptor->start();
                meshAdaptor->updateVisibility ( service->getVisibility()     );

                newPlane->Delete();

                m_planeCollections.push_back(newCollection);
                m_meshServices.push_back(meshAdaptor);
            }

            bool hasItems = m_meshServices.size() > 0;
            service->setActorPropertyToUnclippedMaterial( hasItems );
        }
        else if ( eventId == vtkCommand::UserEvent )
        {
            BOOST_FOREACH( ::visuVTKAdaptor::Mesh::wptr adaptor, m_meshServices )
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

        vtkPlaneCollection *m_planeCollectionSrc;
        ::fwData::Mesh::sptr m_mesh;

        std::vector< ::visuVTKAdaptor::Mesh::wptr > m_meshServices;

        std::vector< PlaneShifterCallback* > m_planeCallbacks;
        std::vector< vtkPlaneCollection* >   m_planeCollections;

        double    m_factor;
};


//------------------------------------------------------------------------------

Mesh::Mesh() throw()
{
    m_material               = ::fwData::Material::New();
    m_unclippedPartMaterial  = ::fwData::Material::New();
    m_unclippedPartMaterial->ambient()->setRGBA("#aaaaff44");

    m_clippingPlanesId  = "";

    m_showClippedPart   = false;
    m_clippingPlanes    = 0;
    m_actor             = 0;

    m_polyData          = 0;
    m_mapper            = vtkPolyDataMapper::New();

    m_autoResetCamera   = true;

    m_planeCollectionShifterCallback = 0;
    m_servicesStarterCallback        = 0;

    m_transform = vtkTransform::New();

    addNewHandledEvent (::fwComEd::MaterialMsg::MATERIAL_IS_MODIFIED );
    addNewHandledEvent (::fwComEd::MeshMsg::NEW_MESH );
    addNewHandledEvent (::fwComEd::MeshMsg::VERTEX_MODIFIED );
    addNewHandledEvent (::fwComEd::MeshMsg::POINT_COLORS_MODIFIED );
    addNewHandledEvent (::fwComEd::MeshMsg::CELL_COLORS_MODIFIED );
    addNewHandledEvent (::fwComEd::MeshMsg::POINT_NORMALS_MODIFIED );
    addNewHandledEvent (::fwComEd::MeshMsg::CELL_NORMALS_MODIFIED );
    addNewHandledEvent ("SHOW_POINT_COLORS");
    addNewHandledEvent ("SHOW_CELL_COLORS");
    addNewHandledEvent ("HIDE_COLORS");
}

//------------------------------------------------------------------------------

Mesh::~Mesh() throw()
{
    m_clippingPlanes = 0;

    m_mapper->Delete();
    m_mapper = 0;

    m_transform->Delete();
    m_transform = 0;

    if(m_actor)
    {
        m_actor->Delete();
        m_actor = 0;
    }

    if (m_polyData)
    {
        m_polyData->Delete();
        m_polyData = 0;
    }
}

//------------------------------------------------------------------------------

void Mesh::configuring() throw(fwTools::Failed)
{
    assert(m_configuration->getName() == "config");

    std::string color = m_configuration->getAttributeValue("color");
    std::string unclippedColor = m_configuration->getAttributeValue("unclippedcolor");

    m_material->ambient()->setRGBA(color.empty() ? "#ffffffff" : color );

    m_unclippedPartMaterial->ambient()->setRGBA(unclippedColor.empty() ? "#aaaaff44" : unclippedColor );

    if (m_configuration->hasAttribute("autoresetcamera") )
    {
        std::string autoresetcamera = m_configuration->getAttributeValue("autoresetcamera");
        m_autoResetCamera = (autoresetcamera == "yes");
    }

    this->setPickerId    ( m_configuration->getAttributeValue ( "picker"    ) );
    this->setRenderId    ( m_configuration->getAttributeValue ( "renderer"  ) );
    this->setTransformId ( m_configuration->getAttributeValue ( "transform" ) );
}

//------------------------------------------------------------------------------

void Mesh::doUpdate() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    ::fwData::Mesh::sptr mesh = this->getObject < ::fwData::Mesh >();
    this->updateMesh( mesh );
}

//------------------------------------------------------------------------------

void Mesh::doUpdate( ::fwServices::ObjectMsg::csptr msg ) throw(::fwTools::Failed)
{
    ::fwComEd::MaterialMsg::csptr materialMsg = ::fwComEd::MaterialMsg::dynamicConstCast(msg);
    ::fwComEd::MeshMsg::csptr meshMsg = ::fwComEd::MeshMsg::dynamicConstCast(msg);

    if( materialMsg && materialMsg->hasEvent(::fwComEd::MaterialMsg::MATERIAL_IS_MODIFIED) )
    {
        this->updateOptionsMode();
    }

    if( meshMsg && meshMsg->hasEvent(::fwComEd::MeshMsg::NEW_MESH) )
    {
        ::fwData::Mesh::sptr mesh = this->getObject < ::fwData::Mesh >();
        this->updateMesh( mesh );
    }
    if( meshMsg && meshMsg->hasEvent(::fwComEd::MeshMsg::POINT_COLORS_MODIFIED))
    {
        ::fwData::Mesh::sptr mesh = this->getObject < ::fwData::Mesh >();
        SLM_ASSERT("m_polyData not instanced", m_polyData);

        ::vtkIO::helper::Mesh::updatePolyDataPointColor(m_polyData, mesh);
        this->setVtkPipelineModified();
    }
    if( meshMsg && meshMsg->hasEvent(::fwComEd::MeshMsg::CELL_COLORS_MODIFIED))
    {
        ::fwData::Mesh::sptr mesh = this->getObject < ::fwData::Mesh >();
        SLM_ASSERT("m_polyData not instanced", m_polyData);

        ::vtkIO::helper::Mesh::updatePolyDataCellColor(m_polyData, mesh);
        this->setVtkPipelineModified();
    }
    if( meshMsg && meshMsg->hasEvent(::fwComEd::MeshMsg::VERTEX_MODIFIED) )
    {
       ::fwData::Mesh::sptr mesh = this->getObject < ::fwData::Mesh >();
       SLM_ASSERT("m_polyData not instanced", m_polyData);

       ::vtkIO::helper::Mesh::updatePolyDataPoints(m_polyData, mesh);
       this->setVtkPipelineModified();
    }
    if( meshMsg && meshMsg->hasEvent(::fwComEd::MeshMsg::POINT_NORMALS_MODIFIED))
    {
        ::fwData::Mesh::sptr mesh = this->getObject < ::fwData::Mesh >();
        ::vtkIO::helper::Mesh::updatePolyDataPointNormals(m_polyData, mesh);
        this->setVtkPipelineModified();
    }
    if( meshMsg && meshMsg->hasEvent(::fwComEd::MeshMsg::CELL_NORMALS_MODIFIED))
    {
        ::fwData::Mesh::sptr mesh = this->getObject < ::fwData::Mesh >();
        ::vtkIO::helper::Mesh::updatePolyDataCellNormals(m_polyData, mesh);
        this->setVtkPipelineModified();
    }
    if (meshMsg && meshMsg->hasEvent("SHOW_POINT_COLORS"))
    {
        m_mapper->ScalarVisibilityOn();
        m_mapper->SetScalarModeToUsePointData();
        this->setVtkPipelineModified();
    }
    else if (meshMsg && meshMsg->hasEvent("SHOW_CELL_COLORS"))
    {
        m_mapper->ScalarVisibilityOn();
        m_mapper->SetScalarModeToUseCellData();
        this->setVtkPipelineModified();
    }
    else if (meshMsg && meshMsg->hasEvent("HIDE_COLORS"))
    {
        m_mapper->ScalarVisibilityOff();
        this->setVtkPipelineModified();
    }
}

//------------------------------------------------------------------------------

void Mesh::doStart() throw(fwTools::Failed)
{
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
}

//------------------------------------------------------------------------------

void Mesh::doSwap() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    m_transformService.lock()->stop();
    ::fwServices::OSR::unregisterService(m_transformService.lock());


    ::fwRenderVTK::IVtkAdaptorService::sptr materialService = m_materialService.lock();
    ::fwRenderVTK::IVtkAdaptorService::sptr unclippedPartMaterialService = m_unclippedPartMaterialService.lock();

    this->setServiceOnMaterial(materialService, m_material);
    this->setServiceOnMaterial(unclippedPartMaterialService , m_unclippedPartMaterial);

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
        m_transform->Concatenate(this->getTransform());
    }

    ::fwData::TransformationMatrix3D::sptr fieldTransform;
    fieldTransform = mesh->setDefaultField_NEWAPI("TransformMatrix", ::fwData::TransformationMatrix3D::New());

    vtkTransform *vtkFieldTransform = vtkTransform::New();
    vtkFieldTransform->Identity();
    m_transformService = ::visuVTKAdaptor::Transform::dynamicCast(
        ::fwServices::add< ::fwRenderVTK::IVtkAdaptorService > (
                fieldTransform,
                "::visuVTKAdaptor::Transform" 
                )
        );
    assert(m_transformService.lock());
    ::visuVTKAdaptor::Transform::sptr transformService = m_transformService.lock();

    transformService->setRenderService ( this->getRenderService()  );
    transformService->setRenderId      ( this->getRenderId()       );

    transformService->setTransform(vtkFieldTransform);
    m_transform->Concatenate(vtkFieldTransform);
    vtkFieldTransform->Delete();

    m_actor->SetUserTransform(m_transform);
}

//------------------------------------------------------------------------------

::fwData::Material::sptr Mesh::getMaterial()
{
    return m_material;
}

//------------------------------------------------------------------------------

::fwData::Material::sptr Mesh::getUnclippedMaterial()
{
    return m_unclippedPartMaterial;
}

//------------------------------------------------------------------------------

void Mesh::setActorPropertyToUnclippedMaterial(bool opt)
{
    ::visuVTKAdaptor::Material::sptr mat;

    if (opt)
    {
        assert(!m_unclippedPartMaterialService.expired());
        mat = ::visuVTKAdaptor::Material::dynamicCast(
                m_unclippedPartMaterialService.lock()
                );
    }
    else
    {
        assert(!m_materialService.expired());
        mat = ::visuVTKAdaptor::Material::dynamicCast(
                m_materialService.lock()
                );
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

void Mesh::setClippingPlanesId(::fwRenderVTK::VtkRenderService::VtkObjectIdType id)
{
    m_clippingPlanesId = id;
}

//------------------------------------------------------------------------------

void Mesh::setServiceOnMaterial(::fwRenderVTK::IVtkAdaptorService::sptr &srv, ::fwData::Material::sptr material)
{
    if (! srv)
    {
        srv = ::fwServices::add< ::fwRenderVTK::IVtkAdaptorService > (
            material, "::visuVTKAdaptor::Material"
            );
        SLM_ASSERT("srv not instanced", srv);

        srv->setRenderService(this->getRenderService());
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
    if (m_material->getOptionsMode() == ::fwData::Material::MODE_NORMALS)
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
        vtkObject          *o      = this->getVtkObject(m_clippingPlanesId);
        vtkPlaneCollection *planes = vtkPlaneCollection::SafeDownCast(o);
        this->setVtkClippingPlanes( planes );
    }

    ::fwRenderVTK::IVtkAdaptorService::sptr materialService;
    ::fwRenderVTK::IVtkAdaptorService::sptr unclippedPartMaterialService;

    this->setServiceOnMaterial(materialService, m_material);
    this->setServiceOnMaterial(unclippedPartMaterialService , m_unclippedPartMaterial);

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
    ::vtkIO::helper::Mesh::toVTKMesh(mesh, m_polyData);
    m_mapper->SetInput(m_polyData);

    if (m_autoResetCamera)
    {
        this->getRenderer()->ResetCamera();
    }
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

vtkActor *Mesh::newActor()
{
    vtkActor *actor = vtkActor::New();
    m_mapper->SetInput(m_polyData);

    if (m_clippingPlanes)
    {
        vtkPlaneCollection *newClippingPlanes = vtkPlaneCollection::New();

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
}

//------------------------------------------------------------------------------

bool Mesh::getVisibility()
{
    bool visible = false;
    if (m_actor)
    {
        visible = m_actor->GetVisibility() == 1;
    }
    return visible;
}

//------------------------------------------------------------------------------

void Mesh::setVtkClippingPlanes(vtkPlaneCollection *planes)
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

} //namespace visuVTKAdaptor



