/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QVBoxLayout>
#include <QEvent>
#include <QDropEvent>

#include <QVTKWidget.h>

#include <QMimeData>

#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>

#include <fwData/String.hpp>

#include <fwTools/UUID.hpp>

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwRenderVTK/SRender.hpp>
#include <fwRenderVTK/registry/macros.hpp>
#include <fwGuiQt/container/QtContainer.hpp>

#include "visuVTKQt/VtkRenderWindowInteractorManager.hpp"

#ifdef __APPLE__
// XXX Retinhack
// hack to fix vtk render windows on mac retina screen, until fixed
// by vtk
#include <QWindow>
#include <QScreen>
#include <QVTKInteractor.h>
#include <vtkCocoaRenderWindow.h>
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <QMetaEnum>

class RetinHackRWI : public QVTKInteractor
{
protected:
    RetinHackRWI() :
        QVTKInteractor()
    {
    }

public:

    static RetinHackRWI* New();

    //------------------------------------------------------------------------------

    virtual int* GetLastEventPosition ()
    {
        auto LastPos = this->QVTKInteractor::GetLastEventPosition();
        const auto r = this->Ratio;

        LastRetinaPos[0] = LastPos[0]*r;
        LastRetinaPos[1] = LastPos[1]*r;

        return LastRetinaPos;
    }

    //------------------------------------------------------------------------------

    virtual void GetLastEventPosition (int& x, int& y)
    {
        auto LastPos = this->QVTKInteractor::GetLastEventPosition();
        const auto r = this->Ratio;

        x = LastPos[0]*r;
        y = LastPos[1]*r;
    }

    //------------------------------------------------------------------------------

    virtual void GetLastEventPosition (int xy[2])
    {
        auto LastPos = this->QVTKInteractor::GetLastEventPosition();
        const auto r = this->Ratio;

        xy[0] = LastPos[0]*r;
        xy[1] = LastPos[1]*r;
    }

    //------------------------------------------------------------------------------

    virtual int* GetEventPosition ()
    {
        auto Pos     = this->QVTKInteractor::GetEventPosition();
        const auto r = this->Ratio;

        RetinaPos[0] = Pos[0]*r;
        RetinaPos[1] = Pos[1]*r;

        return RetinaPos;
    }

    //------------------------------------------------------------------------------

    virtual void GetEventPosition (int& x, int& y)
    {
        auto Pos     = this->QVTKInteractor::GetEventPosition();
        const auto r = this->Ratio;

        x = Pos[0]*r;
        y = Pos[1]*r;
    }

    //------------------------------------------------------------------------------

    virtual void GetEventPosition (int xy[2])
    {
        auto Pos     = this->QVTKInteractor::GetEventPosition();
        const auto r = this->Ratio;

        xy[0] = Pos[0]*r;
        xy[1] = Pos[1]*r;
    }

    //------------------------------------------------------------------------------

    void SetDevicePixelRatio(int r)
    {
        this->Ratio = r;
    }

private:
    int RetinaPos[2];
    int LastRetinaPos[2];

    int Ratio = 2;

};

vtkStandardNewMacro(RetinHackRWI);

class RetinhackRenderWindow : public vtkCocoaRenderWindow
{
protected:
    RetinhackRenderWindow() :
        vtkCocoaRenderWindow()
    {
    }

public:

    static RetinhackRenderWindow* New();

    //------------------------------------------------------------------------------

    int* GetSize()
    {
        this->vtkCocoaRenderWindow::GetSize();

        this->RetinaSize[0] = this->Size[0] * this->Ratio;
        this->RetinaSize[1] = this->Size[1] * this->Ratio;

        return this->RetinaSize;
    }

    //------------------------------------------------------------------------------

    int* GetScreenSize()
    {
        this->vtkCocoaRenderWindow::GetScreenSize();

        this->RetinaSize[0] = this->Size[0] * this->Ratio;
        this->RetinaSize[1] = this->Size[1] * this->Ratio;

        return this->RetinaSize;
    }

    //------------------------------------------------------------------------------

    vtkRenderWindowInteractor* MakeRenderWindowInteractor()
    {
        this->Interactor = RetinHackRWI::New();
        this->Interactor->SetRenderWindow(this);
        return this->Interactor;
    }

    //------------------------------------------------------------------------------

    void SetDevicePixelRatio(int r)
    {
        this->Ratio = r;
        static_cast< RetinHackRWI* >( this->Interactor )->SetDevicePixelRatio(r);
    }

private:
    int RetinaSize[2] = {600, 600};
    int Ratio         = 2;
};

vtkStandardNewMacro(RetinhackRenderWindow);

class RetinhackVtkWidget : public QVTKWidget
{
public:
    RetinhackVtkWidget(QWidget* container = nullptr) :
        QVTKWidget(container)
    {
        vtkSmartPointer< vtkInteractorStyleTrackballCamera > interactorStyle
            = vtkInteractorStyleTrackballCamera::New();
        vtkSmartPointer< RetinhackRenderWindow > renderWindow = RetinhackRenderWindow::New();
        vtkSmartPointer< RetinHackRWI > rwi                   = RetinHackRWI::New();

        rwi->Initialize();
        rwi->SetInteractorStyle(interactorStyle);
        rwi->SetRenderWindow(renderWindow);
        renderWindow->SetInteractor(rwi);

        this->SetRenderWindow(renderWindow);

    }

