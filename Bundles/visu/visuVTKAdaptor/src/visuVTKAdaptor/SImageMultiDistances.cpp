/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/SImageMultiDistances.hpp"

#include "visuVTKAdaptor/SDistance.hpp"
#include "visuVTKAdaptor/SPointList.hpp"

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

#include <boost/assign/std/vector.hpp>

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

#include <algorithm>
#include <sstream>

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::SImageMultiDistances);

namespace visuVTKAdaptor
{

const ::fwServices::IService::KeyType s_IMAGE_INOUT = "image";

//------------------------------------------------------------------------------

class vtkDistanceDeleteCallBack : public vtkCommand
{

public:

    //------------------------------------------------------------------------------

    static vtkDistanceDeleteCallBack* New( SImageMultiDistances* service )
    {
        return new vtkDistanceDeleteCallBack(service);
    }

    vtkDistanceDeleteCallBack( SImageMultiDistances* service ) :
        m_service(service),
        m_picker( vtkSmartPointer< vtkCellPicker>::New() ),
        m_propCollection( vtkSmartPointer< vtkPropCollection>::New() )
    {
        m_lastPos[0] = -1;
        m_lastPos[1] = -1;
        m_picker->PickFromListOn();
        m_picker->SetTolerance(0.001);

        m_display[2] = 0.0;
    }

    //------------------------------------------------------------------------------

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

    //------------------------------------------------------------------------------

    virtual void Execute( vtkObject* /*caller*/, unsigned long eventId, void*)
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
                    ::fwRenderVTK::IAdaptor::sptr plAdaptor = m_service->getAssociatedAdaptor(prop, 1);
                    if (plAdaptor)
                    {
                        ::fwData::PointList::csptr plist =
                            plAdaptor->getInput< ::fwData::PointList >(SPointList::s_POINTLIST_INPUT);

                        ::fwData::Image::sptr image = m_service->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
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

    SImageMultiDistances* m_service;
    vtkSmartPointer< vtkPicker> m_picker;
    vtkSmartPointer< vtkPropCollection> m_propCollection;
    double m_display[3];
    int m_lastPos[2];

};

static const ::fwCom::Slots::SlotKeyType s_CREATE_DISTANCE_SLOT = "createDistance";
static const ::fwCom::Slots::SlotKeyType s_REMOVE_DISTANCE_SLOT = "removeDistance";

//------------------------------------------------------------------------------

SImageMultiDistances::SImageMultiDistances() noexcept :
    m_rightButtonCommand(nullptr),
    m_needSubservicesDeletion(false),
    m_filter(false)
{
    newSlot(s_CREATE_DISTANCE_SLOT, &SImageMultiDistances::createDistance, this);
    newSlot(s_REMOVE_DISTANCE_SLOT, &SImageMultiDistances::removeDistance, this);
}

//------------------------------------------------------------------------------

SImageMultiDistances::~SImageMultiDistances() noexcept
{
}

//------------------------------------------------------------------------------

void SImageMultiDistances::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    const std::string filter = config.get<std::string>("filter", "false");
    SLM_ASSERT("'filter' value must be 'true' or 'false'", filter == "true" || filter == "false");
    m_filter = (filter == "true");
}

//------------------------------------------------------------------------------

void SImageMultiDistances::starting()
{
    this->initialize();

    m_rightButtonCommand = vtkDistanceDeleteCallBack::New(this);
    this->getInteractor()->AddObserver( "RightButtonPressEvent", m_rightButtonCommand, 1 );
    this->getInteractor()->AddObserver( "RightButtonReleaseEvent", m_rightButtonCommand, 1 );
    this->getInteractor()->AddObserver( "StartInteractionEvent", m_rightButtonCommand, 0);

    this->updating();
}

//------------------------------------------------------------------------------

::fwData::Color::sptr generateColor()
{
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
        orange->setRGBA(  1, 0.647f, 0);
        colors.push_back( orange );

        Color::sptr violet = Color::New();
        violet->setRGBA( .5f, 0.26f, 1);
        colors.push_back( violet );

        Color::sptr vertpomme = Color::New();
        vertpomme->setRGBA( .65f, 1, 0);
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

void SImageMultiDistances::installSubServices( ::fwData::PointList::sptr pl )
{
    if ( pl->getPoints().size() > 1 )
    {
        // create the srv configuration for objects auto-connection
        auto serviceDistance = this->registerService< ::fwRenderVTK::IAdaptor>("::visuVTKAdaptor::SDistance");

        // register image
        serviceDistance->registerInput(pl, SDistance::s_POINTLIST_INPUT, true);

        // install  Color Field if none
        pl->setDefaultField( ::fwDataTools::fieldHelper::Image::m_colorId, generateColor() );

        // no mandatory to set picker id
        serviceDistance->setPickerId( this->getPickerId() );
        serviceDistance->setRendererId( this->getRendererId() );
        serviceDistance->setRenderService( this->getRenderService() );
        serviceDistance->start();

        // create the srv configuration for objects auto-connection
        IService::Config srvPLConfig;
        auto servicePointList = this->registerService< ::fwRenderVTK::IAdaptor>("::visuVTKAdaptor::SPointList");
        // register image
        servicePointList->registerInput(pl, SPointList::s_POINTLIST_INPUT, true);

        servicePointList->setPickerId( this->getPickerId() );
        servicePointList->setRendererId( this->getRendererId() );
        servicePointList->setRenderService( this->getRenderService() );
        servicePointList->start();
    }
}

//------------------------------------------------------------------------------

::fwData::Point::sptr SImageMultiDistances::screenToWorld(int X, int Y)
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
        // set the clipping around the focal point
        camera->SetClippingRange( camera->GetDistance() - 0.1, camera->GetDistance() + 0.1 );

        world = worldTmp;
        // (1) this function use the near clipping range to estimate the world point (by defaut 0.1 from camera view).
        // The clipping can be modified
        // by insertion of new object. By setting previously the clipping to the focal point we ensure to not place new
        // point a camera position
        // RETURN HOMOGEN COORD !!!
        this->getInteractor()->GetInteractorStyle()->ComputeDisplayToWorld( this->getRenderer(), X, Y, 0, world);

        // restore initial clipping
        camera->SetClippingRange( clippingCamBackup );
    }

    ::fwData::Point::sptr pt = ::fwData::Point::New();
    std::copy( world, world +3, pt->getCoord().begin() );
    this->setVtkPipelineModified();
    return pt;
}

//------------------------------------------------------------------------------

void SImageMultiDistances::updating()
{
    // get PointList in image Field then install distance service if required
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("Missing image", image);

    ::fwData::Vector::sptr distanceField;
    distanceField = image->getField< ::fwData::Vector >( ::fwDataTools::fieldHelper::Image::m_imageDistancesId);

    bool isShown;
    isShown = image->getField("ShowDistances", ::fwData::Boolean::New(true))->value();

    this->unregisterServices();

    if( isShown && distanceField )
    {
        for(::fwData::Object::sptr object :  *distanceField)
        {
            ::fwData::PointList::sptr distance    = ::fwData::PointList::dynamicCast(object);
            ::fwData::String::sptr relatedService = distance->getField< ::fwData::String >(
                ::fwDataTools::fieldHelper::Image::m_relatedServiceId);

            if ( m_filter && relatedService )
            {
                std::string servId = relatedService->value();
                if ( this->getRenderService()->getID() != servId )
                {
                    continue; // filtering ON + distance instanced from another RenderService
                }
            }
            // test pass OK : install service
            SLM_ASSERT( "Empty Point List for Distance !!!!", !distance->getPoints().empty() );
            this->installSubServices(distance);
        }
    }
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void SImageMultiDistances::removeDistance(::fwData::PointList::csptr plToRemove )
{
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("Missing image", image);

    this->unregisterServices();

    ::fwData::Vector::sptr distanceField;
    distanceField = image->getField< ::fwData::Vector >( ::fwDataTools::fieldHelper::Image::m_imageDistancesId);

    ::fwData::Vector::IteratorType iter = std::find(distanceField->begin(), distanceField->end(), plToRemove);
    if(iter != distanceField->end())
    {
        distanceField->getContainer().erase(iter);
    }

    this->updating();
}

//------------------------------------------------------------------------------

void SImageMultiDistances::createDistance()
{
    std::string sceneId = getRenderService()->getID();
    this->createNewDistance(sceneId);
}

//------------------------------------------------------------------------------

void SImageMultiDistances::createNewDistance( std::string sceneId )
{
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("Missing image", image);

    ::fwData::PointList::sptr newPL = ::fwData::PointList::New();

    newPL->setField( ::fwDataTools::fieldHelper::Image::m_relatedServiceId, ::fwData::String::New( sceneId ) );

    ::fwData::Vector::sptr distanceField;
    distanceField = image->setDefaultField< ::fwData::Vector >(::fwDataTools::fieldHelper::Image::m_imageDistancesId,
                                                               ::fwData::Vector::New());
    distanceField->getContainer().push_back(newPL);

    OSLM_INFO("DistanceField size: " << distanceField->size() );

    int sizeX = this->getRenderer()->GetRenderWindow()->GetSize()[0];
    int sizeY = this->getRenderer()->GetRenderWindow()->GetSize()[1];

    SLM_ASSERT("invalid RenderWindow size",  sizeX > 0 && sizeY > 0 );
    ::fwData::Point::sptr pt1 = this->screenToWorld(sizeX/3, sizeY/2);
    ::fwData::Point::sptr pt2 = this->screenToWorld(2*sizeX/3, sizeY/2);

    newPL->getPoints().push_back( pt1 );
    newPL->getPoints().push_back( pt2 );

    this->installSubServices(newPL);
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void SImageMultiDistances::setNeedSubservicesDeletion( bool _needSubservicesDeletion)
{
    m_needSubservicesDeletion = _needSubservicesDeletion; // to manage point deletion
}

//------------------------------------------------------------------------------

void SImageMultiDistances::stopping()
{
    if ( m_rightButtonCommand ) // can be not instantiated (use of SImageMultiDistances::show() )
    {
        this->getInteractor()->RemoveObserver(m_rightButtonCommand);
        m_rightButtonCommand->Delete();
        m_rightButtonCommand = 0;
    }

    this->unregisterServices();
}

//------------------------------------------------------------------------------

void SImageMultiDistances::show(bool showDistances)
{
    if(showDistances)
    {
        this->starting();
    }
    else
    {
        this->stopping();
    }
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SImageMultiDistances::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_IMAGE_INOUT, ::fwData::Image::s_DISTANCE_ADDED_SIG, s_UPDATE_SLOT);
    connections.push(s_IMAGE_INOUT, ::fwData::Image::s_DISTANCE_REMOVED_SIG, s_REMOVE_DISTANCE_SLOT);
    connections.push(s_IMAGE_INOUT, ::fwData::Image::s_DISTANCE_DISPLAYED_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor
