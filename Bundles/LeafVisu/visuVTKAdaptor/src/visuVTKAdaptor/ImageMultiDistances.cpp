/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/ImageMultiDistances.hpp"
#include "visuVTKAdaptor/Distance.hpp"

#include <fwTools/fwID.hpp>

#include <fwData/Boolean.hpp>
#include <fwData/Color.hpp>
#include <fwData/Image.hpp>
#include <fwData/Material.hpp>
#include <fwData/String.hpp>
#include <fwData/Vector.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/Base.hpp>

#include <fwComEd/Dictionary.hpp>
#include <fwComEd/ImageMsg.hpp>

#include <fwCom/Signal.hxx>

#include <vtkActor.h>
#include <vtkAssemblyNode.h>
#include <vtkAssemblyPath.h>
#include <vtkCamera.h>
#include <vtkCellPicker.h>
#include <vtkCommand.h>
#include <vtkCubeSource.h>
#include <vtkInteractorStyle.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>


#include <boost/assign/std/vector.hpp>
#include <algorithm>
#include <sstream>

fwServicesRegisterMacro( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::ImageMultiDistances, ::fwData::Image );

namespace visuVTKAdaptor
{

//------------------------------------------------------------------------------

class vtkDistanceDeleteCallBack : public vtkCommand
{

public:

    static vtkDistanceDeleteCallBack * New( ImageMultiDistances * service )
    {
        return new vtkDistanceDeleteCallBack(service);
    }

    vtkDistanceDeleteCallBack( ImageMultiDistances *service )
        : m_service(service),
          m_picker( vtkCellPicker::New() ),
          m_propCollection( vtkPropCollection::New() )
    {
        m_lastPos[0] = -1;
        m_lastPos[1] = -1;
        m_picker->PickFromListOn();
        m_picker->SetTolerance(0.001);

        m_display[2] = 0.0;
    }

    void fillPickList()
    {
        m_picker->InitializePickList();
        m_propCollection->RemoveAllItems();
        m_service->getAllSubProps(m_propCollection);
        m_propCollection->InitTraversal();

        vtkProp *prop;

        while ( (prop = m_propCollection->GetNextProp()) )
        {
            m_picker->AddPickList(prop);
        }
    }

    virtual void Execute( vtkObject *caller, unsigned long eventId, void *)
    {
        int pos[2];
        m_service->getInteractor()->GetLastEventPosition(pos);


        if ( eventId == vtkCommand::RightButtonPressEvent )
        {
            std::copy(pos, pos+1, m_lastPos);
        }
        else if (   ( eventId == vtkCommand::RightButtonReleaseEvent ) ||
                    (( eventId == vtkCommand::StartInteractionEvent) && std::equal(pos, pos+1, m_lastPos) ))
        {
            m_display[0] = pos[0];
            m_display[1] = pos[1];


            this->fillPickList();
            if (m_picker->Pick( m_display, m_service->getRenderer() ) )
            {
                vtkPropCollection *propc = m_picker->GetActors();
                vtkProp *prop;

                propc->InitTraversal();
                while ( (prop = propc->GetNextProp()) )
                {
                    ::fwData::PointList::sptr plist =
                        ::fwData::PointList::dynamicCast(m_service->getAssociatedObject(prop,1));

                    if(plist)
                    {
                        ::fwData::Image::sptr image   = m_service->getObject< ::fwData::Image >();
                        ::fwComEd::ImageMsg::sptr msg = ::fwComEd::ImageMsg::New();
                        msg->addEvent( ::fwComEd::ImageMsg::DELETE_DISTANCE, plist );

                        ::fwData::Object::ObjectModifiedSignalType::sptr sig;
                        sig = image->signal< ::fwData::Object::ObjectModifiedSignalType >(
                            ::fwData::Object::s_OBJECT_MODIFIED_SIG );
                        sig->asyncEmit(msg);

                        break;
                    }
                }
            }
        }
    }

protected:

    ImageMultiDistances * m_service;
    vtkPicker * m_picker;
    vtkPropCollection * m_propCollection;
    double m_display[3];
    int m_lastPos[2];

};

//------------------------------------------------------------------------------

ImageMultiDistances::ImageMultiDistances() throw() :
    m_rightButtonCommand(0),
    m_needSubservicesDeletion(false)
{
    //addNewHandledEvent( ::fwComEd::ImageMsg::DISTANCE );
    //addNewHandledEvent( ::fwComEd::ImageMsg::NEW_DISTANCE );
    //addNewHandledEvent( ::fwComEd::ImageMsg::DELETE_DISTANCE );
}

//------------------------------------------------------------------------------

ImageMultiDistances::~ImageMultiDistances() throw()
{
}

//------------------------------------------------------------------------------

void ImageMultiDistances::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    assert(m_configuration->getName() == "config");
    this->setPickerId( m_configuration->getAttributeValue("picker") );
    this->setRenderId( m_configuration->getAttributeValue("renderer") );
}

//------------------------------------------------------------------------------

void ImageMultiDistances::doStart() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    m_rightButtonCommand = vtkDistanceDeleteCallBack::New(this);
    this->getInteractor()->AddObserver( "RightButtonPressEvent", m_rightButtonCommand, 1 );
    this->getInteractor()->AddObserver( "RightButtonReleaseEvent", m_rightButtonCommand, 1 );  // jamais reçu quand TrackBallCameraStyle activé (GrabFocus)
    this->getInteractor()->AddObserver( "StartInteractionEvent", m_rightButtonCommand, 0);     // par contre ce style lance un event d'interaction

    this->doUpdate();
}

//------------------------------------------------------------------------------

void ImageMultiDistances::doSwap() throw(fwTools::Failed)
{
    SLM_TRACE("SWAPPING ImageMultiDistances **TODO**");
    this->doStop();
    this->doStart();
}

//------------------------------------------------------------------------------

::fwData::Color::sptr generateColor()
{
    //using namespace boost::assign; // bring 'operator+=()' for vector into scope
    using namespace fwData;

    static std::vector< Color::sptr > colors;
    static std::vector< Color::sptr >::iterator current;
    if ( colors.empty() )
    {
        Color::sptr magenta = Color::New();
        magenta->setRGBA( 1, 0, 1);
        colors.push_back( magenta );

        Color::sptr cyan = Color::New();
        cyan->setRGBA(0, 1, 1);
        colors.push_back( cyan );

        Color::sptr orange = Color::New();
        orange->setRGBA(  1, 0.647, 0);
        colors.push_back( orange );

        Color::sptr violet = Color::New();
        violet->setRGBA( .5, 0.26, 1);
        colors.push_back( violet );

        Color::sptr vertpomme = Color::New();
        vertpomme->setRGBA( .65, 1, 0);
        colors.push_back( vertpomme );

        Color::sptr jaune = Color::New();
        jaune->setRGBA( 1, 1, 0);
        colors.push_back( jaune );

//        colors +=   Color::New( 1, 0, 1), // magenta
//                    Color::New( 0, 1, 1), // cyan
//                    Color::New( 1, 0.647, 0 ), // orange
//                    Color::New( .5, 0.26, 1 ), // violet
//                    Color::New( .65, 1 , 0), // vert pomme
//                    Color::New( 1, 1, 0 ); // jaune
        current = colors.begin();
    }

    if ( ++current == colors.end() )
    {
        current = colors.begin();
    }

    Color::sptr newColor;
    newColor = ::fwData::Object::copy( *current );
    return newColor;
}

//------------------------------------------------------------------------------

void ImageMultiDistances::installSubServices( ::fwData::PointList::sptr pl )
{
    if ( pl->getPoints().size()>1 )
    {
        // SERVICE DISTANCE
        ::fwRenderVTK::IVtkAdaptorService::sptr serviceDistance;
        serviceDistance =
            ::fwServices::add< ::fwRenderVTK::IVtkAdaptorService >
                ( pl, "::visuVTKAdaptor::Distance");
        SLM_ASSERT("serviceDistance not instanced", serviceDistance);

        // install  Color Field if none
        pl->setDefaultField( ::fwComEd::Dictionary::m_colorId, generateColor() );


        // no mandatory to set picker id
        serviceDistance->setPickerId( this->getPickerId() );
        serviceDistance->setRenderId( this->getRenderId() );
        serviceDistance->setRenderService( this->getRenderService() );
        serviceDistance->setAutoRender( this->getAutoRender() );
        serviceDistance->start();

        // SERVICE POINT LIST
        ::fwRenderVTK::IVtkAdaptorService::sptr servicePointList;
        servicePointList =
            ::fwServices::add< ::fwRenderVTK::IVtkAdaptorService >
                ( pl, "::visuVTKAdaptor::PointList");
        SLM_ASSERT("servicePointList not instanced", servicePointList);

        servicePointList->setPickerId( this->getPickerId() );
        servicePointList->setRenderId( this->getRenderId() );
        servicePointList->setRenderService( this->getRenderService() );
        servicePointList->setAutoRender(m_autoRender);
        servicePointList->start();

        this->registerService( serviceDistance );
        this->registerService( servicePointList );

    }
}

//------------------------------------------------------------------------------

::fwData::Point::sptr ImageMultiDistances::screenToWorld(int X,int Y)
{
    double *world;
    double display[3];
    double worldTmp[4];
    display[0] = X;
    display[1] = Y;
    display[2] = 0;

    if ( this->getPicker() && this->getPicker()->Pick( display, this->getRenderer() ) )
    {
        world = this->getPicker()->GetPickPosition();
    }
    else
    {
        // set temporaly the clipping around the focal point : see (1)
        vtkCamera *camera         = this->getRenderer()->GetActiveCamera();
        double *clippingCamBackup = camera->GetClippingRange();
        camera->SetClippingRange( camera->GetDistance() - 0.1, camera->GetDistance() + 0.1 );  // set the clipping around the focal point

        world = worldTmp;
        // (1) this function use the near clipping range to estimate the world point (by defaut 0.1 from camera view). The clipping can be modified
        // by insertion of new object. By setting previously the clipping to the focal point we ensure to not place new point a camera position
        this->getInteractor()->GetInteractorStyle()->ComputeDisplayToWorld ( this->getRenderer(), X, Y, 0, world);    // RETURN HOMOGEN COORD !!!

        // restore initial clipping
        camera->SetClippingRange( clippingCamBackup );
    }

    ::fwData::Point::sptr pt = ::fwData::Point::New();
    std::copy( world, world +3, pt->getRefCoord().begin() );
    this->setVtkPipelineModified();
    return pt;
}

//------------------------------------------------------------------------------

void ImageMultiDistances::doUpdate() throw(fwTools::Failed)
{
    // get PointList in image Field then install distance service if required
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();

    ::fwData::Vector::sptr distanceField;
    distanceField = image->getField< ::fwData::Vector >( ::fwComEd::Dictionary::m_imageDistancesId);

    bool isShown;
    isShown = image->getField("ShowDistances", ::fwData::Boolean::New(true))->value();

    if ( !isShown || !distanceField  )
    {
        this->unregisterServices();
    }

    if( isShown && distanceField )
    {

        bool filtering = m_configuration->getAttributeValue("filter") == "true";
        BOOST_FOREACH(::fwData::Object::sptr object, *distanceField)
        {
            ::fwData::PointList::sptr distance    = ::fwData::PointList::dynamicCast(object);
            ::fwData::String::sptr relatedService = distance->getField< ::fwData::String >(
                ::fwComEd::Dictionary::m_relatedServiceId);

            if ( filtering && relatedService )
            {
                std::string servId = relatedService->value();
                if ( this->getRenderService()->getID() != servId )
                {
                    continue; // filtering ON + distance instanced from another RenderService
                }
            }
            // test pass OK : install service
            SLM_ASSERT( "Empty Point List for Distance !!!!", !distance->getCRefPoints().empty() );
            this->installSubServices(distance);
        }
    }
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void ImageMultiDistances::removeDistance(::fwData::PointList::sptr plToRemove ) throw(::fwTools::Failed)
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    this->unregisterServices();

    ::fwData::Vector::sptr distanceField;
    distanceField = image->getField< ::fwData::Vector >( ::fwComEd::Dictionary::m_imageDistancesId);

    ::fwData::Vector::IteratorType iter = std::find(distanceField->begin(), distanceField->end(), plToRemove);
    if(iter != distanceField->end())
    {
        distanceField->getContainer().erase(iter);
    }

    this->doUpdate();
}

//------------------------------------------------------------------------------

void ImageMultiDistances::createNewDistance( std::string sceneId ) throw(::fwTools::Failed)
{
    ::fwData::Image::sptr image     = this->getObject< ::fwData::Image >();
    ::fwData::PointList::sptr newPL = ::fwData::PointList::New();

    newPL->setField( ::fwComEd::Dictionary::m_relatedServiceId, ::fwData::String::New( sceneId ) );

    ::fwData::Vector::sptr distanceField;
    distanceField = image->setDefaultField< ::fwData::Vector >(::fwComEd::Dictionary::m_imageDistancesId,
                                                               ::fwData::Vector::New());
    distanceField->getContainer().push_back(newPL);

    OSLM_INFO("DistanceField size: " << distanceField->size() );

    int sizeX = this->getRenderer()->GetRenderWindow()->GetSize()[0];
    int sizeY = this->getRenderer()->GetRenderWindow()->GetSize()[1];

    SLM_ASSERT("invalid RenderWindow size",  sizeX>0 && sizeY>0 );
    ::fwData::Point::sptr pt1 = this->screenToWorld(sizeX/3.0, sizeY/2.0);
    ::fwData::Point::sptr pt2 = this->screenToWorld(2*sizeX/3.0, sizeY/2.0);

    newPL->getRefPoints().push_back( pt1 );
    newPL->getRefPoints().push_back( pt2 );

    this->installSubServices(newPL);
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void ImageMultiDistances::doReceive( ::fwServices::ObjectMsg::csptr msg ) throw(::fwTools::Failed)
{
    // update only if new LandMarks
    ::fwComEd::ImageMsg::csptr imgMsg = ::fwComEd::ImageMsg::dynamicConstCast( msg );
    ::fwData::Image::sptr image       = this->getObject< ::fwData::Image >();
    std::string sceneId = getRenderService()->getID();

    if ( imgMsg && imgMsg->hasEvent( ::fwComEd::ImageMsg::NEW_DISTANCE ) )
    {
        ::fwData::String::csptr dataInfo;
        dataInfo = ::fwData::String::dynamicConstCast(imgMsg->getDataInfo(::fwComEd::ImageMsg::NEW_DISTANCE));
        OSLM_FATAL_IF(" ImageMultiDistances::doUpdate with RenderService "
                      <<  sceneId << "missing sceneId dataInfo !!!", !dataInfo);
        if ( dataInfo->value() == sceneId )
        {
            this->createNewDistance( sceneId );
            ::fwComEd::ImageMsg::sptr msg = ::fwComEd::ImageMsg::New();
            msg->addEvent( ::fwComEd::ImageMsg::DISTANCE );
            msg->setSource( this->getSptr());
            msg->setSubject( image);
            ::fwData::Object::ObjectModifiedSignalType::sptr sig;
            sig = image->signal< ::fwData::Object::ObjectModifiedSignalType >(::fwData::Object::s_OBJECT_MODIFIED_SIG);
            {
                ::fwCom::Connection::Blocker block(sig->getConnection(m_slotReceive));
                sig->asyncEmit( msg );
            }
        }
    }

    if ( imgMsg && imgMsg->hasEvent( ::fwComEd::ImageMsg::DISTANCE ) )
    {
        ::fwData::String::csptr dataInfo;
        dataInfo = ::fwData::String::dynamicConstCast(imgMsg->getDataInfo(::fwComEd::ImageMsg::DISTANCE));
        // update only if the distance is added in this scene
        // or if the service is not filtered
        if ( !dataInfo || dataInfo->value() ==  this->getRenderService()->getID()
             || m_configuration->getAttributeValue("filter") == "false")
        {
            this->doUpdate();
        }
    }

    if ( imgMsg && imgMsg->hasEvent( ::fwComEd::ImageMsg::DELETE_DISTANCE ) )
    {
        ::fwData::PointList::csptr plToRemove;
        plToRemove = ::fwData::PointList::dynamicConstCast(imgMsg->getDataInfo(::fwComEd::ImageMsg::DELETE_DISTANCE));
        SLM_ASSERT( "No dataInfo for the PointList ",plToRemove );
        this->removeDistance( std::const_pointer_cast< ::fwData::PointList>(plToRemove) );
    }
}

//------------------------------------------------------------------------------

void ImageMultiDistances::setNeedSubservicesDeletion( bool _needSubservicesDeletion)
{
    m_needSubservicesDeletion = _needSubservicesDeletion; // to manage point deletion
}

//------------------------------------------------------------------------------

void ImageMultiDistances::doStop() throw(fwTools::Failed)
{
    if ( m_rightButtonCommand ) // can be not instantiated (use of ImageMultiDistances::show() )
    {
        this->getInteractor()->RemoveObserver(m_rightButtonCommand);
        m_rightButtonCommand->Delete();
        m_rightButtonCommand = 0;
    }

    this->unregisterServices();
}

//------------------------------------------------------------------------------

void ImageMultiDistances::show(bool showDistances)
{
    if(showDistances)
    {
        this->doStart();
    }
    else
    {
        this->doStop();
    }
}


} //namespace visuVTKAdaptor
