/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/SLabeledPointList.hpp"

#include "visuVTKAdaptor/SPointLabel.hpp"
#include "visuVTKAdaptor/SPointList.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

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
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>

#include <algorithm>

fwServicesRegisterMacro(::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::SLabeledPointList);

namespace visuVTKAdaptor
{

static const ::fwServices::IService::KeyType s_POINTLIST_INOUT = "pointList";

const ::fwCom::Slots::SlotKeyType s_UPDATE_VISIBILITY_SLOT = "updateVisibility";

//------------------------------------------------------------------------------

class vtkLabeledPointDeleteCallBack : public vtkCommand
{

public:
    //------------------------------------------------------------------------------

    static vtkLabeledPointDeleteCallBack* New( ::fwRenderVTK::IAdaptor* service)
    {
        return new vtkLabeledPointDeleteCallBack(service);
    }

    vtkLabeledPointDeleteCallBack( ::fwRenderVTK::IAdaptor* service ) :
        m_service(service),
        m_picker( vtkCellPicker::New() ),
        m_propCollection( vtkPropCollection::New() )
    {
        m_lastPos[0] = -1;
        m_lastPos[1] = -1;
        m_picker->PickFromListOn();
        m_picker->SetTolerance(0.001);

        m_display[2] = 0.0;
    }

    ~vtkLabeledPointDeleteCallBack( )
    {
        m_picker->Delete();
        m_picker = NULL;

        m_propCollection->Delete();
        m_propCollection = NULL;
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
                if(this->getSelectedPoint())
                {
                    this->SetAbortFlag(1);
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
            ::fwData::PointList::PointListContainer& points       = m_pickedPointList.lock()->getPoints();
            ::fwData::PointList::PointListContainer::iterator itr =
                std::find( points.begin(), points.end(), m_pickedPoint.lock() );
            if(itr != points.end())
            {
                ::fwData::Point::sptr point = *itr;
                points.erase(itr);
                auto sig = m_pickedPointList.lock()->signal< ::fwData::PointList::PointRemovedSignalType >(
                    ::fwData::PointList::s_POINT_REMOVED_SIG );
                sig->asyncEmit(point);
            }
        }
    }
    //------------------------------------------------------------------------------

    bool getSelectedPoint()
    {
        bool isFind              = false;
        vtkPropCollection* propc = m_picker->GetActors();
        vtkProp* prop;

        propc->InitTraversal();
        while ( (prop = propc->GetNextProp()) )
        {
            ::fwRenderVTK::IAdaptor::sptr pointSrv = m_service->getAssociatedAdaptor(prop, 2);

            if(pointSrv)
            {

                m_pickedPoint     = pointSrv->getInOut< ::fwData::Point >(SPoint::s_POINT_INOUT);
                m_pickedPointList = m_service->getInOut< ::fwData::PointList >(s_POINTLIST_INOUT);

                if (!m_pickedPoint.expired() && !m_pickedPointList.expired())
                {
                    ::fwData::PointList::PointListContainer& points       = m_pickedPointList.lock()->getPoints();
                    ::fwData::PointList::PointListContainer::iterator itr =
                        std::find(points.begin(), points.end(), m_pickedPoint.lock());
                    if(itr != points.end() )
                    {
                        isFind = true;
                        break;
                    }
                }
            }
        }
        return isFind;
    }

protected:
    ::fwRenderVTK::IAdaptor* m_service;
    vtkPicker* m_picker;
    vtkPropCollection* m_propCollection;
    double m_display[3];
    int m_lastPos[2];
    ::fwData::Point::wptr m_pickedPoint;
    ::fwData::PointList::wptr m_pickedPointList;

};

//------------------------------------------------------------------------------

SLabeledPointList::SLabeledPointList() noexcept :
    m_rightButtonCommand(nullptr),
    m_radius(7.0),
    m_interaction(true)
{
    newSlot(s_UPDATE_VISIBILITY_SLOT, &SLabeledPointList::updateVisibility, this);
}

//------------------------------------------------------------------------------

SLabeledPointList::~SLabeledPointList() noexcept
{
}

//------------------------------------------------------------------------------

void SLabeledPointList::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    const std::string hexaColor = config.get< std::string >("color", "");
    m_ptColor = ::fwData::Color::New();
    if (!hexaColor.empty())
    {
        m_ptColor->setRGBA(hexaColor);
    }

    const std::string radius = config.get< std::string >("radius", "");
    if(!radius.empty())
    {
        m_radius = std::stod(radius);
    }

    const std::string interaction = config.get< std::string >("interaction", "");
    if(!interaction.empty())
    {
        SLM_FATAL_IF("value for 'interaction' must be 'on' or 'off', actual: " + interaction,
                     interaction != "on" && interaction != "off");
        m_interaction = (interaction == "on");
    }
}

//------------------------------------------------------------------------------

void SLabeledPointList::starting()
{
    this->initialize();

    if(m_interaction)
    {
        m_rightButtonCommand = vtkLabeledPointDeleteCallBack::New(this);
        this->getInteractor()->AddObserver( "RightButtonPressEvent", m_rightButtonCommand, 1 );
        this->getInteractor()->AddObserver( "RightButtonReleaseEvent", m_rightButtonCommand, 1 );
    }

    this->updating();
}

//------------------------------------------------------------------------------

void SLabeledPointList::updating()
{
    // get PointList in image Field then install distance service if required
    ::fwData::PointList::sptr landmarks = this->getInOut< ::fwData::PointList >(s_POINTLIST_INOUT);

    this->unregisterServices();

    if ( !landmarks->getPoints().empty() )
    {
        // create the srv configuration for objects auto-connection
        auto pointListAdaptor = this->registerService< ::visuVTKAdaptor::SPointList>("::visuVTKAdaptor::SPointList");
        // register the point list
        pointListAdaptor->registerInput(landmarks, SPointList::s_POINTLIST_INPUT, true);

        pointListAdaptor->setColor(m_ptColor);
        pointListAdaptor->setRadius(m_radius);
        pointListAdaptor->setInteraction(m_interaction);

        pointListAdaptor->setPickerId( this->getPickerId() );
        pointListAdaptor->setRenderService( this->getRenderService() );
        pointListAdaptor->start();

        for( ::fwData::Point::sptr point :  landmarks->getPoints() )
        {
            auto serviceLabel = this->registerService< ::fwRenderVTK::IAdaptor>("::visuVTKAdaptor::SPointLabel");
            // register the point list
            serviceLabel->registerInput(point, SPointLabel::s_POINT_INPUT, true);

            serviceLabel->setRenderService( this->getRenderService() );
            serviceLabel->start();
        }
    }

    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SLabeledPointList::stopping()
{
    if ( m_rightButtonCommand ) // can be not instanciated (use of SLabeledPointList::show() )
    {
        this->getInteractor()->RemoveObserver(m_rightButtonCommand);
        m_rightButtonCommand->Delete();
        m_rightButtonCommand = 0;
    }

    this->unregisterServices();
    this->requestRender();
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SLabeledPointList::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_POINTLIST_INOUT, ::fwData::PointList::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_POINTLIST_INOUT, ::fwData::PointList::s_POINT_ADDED_SIG, s_UPDATE_SLOT);
    connections.push(s_POINTLIST_INOUT, ::fwData::PointList::s_POINT_REMOVED_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

void SLabeledPointList::updateVisibility( bool isVisible)
{
    const auto& services = this->getRegisteredServices();
    for(const auto& service : services)
    {
        auto srv = ::visuVTKAdaptor::SPointList::dynamicCast(service.lock());
        if(srv)
        {
            srv->updateVisibility(isVisible);
        }
        else
        {
            auto srv2 = ::visuVTKAdaptor::SPointLabel::dynamicCast(service.lock());
            if(srv2)
            {
                srv2->updateVisibility(isVisible);
            }
            else
            {
                SLM_WARN("::visuVTKAdaptor::SPointLabel cast failed");
            }
        }
    }
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor
