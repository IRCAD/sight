/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/SNegatoWindowingInteractor.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>

#include <fwData/Image.hpp>
#include <fwData/Integer.hpp>
#include <fwData/TransferFunction.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>
#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>

#include <fwRenderVTK/IAdaptor.hpp>
#include <fwRenderVTK/vtk/fwVtkCellPicker.hpp>

#include <fwServices/macros.hpp>

#include <vtkCommand.h>
#include <vtkInteractorStyleImage.h>
#include <vtkRenderWindowInteractor.h>

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::SNegatoWindowingInteractor);

#define START_WINDOWING_EVENT vtkCommand::RightButtonPressEvent
#define STOP_WINDOWING_EVENT  vtkCommand::RightButtonReleaseEvent

namespace visuVTKAdaptor
{

static const ::fwServices::IService::KeyType s_IMAGE_INOUT        = "image";
static const ::fwServices::IService::KeyType s_TF_SELECTION_INOUT = "tfSelection";

//------------------------------------------------------------------------------

class NegatoWindowingCallback : public vtkCommand
{
public:
    //------------------------------------------------------------------------------

    static NegatoWindowingCallback* New()
    {
        return new NegatoWindowingCallback();
    }

    NegatoWindowingCallback() :
        m_picker(nullptr),
        m_x(0),
        m_y(0),
        m_windowStep(1.),
        m_levelStep(1.),
        m_mouseMoveObserved(false)
    {
        this->PassiveObserverOff();
    }

    ~NegatoWindowingCallback()
    {

    }

    //------------------------------------------------------------------------------

    virtual void Execute( vtkObject* caller, unsigned long eventId, void*)
    {
        if ( m_mouseMoveObserved || !m_adaptor->getInteractor()->GetShiftKey() )
        {
            if ( eventId == START_WINDOWING_EVENT)
            {
                SLM_ASSERT("m_adaptor not instanced", m_adaptor);
                SLM_ASSERT("m_picker not instanced", m_picker);

                double display[3];

                m_adaptor->getInteractor()->GetEventPosition(m_x, m_y);
                display[0] = m_x;
                display[1] = m_y;
                display[2] = 0;

                if ( m_picker->Pick( display, m_adaptor->getRenderer() ) )
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
                SLM_ASSERT("m_adaptor not instanced", m_adaptor);
                SLM_ASSERT("m_picker not instanced", m_picker);

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
                SLM_ASSERT("m_mouseMoveObserved not instanced", m_mouseMoveObserved);
                int x, y;
                m_adaptor->getInteractor()->GetEventPosition(x, y);

                double dx = m_windowStep * ( x - m_x );
                double dy = m_levelStep  * ( m_y - y );

                m_adaptor->updateWindowing(dx, dy);
                m_adaptor->update();
            }
        }
        else if (m_adaptor->getInteractor()->GetShiftKey())
        {
            vtkRenderWindowInteractor* rwi = vtkRenderWindowInteractor::SafeDownCast(caller);
            char* keySym                   = rwi->GetKeySym();
            if(keySym != nullptr)
            {
                if (std::string(keySym) == "R")
                {
                    m_adaptor->resetWindowing();
                }
            }
        }
    }

    //------------------------------------------------------------------------------

    void setAdaptor( SNegatoWindowingInteractor::sptr adaptor)
    {
        m_adaptor = adaptor;
    }

    //------------------------------------------------------------------------------

    void setPicker( vtkAbstractPropPicker* picker)
    {
        m_picker = picker;
    }

protected:
    SNegatoWindowingInteractor::sptr m_adaptor;
    vtkAbstractPropPicker* m_picker;

    int m_x;
    int m_y;

    double m_windowStep;
    double m_levelStep;

    bool m_mouseMoveObserved;

};

//------------------------------------------------------------------------------

SNegatoWindowingInteractor::SNegatoWindowingInteractor() noexcept :
    m_vtkObserver(nullptr),
    m_initialWindow(0.),
    m_initialLevel(0.),
    m_priority(.6f)
{
}

//------------------------------------------------------------------------------

SNegatoWindowingInteractor::~SNegatoWindowingInteractor() noexcept
{
}

//------------------------------------------------------------------------------

void SNegatoWindowingInteractor::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    this->setSelectedTFKey(config.get<std::string>("selectedTFKey", ""));
}

//------------------------------------------------------------------------------

void SNegatoWindowingInteractor::starting()
{
    this->initialize();

    ::fwData::Composite::sptr tfSelection = this->getInOut< ::fwData::Composite >(s_TF_SELECTION_INOUT);
    this->setTransferFunctionSelection(tfSelection);

    NegatoWindowingCallback* observer = NegatoWindowingCallback::New();
    observer->setAdaptor( SNegatoWindowingInteractor::dynamicCast(this->getSptr()) );
    observer->setPicker(this->getPicker());

    m_vtkObserver = observer;

    this->getInteractor()->AddObserver(START_WINDOWING_EVENT, m_vtkObserver, m_priority);
    this->getInteractor()->AddObserver(STOP_WINDOWING_EVENT, m_vtkObserver, m_priority);
    this->getInteractor()->AddObserver(vtkCommand::KeyPressEvent, m_vtkObserver, m_priority);

    this->updating();
}

//------------------------------------------------------------------------------

void SNegatoWindowingInteractor::updating()
{
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("Missing image", image);

    this->updateImageInfos(image);
    this->updateTransferFunction(image);
    this->requestRender();
}

//------------------------------------------------------------------------------

void SNegatoWindowingInteractor::stopping()
{
    this->getInteractor()->RemoveObservers(START_WINDOWING_EVENT, m_vtkObserver);
    this->getInteractor()->RemoveObservers(STOP_WINDOWING_EVENT, m_vtkObserver);
    this->getInteractor()->RemoveObservers(vtkCommand::KeyPressEvent, m_vtkObserver);
    m_vtkObserver->Delete();
    m_vtkObserver = nullptr;
    this->removeAllPropFromRenderer();
}

//------------------------------------------------------------------------------

void SNegatoWindowingInteractor::startWindowing( )
{
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("Missing image", image);

    this->updating();

    m_initialLevel  = this->getLevel();
    m_initialWindow = this->getWindow();
}

//------------------------------------------------------------------------------

void SNegatoWindowingInteractor::stopWindowing( )
{
}

//------------------------------------------------------------------------------

void SNegatoWindowingInteractor::updateWindowing( double dw, double dl )
{
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("Missing image", image);
    ::fwData::TransferFunction::sptr tf = this->getTransferFunction();

    double newWindow = m_initialWindow + dw;
    double newLevel  = m_initialLevel - dl;

    tf->setWindow( newWindow );
    tf->setLevel( newLevel );

    auto sig = tf->signal< ::fwData::TransferFunction::WindowingModifiedSignalType >(
        ::fwData::TransferFunction::s_WINDOWING_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdateTFWindowing));
        sig->asyncEmit( newWindow, newLevel);
    }

    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void SNegatoWindowingInteractor::resetWindowing()
{
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("Missing image", image);
    ::fwData::TransferFunction::sptr tf = this->getTransferFunction();

    double newWindow = image->getWindowWidth();
    double newLevel  = image->getWindowCenter();

    tf->setWindow( newWindow );
    tf->setLevel( newLevel );

    auto sig = tf->signal< ::fwData::TransferFunction::WindowingModifiedSignalType >(
        ::fwData::TransferFunction::s_WINDOWING_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdateTFWindowing));
        sig->asyncEmit( newWindow, newLevel);
    }

    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SNegatoWindowingInteractor::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_IMAGE_INOUT, ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_IMAGE_INOUT, ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor
