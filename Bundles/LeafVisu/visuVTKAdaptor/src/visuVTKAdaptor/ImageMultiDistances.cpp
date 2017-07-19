/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/ImageMultiDistances.hpp"

#include "visuVTKAdaptor/Distance.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Boolean.hpp>
#include <fwData/Color.hpp>
#include <fwData/Image.hpp>
#include <fwData/Material.hpp>
#include <fwData/String.hpp>
#include <fwData/Vector.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>

#include <fwTools/fwID.hpp>

#include <vtkActor.h>
#include <vtkAssemblyNode.h>
#include <vtkAssemblyPath.h>
#include <vtkCamera.h>
#include <vtkCellPicker.h>
#include <vtkCommand.h>
#include <vtkCubeSource.h>
#include <vtkInteractorStyle.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

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

    static vtkDistanceDeleteCallBack* New( ImageMultiDistances* service )
    {
        return new vtkDistanceDeleteCallBack(service);
    }

    vtkDistanceDeleteCallBack( ImageMultiDistances* service )
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

        vtkProp* prop;

        while ( (prop = m_propCollection->GetNextProp()) )
        {
            m_picker->AddPickList(prop);
        }
    }

    virtual void Execute( vtkObject* caller, unsigned long eventId, void*)
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
                vtkPropCollection* propc = m_picker->GetActors();
                vtkProp* prop;

                propc->InitTraversal();
                while ( (prop = propc->GetNextProp()) )
                {
                    ::fwData::PointList::sptr plist =
                        ::fwData::PointList::dynamicCast(m_service->getAssociatedObject(prop,1));

                    if(plist)
                    {
                        ::fwData::Image::sptr image = m_service->getObject< ::fwData::Image >();
                        auto sig = image->signal< ::fwData::Image::DistanceRemovedSignalType >(
                            ::fwData::Image::s_DISTANCE_REMOVED_SIG );
                        sig->asyncEmit(plist);

                        break;
                    }
                }
            }
        }
    }

protected:

    ImageMultiDistances* m_service;
    vtkPicker* m_picker;
    vtkPropCollection* m_propCollection;
    double m_display[3];
    int m_lastPos[2];

};

static const ::fwCom::Slots::SlotKeyType s_CREATE_DISTANCE_SLOT = "createDistance";
static const ::fwCom::Slots::SlotKeyType s_REMOVE_DISTANCE_SLOT = "removeDistance";

//------------------------------------------------------------------------------

ImageMultiDistances::ImageMultiDistances() noexcept :
    m_rightButtonCommand(nullptr),
    m_needSubservicesDeletion(false)
{
    newSlot(s_CREATE_DISTANCE_SLOT, &ImageMultiDistances::createDistance, this);
    newSlot(s_REMOVE_DISTANCE_SLOT, &ImageMultiDistances::removeDistance, this);
}

//------------------------------------------------------------------------------

ImageMultiDistances::~ImageMultiDistances() noexcept
{
}

//------------------------------------------------------------------------------

void ImageMultiDistances::doConfigure()
{
}

//------------------------------------------------------------------------------

void ImageMultiDistances::doStart()
{
    SLM_TRACE_FUNC();

    m_rightButtonCommand = vtkDistanceDeleteCallBack::New(this);
    this->getInteractor()->AddObserver( "RightButtonPressEvent", m_rightButtonCommand, 1 );
    this->getInteractor()->AddObserver( "RightButtonReleaseEvent", m_rightButtonCommand, 1 );  // jamais reçu quand TrackBallCameraStyle activé (GrabFocus)
    this->getInteractor()->AddObserver( "StartInteractionEvent", m_rightButtonCommand, 0);     // par contre ce style lance un event d'interaction

    this->doUpdate();
}

//------------------------------------------------------------------------------

void ImageMultiDistances::doSwap()
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
        pl->setDefaultField( ::fwDataTools::fieldHelper::Image::m_colorId, generateColor() );


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
    double* world;
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
        vtkCamera* camera         = this->getRenderer()->GetActiveCamera();
        double* clippingCamBackup = camera->GetClippingRange();
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

void ImageMultiDistances::doUpdate()
{
    // get PointList in image Field then install distance service if required
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();

    ::fwData::Vector::sptr distanceField;
    distanceField = image->getField< ::fwData::Vector >( ::fwDataTools::fieldHelper::Image::m_imageDistancesId);

    bool isShown;
    isShown = image->getField("ShowDistances", ::fwData::Boolean::New(true))->value();

    if ( !isShown || !distanceField  )
    {
        this->unregisterServices();
    }

    if( isShown && distanceField )
    {

        bool filtering = m_configuration->getAttributeValue("filter") == "true";
        for(::fwData::Object::sptr object :  *distanceField)
        {
            ::fwData::PointList::sptr distance    = ::fwData::PointList::dynamicCast(object);
            ::fwData::String::sptr relatedService = distance->getField< ::fwData::String >(
                ::fwDataTools::fieldHelper::Image::m_relatedServiceId);

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

void ImageMultiDistances::removeDistance(::fwData::PointList::sptr plToRemove )
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    this->unregisterServices();

    ::fwData::Vector::sptr distanceField;
    distanceField = image->getField< ::fwData::Vector >( ::fwDataTools::fieldHelper::Image::m_imageDistancesId);

    ::fwData::Vector::IteratorType iter = std::find(distanceField->begin(), distanceField->end(), plToRemove);
    if(iter != distanceField->end())
    {
        distanceField->getContainer().erase(iter);
    }

    this->doUpdate();
}

//------------------------------------------------------------------------------

void ImageMultiDistances::createDistance()
{
    std::string sceneId = getRenderService()->getID();
    this->createNewDistance(sceneId);
}

//------------------------------------------------------------------------------

void ImageMultiDistances::createNewDistance( std::string sceneId )
{
    ::fwData::Image::sptr image     = this->getObject< ::fwData::Image >();
    ::fwData::PointList::sptr newPL = ::fwData::PointList::New();

    newPL->setField( ::fwDataTools::fieldHelper::Image::m_relatedServiceId, ::fwData::String::New( sceneId ) );

    ::fwData::Vector::sptr distanceField;
    distanceField = image->setDefaultField< ::fwData::Vector >(::fwDataTools::fieldHelper::Image::m_imageDistancesId,
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

void ImageMultiDistances::setNeedSubservicesDeletion( bool _needSubservicesDeletion)
{
    m_needSubservicesDeletion = _needSubservicesDeletion; // to manage point deletion
}

//------------------------------------------------------------------------------

void ImageMultiDistances::doStop()
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

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsType ImageMultiDistances::getObjSrvConnections() const
{
    KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwData::Image::s_DISTANCE_ADDED_SIG, s_UPDATE_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Image::s_DISTANCE_REMOVED_SIG, s_REMOVE_DISTANCE_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Image::s_DISTANCE_DISPLAYED_SIG, s_UPDATE_SLOT ) );

    return connections;
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor
