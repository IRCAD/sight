/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/ImageLandmarks.hpp"

#include "visuVTKAdaptor/PointLabel.hpp"
#include "visuVTKAdaptor/PointList.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Boolean.hpp>
#include <fwData/Image.hpp>
#include <fwData/Material.hpp>
#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>

#include <vtkActor.h>
#include <vtkAssemblyNode.h>
#include <vtkAssemblyPath.h>
#include <vtkCellPicker.h>
#include <vtkCommand.h>
#include <vtkCubeSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

#include <algorithm>

fwServicesRegisterMacro( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::ImageLandmarks, ::fwData::Image );

namespace visuVTKAdaptor
{

static const ::fwCom::Slots::SlotKeyType s_UPDATE_LANDMARKS_SLOT       = "updateLandmaks";
static const ::fwCom::Slots::SlotKeyType s_UPDATE_LANDMARKS_FIELD_SLOT = "updateLandmarksField";

//------------------------------------------------------------------------------

void notifyRemoveLandMark( ::fwData::Image::sptr image, ::fwServices::IService* _service, ::fwData::Point::sptr point )
{
    SLM_ASSERT("NULL Service", _service);

    ::fwData::PointList::sptr pointList = image->getField< ::fwData::PointList >(
        ::fwDataTools::fieldHelper::Image::m_imageLandmarksId );

    auto sig =
        pointList->signal< ::fwData::PointList::PointRemovedSignalType >(::fwData::PointList::s_POINT_REMOVED_SIG);
    sig->asyncEmit(point);

    auto sigImg = image->signal< ::fwData::Image::LandmarkRemovedSignalType >(
        ::fwData::Image::s_LANDMARK_REMOVED_SIG );
    sigImg->asyncEmit(point);
}

//------------------------------------------------------------------------------

class vtkPointDeleteCallBack : public vtkCommand
{

public:
    static vtkPointDeleteCallBack* New( ::fwRenderVTK::IVtkAdaptorService* service)
    {
        return new vtkPointDeleteCallBack(service);
    }

    vtkPointDeleteCallBack( ::fwRenderVTK::IVtkAdaptorService* service )
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

