/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
/* ***** BEGIN CONTRIBUTORS BLOCK *****
 * Initial authors:
 *  - Jean-Baptiste.Fasquel (LISA Laboratory, Angers University, France)
 * ****** END CONTRIBUTORS BLOCK ****** */

#include <boost/foreach.hpp>


#include <fwTools/helpers.hpp>

#include <fwData/Integer.hpp>
#include <fwData/Image.hpp>
#include <fwData/TransfertFunction.hpp>

#include <fwComEd/Dictionary.hpp>
#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>
#include <fwComEd/ImageMsg.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/Factory.hpp>

#include <fwServices/ObjectServiceRegistry.hpp>

#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleImage.h>
#include <vtkCommand.h>

#include <fwRenderVTK/IVtkAdaptorService.hpp>
#include <fwRenderVTK/vtk/fwVtkCellPicker.hpp>

#include "visuVTKAdaptor/NegatoWindower.hpp"


REGISTER_SERVICE( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::NegatoWindower, ::visuVTKAdaptor::NegatoOneSlice ) ;


#define START_WINDOWING_EVENT vtkCommand::RightButtonPressEvent
#define STOP_WINDOWING_EVENT  vtkCommand::RightButtonReleaseEvent

namespace visuVTKAdaptor
{

class NegatoWindowerCallback : public vtkCommand
{
public:
    static NegatoWindowerCallback *New()
    { return new NegatoWindowerCallback(); }

    NegatoWindowerCallback() : m_mouseMoveObserved(false)
    {
        m_windowStep = 1. ;
        m_levelStep  = 1. ;
        this->PassiveObserverOff();
    }

    ~NegatoWindowerCallback()
    {

    }

    virtual void Execute( vtkObject *caller, unsigned long eventId, void *)
    {
         if ( m_mouseMoveObserved || !m_adaptor->getInteractor()->GetShiftKey() )
        {
            if ( eventId == START_WINDOWING_EVENT)
            {
                assert(m_adaptor);
                assert(m_picker);

                double display[3];

                m_adaptor->getInteractor()->GetEventPosition(m_x, m_y);
                display[0] = m_x;
                display[1] = m_y;
                display[2] = 0;

                if ( m_picker->Pick( display , m_adaptor->getRenderer() ) )
                {
                    assert(!m_mouseMoveObserved);
                    m_adaptor->startWindowing();
                    m_adaptor->getInteractor()->AddObserver(vtkCommand::MouseMoveEvent, this, 1.);
                    m_mouseMoveObserved = true;
                    SetAbortFlag(1);
                    m_adaptor->update();
                }

            }

            else if ( eventId == STOP_WINDOWING_EVENT)
            {
                assert(m_adaptor);
                assert(m_picker);

                if(m_mouseMoveObserved)
                {
                    m_adaptor->getInteractor()->RemoveObservers(vtkCommand::MouseMoveEvent, this);
                    m_mouseMoveObserved = false;
                    m_adaptor->stopWindowing();
                    m_adaptor->update();
                }
            }
            else if (eventId == vtkCommand::MouseMoveEvent)
            {
                assert(m_mouseMoveObserved);
                int x,y;
                m_adaptor->getInteractor()->GetEventPosition(x,y);

                double dx = m_windowStep * ( x - m_x ) ;
                double dy = m_levelStep  * ( m_y - y ) ;

                m_adaptor->updateWindowing(dx, dy);
                m_adaptor->update();
            }
        }
        else if (m_adaptor->getInteractor()->GetShiftKey())
        {
            vtkRenderWindowInteractor *rwi = vtkRenderWindowInteractor::SafeDownCast(caller);
            char *keySym = rwi->GetKeySym();
            if(keySym != NULL)
            {
                if (std::string(keySym) == "R")
                {
                    m_adaptor->resetWindowing();
                }
            }
        }
    }

    void setAdaptor( NegatoWindower::sptr adaptor)
    {
        m_adaptor = adaptor;
    }

