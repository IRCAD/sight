/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/LabeledPointList.hpp"

#include "visuVTKAdaptor/PointLabel.hpp"
#include "visuVTKAdaptor/PointList.hpp"

#include <fwCom/Signal.hxx>

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

fwServicesRegisterMacro( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::LabeledPointList, ::fwData::PointList );

namespace visuVTKAdaptor
{

//------------------------------------------------------------------------------

class vtkLabeledPointDeleteCallBack : public vtkCommand
{

public:
    static vtkLabeledPointDeleteCallBack* New( ::fwRenderVTK::IVtkAdaptorService* service)
    {
        return new vtkLabeledPointDeleteCallBack(service);
    }

    vtkLabeledPointDeleteCallBack( ::fwRenderVTK::IVtkAdaptorService* service )
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

    ~vtkLabeledPointDeleteCallBack( )
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
            ::fwData::PointList::PointListContainer::iterator itr = std::find(
                m_pickedPointList.lock()->getRefPoints().begin(),
                m_pickedPointList.lock()->getRefPoints().end(), m_pickedPoint.lock() );
            if(itr != m_pickedPointList.lock()->getRefPoints().end())
            {
                ::fwData::Point::sptr point = *itr;
                m_pickedPointList.lock()->getRefPoints().erase(itr);
                auto sig = m_pickedPointList.lock()->signal< ::fwData::PointList::PointRemovedSignalType >(
                    ::fwData::PointList::s_POINT_REMOVED_SIG );
                sig->asyncEmit(point);
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

LabeledPointList::LabeledPointList() throw() :
    m_rightButtonCommand(nullptr),
    m_radius(7.0)
{
}

//------------------------------------------------------------------------------

LabeledPointList::~LabeledPointList() throw()
{
}

//------------------------------------------------------------------------------

void LabeledPointList::doConfigure() throw(fwTools::Failed)
{
    SLM_ASSERT("configuration missing", m_configuration->getName() == "config");

    std::string hexaColor = m_configuration->getAttributeValue("color");
    m_ptColor = ::fwData::Color::New();
    if (!hexaColor.empty())
    {
        m_ptColor->setRGBA(hexaColor);
    }

    std::string radius = m_configuration->getAttributeValue("radius");
    if(!radius.empty())
    {
        m_radius = std::stod(radius);
    }
}

//------------------------------------------------------------------------------

void LabeledPointList::doStart() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    m_rightButtonCommand = vtkLabeledPointDeleteCallBack::New(this);
    this->getInteractor()->AddObserver( "RightButtonPressEvent", m_rightButtonCommand, 1 );
    this->getInteractor()->AddObserver( "RightButtonReleaseEvent", m_rightButtonCommand, 1 );

    this->doUpdate();
}

//------------------------------------------------------------------------------

void LabeledPointList::doSwap() throw(fwTools::Failed)
{
    SLM_TRACE("SWAPPING LabeledPointList **TODO**");
    this->doStop();
    this->doStart();
}

//------------------------------------------------------------------------------

void LabeledPointList::doUpdate() throw(fwTools::Failed)
{
    // get PointList in image Field then install distance service if required
    ::fwData::PointList::sptr landmarks = this->getObject< ::fwData::PointList >();

    this->unregisterServices();

    if ( !landmarks->getPoints().empty() )
    {
        ::fwRenderVTK::IVtkAdaptorService::sptr servicePointList;
        servicePointList = ::fwServices::add< ::fwRenderVTK::IVtkAdaptorService >( landmarks,
                                                                                   "::visuVTKAdaptor::PointList");
        SLM_ASSERT("servicePointList not instanced", servicePointList);

        ::visuVTKAdaptor::PointList::sptr pointListAdaptor = ::visuVTKAdaptor::PointList::dynamicCast(servicePointList);

        SLM_ASSERT("Bad cast of IVtkAdaptorService to servicePointList", pointListAdaptor);

        pointListAdaptor->setColor(m_ptColor);
        pointListAdaptor->setRadius(m_radius);

        servicePointList->setPickerId( this->getPickerId() );
        servicePointList->setRenderService( this->getRenderService() );
        servicePointList->setAutoRender( this->getAutoRender() );
        servicePointList->start();

        this->registerService( servicePointList );


        for( ::fwData::Point::sptr point :  landmarks->getRefPoints() )
        {
            ::fwRenderVTK::IVtkAdaptorService::sptr serviceLabel;
            serviceLabel =
                ::fwServices::add< ::fwRenderVTK::IVtkAdaptorService >(point, "::visuVTKAdaptor::PointLabel");
            SLM_ASSERT("serviceLabel not instanced", serviceLabel);
            serviceLabel->setRenderService( this->getRenderService() );
            serviceLabel->setAutoRender( this->getAutoRender() );
            serviceLabel->start();
            this->registerService( serviceLabel );
        }
    }

    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void LabeledPointList::doStop() throw(fwTools::Failed)
{
    if ( m_rightButtonCommand ) // can be not instanciated (use of LabeledPointList::show() )
    {
        this->getInteractor()->RemoveObserver(m_rightButtonCommand);
        m_rightButtonCommand->Delete();
        m_rightButtonCommand = 0;
    }

    this->unregisterServices();
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsType LabeledPointList::getObjSrvConnections() const
{
    KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwData::PointList::s_MODIFIED_SIG, s_UPDATE_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::PointList::s_POINT_ADDED_SIG, s_UPDATE_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::PointList::s_POINT_REMOVED_SIG, s_UPDATE_SLOT ) );

    return connections;
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor
