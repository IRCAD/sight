/************************************************************************
 *
 * Copyright (C) 2014-2022 IRCAD France
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
#include <ui/qt/gestures/QPanGestureRecognizer.hpp>

#include <viz/scene3d/registry/macros.hpp>
#include <viz/scene3d/SRender.hpp>

#include <QDesktopWidget>
#include <QEvent>
#include <QGestureEvent>
#include <QGuiApplication>
#include <QRect>
#include <QShortcut>
#include <QVBoxLayout>
#include <QWidget>

#include <utility>

//-----------------------------------------------------------------------------

SIGHT_REGISTER_SCENE3D(
    sight::module::viz::scene3dQt::WindowInteractor,
    sight::viz::scene3d::IWindowInteractor::REGISTRY_KEY
);

//-----------------------------------------------------------------------------

EventDispatcher::EventDispatcher(QObject* dispatchedTo, const QList<QEvent::Type>& eventsToDispatch) :
    m_dispatchedTo(dispatchedTo),
    m_eventsToDispatch(eventsToDispatch)
{
}

//------------------------------------------------------------------------------

bool EventDispatcher::eventFilter(QObject* /*watched*/, QEvent* event)
{
    if(m_eventsToDispatch.contains(event->type()))
    {
        QCoreApplication::sendEvent(m_dispatchedTo, event);
        return true;
    }

    return false;
}

GestureFilter::GestureFilter(QPointer<sight::module::viz::scene3dQt::Window> target) :
    m_target(std::move(target))
{
}

//------------------------------------------------------------------------------

bool GestureFilter::eventFilter(QObject* /*watched*/, QEvent* event)
{
    if(event->type() == QEvent::Gesture)
    {
        auto* ge = static_cast<QGestureEvent*>(event);
        m_target->gestureEvent(ge);
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------

bool TouchToMouseFixFilter::eventFilter(QObject* watched, QEvent* event)
{
    if(event->type() == QEvent::MouseButtonPress)
    {
        auto* me = static_cast<QMouseEvent*>(event);
        if(me->source() == Qt::MouseEventSynthesizedByQt)
        {
            QMouseEvent newEvent(QEvent::MouseButtonPress, me->localPos() - QPointF(49, 5), me->button(), me->buttons(),
                                 me->modifiers());
            QCoreApplication::sendEvent(watched, &newEvent);
            return true;
        }
    }

    return false;
}

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
    if((m_windowContainer != nullptr) && m_windowContainer->parent() == nullptr)
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
    m_windowContainer->setObjectName(QString::fromStdString(id));
    m_windowContainer->installEventFilter(
        new EventDispatcher(
            m_qOgreWidget,
            {QEvent::MouseButtonPress, QEvent::MouseButtonRelease, QEvent::Enter, QEvent::MouseMove, QEvent::Leave,
             QEvent::Wheel
            })
    );
    layout->addWidget(m_windowContainer);
    m_windowContainer->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    m_windowContainer->setMouseTracking(true);

    m_windowContainer->grabGesture(Qt::PinchGesture);                                         // For zooming
    m_windowContainer->grabGesture(sight::ui::qt::gestures::QPanGestureRecognizer::get<1>()); // For rotating
    m_windowContainer->grabGesture(Qt::TapAndHoldGesture);                                    // For placing a landmark
    m_windowContainer->grabGesture(sight::ui::qt::gestures::QPanGestureRecognizer::get<2>()); // For translating
    m_windowContainer->installEventFilter(new GestureFilter(m_qOgreWidget));                  // Sends the gesture
                                                                                              // events
                                                                                              // to window
    m_qOgreWidget->installEventFilter(
        new EventDispatcher(
            m_windowContainer,
            {QEvent::TouchBegin, QEvent::TouchCancel, QEvent::TouchEnd, QEvent::TouchUpdate
            })
    );
    m_qOgreWidget->installEventFilter(new TouchToMouseFixFilter);

    this->setFullscreen(_fullscreen, -1);

    auto disableFullscreen = [this]{this->disableFullscreen();};
    auto* disableShortcut  = new QShortcut(QString("Escape"), m_windowContainer);
    QObject::connect(disableShortcut, &QShortcut::activated, disableFullscreen);

    m_qOgreWidget->initialize();
}

//-----------------------------------------------------------------------------

void WindowInteractor::connectToContainer()
{
    // Connect widget window render to render service start adaptors
    service::IService::sptr renderService                = m_renderService.lock();
    sight::viz::scene3d::SRender::sptr ogreRenderService = sight::viz::scene3d::SRender::dynamicCast(renderService);

    if(!ogreRenderService)
    {
        SIGHT_ERROR("RenderService wrongly instantiated. ");
    }

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

Ogre::RenderTarget* WindowInteractor::getRenderTarget()
{
    return m_qOgreWidget->getOgreRenderWindow();
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

    if(_fullscreen)
    {
        container->layout()->removeWidget(m_windowContainer);

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
        service::IService::sptr renderService                = m_renderService.lock();
        sight::viz::scene3d::SRender::sptr ogreRenderService =
            sight::viz::scene3d::SRender::dynamicCast(renderService);

        auto toggleSlot = ogreRenderService->slot(sight::viz::scene3d::SRender::s_DISABLE_FULLSCREEN);
        toggleSlot->run();
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3dQt