    void setPicker( vtkAbstractPropPicker *picker)
    {
        m_picker = picker;
    }

protected :
    NegatoWindower::sptr m_adaptor;
    vtkAbstractPropPicker *m_picker;

    int m_x;
    int m_y;

    double m_windowStep;
    double m_levelStep;

    bool m_mouseMoveObserved;

};

//------------------------------------------------------------------------------

NegatoWindower::NegatoWindower() throw()
{
    m_priority = .6;
    //handlingEventOff();
    addNewHandledEvent( ::fwComEd::ImageMsg::BUFFER );
    addNewHandledEvent( ::fwComEd::ImageMsg::NEW_IMAGE );
}

//------------------------------------------------------------------------------

NegatoWindower::~NegatoWindower() throw()
{


}

//------------------------------------------------------------------------------

void NegatoWindower::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    m_negato = this->getObject< ::visuVTKAdaptor::NegatoOneSlice >();

    m_negato->configure();
    SLM_ASSERT("NegatoOneSlice must be configured", m_negato->getConfigurationStatus() == CONFIGURED ) ;

    this->setRenderId( m_negato->getRenderId() );
    this->setPickerId( m_negato->getPickerId() );
}

//------------------------------------------------------------------------------

void NegatoWindower::doStart() throw(fwTools::Failed)
{
	m_negato->start();
	this->setRenderService(m_negato->getRenderService());

    SLM_ASSERT("NegatoOneSlice must be started", this->getObject< ::visuVTKAdaptor::NegatoOneSlice >()->getStatus() == STARTED ) ;
    NegatoWindowerCallback *observer = NegatoWindowerCallback::New();


    observer->setAdaptor( NegatoWindower::dynamicCast(this->getSptr()) );
    observer->setPicker(this->getPicker());

    m_vtkObserver = observer;

    m_negato->getInteractor()->AddObserver(START_WINDOWING_EVENT, m_vtkObserver, m_priority);
    m_negato->getInteractor()->AddObserver(STOP_WINDOWING_EVENT, m_vtkObserver, m_priority);
    m_negato->getInteractor()->AddObserver(vtkCommand::KeyPressEvent  , m_vtkObserver, m_priority);

    this->doUpdate();
}

//------------------------------------------------------------------------------

void NegatoWindower::doUpdate() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::fwData::Image::sptr image = m_negato->getObject< ::fwData::Image >();
    m_negato->updateImageInfos(image);
}

//------------------------------------------------------------------------------

void NegatoWindower::doSwap() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->doUpdate();
}

//------------------------------------------------------------------------------

void NegatoWindower::doStop() throw(fwTools::Failed)
{
	m_negato->getInteractor()->RemoveObservers(START_WINDOWING_EVENT  , m_vtkObserver);
	m_negato->getInteractor()->RemoveObservers(STOP_WINDOWING_EVENT, m_vtkObserver);
	m_negato->getInteractor()->RemoveObservers(vtkCommand::KeyPressEvent  , m_vtkObserver);
//  delete m_vtkObserver;
    m_vtkObserver = NULL;
    this->removeAllPropFromRenderer();
}

//------------------------------------------------------------------------------

void NegatoWindower::doUpdate( ::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed)
{
    //::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
	::fwData::Image::sptr image = m_negato->getObject< ::fwData::Image >();
    bool imageIsValid = ::fwComEd::fieldHelper::MedicalImageHelpers::checkImageValidity( image );

    if (imageIsValid)
    {
        if ( msg->hasEvent( ::fwComEd::ImageMsg::BUFFER ) || ( msg->hasEvent( ::fwComEd::ImageMsg::NEW_IMAGE )) )
        {
            doUpdate();
        }
    }
}

//------------------------------------------------------------------------------

