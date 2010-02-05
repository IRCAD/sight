/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwTools/UUID.hpp>

#include <fwData/Material.hpp>
#include <fwData/TriangularMesh.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/Factory.hpp>

#include <fwComEd/MaterialMsg.hpp>

#include <vtkIO/vtk.hpp>

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
#include "visuVTKAdaptor/Normals.hpp"
#include "visuVTKAdaptor/TriangularMesh.hpp"


REGISTER_SERVICE( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::TriangularMesh, ::fwData::TriangularMesh ) ;

namespace visuVTKAdaptor
{


class TriangularMeshVtkCommand : public vtkCommand
{
public:
    virtual void Stop() = 0;
};


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#ifndef USE_DEPTH_PEELING // replacement for depth peeling
#ifdef USE_DEPTH_SORT
class vtkDepthSortCallBack : public vtkCommand
{

public :
    //--------------------------------------------------------------------------
    static vtkDepthSortCallBack *New( TriangularMesh *service)
    { return new vtkDepthSortCallBack(service); }

    //--------------------------------------------------------------------------
    vtkDepthSortCallBack( TriangularMesh *service )
    : m_service(service), m_needUpdate(true)
    {
        m_oldDirection[0] = m_oldDirection[1] = 0.;
        m_oldDirection[2] = 1.;
    }

    //--------------------------------------------------------------------------
    virtual void Execute( vtkObject *caller, unsigned long eventId, void *)
    {

        double direction[3];
        m_service->getRenderer()->GetActiveCamera()->GetDirectionOfProjection(direction);

        vtkMath::Normalize(direction);
        double dot = vtkMath::Dot(direction, m_oldDirection);

        m_needUpdate = ( dot < 0.5 );

        if (m_needUpdate)
        {
            m_service->getRenderer()->GetActiveCamera()->GetDirectionOfProjection(m_oldDirection);
            vtkMath::Normalize(m_oldDirection);

//          if ( m_needUpdate && eventId == vtkCommand::EndInteractionEvent)
//          {
            m_service->getDepthSort()->SetDirectionToSpecifiedVector ();
            m_service->getDepthSort()->SetVector(m_service->getRenderer()->GetActiveCamera()->GetDirectionOfProjection());
            m_needUpdate = false;
//            }
        }
    }

protected :
    TriangularMesh *m_service;
    double m_oldDirection[3];
    bool m_needUpdate;


};
#endif //ifdef USE_DEPTH_SORT
#endif //ifndef USE_DEPTH_PEELING


class PlaneShifterCallback : public TriangularMeshVtkCommand
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



class PlaneCollectionShifterCallback : public TriangularMeshVtkCommand
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



class PlaneCollectionAdaptorStarter : public TriangularMeshVtkCommand
{
    public:

    static PlaneCollectionAdaptorStarter *New(
            ::visuVTKAdaptor::TriangularMesh::sptr service,
            vtkPlaneCollection *src,
            double factor = 1. )
    {
        return new PlaneCollectionAdaptorStarter( service, src, factor );
    }

    //--------------------------------------------------------------------------
    PlaneCollectionAdaptorStarter(
            ::visuVTKAdaptor::TriangularMesh::sptr service,
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
        BOOST_FOREACH( ::visuVTKAdaptor::TriangularMesh::wptr adaptor, m_meshServices )
        {
            if (!adaptor.expired())
            {
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
        ::visuVTKAdaptor::TriangularMesh::sptr service;

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
                            "::visuVTKAdaptor::TriangularMesh" );

                ::visuVTKAdaptor::TriangularMesh::sptr meshAdaptor = TriangularMesh::dynamicCast(meshService);

                meshAdaptor->setRenderService ( service->getRenderService()  );
                meshAdaptor->setRenderId      ( service->getRenderId()       );
                meshAdaptor->setPickerId      ( service->getPickerId()       );
                meshAdaptor->setMapperInput   ( service->getMapperInput()    );
                meshAdaptor->setSharpEdgeAngle( service->getSharpEdgeAngle() );
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
            BOOST_FOREACH( ::visuVTKAdaptor::TriangularMesh::wptr adaptor, m_meshServices )
            {
                if (!adaptor.expired())
                {
                    adaptor.lock()->updateVisibility(*(bool*)data);
                }
            }
        }
    }

    protected:

        ::visuVTKAdaptor::TriangularMesh::wptr m_service;

        vtkPlaneCollection *m_planeCollectionSrc;
        ::fwData::TriangularMesh::sptr m_mesh;

        std::vector< ::visuVTKAdaptor::TriangularMesh::wptr > m_meshServices;

        std::vector< PlaneShifterCallback* > m_planeCallbacks;
        std::vector< vtkPlaneCollection* >   m_planeCollections;

        double    m_factor;
};







//------------------------------------------------------------------------------

TriangularMesh::TriangularMesh() throw()
{
    m_material               = ::fwData::Material::New();
    m_unclippedPartMaterial  = ::fwData::Material::New();
    m_unclippedPartMaterial->ambient().setRGBA("#aaaaff44");

    m_clippingPlanesId       = "";
    m_sharpEdgeAngle         = 180;

    m_showClippedPart        = false;
    m_clippingPlanes         = 0;
    m_actor                  = 0;
    m_normals                = vtkPolyDataNormals::New();

    m_manageMapperInput     = true;
    m_mapperInput            = 0;

    m_planeCollectionShifterCallback = 0;
    m_servicesStarterCallback        = 0;

#ifndef USE_DEPTH_PEELING // replacement for depth peeling
#ifdef USE_DEPTH_SORT
    m_depthSortCommand = 0;
    m_depthSort        = vtkDepthSortPolyData::New();
    m_hasAlpha         = false;
#endif
#endif

    addNewHandledEvent (::fwComEd::MaterialMsg::MATERIAL_IS_MODIFIED );
}

//------------------------------------------------------------------------------

TriangularMesh::~TriangularMesh() throw()
{
    m_clippingPlanes = 0;
#ifndef USE_DEPTH_PEELING // replacement for depth peeling
#ifdef USE_DEPTH_SORT
    m_depthSort->Delete();
    m_depthSort = 0;
#endif
#endif

    m_normals->Delete();
    m_normals = 0;

    if(m_actor)
    {
        m_actor->Delete();
        m_actor = 0;
    }
}

//------------------------------------------------------------------------------

void TriangularMesh::configuring() throw(fwTools::Failed)
{
    assert(m_configuration->getName() == "config");

    std::string color = m_configuration->getAttributeValue("color");
    std::string unclippedColor = m_configuration->getAttributeValue("unclippedcolor");

    m_material->ambient().setRGBA(color.empty() ? "#ffffffff" : color );

    m_unclippedPartMaterial->ambient().setRGBA(unclippedColor.empty() ? "#aaaaff44" : unclippedColor );

    this->setPickerId    ( m_configuration->getAttributeValue ( "picker"    ) );
    this->setRenderId    ( m_configuration->getAttributeValue ( "renderer"  ) );
    this->setTransformId ( m_configuration->getAttributeValue ( "transform" ) );
}

//------------------------------------------------------------------------------

void TriangularMesh::doUpdate() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void TriangularMesh::doUpdate( ::fwServices::ObjectMsg::csptr msg ) throw(::fwTools::Failed)
{
    ::fwComEd::MaterialMsg::csptr materialMsg = ::fwComEd::MaterialMsg::dynamicConstCast(msg);
    if( materialMsg && materialMsg->hasEvent(::fwComEd::MaterialMsg::MATERIAL_IS_MODIFIED) )
    {
        this->updateOptionsMode();
    }
}

//------------------------------------------------------------------------------

void TriangularMesh::doStart() throw(fwTools::Failed)
{
    this->buildPipeline();

}

//------------------------------------------------------------------------------

void TriangularMesh::doStop() throw(fwTools::Failed)
{
    this->removeAllPropFromRenderer();
    if (this->getPicker())
    {
        this->removeFromPicker(m_actor);
    }

    removeNormalsService();
    removePlaneCollectionShifterCommand();
    removeServicesStarterCommand();

#ifndef USE_DEPTH_PEELING // replacement for depth peeling
#ifdef USE_DEPTH_SORT
    removeDepthSortCommand();
#endif
#endif

    this->unregisterServices();
}


//------------------------------------------------------------------------------

void TriangularMesh::doSwap() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::fwData::TriangularMesh::sptr triangularMesh
        = this->getObject < ::fwData::TriangularMesh >();
    this->updateTriangularMesh( triangularMesh );
}

//------------------------------------------------------------------------------

::fwData::Material::sptr TriangularMesh::getMaterial()
{
    return m_material;
}

//------------------------------------------------------------------------------

::fwData::Material::sptr TriangularMesh::getUnclippedMaterial()
{
    return m_unclippedPartMaterial;
}

//------------------------------------------------------------------------------

void TriangularMesh::setActorPropertyToUnclippedMaterial(bool opt)
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

void TriangularMesh::setShowClippedPart(bool show)
{
    m_showClippedPart = show;
}

//------------------------------------------------------------------------------

void TriangularMesh::setClippingPlanesId(::fwRenderVTK::VtkRenderService::VtkObjectIdType id)
{
    m_clippingPlanesId = id;
}

//------------------------------------------------------------------------------

void TriangularMesh::setSharpEdgeAngle(double angle)
{
    m_sharpEdgeAngle = angle;
}

//------------------------------------------------------------------------------

double TriangularMesh::getSharpEdgeAngle()
{
    return m_sharpEdgeAngle;
}

//------------------------------------------------------------------------------

vtkAlgorithmOutput *TriangularMesh::getMapperInput()
{
    return m_mapperInput;
}

//------------------------------------------------------------------------------

void TriangularMesh::setServiceOnMaterial(::fwRenderVTK::IVtkAdaptorService::sptr &srv, ::fwData::Material::sptr material)
{

    if (! srv)
    {
        srv = ::fwServices::add< ::fwRenderVTK::IVtkAdaptorService > (
            material, "::visuVTKAdaptor::Material"
            );
        assert(srv);

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

void TriangularMesh::setMaterial(::fwData::Material::sptr material)
{
    m_material = material;
}

void TriangularMesh::setUnclippedPartMaterial(::fwData::Material::sptr material)
{
    m_unclippedPartMaterial = material;
}


void TriangularMesh::setMapperInput(vtkAlgorithmOutput *input)
{
    if (input)
    {
        m_mapperInput = input;
        m_manageMapperInput = false;
    }
}

//------------------------------------------------------------------------------

void TriangularMesh::updateOptionsMode()
{
    if (m_material->getOptionsMode() == ::fwData::Material::MODE_NORMALS)
    {
        createNormalsService();
    }
    else
    {
        removeNormalsService();
    }
}

void TriangularMesh::createNormalsService()
{
    ::fwData::TriangularMesh::sptr TriangularMesh
        = this->getObject < ::fwData::TriangularMesh >();


    if(m_mapperInput)
    {
        ::fwRenderVTK::IVtkAdaptorService::sptr service =
            ::fwServices::add< ::fwRenderVTK::IVtkAdaptorService >(
                    TriangularMesh,
                    "::visuVTKAdaptor::Normals"
                    );
        assert(service);

        service->setRenderService( this->getRenderService() );
        service->setRenderId     ( this->getRenderId()      );
        service->setPickerId     ( this->getPickerId()      );
        ::visuVTKAdaptor::Normals::dynamicCast(service)->setMapperInput( m_mapperInput );
        service->start();


        m_normalsService = service;
    }
}

void TriangularMesh::removeNormalsService()
{
    if ( !m_normalsService.expired() )
    {
        ::fwServices::OSR::unregisterService(m_normalsService.lock());
    }
}

void TriangularMesh::buildPipeline()
{

    if ( m_manageMapperInput )
    {
        m_normals->ComputePointNormalsOn();
        m_normals->ComputeCellNormalsOff();
        m_normals->ConsistencyOn();
        m_normals->SplittingOn();
        m_normals->SetFeatureAngle( m_sharpEdgeAngle );

        m_mapperInput = m_normals->GetOutputPort();

#ifndef USE_DEPTH_PEELING // replacement for depth peeling
#ifdef USE_DEPTH_SORT
        m_depthSort->SetInputConnection( m_normals->GetOutputPort() );
        m_depthSort->SetDirectionToBackToFront();
        m_mapperInput = m_depthSort->GetOutputPort();
#endif
#endif
    }


    ::fwData::TriangularMesh::sptr triangularMesh = this->getObject < ::fwData::TriangularMesh >();

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

    if ( m_manageMapperInput )
    {
        this->updateMaterial( m_material );
        this->updateTriangularMesh( triangularMesh );

        this->updateOptionsMode();

#ifndef USE_DEPTH_PEELING // replacement for depth peeling
#ifdef USE_DEPTH_SORT
        this->updateDepthSort();
#endif
#endif
    }

    if (!m_actor)
    {
        m_actor = this->newActor();
        this->addToRenderer(m_actor);
        if (this->getPicker())
        {
            this->addToPicker(m_actor);
        }
    }
    
    setActorPropertyToUnclippedMaterial(false);

    removeServicesStarterCommand();

    if( m_clippingPlanes && m_showClippedPart )
    {
        createServicesStarterCommand();
    }
    this->setVtkPipelineModified();
}



//------------------------------------------------------------------------------

void TriangularMesh::updateTriangularMesh( ::fwData::TriangularMesh::sptr mesh )
{
    if (m_triangularMesh.expired() || mesh != m_triangularMesh.lock())
    {
        m_triangularMesh = mesh;

        vtkPolyData * polyData = ::vtkIO::toVTKMesh(mesh);

        m_normals->SetInput( polyData );

        polyData->Delete();

        this->setVtkPipelineModified();
    }
}

//------------------------------------------------------------------------------

vtkActor *TriangularMesh::newActor()
{

    vtkPolyDataMapper *mapper = vtkPolyDataMapper::New();
    vtkActor          *actor  = vtkActor::New();


    mapper->SetInputConnection(m_mapperInput);

    if (m_clippingPlanes)
    {
        mapper->RemoveAllClippingPlanes();
        vtkPlaneCollection *newClippingPlanes = vtkPlaneCollection::New();

        removePlaneCollectionShifterCommand();

        m_planeCollectionShifterCallback = 
            PlaneCollectionShifterCallback::New(m_clippingPlanes, newClippingPlanes, 2.);

        mapper->SetClippingPlanes(newClippingPlanes);
    }

    actor->SetMapper(mapper);
    mapper->Delete();

    if(!this->getTransformId().empty())
    {
        actor->SetUserTransform(this->getTransform());
    }
    this->setVtkPipelineModified();
    return actor;
}

//------------------------------------------------------------------------------

void TriangularMesh::updateMaterial( ::fwData::Material::sptr material )
{
#ifndef USE_DEPTH_PEELING // replacement for depth peeling
#ifdef USE_DEPTH_SORT

    ::fwData::Color & color = m_material->ambient();

    if (color.alpha() < 1.)
    {
        m_mapperInput = m_depthSort->GetOutputPort();
        m_hasAlpha = true;
    }
    else
    {
        m_mapperInput = m_normals->GetOutputPort();
        m_hasAlpha = false;
    }
#endif
#endif

}

//------------------------------------------------------------------------------

void TriangularMesh::updateVisibility( bool isVisible)
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


bool TriangularMesh::getVisibility()
{
    bool visible = false;
    if (m_actor)
    {
        visible = m_actor->GetVisibility() == 1;
    }
    return visible;
}
void TriangularMesh::setVtkClippingPlanes(vtkPlaneCollection *planes)
{
    m_clippingPlanes = planes;
}


void TriangularMesh::removePlaneCollectionShifterCommand()
{
    if (m_planeCollectionShifterCallback)
    {
        m_planeCollectionShifterCallback->Stop();
        m_planeCollectionShifterCallback->Delete();
        m_planeCollectionShifterCallback = 0;
    }
}

void TriangularMesh::createServicesStarterCommand()
{
    if(!m_servicesStarterCallback)
    {
        ::visuVTKAdaptor::TriangularMesh::sptr srv = 
            ::visuVTKAdaptor::TriangularMesh::dynamicCast(
                    this->getSptr()
                    );
        m_servicesStarterCallback = PlaneCollectionAdaptorStarter::New( srv, m_clippingPlanes, -1. );
    }
}

void TriangularMesh::removeServicesStarterCommand()
{
    if(m_servicesStarterCallback)
    {
        m_servicesStarterCallback->Stop();
        m_servicesStarterCallback->Delete();
        m_servicesStarterCallback = 0;
    }
}


//------------------------------------------------------------------------------

#ifndef USE_DEPTH_PEELING // replacement for depth peeling
#ifdef USE_DEPTH_SORT
bool TriangularMesh::hasAlpha()
{
    return m_hasAlpha;
};


void TriangularMesh::updateDepthSort()
{
        if (hasAlpha())
        {
            if(m_depthSortCommand == 0)
            {
                m_depthSortCommand = vtkDepthSortCallBack::New(this);
                this->getInteractor()->AddObserver( "EndInteractionEvent" , m_depthSortCommand );
                getRenderer()->GetActiveCamera()->AddObserver("AnyEvent"  , m_depthSortCommand );

                m_depthSort->SetDirectionToSpecifiedVector ();
                m_depthSort->SetVector(getRenderer()->GetActiveCamera()->GetDirectionOfProjection());
            }
        }
        else
        {
            removeDepthSortCommand();
        }
        this->setVtkPipelineModified();
}


void TriangularMesh::removeDepthSortCommand()
{
    if(m_depthSortCommand)
    {
        this->getInteractor()->RemoveObserver(m_depthSortCommand);
        getRenderer()->GetActiveCamera()->RemoveObserver(m_depthSortCommand);
        m_depthSortCommand->Delete();
        m_depthSortCommand = 0;
    }
}

vtkDepthSortPolyData * TriangularMesh::getDepthSort()
{
    return m_depthSort;
}
#endif
#endif



} //namespace visuVTKAdaptor



