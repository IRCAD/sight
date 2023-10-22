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

#pragma once

#include "modules/viz/scene3d_qt/config.hpp"
#include "modules/viz/scene3d_qt/Window.hpp"

#include <core/base.hpp>

#include <ui/__/container/widget.hpp>

#include <viz/scene3d/window_interactor.hpp>

#include <QObject>
#include <QPointer>

namespace sight::ui::qt::container
{

class widget;

} // namespace sight::ui::qt::container

namespace sight::module::viz::scene3d_qt
{

/// Defines a class to manage a render window.
class MODULE_VIZ_SCENE3D_QT_CLASS_API window_interactor final : public QObject,
                                                                public sight::viz::scene3d::window_interactor
{
Q_OBJECT

public:

    SIGHT_DECLARE_CLASS(window_interactor, sight::viz::scene3d::window_interactor);

    /// Destroys the window container.
    MODULE_VIZ_SCENE3D_QT_API ~window_interactor() final;

    /// Renders immediately the frame.
    MODULE_VIZ_SCENE3D_QT_API void renderNow() final;

    /// Renders the frame as soon as possible.
    MODULE_VIZ_SCENE3D_QT_API void requestRender() final;

    /**
     * @brief Creates the container that holds the QtWidget.
     * @param _parent the parent container of the widget.
     * @param _renderOnDemand if true, the rendering will be done only when it's requested.
     * @param _fullscreen enable the fullscreen.
     */
    MODULE_VIZ_SCENE3D_QT_API void createContainer(
        ui::container::widget::sptr _parent,
        bool _fullscreen,
        const std::string& _service_id
    ) final;

    /// Connects the widget and the render signals and slots.
    MODULE_VIZ_SCENE3D_QT_API void connectToContainer() final;

    /// Disconnects the widget and the render signals and slots.
    MODULE_VIZ_SCENE3D_QT_API void disconnectInteractor() final;

    /// Returns the unique identifier of the widget.
    MODULE_VIZ_SCENE3D_QT_API int getWidgetId() const final;

    /// Returns current frame number of the render window.
    MODULE_VIZ_SCENE3D_QT_API int getFrameId() const final;

    /// Makes the OpenGL context as current one on this thread against this window.
    MODULE_VIZ_SCENE3D_QT_API void makeCurrent() final;

    /// Returns a nullptr. This is due to the fact that this manager doesn't write to a texture.
    MODULE_VIZ_SCENE3D_QT_API Ogre::TexturePtr getRenderTexture() final;

    /// Creates a worker able to handle resources from the window's OpenGL context.
    MODULE_VIZ_SCENE3D_QT_API sight::viz::scene3d::graphics_worker* createGraphicsWorker() final;

    /// Gets the vertical logical DPI of the monitor on which the window is displayed.
    /// The logical DPI takes accessibility features and desktop zoom into account and is used for font rendering.
    MODULE_VIZ_SCENE3D_QT_API float getLogicalDotsPerInch() const final;

    /**
     * @brief Sets the fullscreen or windowed rendering mode.
     * @param _fullscreen whether to render in fullscreen mode. Use windowed mode otherwise.
     * @param _screenNumber index of the screen on which to render in fullscreen mode.
     */
    MODULE_VIZ_SCENE3D_QT_API void setFullscreen(bool _fullscreen, int _screen_number) final;

    QWidget* getQtWidget() const;

private Q_SLOTS:

    /// Called when the user interacts with the scene using the mouse and keyboard, connected to @ref m_qOgreWidget.
    void onInteracted(sight::viz::scene3d::window_interactor::InteractionInfo _info);

private:

    bool m_isFullScreen {false};
    /// Contains Qt element of the Widget.
    QPointer<module::viz::scene3d_qt::Window> m_qOgreWidget;

    /// Contains the parent of the widget.
    SPTR(ui::qt::container::widget) m_parentContainer;
};

//-----------------------------------------------------------------------------

inline Ogre::TexturePtr window_interactor::getRenderTexture()
{
    SIGHT_ERROR("'window_interactor' doesn't render in a texture.");
    return {};
}

//-----------------------------------------------------------------------------

inline QWidget* window_interactor::getQtWidget() const
{
    return m_qOgreWidget;
}

} // namespace sight::module::viz::scene3d_qt.
