/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

#include "modules/viz/scene3dQt/WindowInteractor.hpp"

#include "modules/viz/scene3dQt/OpenGLContext.hpp"
#include "modules/viz/scene3dQt/OpenGLWorker.hpp"

#include <core/com/Slots.hxx>

#include <ui/qt/container/QtContainer.hpp>

#include <viz/scene3d/registry/macros.hpp>
#include <viz/scene3d/SRender.hpp>

#include <QDesktopWidget>
#include <QEvent>
#include <QGestureEvent>
#include <QGuiApplication>
#include <QRect>
#include <QShortcut>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#include <utility>

//-----------------------------------------------------------------------------

SIGHT_REGISTER_SCENE3D(
    sight::module::viz::scene3dQt::WindowInteractor,
    sight::viz::scene3d::IWindowInteractor::REGISTRY_KEY
);

namespace sight::module::viz::scene3dQt
{

//-----------------------------------------------------------------------------

WindowInteractor::WindowInteractor(
    sight::viz::scene3d::IWindowInteractor::Key
    /*unused*/
)
{
}

//-----------------------------------------------------------------------------

WindowInteractor::~WindowInteractor()
{
    // Delete the window container if it is not attached to the parent container.
    // i.e. it is shown in fullscreen.
    if((m_qOgreWidget != nullptr) && m_qOgreWidget->parent() == nullptr)
    {
        delete m_qOgreWidget;
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

void WindowInteractor::createContainer(
    sight::ui::base::container::fwContainer::sptr _parent,
    bool _fullscreen,
    const std::string& id
)
{
    SIGHT_ASSERT("Invalid parent.", _parent);
    m_parentContainer = ui::qt::container::QtContainer::dynamicCast(_parent);

    auto* layout = new QVBoxLayout();
    m_parentContainer->setLayout(layout);
    layout->setContentsMargins(0, 0, 0, 0);

    m_qOgreWidget = new module::viz::scene3dQt::Window();

    m_qOgreWidget->setObjectName(QString::fromStdString(id));

    layout->addWidget(m_qOgreWidget);
    m_qOgreWidget->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    m_qOgreWidget->setMouseTracking(true);

    m_qOgreWidget->grabGesture(Qt::PinchGesture); // For zooming
    m_qOgreWidget->grabGesture(Qt::PanGesture);   // For translating

    this->setFullscreen(_fullscreen, -1);

    auto toggleFullscreen = [this]
                            {
                                this->setFullscreen(!m_isFullScreen, -1);

                                service::IService::sptr renderService                = m_renderService.lock();
                                sight::viz::scene3d::SRender::sptr ogreRenderService =
                                    sight::viz::scene3d::SRender::dynamicCast(renderService);
                                if(m_isFullScreen)
                                {
                                    auto enableFullScreenSlot = ogreRenderService->slot(
                                        sight::viz::scene3d::SRender::s_ENABLE_FULLSCREEN
                                    );
                                    enableFullScreenSlot->run(0);
                                }
                                else
                                {
                                    auto disableFullScreenSlot = ogreRenderService->slot(
                                        sight::viz::scene3d::SRender::s_DISABLE_FULLSCREEN
                                    );
                                    disableFullScreenSlot->run();
                                }
                            };

    auto* toggleFullscreenShortcut = new QShortcut(QString("F11"), m_qOgreWidget);
    QObject::connect(toggleFullscreenShortcut, &QShortcut::activated, toggleFullscreen);

    const auto renderService = sight::viz::scene3d::SRender::dynamicCast(m_renderService.lock());
    SIGHT_ASSERT("RenderService wrongly instantiated. ", renderService);

    std::map<int, sight::viz::scene3d::Layer::wptr> orderedLayers;
    for(auto& layer : renderService->getLayers())
    {
        orderedLayers[layer.second->getOrder()] = layer.second;
    }

    for(auto& layer : orderedLayers)
    {
        m_qOgreWidget->registerLayer(layer.second);
    }

    QShowEvent showEvent;
    QCoreApplication::sendEvent(m_qOgreWidget, &showEvent);
}

//-----------------------------------------------------------------------------

void WindowInteractor::connectToContainer()
{
    // Connect widget window render to render service start adaptors
    const auto renderService = sight::viz::scene3d::SRender::dynamicCast(m_renderService.lock());
    SIGHT_ASSERT("RenderService wrongly instantiated. ", renderService);

    QObject::connect(m_qOgreWidget, SIGNAL(cameraClippingComputation()), this, SLOT(onCameraClippingComputation()));
    QObject::connect(
        m_qOgreWidget,
        SIGNAL(
            interacted(
                sight::viz::scene3d::IWindowInteractor::InteractionInfo
            )
        ),
        this,
        SLOT(onInteracted(sight::viz::scene3d::IWindowInteractor::InteractionInfo))
    );
}

//-----------------------------------------------------------------------------

void WindowInteractor::disconnectInteractor()
{
    QObject::disconnect(m_qOgreWidget, SIGNAL(cameraClippingComputation()), this, SLOT(onCameraClippingComputation()));
    QObject::disconnect(
        m_qOgreWidget,
        SIGNAL(
            interacted(
                sight::viz::scene3d::IWindowInteractor::InteractionInfo
            )
        ),
        this,
        SLOT(onInteracted(sight::viz::scene3d::IWindowInteractor::InteractionInfo))
    );
    QWidget* const container = m_parentContainer->getQtContainer();
    container->layout()->removeWidget(m_qOgreWidget);
    m_qOgreWidget->destroyWindow();
    delete m_qOgreWidget;
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

float WindowInteractor::getLogicalDotsPerInch() const
{
    SIGHT_ASSERT("Trying to query dots per inch on a non-existing windows.", m_qOgreWidget);
    return static_cast<float>(m_qOgreWidget->screen()->logicalDotsPerInchY());
}

//-----------------------------------------------------------------------------

void WindowInteractor::onInteracted(sight::viz::scene3d::IWindowInteractor::InteractionInfo _info)
{
    service::IService::sptr renderService                = m_renderService.lock();
    sight::viz::scene3d::SRender::sptr ogreRenderService = sight::viz::scene3d::SRender::dynamicCast(renderService);

    for(const auto& layerMap : ogreRenderService->getLayers())
    {
        sight::viz::scene3d::Layer::sptr layer = layerMap.second;
        layer->slot<sight::viz::scene3d::Layer::InteractionSlotType>(sight::viz::scene3d::Layer::s_INTERACTION_SLOT)->
        asyncRun(
            _info
        );
    }
}

//-----------------------------------------------------------------------------

void WindowInteractor::onCameraClippingComputation()
{
    service::IService::sptr renderService                = m_renderService.lock();
    sight::viz::scene3d::SRender::sptr ogreRenderService = sight::viz::scene3d::SRender::dynamicCast(renderService);

    ogreRenderService->slot(sight::viz::scene3d::SRender::s_COMPUTE_CAMERA_CLIPPING_SLOT)->asyncRun();
}

//-----------------------------------------------------------------------------

sight::viz::scene3d::IGraphicsWorker* WindowInteractor::createGraphicsWorker()
{
    return new OpenGLWorker(m_qOgreWidget);
}

//-----------------------------------------------------------------------------

void WindowInteractor::setFullscreen(bool _fullscreen, int _screenNumber)
{
    QWidget* const container = m_parentContainer->getQtContainer();
    m_isFullScreen = _fullscreen;
    if(_fullscreen)
    {
        container->layout()->removeWidget(m_qOgreWidget);

        const QDesktopWidget* desktop = QApplication::desktop();

        QRect screenres;
        if(_screenNumber < 0)
        {
            _screenNumber = desktop->screenNumber(container) + 1;
        }

        if(_screenNumber >= QGuiApplication::screens().count())
        {
            screenres = QGuiApplication::primaryScreen()->geometry();
        }
        else
        {
            screenres = QGuiApplication::screens()[_screenNumber]->geometry();
        }

        m_qOgreWidget->setParent(nullptr);
        m_qOgreWidget->showFullScreen();
        m_qOgreWidget->setGeometry(screenres);
    }
    else if(container->layout()->isEmpty())
    {
        container->layout()->addWidget(m_qOgreWidget);
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3dQt
