/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QVBoxLayout>

#include <QVTKWidget.h>

#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>

#include <fwTools/ClassRegistrar.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include "visuVTKQt/VtkRenderWindowInteractorManager.hpp"


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

void VtkRenderWindowInteractorManager::installInteractor( ::fwGui::fwContainer::sptr _parent )
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

    layout->addWidget(m_qVTKWidget);
    container->setLayout(layout);

    m_interactor = m_qVTKWidget->GetRenderWindow()->GetInteractor();
    m_qVTKWidget->GetRenderWindow()->AddRenderer(vtkRenderer::New());
}

//-----------------------------------------------------------------------------

void VtkRenderWindowInteractorManager::uninstallInteractor()
{
    QWidget* container = qobject_cast <QWidget*> (m_qVTKWidget->parent());

    m_interactor = 0;

    if (container)
    {
        container->layout()->deleteLater();
    }

    m_qVTKWidget->hide();
    m_qVTKWidget->deleteLater();
}

//-----------------------------------------------------------------------------

::vtkRenderWindowInteractor * VtkRenderWindowInteractorManager::getInteractor()
{
    return m_interactor;
}

//-----------------------------------------------------------------------------

} // namespace visuVTKQt




