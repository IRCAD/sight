/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>


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

#include "visuVTKAdaptor/NegatoWindowingInteractor.hpp"
#include "fwRenderVTK/vtk/fwVtkCellPicker.hpp"

REGISTER_SERVICE( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::NegatoWindowingInteractor, ::fwData::Image ) ;


#define START_WINDOWING_EVENT vtkCommand::RightButtonPressEvent
#define STOP_WINDOWING_EVENT  vtkCommand::RightButtonReleaseEvent

namespace visuVTKAdaptor
{

class NegatoWindowingCallback : public vtkCommand
{
public:
    static NegatoWindowingCallback *New()
    { return new NegatoWindowingCallback(); }

    NegatoWindowingCallback() : m_mouseMoveObserved(false)
    {
        m_windowStep = 1. ;
        m_levelStep  = 1. ;
        this->PassiveObserverOff();
    }

    ~NegatoWindowingCallback()
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
                    m_adaptor->getInteractor()->AddObserver(vtkCommand::MouseMoveEvent, this, 1.);
                    m_mouseMoveObserved = true;
                    SetAbortFlag(1);
                    m_adaptor->startWindowing();
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

            if (std::string(keySym) == "R")
            {
                m_adaptor->resetWindowing();
            }
        }
    }

    void setAdaptor( NegatoWindowingInteractor::sptr adaptor)
    {
        m_adaptor = adaptor;
    }

    void setPicker( vtkAbstractPropPicker *picker)
    {
        m_picker = picker;
    }

protected :
    NegatoWindowingInteractor::sptr m_adaptor;
    vtkAbstractPropPicker *m_picker;

    int m_x;
    int m_y;

    double m_windowStep;
    double m_levelStep;

    bool m_mouseMoveObserved;

};

//------------------------------------------------------------------------------

NegatoWindowingInteractor::NegatoWindowingInteractor() throw()
{
    m_priority = .6;
    handlingEventOff();
}

//------------------------------------------------------------------------------

NegatoWindowingInteractor::~NegatoWindowingInteractor() throw()
{


}

//------------------------------------------------------------------------------

void NegatoWindowingInteractor::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    SLM_ASSERT("Tag config is required", m_configuration->getName() == "config");
    this->setRenderId( m_configuration->getAttributeValue("renderer") );
    this->setPickerId( m_configuration->getAttributeValue("picker") );
}

//------------------------------------------------------------------------------

void NegatoWindowingInteractor::doStart() throw(fwTools::Failed)
{
    NegatoWindowingCallback *observer = NegatoWindowingCallback::New();
    observer->setAdaptor( NegatoWindowingInteractor::dynamicCast(this->getSptr()) );
    observer->setPicker(this->getPicker());

    m_vtkObserver = observer;

    this->getInteractor()->AddObserver(START_WINDOWING_EVENT, m_vtkObserver, m_priority);
    this->getInteractor()->AddObserver(STOP_WINDOWING_EVENT, m_vtkObserver, m_priority);
    this->getInteractor()->AddObserver(vtkCommand::KeyPressEvent  , m_vtkObserver, m_priority);
}

//------------------------------------------------------------------------------

void NegatoWindowingInteractor::doUpdate() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void NegatoWindowingInteractor::doSwap() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void NegatoWindowingInteractor::doStop() throw(fwTools::Failed)
{
    this->getInteractor()->RemoveObservers(START_WINDOWING_EVENT  , m_vtkObserver);
    this->getInteractor()->RemoveObservers(STOP_WINDOWING_EVENT, m_vtkObserver);
    this->getInteractor()->RemoveObservers(vtkCommand::KeyPressEvent  , m_vtkObserver);
//  delete m_vtkObserver;
    m_vtkObserver = NULL;
    this->removeAllPropFromRenderer();
}

//------------------------------------------------------------------------------

void NegatoWindowingInteractor::doUpdate( ::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void NegatoWindowingInteractor::startWindowing( )
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    this->updateImageInfos(image);

    ::fwData::Integer::sptr imageMin = m_windowMin;
    ::fwData::Integer::sptr imageMax = m_windowMax;

    ::fwComEd::fieldHelper::MedicalImageHelpers::updateMinMaxFromTF( image );

    int max = imageMax->value();
    int min = imageMin->value();

    m_initialLevel = 0.5*(min + max);
    m_initialWindow = max - min;
}

//------------------------------------------------------------------------------

void NegatoWindowingInteractor::stopWindowing( )
{}

//------------------------------------------------------------------------------

void NegatoWindowingInteractor::updateWindowing( double dw, double dl )
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();

    ::fwData::Integer::sptr imageMin = m_windowMin;
    ::fwData::Integer::sptr imageMax = m_windowMax;

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

    if(imageMax->value() != rmax || imageMin->value() != rmin)
    {
        imageMax->value() = rmax;
        imageMin->value() = rmin;

        // Update TF
        ::fwComEd::fieldHelper::MedicalImageHelpers::updateTFFromMinMax( image );

        // Check if fields exist and are valid
        std::pair<bool,bool> fieldsAreModified = ::fwComEd::fieldHelper::MedicalImageHelpers::checkMinMaxTF( image );

        // Temp test because theses cases are not manage ( need to notify if there are modifications of Min/Max/TF )
        assert( ! fieldsAreModified.first && ! fieldsAreModified.second );

        // Fire the message
        ::fwComEd::ImageMsg::NewSptr msg;
        msg->addEvent( ::fwComEd::ImageMsg::WINDOWING ) ;
        ::fwServices::IEditionService::notify(this->getSptr(), image, msg);

        this->setVtkPipelineModified();
    }
}

void NegatoWindowingInteractor::resetWindowing()
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    double newWindow = image->getWindowWidth();
    double newLevel  = image->getWindowCenter();
    double rmin, rmax;

    ::fwData::Integer::sptr imageMin = m_windowMin;
    ::fwData::Integer::sptr imageMax = m_windowMax;

    rmin = newLevel - 0.5*fabs( newWindow );
    rmax = rmin + fabs( newWindow );

    SLM_ASSERT("Max should be greater than min", rmax > rmin );
    SLM_ASSERT("TF doesn't support max - min <= 10 [see ACH]", rmax - rmin > 10 );

    if(imageMax->value() != rmax || imageMin->value() != rmin)
    {
        imageMax->value() = rmax;
        imageMin->value() = rmin;

        // Update TF
        ::fwComEd::fieldHelper::MedicalImageHelpers::updateTFFromMinMax( image );

        // Check if fields exist and are valid
        std::pair<bool,bool> fieldsAreModified = ::fwComEd::fieldHelper::MedicalImageHelpers::checkMinMaxTF( image );

        // Temp test because theses cases are not manage ( need to notify if there are modifications of Min/Max/TF )
        assert( ! fieldsAreModified.first && ! fieldsAreModified.second );

        // Fire the message
        ::fwComEd::ImageMsg::NewSptr msg;
        msg->addEvent( ::fwComEd::ImageMsg::WINDOWING ) ;
        ::fwServices::IEditionService::notify(this->getSptr(), image, msg);

        this->setVtkPipelineModified();
    }
}


} //namespace visuVTKAdaptor
