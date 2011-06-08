/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/function.hpp>

#include <vtkActor.h>
#include <vtkCellPicker.h>
#include <vtkFrustumCoverageCuller.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLight.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRendererCollection.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkInstantiator.h>
#include <vtkTransform.h>
#include <vtkCamera.h>


#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>
#include <fwComEd/CompositeMsg.hpp>
#include <fwTools/fwID.hpp>
#include <fwData/Color.hpp>

#include <fwRuntime/ConfigurationElementContainer.hpp>
#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <fwComEd/CameraMsg.hpp>

#include "fwRenderVTK/IVtkAdaptorService.hpp"
#include "fwRenderVTK/VtkRenderService.hpp"


REGISTER_SERVICE( ::fwRender::IRender , ::fwRenderVTK::VtkRenderService , ::fwData::Composite ) ;

using namespace fwServices;


namespace fwRenderVTK
{

//-----------------------------------------------------------------------------

VtkRenderService::VtkRenderService() throw() :
     m_pendingRenderRequest(false)
{
    addNewHandledEvent( ::fwComEd::CompositeMsg::MODIFIED_FIELDS );
}

//-----------------------------------------------------------------------------

VtkRenderService::~VtkRenderService() throw() {

}

//-----------------------------------------------------------------------------

void VtkRenderService::configureRenderer( ConfigurationType conf )
{
    assert(conf->getName() == "renderer");

    std::string id = conf->getAttributeValue("id");
    std::string background = conf->getAttributeValue("background");

    if(m_renderers.count(id) == 0)
    {
        m_renderers[id] = vtkRenderer::New();

        m_renderers[id]->SetUseDepthPeeling     ( 1  );
        m_renderers[id]->SetMaximumNumberOfPeels( 8  );
        m_renderers[id]->SetOcclusionRatio      ( 0. );

        if(conf->hasAttribute("layer") )
        {
            int layer = ::boost::lexical_cast< int >(conf->getAttributeValue("layer"));
            m_renderers[id]->SetLayer(layer);
        }
    }

    if ( !background.empty() )
    {
        if(background[0] == '#')
        {
            ::fwData::Color::NewSptr color;
            color->setRGBA(background);
            m_renderers[id]->SetBackground(color->getRefRGBA()[0], color->getRefRGBA()[1], color->getRefRGBA()[2]);
        }
        else
        {
            // compatibility with "old" color
            double color = ::boost::lexical_cast<double> (background);
            m_renderers[id]->SetBackground(color, color, color);
        }
    }
}

//-----------------------------------------------------------------------------

void VtkRenderService::configurePicker( ConfigurationType conf )
{
    assert(conf->getName() == "picker");

    std::string id = conf->getAttributeValue("id");
    std::string vtkclass = conf->getAttributeValue("vtkclass");

    if (vtkclass.empty())
    {
        vtkclass = "vtkCellPicker";
    }

    if(m_pickers.count(id) == 0)
    {
        m_pickers[id] =  vtkAbstractPropPicker::SafeDownCast(vtkInstantiator::CreateInstance(vtkclass.c_str()));
        OSLM_ASSERT("'" << vtkclass.c_str() << "' instantiation failled.",m_pickers[id]);
        m_pickers[id]->InitializePickList();
        m_pickers[id]->PickFromListOn();
        vtkPicker *picker = vtkPicker::SafeDownCast(m_pickers[id]);
        if (picker)
        {
            picker->SetTolerance(0);
        }
    }
}

//-----------------------------------------------------------------------------

void VtkRenderService::configureObject( ConfigurationType conf )
{
    assert(conf->getName() == "adaptor");
    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >() ;

    const std::string id            = conf->getAttributeValue("id");
    const std::string objectId      = conf->getAttributeValue("objectId");
    const std::string adaptor       = conf->getAttributeValue("class");
    const std::string uid           = conf->getAttributeValue("uid");
    const std::string compositeName = "self";

    SLM_ASSERT( "'id' required attribute missing or empty"      , !id.empty() );
    SLM_ASSERT( "'objectId' required attribute missing or empty", !objectId.empty() );
    SLM_ASSERT( "'adaptor' required attribute missing or empty" , !adaptor.empty() );

    const unsigned int compositeObjectCount = composite->getRefMap().count(objectId);

    OSLM_TRACE_IF(objectId << " not found in composite. If it exist, associated Adaptor will be destroyed",
                  ! (compositeObjectCount == 1 || objectId == compositeName) );


    ::fwTools::Object::sptr object;
    if (compositeObjectCount)
    {
        object = ::fwTools::Object::dynamicCast(composite->getRefMap()[objectId]);
    }
    else if (objectId == compositeName)
    {
        object = ::fwTools::Object::dynamicCast(composite);
    }

    if ( m_sceneAdaptors.count(id) == 0 && object )
    {
        OSLM_TRACE ("Adding service : IVtkAdaptorService " << adaptor << " on "<< objectId );
        SceneAdaptor adaptee;
        adaptee.m_config = * (conf->begin());
        if (!uid.empty())
        {
            OSLM_TRACE("VtkRenderService::configureObject : uid = " << uid);
            adaptee.m_service = ::fwServices::add< ::fwRenderVTK::IVtkAdaptorService >( object , adaptor, uid);
        }
        else
        {
            adaptee.m_service = ::fwServices::add< ::fwRenderVTK::IVtkAdaptorService >( object , adaptor);
        }

        assert(adaptee.m_config->getName() == "config");
        assert(adaptee.getService());

        adaptee.getService()->setConfiguration(adaptee.m_config);
        adaptee.getService()->configure();
        adaptee.getService()->setRenderService(VtkRenderService::dynamicCast(this->shared_from_this()));
        adaptee.getService()->setName(id);

        if (this->isStarted())
        {
            adaptee.getService()->start();
        }

        m_sceneAdaptors[id] = adaptee;
    }
    else if(m_sceneAdaptors.count(id) == 1)
    {
        SceneAdaptor &adaptee = m_sceneAdaptors[id];
        SLM_ASSERT("Adaptor service expired !", adaptee.getService() );
        OSLM_ASSERT( adaptee.getService()->getID() <<  " is not started " ,adaptee.getService()->isStarted());
        if (object)
        {
            OSLM_TRACE ("Swapping IVtkAdaptorService " << adaptor << " on "<< objectId );
            if(adaptee.getService()->getObject() != object)
            {
                adaptee.getService()->swap(object);
            }
            else
            {
                OSLM_WARN(adaptor << "'s object already is '"
                        << adaptee.getService()->getObject()->getID()
                        << "', no need to swap");
            }
        }
        else
        {
            adaptee.getService()->stop();
            ::fwServices::OSR::unregisterService(adaptee.getService());
            adaptee.m_service.reset();
            m_sceneAdaptors.erase(id);
        }
    }
    else
    {
            OSLM_TRACE ( "'"<< objectId << "' inexistant, passing by '" << adaptor << "'");
    }
}

//-----------------------------------------------------------------------------

void VtkRenderService::configureVtkObject( ConfigurationType conf )
{
    assert(conf->getName() == "vtkObject");

    std::string id = conf->getAttributeValue("id");
    std::string vtkClass = conf->getAttributeValue("class");
    assert( !id.empty() );
    assert( !vtkClass.empty() );

    if( m_vtkObjects.count(id) == 0 )
    {

        if ( vtkClass == "vtkTransform" && conf->size() == 1 )
        {
            m_vtkObjects[id] = createVtkTransform( conf );
        }
        else
        {
            m_vtkObjects[id] = vtkInstantiator::CreateInstance(vtkClass.c_str());
        }


    }
}

//-----------------------------------------------------------------------------

vtkTransform * VtkRenderService::createVtkTransform( ConfigurationType conf )
{
    SLM_ASSERT("vtkObject must be contain just only one sub xml element called vtkTransform.", conf->size() == 1 && ( *conf->begin() )->getName() == "vtkTransform");

    ConfigurationType vtkTransformXmlElem = *conf->begin();

    vtkTransform * newMat = vtkTransform::New();

    for(    ::fwRuntime::ConfigurationElement::Iterator elem = vtkTransformXmlElem->begin();
            !(elem == vtkTransformXmlElem->end());
            ++elem )
    {
        SLM_ASSERT("Sorry, the name of xml element must be concatenate.", (*elem)->getName() == "concatenate" );

        std::string transformId = (*elem)->getValue();

        vtkTransform * mat = vtkTransform::SafeDownCast( getVtkObject(transformId) );

        if ( (*elem)->hasAttribute( "inverse" ) && (*elem)->getAttributeValue( "inverse" ) == "yes" )
        {
            newMat->Concatenate( mat->GetLinearInverse() );
        }
        else
        {
            newMat->Concatenate( mat );
        }

    }

    return newMat;
}

//-----------------------------------------------------------------------------

void VtkRenderService::addVtkObject( VtkObjectIdType _id, vtkObject * _vtkObj )
{
    assert( ! _id.empty() );

    if( m_vtkObjects.count(_id) == 0 )
    {
        m_vtkObjects[_id] = _vtkObj;
    }
}

//-----------------------------------------------------------------------------

void VtkRenderService::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    SLM_FATAL_IF( "Depreciated tag \"win\" in configuration", m_configuration->findConfigurationElement("win") );
    this->initialize();

    std::vector < ::fwRuntime::ConfigurationElement::sptr > vectConfig = m_configuration->find("scene");
    //assert(m_configuration->getName() == "scene");
    assert(!vectConfig.empty());
    m_sceneConfiguration = vectConfig.at(0);
}

//-----------------------------------------------------------------------------

void VtkRenderService::starting() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    this->create();
    this->startContext();

    // Instantiate vtk object, class...
    ::fwRuntime::ConfigurationElementContainer::Iterator iter;
    for (iter = m_sceneConfiguration->begin() ; iter != m_sceneConfiguration->end() ; ++iter)
    {
        if ((*iter)->getName() == "renderer")
        {
            this->configureRenderer(*iter);
        }
        else if ((*iter)->getName() == "picker")
        {
            this->configurePicker(*iter);
        }
        else if ((*iter)->getName() == "adaptor")
        {
            this->configureObject(*iter);
        }
        else if ((*iter)->getName() == "vtkObject")
        {
            this->configureVtkObject(*iter);
        }
        else
        {
            OSLM_ASSERT("Bad scene configurationType, unknown xml node : " << (*iter)->getName(), false);
        }
    }

    m_interactorManager->getInteractor()->GetRenderWindow()->SetNumberOfLayers(m_renderers.size());
    for( RenderersMapType::iterator iter = m_renderers.begin(); iter != m_renderers.end(); ++iter )
    {
        vtkRenderer *renderer = (*iter).second;
        m_interactorManager->getInteractor()->GetRenderWindow()->AddRenderer(renderer);
    }
    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >() ;

    SceneAdaptorsMapType::iterator adaptorIter ;
    for ( adaptorIter = m_sceneAdaptors.begin();
          adaptorIter != m_sceneAdaptors.end();
          ++adaptorIter)
    {
        adaptorIter->second.getService()->start();
        assert(adaptorIter->second.getService()->isStarted());
    }
}

//-----------------------------------------------------------------------------

void VtkRenderService::stopping() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >() ;

    SceneAdaptorsMapType::iterator adaptorIter ;

    for ( adaptorIter = m_sceneAdaptors.begin();
          adaptorIter != m_sceneAdaptors.end();
          ++adaptorIter)
    {
        adaptorIter->second.getService()->stop();
        ::fwServices::OSR::unregisterService(adaptorIter->second.getService());
        adaptorIter->second.getService().reset();
    }

    this->stopContext();

    this->getContainer()->clean();
    this->destroy();

    m_sceneAdaptors.clear();
}

//-----------------------------------------------------------------------------

void VtkRenderService::updating( ::fwServices::ObjectMsg::csptr message ) throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    ::fwComEd::CompositeMsg::csptr compositeMsg = ::fwComEd::CompositeMsg::dynamicConstCast(message);

    if(compositeMsg && compositeMsg->hasEvent( ::fwComEd::CompositeMsg::MODIFIED_FIELDS ) )
    {
        std::vector< std::string > objectIds = compositeMsg->getEventModifiedFields();
        std::vector< std::string >::iterator iter;

        assert ( m_sceneConfiguration );

        BOOST_FOREACH( std::string objectId, objectIds)
        {
            BOOST_FOREACH(
                    ConfigurationType cfg,
                    m_sceneConfiguration->find("adaptor","objectId",objectId))
            {
                this->configureObject(cfg);
            }
        }
    }
}

//-----------------------------------------------------------------------------

void VtkRenderService::updating() throw(fwTools::Failed)
{}

//-----------------------------------------------------------------------------

void VtkRenderService::render()
{
    m_interactorManager->getInteractor()->Render();
}


//-----------------------------------------------------------------------------

bool VtkRenderService::isShownOnScreen()
{
   return this->getContainer()->isShownOnScreen();
}

//-----------------------------------------------------------------------------

void VtkRenderService::startContext()
{
    m_interactorManager = ::fwRenderVTK::IVtkRenderWindowInteractorManager::createManager();
    m_interactorManager->installInteractor( this->getContainer() );

    // For Depth peeling (translucent rendering)
//    m_interactorManager->getInteractor()->SetRenderWhenDisabled(false);

#ifndef __linux
    m_interactorManager->getInteractor()->GetRenderWindow()->SetAlphaBitPlanes(1);
    m_interactorManager->getInteractor()->GetRenderWindow()->SetMultiSamples(0);
#endif

//    m_interactor->GetRenderWindow()->PointSmoothingOn();
//    m_interactor->GetRenderWindow()->LineSmoothingOn();
//    m_interactor->GetRenderWindow()->PolygonSmoothingOn();
//    m_interactor->Register(NULL);
//    m_interactor->SetInteractorStyle( vtkInteractorStyleTrackballCamera::New() );

//    m_interactorManager->getInteractor()->SetRenderModeToDirect();
    //m_interactor->SetRenderModeToFrameRated();
//    m_interactor->SetRenderModeToOneShot();
//    m_interactor->SetRenderModeToMeanTime();
}

//-----------------------------------------------------------------------------

void VtkRenderService::stopContext()
{
    SLM_TRACE_FUNC();

    for( RenderersMapType::iterator iter = m_renderers.begin(); iter != m_renderers.end(); ++iter )
    {
        vtkRenderer *renderer = iter->second;
        renderer->InteractiveOff();
        m_interactorManager->getInteractor()->GetRenderWindow()->RemoveRenderer(renderer);
        renderer->Delete();
    }

    m_renderers.clear();

    m_interactorManager->uninstallInteractor();
    m_interactorManager.reset();
}

//-----------------------------------------------------------------------------

vtkRenderer * VtkRenderService::getRenderer(RendererIdType rendererId)
{
    OSLM_ASSERT("Renderer not found : '" << rendererId << "'" , m_renderers.count(rendererId) == 1);

    return m_renderers[rendererId];
}

//-----------------------------------------------------------------------------

vtkAbstractPropPicker * VtkRenderService::getPicker(PickerIdType pickerId)
{
    OSLM_ASSERT("Picker '" << pickerId << "' not found",
            pickerId.empty() ||  ( !pickerId.empty() && m_pickers.count(pickerId) == 1));

    if ( pickerId.empty() )
    {
        return NULL;
    }
    return m_pickers[pickerId];
}

//-----------------------------------------------------------------------------

vtkObject * VtkRenderService::getVtkObject(VtkObjectIdType objectId)
{
    OSLM_WARN_IF("vtkObject '" << objectId << "' not found",
            !(objectId.empty() ||  ( !objectId.empty() && m_vtkObjects.count(objectId) == 1)));

    if ( objectId.empty() )
    {
        return NULL;
    }
    return m_vtkObjects[objectId];
}

//-----------------------------------------------------------------------------

} //namespace fwRenderVTK



