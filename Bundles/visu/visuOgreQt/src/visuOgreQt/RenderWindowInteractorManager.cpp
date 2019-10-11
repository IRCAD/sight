/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "visuOgreQt/RenderWindowInteractorManager.hpp"

#include "visuOgreQt/OpenGLContext.hpp"
#include "visuOgreQt/OpenGLWorker.hpp"

#include <fwCom/Slots.hxx>

#include <fwData/String.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwRenderOgre/registry/macros.hpp>
#include <fwRenderOgre/SRender.hpp>

#include <QDesktopWidget>
#include <QEvent>
#include <QRect>
#include <QShortcut>
#include <QThread>
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

void RenderWindowInteractorManager::renderNow()
{
    m_qOgreWidget->renderNow();
}

//-----------------------------------------------------------------------------

void RenderWindowInteractorManager::requestRender()
{
    m_qOgreWidget->requestRender();
}

//-----------------------------------------------------------------------------

void RenderWindowInteractorManager::createContainer( ::fwGui::container::fwContainer::sptr _parent,
                                                     bool _renderOnDemand, bool _fullscreen, bool _vsync)
{
    SLM_ASSERT("Invalid parent.", _parent );
    m_parentContainer = ::fwGuiQt::container::QtContainer::dynamicCast( _parent );

    QVBoxLayout* layout = new QVBoxLayout();
    m_parentContainer->setLayout(layout);
    layout->setContentsMargins(0, 0, 0, 0);

    m_qOgreWidget = new ::visuOgreQt::Window();
    m_qOgreWidget->setAnimating(!_renderOnDemand);
#ifdef __linux
    // When using qt on linux we need to allocate the window resources before being able to use openGL.
    // This is because the underlying X API requires a drawable surface to draw on when calling
    // 'glXMakeCurrent' (see https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glXMakeCurrent.xml)
    // which is does not exist before 'QWindow::create' is called.
    //
    // This is not required for Windows and macOS and will actually break the app on these platforms.
    // (see https://developer.apple.com/documentation/appkit/nsopenglcontext/1436212-makecurrentcontext
    //  and https://docs.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-wglmakecurrent).
    m_qOgreWidget->create();
#endif

    m_windowContainer = QWidget::createWindowContainer(m_qOgreWidget);
    layout->addWidget(m_windowContainer);
    m_windowContainer->setSizePolicy(QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding));

    this->setFullscreen(_fullscreen, -1);

    auto disableFullscreen = [this] { this->disableFullscreen(); };
    auto* disableShortcut  = new QShortcut(QString("Escape"), m_windowContainer);
    QObject::connect(disableShortcut, &QShortcut::activated, disableFullscreen);

    m_qOgreWidget->initialize(_vsync);
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

float RenderWindowInteractorManager::getLogicalDotsPerInch() const
{
    SLM_ASSERT("Trying to query dots per inch on a non-existing windows.", m_qOgreWidget);
    return static_cast<float>(m_qOgreWidget->screen()->logicalDotsPerInchY());
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

::fwRenderOgre::IGraphicsWorker* RenderWindowInteractorManager::createGraphicsWorker()
{
    return new OpenGLWorker(m_qOgreWidget);
}

//-----------------------------------------------------------------------------

void RenderWindowInteractorManager::setFullscreen(bool _fullscreen, int _screenNumber)
{
    QWidget* const container = m_parentContainer->getQtContainer();

    if(_fullscreen)
    {
        container->layout()->removeWidget(m_windowContainer);

        QDesktopWidget* desktop = QApplication::desktop();

        if(_screenNumber < 0)
        {
            _screenNumber = desktop->screenNumber(container) + 1;
        }

        if(_screenNumber >= desktop->screenCount())
        {
            _screenNumber = desktop->primaryScreen();
        }

        QRect screenres = desktop->screenGeometry(_screenNumber);

        m_windowContainer->setParent(nullptr);
        m_windowContainer->setGeometry(screenres);
        m_windowContainer->showFullScreen();
    }
    else if(container->layout()->isEmpty())
    {
        container->layout()->addWidget(m_windowContainer);
    }
}

//-----------------------------------------------------------------------------

void RenderWindowInteractorManager::disableFullscreen()
{
    QWidget* const container = m_parentContainer->getQtContainer();

    if(container->layout()->isEmpty())
    {
        ::fwServices::IService::sptr renderService      = m_renderService.lock();
        ::fwRenderOgre::SRender::sptr ogreRenderService = ::fwRenderOgre::SRender::dynamicCast( renderService );

        auto toggleSlot = ogreRenderService->slot(::fwRenderOgre::SRender::s_DISABLE_FULLSCREEN);
        toggleSlot->asyncRun();
    }
}

//-----------------------------------------------------------------------------

} // namespace visuOgreQt
