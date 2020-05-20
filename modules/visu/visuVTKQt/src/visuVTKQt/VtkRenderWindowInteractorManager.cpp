/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

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
#if VTK_MAJOR_VERSION > 7
#include <QVTKOpenGLNativeWidget.h>
#else
#include <QVTKWidget.h>
#endif
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

#if VTK_MAJOR_VERSION > 7
    m_parentContainer->setLayout(layout);

    // Create the render window and the associated QVTKOpenGLWidget
    vtkNew<vtkGenericOpenGLRenderWindow> window;
    m_qVTKWidget = new QVTKOpenGLNativeWidget(m_parentContainer->getQtContainer());
    m_qVTKWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addWidget(m_qVTKWidget);
    m_qVTKWidget->SetRenderWindow(window.Get());
#else
    m_qVTKWidget = new QVTKWidget();
#endif
    // Install Drop filter
    if(auto locked = m_renderService.lock())
    {
        m_qVTKWidget->setAcceptDrops(true);
        m_qVTKWidget->installEventFilter(new DropFilter(locked));
    }

#if VTK_MAJOR_VERSION > 7
    m_interactor = window->GetInteractor();
#else
    layout->addWidget(m_qVTKWidget);
    m_parentContainer->setLayout(layout);

    m_interactor = m_qVTKWidget->GetRenderWindow()->GetInteractor();
#endif
}

//-----------------------------------------------------------------------------

void VtkRenderWindowInteractorManager::uninstallInteractor()
{
    m_interactor = nullptr;

    delete m_qVTKWidget;
    m_qVTKWidget.clear();

    m_parentContainer->clean();
}

//-----------------------------------------------------------------------------

::vtkRenderWindowInteractor* VtkRenderWindowInteractorManager::getInteractor()
{
    return m_interactor;
}

//-----------------------------------------------------------------------------

} // namespace visuVTKQt