void NegatoWindower::startWindowing( )
{
    //::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
	::fwData::Image::sptr image = m_negato->getObject< ::fwData::Image >();
    this->doUpdate();

    //Access to image information (through getObject being the negato service)
    ::fwData::Integer::sptr windowMin = m_negato->getWindowMin();
    ::fwData::Integer::sptr windowMax = m_negato->getWindowMax();
    ::fwData::TransfertFunction::sptr tfFct = m_negato->getCurrentTransfertFunction();

    //    ::fwComEd::fieldHelper::MedicalImageHelpers::updateMinMaxFromTF( m_windowMin, m_windowMax, this->getCurrentTransfertFunction() );
    ::fwComEd::fieldHelper::MedicalImageHelpers::updateMinMaxFromTF( windowMin, windowMax, tfFct );
    int min = windowMin->value();
    int max = windowMax->value();

    m_initialLevel = 0.5*(min + max);
    m_initialWindow = max - min;
}

//------------------------------------------------------------------------------

void NegatoWindower::stopWindowing( )
{}

//------------------------------------------------------------------------------

void NegatoWindower::updateWindowing( double dw, double dl )
{
//    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    ::fwData::Image::sptr image = m_negato->getObject< ::fwData::Image >();

    double newWindow = m_initialWindow + dw;
    double newLevel  = m_initialLevel - dl;
    double rmin, rmax;

    if ( fabs( newLevel ) < 0.01 )
    {
        newLevel = 0.01 * ( newLevel < 0 ? -1 : 1 );
    }

    //window's min value  thresholded to 10.01
    if ( newWindow <= 10.01 )
    {
        newWindow = 10.01;
    }

    rmin = newLevel - 0.5*fabs( newWindow );
    rmax = rmin + fabs( newWindow );

    SLM_ASSERT("Max should be greater than min", rmax > rmin );
    SLM_ASSERT("TF doesn't support max - min <= 10 [see ACH]", rmax - rmin > 10 );

    //Access to image information (through getObject being the negato service)
    ::fwData::Integer::sptr windowMin = m_negato->getWindowMin();
    ::fwData::Integer::sptr windowMax = m_negato->getWindowMax();
    ::fwData::TransfertFunction::sptr tfFct = m_negato->getCurrentTransfertFunction();


    if(windowMax->value() != rmax || windowMin->value() != rmin)
    {
    	windowMax->value() = rmax;
        windowMin->value() = rmin;

        // Update TF
        ::fwComEd::fieldHelper::MedicalImageHelpers::updateTFFromMinMax( windowMin, windowMax, tfFct );

        // Fire the message
        ::fwComEd::ImageMsg::NewSptr msg;
        msg->setWindowMinMax(windowMin, windowMax);
        ::fwServices::IEditionService::notify(this->getSptr(), image, msg);

        this->setVtkPipelineModified();
    }
}

void NegatoWindower::resetWindowing()
{
    //::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
	::fwData::Image::sptr image = m_negato->getObject< ::fwData::Image >();
    double newWindow = image->getWindowWidth();
    double newLevel  = image->getWindowCenter();
    double rmin, rmax;

    rmin = newLevel - 0.5*fabs( newWindow );
    rmax = rmin + fabs( newWindow );

    SLM_ASSERT("Max should be greater than min", rmax > rmin );
    SLM_ASSERT("TF doesn't support max - min <= 10 [see ACH]", rmax - rmin > 10 );

    //Access to image information (through getObject being the negato service)
    ::fwData::Integer::sptr windowMin = m_negato->getWindowMin();
    ::fwData::Integer::sptr windowMax = m_negato->getWindowMax();
    ::fwData::TransfertFunction::sptr tfFct = m_negato->getCurrentTransfertFunction();


    if(windowMax->value() != rmax || windowMin->value() != rmin)
    {
    	windowMax->value() = rmax;
        windowMin->value() = rmin;

        // Update TF
        ::fwComEd::fieldHelper::MedicalImageHelpers::updateTFFromMinMax( windowMin, windowMax, tfFct );

        // Fire the message
        ::fwComEd::ImageMsg::NewSptr msg;
        msg->setWindowMinMax(windowMin, windowMax);
        ::fwServices::IEditionService::notify(this->getSptr(), image, msg);

        this->setVtkPipelineModified();
    }
}


} //namespace visuVTKAdaptor
