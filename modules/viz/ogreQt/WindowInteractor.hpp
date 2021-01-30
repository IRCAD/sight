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

#pragma once

#include "modules/viz/ogreQt/config.hpp"
#include "modules/viz/ogreQt/Window.hpp"

#include <core/base.hpp>

#include <QObject>
#include <QPointer>

#include <ui/base/container/fwContainer.hpp>

#include <viz/ogre/IWindowInteractor.hpp>

namespace sight::ui::qt
{
namespace container
{
class QtContainer;
}
}

namespace sight::modules::viz::ogreQt
{

/// Defines a class to manage a render window.
class MODULE_VIZ_OGREQT_CLASS_API WindowInteractor : public QObject,
                                                     public sight::viz::ogre::IWindowInteractor
{

Q_OBJECT

public:

    fwCoreClassMacro(WindowInteractor, sight::viz::ogre::IWindowInteractor)

    /// Initialize members.
    MODULE_VIZ_OGREQT_API WindowInteractor(sight::viz::ogre::IWindowInteractor::Key key);

    /// Destroys the window container.
    MODULE_VIZ_OGREQT_API ~WindowInteractor() final;

    /// Renders immediately the frame.
    MODULE_VIZ_OGREQT_API void renderNow() final;

    /// Renders the frame as soon as possible.
    MODULE_VIZ_OGREQT_API void requestRender() final;

    /**
     * @brief Creates the container that holds the QtWidget.
     * @param _parent the parent container of the widget.
     * @param _renderOnDemand if true, the rendering will be done only when it's requested.
     * @param _fullscreen enable the fullscreen.
     */
    MODULE_VIZ_OGREQT_API void createContainer(ui::base::container::fwContainer::sptr _parent,
                                               bool _renderOnDemand,
                                               bool _fullscreen) final;

    /// Connects the widget and the SRender signals and slots.
    MODULE_VIZ_OGREQT_API void connectToContainer() final;

    /// Disconnects the widget and the SRender signals and slots.
    MODULE_VIZ_OGREQT_API void disconnectInteractor() final;

    /// Returns the unique identifier of the widget.
    MODULE_VIZ_OGREQT_API int getWidgetId() const final;

    /// Returns current frame number of the render window.
    MODULE_VIZ_OGREQT_API int getFrameId() const final;

    /// Makes the OpenGL context as current one on this thread against this window.
    MODULE_VIZ_OGREQT_API void makeCurrent() final;

    /// Gets the Ogre render target.
    MODULE_VIZ_OGREQT_API ::Ogre::RenderTarget* getRenderTarget() final;

    /// Returns a nullptr. This is due to the fact that this manager doesn't write to a texture.
    MODULE_VIZ_OGREQT_API ::Ogre::TexturePtr getRenderTexture() final;

    /// Creates a worker able to handle resources from the window's OpenGL context.
    MODULE_VIZ_OGREQT_API sight::viz::ogre::IGraphicsWorker* createGraphicsWorker() final;

    /// Gets the vertical logical DPI of the monitor on which the window is displayed.
    /// The logical DPI takes accessibility features and desktop zoom into account and is used for font rendering.
    MODULE_VIZ_OGREQT_API float getLogicalDotsPerInch() const final;

    /**
     * @brief Sets the fullscreen or windowed rendering mode.
     * @param _fullscreen whether to render in fullscreen mode. Use windowed mode otherwise.
     * @param _screenNumber index of the screen on which to render in fullscreen mode.
     */
    MODULE_VIZ_OGREQT_API void setFullscreen(bool _fullscreen, int _screenNumber) final;

private Q_SLOTS:

    /// Called when the user interacts with the scene using the mouse and keyboard, connected to @ref m_qOgreWidget.
    void onInteracted(sight::viz::ogre::IWindowInteractor::InteractionInfo _info);

    /// Called when the clipping range has to match the last updating of the scene bounding box
    void onCameraClippingComputation();

private:

    /// Disables fullscreen rendering.
    void disableFullscreen();

    /// Contains Qt element of the Widget.
    QPointer< modules::viz::ogreQt::Window > m_qOgreWidget;

    /// Contains the parent of the widget.
    SPTR(ui::qt::container::QtContainer) m_parentContainer;

    /// Contains the window container.
    QWidget* m_windowContainer {nullptr};

};

//-----------------------------------------------------------------------------

inline ::Ogre::TexturePtr WindowInteractor::getRenderTexture()
{
    SLM_ERROR("'WindowInteractor' doesn't render in a texture.");
    return ::Ogre::TexturePtr();
}

} // namespace sight::modules::viz::ogreQt.