    ~vtkPointDeleteCallBack( )
    {
        m_picker->Delete();
        m_picker = NULL;

        m_propCollection->Delete();
        m_propCollection = NULL;
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
        OSLM_TRACE( "EventId: " << eventId);

        if ( eventId == vtkCommand::RightButtonPressEvent )
        {
            std::copy(pos, pos+1, m_lastPos);
            m_display[0] = pos[0];
            m_display[1] = pos[1];

            this->fillPickList();
            if (m_picker->Pick( m_display, m_service->getRenderer() ) )
            {
                if(getSelectedPoint())
                {
                    SetAbortFlag(1);
                }
                else
                {
                    m_pickedPoint.reset();
                    m_pickedPointList.reset();
                }
            }
        }
        else if ( (eventId == vtkCommand::RightButtonReleaseEvent ) && !m_pickedPoint.expired() &&
                  !m_pickedPointList.expired() && std::equal(pos, pos+1, m_lastPos) )
        {
            ::fwData::Image::sptr image                           = m_service->getObject< ::fwData::Image >();
            ::fwData::PointList::PointListContainer::iterator itr = std::find(
                m_pickedPointList.lock()->getRefPoints().begin(),
                m_pickedPointList.lock()->getRefPoints().end(), m_pickedPoint.lock() );
            if(itr != m_pickedPointList.lock()->getRefPoints().end())
            {
                ::fwData::Point::sptr point = *itr;
                m_pickedPointList.lock()->getRefPoints().erase(itr);
                notifyRemoveLandMark(image, m_service, point);
            }
        }
    }
    bool getSelectedPoint()
    {
        bool isFind              = false;
        vtkPropCollection* propc = m_picker->GetActors();
        vtkProp* prop;

        propc->InitTraversal();
        while ( (prop = propc->GetNextProp()) )
        {
            m_pickedPoint     = ::fwData::Point::dynamicCast(m_service->getAssociatedObject(prop,2));
            m_pickedPointList = ::fwData::PointList::dynamicCast(m_service->getAssociatedObject(prop,1));

            if( !m_pickedPoint.expired() && !m_pickedPointList.expired() )
            {
                ::fwData::PointList::PointListContainer::iterator itr = std::find(
                    m_pickedPointList.lock()->getRefPoints().begin(),
                    m_pickedPointList.lock()->getRefPoints().end(), m_pickedPoint.lock());
                if(itr != m_pickedPointList.lock()->getRefPoints().end() )
                {
                    isFind = true;
                    break;
                }
            }
        }
        return isFind;
    }

protected:
    ::fwRenderVTK::IVtkAdaptorService *m_service;
    vtkPicker* m_picker;
    vtkPropCollection* m_propCollection;
    double m_display[3];
    int m_lastPos[2];
    ::fwData::Point::wptr m_pickedPoint;
    ::fwData::PointList::wptr m_pickedPointList;

};

//------------------------------------------------------------------------------

ImageLandmarks::ImageLandmarks() throw() :
    m_rightButtonCommand(nullptr),
    m_needSubservicesDeletion(false)
{
    newSlot(s_UPDATE_LANDMARKS_SLOT, &ImageLandmarks::updateLandmaks, this);
    newSlot(s_UPDATE_LANDMARKS_FIELD_SLOT, &ImageLandmarks::updateLandmaksField, this);
}

//------------------------------------------------------------------------------

ImageLandmarks::~ImageLandmarks() throw()
{
}

//------------------------------------------------------------------------------

void ImageLandmarks::doConfigure() throw(fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void ImageLandmarks::doStart() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    m_rightButtonCommand = vtkPointDeleteCallBack::New(this);
    this->getInteractor()->AddObserver( "RightButtonPressEvent", m_rightButtonCommand, 1 );
    this->getInteractor()->AddObserver( "RightButtonReleaseEvent", m_rightButtonCommand, 1 );

    this->doUpdate();
}

//------------------------------------------------------------------------------

void ImageLandmarks::doSwap() throw(fwTools::Failed)
{
    SLM_TRACE("SWAPPING ImageLandmarks **TODO**");
    this->doStop();
    this->doStart();
}

//------------------------------------------------------------------------------

void ImageLandmarks::doUpdate() throw(fwTools::Failed)
{
    // get PointList in image Field then install distance service if required
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();

    ::fwData::PointList::sptr landmarks;
    bool isShown;
    landmarks = image->getField< ::fwData::PointList >(  ::fwDataTools::fieldHelper::Image::m_imageLandmarksId );
    isShown   = image->getField("ShowLandmarks", ::fwData::Boolean::New(true))->value();

    if (!isShown || !landmarks || m_needSubservicesDeletion)
    {
        this->unregisterServices();
        m_needSubservicesDeletion = false;
    }

    if( isShown && landmarks )
    {
        if ( !landmarks->getPoints().empty() )
        {
            ::fwRenderVTK::IVtkAdaptorService::sptr servicePointList;
            servicePointList = ::fwServices::add< ::fwRenderVTK::IVtkAdaptorService >( landmarks,
                                                                                       "::visuVTKAdaptor::PointList");
            SLM_ASSERT("servicePointList not instanced", servicePointList);

            servicePointList->setPickerId( this->getPickerId() );
            servicePointList->setRenderService( this->getRenderService() );
            servicePointList->setAutoRender( this->getAutoRender() );
            servicePointList->start();

            this->registerService( servicePointList );


            for( ::fwData::Point::sptr point :  landmarks->getRefPoints() )
            {
                ::fwRenderVTK::IVtkAdaptorService::sptr serviceLabel;
                serviceLabel = ::fwServices::add< ::fwRenderVTK::IVtkAdaptorService >(point,
                                                                                      "::visuVTKAdaptor::PointLabel");
                SLM_ASSERT("serviceLabel not instanced", serviceLabel);
                serviceLabel->setRenderService( this->getRenderService() );
                serviceLabel->setAutoRender( this->getAutoRender() );
                serviceLabel->start();
                this->registerService( serviceLabel );
            }
        }
    }
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void ImageLandmarks::updateLandmaks()
{
    m_needSubservicesDeletion = true; // to manage point deletion
    this->updating();
}

//------------------------------------------------------------------------------

void ImageLandmarks::updateLandmaksField(::fwData::Object::FieldsContainerType fieldsContainer)
{
    auto landmarkFound = fieldsContainer.find(::fwDataTools::fieldHelper::Image::m_imageLandmarksId);

    if( landmarkFound != fieldsContainer.end())
    {
        m_needSubservicesDeletion = true; // to manage point deletion
        this->updating();
    }
}

//------------------------------------------------------------------------------

void ImageLandmarks::doStop() throw(fwTools::Failed)
{
    if ( m_rightButtonCommand ) // can be not instanciated (use of ImageLandmarks::show() )
    {
        this->getInteractor()->RemoveObserver(m_rightButtonCommand);
        m_rightButtonCommand->Delete();
        m_rightButtonCommand = 0;
    }

    this->unregisterServices();
}

//------------------------------------------------------------------------------

void ImageLandmarks::show(bool b)
{
    if (b)
    {
        this->doStart();
    }
    else
    {
        this->doStop();
    }
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsType ImageLandmarks::getObjSrvConnections() const
{
    KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwData::Image::s_LANDMARK_ADDED_SIG, s_UPDATE_LANDMARKS_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Image::s_LANDMARK_REMOVED_SIG, s_UPDATE_LANDMARKS_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Image::s_LANDMARK_DISPLAYED_SIG, s_UPDATE_LANDMARKS_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Image::s_ADDED_FIELDS_SIG, s_UPDATE_LANDMARKS_FIELD_SLOT ) );

    return connections;
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor
