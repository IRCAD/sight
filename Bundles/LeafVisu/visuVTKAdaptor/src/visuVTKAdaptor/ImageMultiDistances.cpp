/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <algorithm>

#include <sstream>
#include <boost/assign/std/vector.hpp>

#include <fwTools/fwID.hpp>

#include <fwData/Boolean.hpp>
#include <fwData/String.hpp>
#include <fwData/Image.hpp>
#include <fwData/Material.hpp>
#include <fwData/Color.hpp>
#include <fwData/DownCastIterator.hpp>


#include <fwServices/macros.hpp>
#include <fwServices/Factory.hpp>

#include <fwComEd/Dictionary.hpp>
#include <fwComEd/ImageMsg.hpp>

#include <vtkActor.h>
#include <vtkAssemblyNode.h>
#include <vtkAssemblyPath.h>
#include <vtkCommand.h>
#include <vtkCubeSource.h>
#include <vtkCellPicker.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyle.h>
#include <vtkRenderWindow.h>
#include <vtkCamera.h>

#include "visuVTKAdaptor/ImageMultiDistances.hpp"
#include "visuVTKAdaptor/Distance.hpp"


REGISTER_SERVICE( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::ImageMultiDistances, ::fwData::Image ) ;

namespace visuVTKAdaptor
{

//------------------------------------------------------------------------------

class vtkDistanceDeleteCallBack : public vtkCommand
{

public :

    static vtkDistanceDeleteCallBack * New( ImageMultiDistances * service )
    {
        return new vtkDistanceDeleteCallBack(service);
    }

    vtkDistanceDeleteCallBack( ImageMultiDistances *service )
    : m_service(service),
      m_picker( vtkCellPicker::New() ),
      m_propCollection( vtkPropCollection::New() )
    {
        m_picker->PickFromListOn();
        m_picker->SetTolerance(0.001);

        m_display[2]=0.0;
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
            if (m_picker->Pick( m_display , m_service->getRenderer() ) )
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
                        ::fwData::Image::sptr image = m_service->getObject< ::fwData::Image >();
                        ::fwComEd::ImageMsg::NewSptr msg;
                        msg->addEvent( ::fwComEd::ImageMsg::DELETE_DISTANCE, plist );
                        ::fwServices::IEditionService::notify( m_service->getSptr(), image, msg , ::fwServices::ComChannelService::NOTIFY_SOURCE);
                        break;
                    }
                }
            }
        }
    }

protected :

    ImageMultiDistances * m_service;
    vtkPicker * m_picker;
    vtkPropCollection * m_propCollection;
    double m_display[3];
    int m_lastPos[2];

};

//------------------------------------------------------------------------------

ImageMultiDistances::ImageMultiDistances() throw():
    m_rightButtonCommand(0),
    m_needSubservicesDeletion(false)
{
    addNewHandledEvent( ::fwComEd::ImageMsg::DISTANCE );
    addNewHandledEvent( ::fwComEd::ImageMsg::NEW_DISTANCE );
    addNewHandledEvent( ::fwComEd::ImageMsg::DELETE_DISTANCE );
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
    this->getInteractor()->AddObserver( "RightButtonPressEvent" , m_rightButtonCommand, 1 );
    this->getInteractor()->AddObserver( "RightButtonReleaseEvent" , m_rightButtonCommand, 1 ); // jamais reçu quand TrackBallCameraStyle activé (GrabFocus)
    this->getInteractor()->AddObserver( "StartInteractionEvent" , m_rightButtonCommand, 0);    // par contre ce style lance un event d'interaction

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
        Color::NewSptr magenta;
        magenta->setRGBA( 1, 0, 1);
        colors.push_back( magenta );

        Color::NewSptr cyan;
        cyan->setRGBA(0, 1, 1);
        colors.push_back( cyan );

        Color::NewSptr orange;
        orange->setRGBA(  1, 0.647, 0);
        colors.push_back( orange );

        Color::NewSptr violet;
        violet->setRGBA( .5, 0.26, 1);
        colors.push_back( violet );

        Color::NewSptr vertpomme;
        vertpomme->setRGBA( .65, 1 , 0);
        colors.push_back( vertpomme );

        Color::NewSptr jaune;
        jaune->setRGBA( 1, 1, 0);
        colors.push_back( jaune );

//        colors +=   Color::New( 1, 0, 1), // magenta
//                    Color::New( 0, 1, 1), // cyan
//                    Color::New( 1, 0.647, 0 ), // orange
//                    Color::New( .5, 0.26, 1 ), // violet
//                    Color::New( .65, 1 , 0), // vert pomme
//                    Color::New( 1, 1, 0 ); // jaune
        current= colors.begin();
    }

    if ( ++current == colors.end() )
    {
        current = colors.begin();
    }

    Color::NewSptr newColor;
    newColor->deepCopy( *current );
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
        ( pl , "::visuVTKAdaptor::Distance");
        assert( serviceDistance );

        // install  Color Field if none
        if ( ! pl->getFieldSize( ::fwComEd::Dictionary::m_colorId ) )
        {
            pl->setFieldSingleElement( ::fwComEd::Dictionary::m_colorId, generateColor() );
        }


        // no mandatory to set picker id
        serviceDistance->setPickerId( this->getPickerId() );
        serviceDistance->setRenderId( this->getRenderId() );
        serviceDistance->setRenderService( this->getRenderService() );
        serviceDistance->start();

        // SERVICE POINT LIST
        ::fwRenderVTK::IVtkAdaptorService::sptr servicePointList;
        servicePointList =
            ::fwServices::add< ::fwRenderVTK::IVtkAdaptorService >
        ( pl , "::visuVTKAdaptor::PointList");
        assert( servicePointList );

        servicePointList->setPickerId( this->getPickerId() );
        servicePointList->setRenderId( this->getRenderId() );
        servicePointList->setRenderService( this->getRenderService() );
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
    display[0]=X;
    display[1]=Y;
    display[2]=0;

    if ( this->getPicker() && this->getPicker()->Pick( display , this->getRenderer() ) )
    {
        world = this->getPicker()->GetPickPosition();
    }
    else
    {
        // set temporaly the clipping around the focal point : see (1)
        vtkCamera *camera = this->getRenderer()->GetActiveCamera();
        double *clippingCamBackup = camera->GetClippingRange();
        camera->SetClippingRange( camera->GetDistance() - 0.1 , camera->GetDistance() + 0.1 ); // set the clipping around the focal point

        world=worldTmp;
        // (1) this function use the near clipping range to estimate the world point (by defaut 0.1 from camera view). The clipping can be modified
        // by insertion of new object. By setting previously the clipping to the focal point we ensure to not place new point a camera position
        this->getInteractor()->GetInteractorStyle()->ComputeDisplayToWorld ( this->getRenderer(), X , Y , 0 , world); // RETURN HOMOGEN COORD !!!

        // restore initial clipping
        camera->SetClippingRange( clippingCamBackup );
    }

    ::fwData::Point::NewSptr pt;
    std::copy( world, world +3, pt->getRefCoord().begin() );
    this->setVtkPipelineModified();
    return pt;
}

//------------------------------------------------------------------------------

void ImageMultiDistances::doUpdate() throw(fwTools::Failed)
{
    // get PointList in image Field then install distance service if required
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();

    bool hasDistanceField = image->getFieldSize( ::fwComEd::Dictionary::m_imageDistancesId );
    bool isShown = true;
    if(image->getFieldSize("ShowDistances"))
    {
        isShown = image->getFieldSingleElement< ::fwData::Boolean > ("ShowDistances")->value();
    }

    if ( !isShown || !hasDistanceField  )
    {
        this->unregisterServices();
    }

    if( isShown && hasDistanceField )
    {
        OSLM_TRACE(" image->getField(::fwComEd::Dictionary::m_imageDistancesId)->children().size()");

        typedef ::fwData::ContainerCaster< ::fwData::PointList >::iterator Iterator;
        Iterator plIter( image->getField(::fwComEd::Dictionary::m_imageDistancesId)->children().begin() );
        Iterator end( image->getField(::fwComEd::Dictionary::m_imageDistancesId)->children().end() );

        for (  ; plIter != end; ++plIter )
        {
            assert ( *plIter );
            ::fwData::PointList::sptr pl = *plIter;
            bool filtering = m_configuration->getAttributeValue("filter") == "true" ;
            // skip this pl ??
            if ( filtering  && pl->getFieldSize( ::fwComEd::Dictionary::m_relatedServiceId ) )
            {
                std::string servId = pl->getFieldSingleElement< ::fwData::String >( ::fwComEd::Dictionary::m_relatedServiceId )->value();
                if (   getRenderService()->getID() != servId )
                {
                    continue; // filtering ON + distance instanced from another RenderService
                }
            }
            // test pass OK : install service
            SLM_ASSERT( "Empty Point List for Distance !!!!", !pl->getCRefPoints().empty() );
            this->installSubServices(pl);
        }
    }
    this->setVtkPipelineModified();
}


void ImageMultiDistances::removeDistance(  ::fwData::PointList::sptr plToRemove ) throw(::fwTools::Failed)
{

    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    this->unregisterServices();
    image->removeFieldElement( ::fwComEd::Dictionary::m_imageDistancesId , plToRemove);
    doUpdate();
}


void ImageMultiDistances::createNewDistance( std::string sceneId ) throw(::fwTools::Failed)
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    ::fwData::PointList::NewSptr newPL;

    newPL->setFieldSingleElement( ::fwComEd::Dictionary::m_relatedServiceId ,  ::fwData::String::NewSptr( sceneId ) );

    image->addFieldElement( ::fwComEd::Dictionary::m_imageDistancesId , newPL  );

    OSLM_INFO("AddDistance::image->getField( LAND).size() " << image->getField( ::fwComEd::Dictionary::m_imageDistancesId)->children().size() );
    assert( image->getFieldSize( ::fwComEd::Dictionary::m_imageDistancesId ) );


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

void ImageMultiDistances::doUpdate( ::fwServices::ObjectMsg::csptr msg ) throw(::fwTools::Failed)
{
    // update only if new LandMarks
    ::fwComEd::ImageMsg::csptr imgMsg =  ::fwComEd::ImageMsg::dynamicConstCast( msg );
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    std::string  sceneId =  getRenderService()->getID();

    if ( imgMsg && imgMsg->hasEvent( ::fwComEd::ImageMsg::NEW_DISTANCE ) )
    {
        ::fwData::String::csptr dataInfo = ::fwData::String::dynamicConstCast(imgMsg->getDataInfo(::fwComEd::ImageMsg::NEW_DISTANCE));
        OSLM_FATAL_IF(" ImageMultiDistances::doUpdate with RenderService "<<  sceneId << "missing sceneId dataInfo !!!", !dataInfo);
        if ( dataInfo->value() == sceneId )
        {
            this->createNewDistance( sceneId );
            ::fwComEd::ImageMsg::NewSptr msg;
            msg->addEvent( ::fwComEd::ImageMsg::DISTANCE );
            ::fwServices::IEditionService::notify( this->getSptr(), image, msg );
        }
    }

    if ( imgMsg && imgMsg->hasEvent( ::fwComEd::ImageMsg::DISTANCE ) )
    {
        ::fwData::String::csptr dataInfo = ::fwData::String::dynamicConstCast(imgMsg->getDataInfo(::fwComEd::ImageMsg::DISTANCE));
        // update only if the distance is added in this scene
        // or if the service is not filtered
        if ( !dataInfo || dataInfo->value() ==  getRenderService()->getID()
                || m_configuration->getAttributeValue("filter") == "false")
        {
            doUpdate();
        }
    }

    if ( imgMsg && imgMsg->hasEvent( ::fwComEd::ImageMsg::DELETE_DISTANCE ) )
    {
        ::fwData::PointList::csptr plToRemove = ::fwData::PointList::dynamicConstCast(imgMsg->getDataInfo(::fwComEd::ImageMsg::DELETE_DISTANCE));
        SLM_ASSERT( " ImageMsg::DELETE_DISTANCE , no dataInfo for the PointList ",plToRemove );
        this->removeDistance( ::boost::const_pointer_cast< ::fwData::PointList>(plToRemove) );
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
    if ( m_rightButtonCommand ) // can be not instanciated (use of ImageMultiDistances::show() )
    {
        this->getInteractor()->RemoveObserver(m_rightButtonCommand);
        m_rightButtonCommand->Delete();
        m_rightButtonCommand = 0;
    }

    this->unregisterServices();
}

//------------------------------------------------------------------------------

void ImageMultiDistances::show(bool b)
{
    b?doStart():doStop();
}


} //namespace visuVTKAdaptor
