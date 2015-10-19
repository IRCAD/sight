/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuOgreQt/RenderWindowInteractorManager.hpp"

#include <fwData/String.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwRenderOgre/registry/macros.hpp>
#include <fwRenderOgre/SRender.hpp>

#include <QEvent>
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
    ::fwRenderOgre::IRenderWindowInteractorManager::Key key )
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

void RenderWindowInteractorManager::createContainer( ::fwGui::container::fwContainer::sptr _parent, bool showOverlay )
{
    SLM_ASSERT("Invalid parent.", _parent );
    m_parentContainer = ::fwGuiQt::container::QtContainer::dynamicCast( _parent );
    QWidget* container = m_parentContainer->getQtContainer();
    SLM_ASSERT("The container is not a qtContainer.", container );

    m_qOgreWidget = new ::visuOgreQt::Window();
    m_qOgreWidget->showOverlay(showOverlay);
    QWidget* renderingContainer = QWidget::createWindowContainer(m_qOgreWidget);
    renderingContainer->setSizePolicy(QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding));

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(renderingContainer);

    container->setLayout(layout);

    QObject::connect(m_qOgreWidget, SIGNAL(renderWindowCreated()), this, SLOT(onRenderWindowCreated()));
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

    QObject::connect(m_qOgreWidget, SIGNAL(rayCastRequested(int, int, int, int)), this,
                     SLOT(onRayCastRequested(int, int, int, int)));
    QObject::connect(m_qOgreWidget, SIGNAL(cameraClippingComputation()), this, SLOT(onCameraClippingComputation()));
    QObject::connect(m_qOgreWidget, SIGNAL(interacted(
                                               ::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo)), this,
                     SLOT(onInteracted(::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo)));
}

//-----------------------------------------------------------------------------

void RenderWindowInteractorManager::disconnectInteractor()
{
    QObject::disconnect(m_qOgreWidget, SIGNAL(renderWindowCreated()), this, SLOT(onRenderWindowCreated()));
    QObject::disconnect(m_qOgreWidget, SIGNAL(rayCastRequested(int, int, int, int)), this,
                        SLOT(onRayCastRequested(int, int, int, int)));
    QObject::disconnect(m_qOgreWidget, SIGNAL(cameraClippingComputation()), this, SLOT(onCameraClippingComputation()));
    QObject::disconnect(m_qOgreWidget, SIGNAL(interacted(
                                                  ::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo)), this,
                        SLOT(onInteracted(::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo)));

    ::fwServices::IService::sptr renderService      = m_renderService.lock();
    ::fwRenderOgre::SRender::sptr ogreRenderService = ::fwRenderOgre::SRender::dynamicCast( renderService );

    for(auto layerMap : ogreRenderService->getLayers())
    {
        ::fwRenderOgre::Layer::sptr layer = layerMap.second;
        layer->slot(::fwRenderOgre::Layer::s_DESTROY_SLOT)->asyncRun();
    }

    m_qOgreWidget->destroyWindow();
    m_qOgreWidget = nullptr;
}

//-----------------------------------------------------------------------------

void RenderWindowInteractorManager::makeCurrent()
{
    m_qOgreWidget->makeCurrent();
}

//-----------------------------------------------------------------------------

int RenderWindowInteractorManager::getWidgetId()
{
    return m_qOgreWidget->getId();
}

//-----------------------------------------------------------------------------

::Ogre::RenderWindow* RenderWindowInteractorManager::getRenderWindow()
{
    return m_qOgreWidget->getOgreRenderWindow();
}

//-----------------------------------------------------------------------------

void RenderWindowInteractorManager::onRenderWindowCreated()
{
    ::fwServices::IService::sptr renderService      = m_renderService.lock();
    ::fwRenderOgre::SRender::sptr ogreRenderService = ::fwRenderOgre::SRender::dynamicCast( renderService );

    ogreRenderService->slot(::fwRenderOgre::SRender::s_START_OBJECT_SLOT)->asyncRun();
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

void RenderWindowInteractorManager::onRayCastRequested(int _x, int _y, int _w, int _h)
{
    ::fwServices::IService::sptr renderService      = m_renderService.lock();
    ::fwRenderOgre::SRender::sptr ogreRenderService = ::fwRenderOgre::SRender::dynamicCast( renderService );

    ogreRenderService->slot< ::fwRenderOgre::SRender::DoRayCastSlotType>(::fwRenderOgre::SRender::s_DO_RAY_CAST_SLOT)->
    asyncRun(_x, _y, _w, _h);
}

//-----------------------------------------------------------------------------

} // namespace visuOgreQt




