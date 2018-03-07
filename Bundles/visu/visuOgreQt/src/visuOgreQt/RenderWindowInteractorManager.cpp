/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuOgreQt/RenderWindowInteractorManager.hpp"

#include <fwData/String.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwRenderOgre/registry/macros.hpp>
#include <fwRenderOgre/SRender.hpp>

#include <QDesktopWidget>
#include <QEvent>
#include <QRect>
#include <QVBoxLayout>
#include <QWidget>

//-----------------------------------------------------------------------------

fwRenderOgreRegisterMacro( ::visuOgreQt::RenderWindowInteractorManager,
                           ::fwRenderOgre::IRenderWindowInteractorManager::REGISTRY_KEY );

//-----------------------------------------------------------------------------

namespace visuOgreQt
{

//-----------------------------------------------------------------------------

RenderWindowInteractorManager::RenderWindowInteractorManager(
    ::fwRenderOgre::IRenderWindowInteractorManager::Key /*key*/ )
{
}

//-----------------------------------------------------------------------------

RenderWindowInteractorManager::~RenderWindowInteractorManager()
{

}

//-----------------------------------------------------------------------------

void RenderWindowInteractorManager::requestRender()
{
    m_qOgreWidget->requestRender();
}

//-----------------------------------------------------------------------------

void RenderWindowInteractorManager::createContainer( ::fwGui::container::fwContainer::sptr _parent, bool showOverlay,
                                                     bool renderOnDemand, bool fullscreen)
{
    SLM_ASSERT("Invalid parent.", _parent );
    m_parentContainer = ::fwGuiQt::container::QtContainer::dynamicCast( _parent );

    QVBoxLayout* layout = new QVBoxLayout();
    m_parentContainer->setLayout(layout);
    layout->setContentsMargins(0, 0, 0, 0);

    m_qOgreWidget = new ::visuOgreQt::Window();
    m_qOgreWidget->showOverlay(showOverlay);
    m_qOgreWidget->setAnimating(!renderOnDemand);

    QWidget* renderingContainer = QWidget::createWindowContainer(m_qOgreWidget);
    layout->addWidget(renderingContainer);
    renderingContainer->setSizePolicy(QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding));

    if(fullscreen)
    {
        // Open fullscreen widget on secondary monitor if there is one.
        QWidget* const container = m_parentContainer->getQtContainer();
        QDesktopWidget* desktop  = QApplication::desktop();
        int screenNumber         = desktop->screenNumber(container) + 1;

        if(screenNumber >= desktop->screenCount())
        {
            screenNumber = desktop->primaryScreen();
        }

        QRect screenres = desktop->screenGeometry(screenNumber);

        container->setParent(nullptr);
        container->showFullScreen();

        container->setGeometry(screenres);

        m_qOgreWidget->setFullScreen(fullscreen);
    }

    m_qOgreWidget->initialise();
}

//-----------------------------------------------------------------------------

void RenderWindowInteractorManager::connectToContainer()
{
    // Connect widget window render to render service start adaptors
    ::fwServices::IService::sptr renderService      = m_renderService.lock();
    ::fwRenderOgre::SRender::sptr ogreRenderService = ::fwRenderOgre::SRender::dynamicCast( renderService );

    if( !ogreRenderService )
    {
        SLM_ERROR("RenderService wrongly instantiated. ");
    }

    QObject::connect(m_qOgreWidget, SIGNAL(cameraClippingComputation()), this, SLOT(onCameraClippingComputation()));
    QObject::connect(m_qOgreWidget, SIGNAL(interacted(
                                               ::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo)), this,
                     SLOT(onInteracted(::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo)));
}

//-----------------------------------------------------------------------------

void RenderWindowInteractorManager::disconnectInteractor()
{
    QObject::disconnect(m_qOgreWidget, SIGNAL(cameraClippingComputation()), this, SLOT(onCameraClippingComputation()));
    QObject::disconnect(m_qOgreWidget, SIGNAL(interacted(
                                                  ::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo)), this,
                        SLOT(onInteracted(::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo)));

    m_qOgreWidget->destroyWindow();
    m_qOgreWidget = nullptr;
}

//-----------------------------------------------------------------------------

void RenderWindowInteractorManager::makeCurrent()
{
    m_qOgreWidget->makeCurrent();
}

//-----------------------------------------------------------------------------

int RenderWindowInteractorManager::getWidgetId() const
{
    return m_qOgreWidget->getId();
}

//-----------------------------------------------------------------------------

int RenderWindowInteractorManager::getFrameId() const
{
    return m_qOgreWidget->getFrameId();
}

//-----------------------------------------------------------------------------

::Ogre::RenderTarget* RenderWindowInteractorManager::getRenderTarget()
{
    return m_qOgreWidget->getOgreRenderWindow();
}

//-----------------------------------------------------------------------------

void RenderWindowInteractorManager::onInteracted(::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo _info)
{
    ::fwServices::IService::sptr renderService      = m_renderService.lock();
    ::fwRenderOgre::SRender::sptr ogreRenderService = ::fwRenderOgre::SRender::dynamicCast( renderService );

    for(auto layerMap : ogreRenderService->getLayers())
    {
        ::fwRenderOgre::Layer::sptr layer = layerMap.second;
        layer->slot< ::fwRenderOgre::Layer::InteractionSlotType>(::fwRenderOgre::Layer::s_INTERACTION_SLOT)->asyncRun(
            _info);
    }
}

//-----------------------------------------------------------------------------

void RenderWindowInteractorManager::onCameraClippingComputation()
{
    ::fwServices::IService::sptr renderService      = m_renderService.lock();
    ::fwRenderOgre::SRender::sptr ogreRenderService = ::fwRenderOgre::SRender::dynamicCast( renderService );

    ogreRenderService->slot(::fwRenderOgre::SRender::s_COMPUTE_CAMERA_CLIPPING_SLOT)->asyncRun();
}

//-----------------------------------------------------------------------------

} // namespace visuOgreQt