    //------------------------------------------------------------------------------

    bool event(QEvent* e)
    {
        auto ratio = this->devicePixelRatio();
        if (m_ratio != ratio )
        {
            SLM_ASSERT( "Render window shall be a RetinhackRenderWindow",
                        dynamic_cast< RetinhackRenderWindow* >( this->mRenWin ));
            static_cast< RetinhackRenderWindow* >( this->mRenWin )->SetDevicePixelRatio(ratio);
            m_ratio = ratio;
        }

        if(e->type() == QEvent::ShowToParent)
        {
            QObject::disconnect(m_renderConnection);
            QPointer<RetinhackVtkWidget> widget(this);
            m_renderConnection = QObject::connect(this->window()->windowHandle(), &QWindow::screenChanged, [ = ] ()
            {
                if(!widget)
                {
                    return;
                }
                auto ratio = widget->devicePixelRatio();
                SLM_ASSERT( "Render window shall be a RetinhackRenderWindow",
                            dynamic_cast< RetinhackRenderWindow* >( widget->mRenWin ));
                static_cast< RetinhackRenderWindow* >( widget->mRenWin )->SetDevicePixelRatio(ratio);
                widget->mRenWin->Render();
            });
        }

        return QVTKWidget::event(e);
    }

    //------------------------------------------------------------------------------

    virtual void SetRenderWindow(vtkRenderWindow* rw)
    {
        QVTKWidget::SetRenderWindow(rw);

        auto ratio = this->devicePixelRatio();

        SLM_ASSERT( "Render window shall be a RetinhackRenderWindow",
                    dynamic_cast< RetinhackRenderWindow* >( this->mRenWin ));
        static_cast< RetinhackRenderWindow* >( this->mRenWin )->SetDevicePixelRatio(ratio);
    }

protected:
    int m_ratio = -1;

    QMetaObject::Connection m_renderConnection;
};

#define QVTKWidget RetinhackVtkWidget

#endif //__APPLE__

class DropFilter : public QObject
{
public:
    DropFilter(::fwServices::IService::sptr service) :
        m_service(service)
    {
    }
protected:
    bool eventFilter(QObject* obj, QEvent* event);

private:
    ::fwServices::IService::wptr m_service;
};

//------------------------------------------------------------------------------

bool DropFilter::eventFilter(QObject* obj, QEvent* event)
{
    if( event->type() == QEvent::DragEnter)
    {
        QDragEnterEvent* dragEvent = dynamic_cast< QDragEnterEvent* >(event);
        if (dragEvent->mimeData()->hasFormat("text/plain"))
        {
            QString data = dragEvent->mimeData()->text();
            dragEvent->acceptProposedAction();
        }
    }
    else if (event->type() == QEvent::Drop )
    {
        QDropEvent* dropEvent = dynamic_cast< QDropEvent* >(event);
        QString data          = dropEvent->mimeData()->text();
        ::fwServices::IService::sptr service = m_service.lock();
        auto sig = service->signal< ::fwRenderVTK::SRender::DroppedSignalType >(
            ::fwRenderVTK::SRender::s_DROPPED_SIG);
        sig->asyncEmit( data.toStdString() );
    }
    else
    {
        // standard event processing
        return QObject::eventFilter(obj, event);
    }
    return true;
}

//-----------------------------------------------------------------------------

fwRenderVTKRegisterMacro( ::visuVTKQt::VtkRenderWindowInteractorManager,
                          ::fwRenderVTK::IVtkRenderWindowInteractorManager::REGISTRY_KEY );

//-----------------------------------------------------------------------------

namespace visuVTKQt
{

//-----------------------------------------------------------------------------

VtkRenderWindowInteractorManager::VtkRenderWindowInteractorManager(
    ::fwRenderVTK::IVtkRenderWindowInteractorManager::Key key )
{
}

//-----------------------------------------------------------------------------

VtkRenderWindowInteractorManager::~VtkRenderWindowInteractorManager()
{
}

//-----------------------------------------------------------------------------

void VtkRenderWindowInteractorManager::installInteractor( ::fwGui::container::fwContainer::sptr _parent )
{
    SLM_ASSERT("Invalid parent.", _parent );
    m_parentContainer = ::fwGuiQt::container::QtContainer::dynamicCast( _parent );

    m_qVTKWidget = new QVTKWidget();

    QVBoxLayout* layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);

    if(m_renderService.lock())
    {
        m_qVTKWidget->setAcceptDrops(true);
        m_qVTKWidget->installEventFilter(new DropFilter(m_renderService.lock()));
    }

    layout->addWidget(m_qVTKWidget);
    m_parentContainer->setLayout(layout);

    m_interactor = m_qVTKWidget->GetRenderWindow()->GetInteractor();
}

//-----------------------------------------------------------------------------

void VtkRenderWindowInteractorManager::uninstallInteractor()
{
    m_interactor = nullptr;

    delete m_qVTKWidget;
    m_qVTKWidget = nullptr;

    m_parentContainer->clean();
}

//-----------------------------------------------------------------------------

::vtkRenderWindowInteractor* VtkRenderWindowInteractorManager::getInteractor()
{
    return m_interactor;
}

//-----------------------------------------------------------------------------

} // namespace visuVTKQt

