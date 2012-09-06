/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/ClassRegistrar.hpp>

#include <QVBoxLayout>
#include <QEvent>
#include <QDropEvent>

#include <QVTKWidget.h>

#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>

#include <fwData/String.hpp>

#include <fwTools/UUID.hpp>

#include <fwServices/GlobalEventManager.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include "visuVTKQt/VtkRenderWindowInteractorManager.hpp"

class DropFilter : public QObject
 {
 public:
     DropFilter(::fwServices::IService::sptr service): m_service(service){};
 protected:
     bool eventFilter(QObject *obj, QEvent *event);

 private:
     ::fwServices::IService::wptr m_service;
 };

 bool DropFilter::eventFilter(QObject *obj, QEvent *event)
 {
     if( event->type() == QEvent::DragEnter)
     {
         QDragEnterEvent *dragEvent=dynamic_cast< QDragEnterEvent* >(event);
         if (dragEvent->mimeData()->hasFormat("text/plain"))
         {
             QString data = dragEvent->mimeData()->text();
             dragEvent->acceptProposedAction();
         }
     }
     else if (event->type() == QEvent::Drop )
     {
         QDropEvent* dropEvent = dynamic_cast< QDropEvent* >(event);
         QString data = dropEvent->mimeData()->text();
         ::fwServices::ObjectMsg::sptr message = ::fwServices::ObjectMsg::New();
         message->addEvent("DROPPED_UUID", ::fwData::String::New(data.toStdString()));
         ::fwServices::IService::sptr service = m_service.lock();
         ::fwServices::IEditionService::notify( service, service->getObject< ::fwData::Object >(), message );
     } else {
         // standard event processing
         return QObject::eventFilter(obj, event);
     }
     return true;
 }

//-----------------------------------------------------------------------------

REGISTER_BINDING(   ::fwRenderVTK::IVtkRenderWindowInteractorManager,
        ::visuVTKQt::VtkRenderWindowInteractorManager,
        ::fwRenderVTK::IVtkRenderWindowInteractorManager::FactoryRegistryKeyType,
        ::fwRenderVTK::IVtkRenderWindowInteractorManager::REGISTRY_KEY );

//-----------------------------------------------------------------------------

namespace visuVTKQt
{

//-----------------------------------------------------------------------------

VtkRenderWindowInteractorManager::VtkRenderWindowInteractorManager()
{}

//-----------------------------------------------------------------------------

VtkRenderWindowInteractorManager::~VtkRenderWindowInteractorManager()
{}

//-----------------------------------------------------------------------------

void VtkRenderWindowInteractorManager::installInteractor( ::fwGui::container::fwContainer::sptr _parent )
{
    SLM_ASSERT("Invalid parent.", _parent ) ;
    ::fwGuiQt::container::QtContainer::sptr qtContainer =  ::fwGuiQt::container::QtContainer::dynamicCast( _parent );
    QWidget* container = qtContainer->getQtContainer();
    SLM_ASSERT("The container is not a qtContainer.", container ) ;

    m_qVTKWidget = new QVTKWidget(container);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    //m_qVTKWidget->resize(container->width(), container->height());
    //m_qVTKWidget->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
    if(m_renderService.lock())
    {
        m_qVTKWidget->setAcceptDrops(true);
        m_qVTKWidget->installEventFilter(new DropFilter(m_renderService.lock()));
    }

    layout->addWidget(m_qVTKWidget);
    container->setLayout(layout);

    m_interactor = m_qVTKWidget->GetRenderWindow()->GetInteractor();
}

//-----------------------------------------------------------------------------

void VtkRenderWindowInteractorManager::uninstallInteractor()
{
    QWidget* container = qobject_cast <QWidget*> (m_qVTKWidget->parent());

    m_interactor = 0;

    if (container)
    {
        container->layout()->deleteLater();
        container->setLayout(0);
    }

    m_qVTKWidget->hide();
    m_qVTKWidget->setParent(0);
    m_qVTKWidget->deleteLater();
}

//-----------------------------------------------------------------------------

::vtkRenderWindowInteractor * VtkRenderWindowInteractorManager::getInteractor()
{
    return m_interactor;
}

//-----------------------------------------------------------------------------

} // namespace visuVTKQt




