/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKQt/VtkRenderWindowInteractorManager.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwData/String.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwRenderVTK/registry/macros.hpp>
#include <fwRenderVTK/SRender.hpp>

#include <fwTools/UUID.hpp>

#include <QDropEvent>
#include <QEvent>
#include <QMimeData>
#include <QSurfaceFormat>
#include <QVBoxLayout>
#include <QVTKOpenGLWidget.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>

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

    QVBoxLayout* layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    m_parentContainer->setLayout(layout);

    // Create the render window and the associated QVTKOpenGLWidget
    vtkNew<vtkGenericOpenGLRenderWindow> window;
    m_QVTKOpenGLWidget = new QVTKOpenGLWidget(m_parentContainer->getQtContainer());
    layout->addWidget(m_QVTKOpenGLWidget);
    m_QVTKOpenGLWidget->SetRenderWindow(window.Get());

    // Install Drop filter
    if(auto locked = m_renderService.lock())
    {
        m_QVTKOpenGLWidget->setAcceptDrops(true);
        m_QVTKOpenGLWidget->installEventFilter(new DropFilter(locked));
    }

    m_interactor = window->GetInteractor();
}

//-----------------------------------------------------------------------------

void VtkRenderWindowInteractorManager::uninstallInteractor()
{
    m_interactor = nullptr;

    delete m_QVTKOpenGLWidget;
    m_QVTKOpenGLWidget.clear();

    m_parentContainer->clean();
}

//-----------------------------------------------------------------------------

::vtkRenderWindowInteractor* VtkRenderWindowInteractorManager::getInteractor()
{
    return m_interactor;
}

//-----------------------------------------------------------------------------

} // namespace visuVTKQt

