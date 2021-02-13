/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "modules/viz/ogreQt/WindowInteractor.hpp"

#include "modules/viz/ogreQt/OpenGLContext.hpp"
#include "modules/viz/ogreQt/OpenGLWorker.hpp"

#include <core/com/Slots.hxx>

#include <viz/ogre/registry/macros.hpp>
#include <viz/ogre/SRender.hpp>

#include <QDesktopWidget>
#include <QEvent>
#include <QRect>
#include <QShortcut>
#include <QVBoxLayout>
#include <QWidget>

#include <ui/qt/container/QtContainer.hpp>

//-----------------------------------------------------------------------------

fwRenderOgreRegisterMacro( sight::modules::viz::ogreQt::WindowInteractor,
                           sight::viz::ogre::IWindowInteractor::REGISTRY_KEY );

//-----------------------------------------------------------------------------

namespace sight::modules::viz::ogreQt
{

//-----------------------------------------------------------------------------

WindowInteractor::WindowInteractor(
    sight::viz::ogre::IWindowInteractor::Key)
{
}

//-----------------------------------------------------------------------------

WindowInteractor::~WindowInteractor()
{
    // Delete the window container if it is not attached to the parent container.
    // i.e. it is shown in fullscreen.
    if(m_windowContainer && m_windowContainer->parent() == nullptr)
    {
        delete m_windowContainer;
    }
}

//-----------------------------------------------------------------------------

void WindowInteractor::renderNow()
{
    m_qOgreWidget->renderNow();
}

//-----------------------------------------------------------------------------

void WindowInteractor::requestRender()
{
    m_qOgreWidget->requestRender();
}

//-----------------------------------------------------------------------------

void WindowInteractor::createContainer( sight::ui::base::container::fwContainer::sptr _parent,
                                        bool _renderOnDemand,
                                        bool _fullscreen)
{
    SLM_ASSERT("Invalid parent.", _parent );
    m_parentContainer = ui::qt::container::QtContainer::dynamicCast( _parent );

    QVBoxLayout* layout = new QVBoxLayout();
    m_parentContainer->setLayout(layout);
    layout->setContentsMargins(0, 0, 0, 0);

    m_qOgreWidget = new modules::viz::ogreQt::Window();
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
    m_windowContainer->setMouseTracking(true);

    this->setFullscreen(_fullscreen, -1);

    auto disableFullscreen = [this] { this->disableFullscreen(); };
    auto* disableShortcut  = new QShortcut(QString("Escape"), m_windowContainer);
    QObject::connect(disableShortcut, &QShortcut::activated, disableFullscreen);

    m_qOgreWidget->initialize();
}

//-----------------------------------------------------------------------------

void WindowInteractor::connectToContainer()
{
    // Connect widget window render to render service start adaptors
    service::IService::sptr renderService             = m_renderService.lock();
    sight::viz::ogre::SRender::sptr ogreRenderService = sight::viz::ogre::SRender::dynamicCast( renderService );

    if( !ogreRenderService )
    {
        SLM_ERROR("RenderService wrongly instantiated. ");
    }

    QObject::connect(m_qOgreWidget, SIGNAL(cameraClippingComputation()), this, SLOT(onCameraClippingComputation()));
    QObject::connect(m_qOgreWidget, SIGNAL(interacted(
                                               sight::viz::ogre::IWindowInteractor::InteractionInfo)), this,
                     SLOT(onInteracted(sight::viz::ogre::IWindowInteractor::InteractionInfo)));
}

//-----------------------------------------------------------------------------

void WindowInteractor::disconnectInteractor()
{
    QObject::disconnect(m_qOgreWidget, SIGNAL(cameraClippingComputation()), this, SLOT(onCameraClippingComputation()));
    QObject::disconnect(m_qOgreWidget, SIGNAL(interacted(
                                                  sight::viz::ogre::IWindowInteractor::InteractionInfo)), this,
                        SLOT(onInteracted(sight::viz::ogre::IWindowInteractor::InteractionInfo)));

    m_qOgreWidget->destroyWindow();
    m_qOgreWidget = nullptr;
}

//-----------------------------------------------------------------------------

void WindowInteractor::makeCurrent()
{
    m_qOgreWidget->makeCurrent();
}

//-----------------------------------------------------------------------------

int WindowInteractor::getWidgetId() const
{
    return m_qOgreWidget->getId();
}

//-----------------------------------------------------------------------------

int WindowInteractor::getFrameId() const
{
    return m_qOgreWidget->getFrameId();
}

//-----------------------------------------------------------------------------

::Ogre::RenderTarget* WindowInteractor::getRenderTarget()
{
    return m_qOgreWidget->getOgreRenderWindow();
}

//-----------------------------------------------------------------------------

float WindowInteractor::getLogicalDotsPerInch() const
{
    SLM_ASSERT("Trying to query dots per inch on a non-existing windows.", m_qOgreWidget);
    return static_cast<float>(m_qOgreWidget->screen()->logicalDotsPerInchY());
}

//-----------------------------------------------------------------------------

void WindowInteractor::onInteracted(sight::viz::ogre::IWindowInteractor::InteractionInfo _info)
{
    service::IService::sptr renderService             = m_renderService.lock();
    sight::viz::ogre::SRender::sptr ogreRenderService = sight::viz::ogre::SRender::dynamicCast( renderService );

    for(auto layerMap : ogreRenderService->getLayers())
    {
        sight::viz::ogre::Layer::sptr layer = layerMap.second;
        layer->slot< sight::viz::ogre::Layer::InteractionSlotType>(sight::viz::ogre::Layer::s_INTERACTION_SLOT)->
        asyncRun(
            _info);
    }
}

//-----------------------------------------------------------------------------

void WindowInteractor::onCameraClippingComputation()
{
    service::IService::sptr renderService             = m_renderService.lock();
    sight::viz::ogre::SRender::sptr ogreRenderService = sight::viz::ogre::SRender::dynamicCast( renderService );

    ogreRenderService->slot(sight::viz::ogre::SRender::s_COMPUTE_CAMERA_CLIPPING_SLOT)->asyncRun();
}

//-----------------------------------------------------------------------------

sight::viz::ogre::IGraphicsWorker* WindowInteractor::createGraphicsWorker()
{
    return new OpenGLWorker(m_qOgreWidget);
}

//-----------------------------------------------------------------------------

void WindowInteractor::setFullscreen(bool _fullscreen, int _screenNumber)
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
        m_windowContainer->showFullScreen();
        m_windowContainer->setGeometry(screenres);
    }
    else if(container->layout()->isEmpty())
    {
        container->layout()->addWidget(m_windowContainer);
    }
}

//-----------------------------------------------------------------------------

void WindowInteractor::disableFullscreen()
{
    QWidget* const container = m_parentContainer->getQtContainer();

    if(container->layout()->isEmpty())
    {
        service::IService::sptr renderService             = m_renderService.lock();
        sight::viz::ogre::SRender::sptr ogreRenderService = sight::viz::ogre::SRender::dynamicCast( renderService );

        auto toggleSlot = ogreRenderService->slot(sight::viz::ogre::SRender::s_DISABLE_FULLSCREEN);
        toggleSlot->run();
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::modules::viz::ogreQt
