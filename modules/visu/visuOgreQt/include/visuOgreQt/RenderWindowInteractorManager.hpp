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

#include "visuOgreQt/config.hpp"
#include "visuOgreQt/Window.hpp"

#include <core/base.hpp>

#include <fwGui/container/fwContainer.hpp>

#include <fwRenderOgre/IRenderWindowInteractorManager.hpp>

#include <QObject>
#include <QPointer>

namespace fwGuiQt
{
namespace container
{
class QtContainer;
}
}

namespace visuOgreQt
{

/// Defines a class to manage a render window.
class VISUOGREQT_CLASS_API RenderWindowInteractorManager : public QObject,
                                                           public ::fwRenderOgre::IRenderWindowInteractorManager
{

Q_OBJECT

public:

    fwCoreClassMacro(RenderWindowInteractorManager, ::fwRenderOgre::IRenderWindowInteractorManager)

    /// Initialize members.
    VISUOGREQT_API RenderWindowInteractorManager(::fwRenderOgre::IRenderWindowInteractorManager::Key key);

    /// Destroys the window container.
    VISUOGREQT_API virtual ~RenderWindowInteractorManager() final;

    /// Renders immediately the frame.
    VISUOGREQT_API virtual void renderNow() final;

    /// Renders the frame as soon as possible.
    VISUOGREQT_API virtual void requestRender() final;

    /**
     * @brief Creates the container that holds the QtWidget.
     * @param _parent the parent container of the widget.
     * @param _renderOnDemand if true, the rendering will be done only when it's requested.
     * @param _fullscreen enable the fullscreen.
     */
    VISUOGREQT_API virtual void createContainer(::fwGui::container::fwContainer::sptr _parent,
                                                bool _renderOnDemand,
                                                bool _fullscreen) final;

    /// Connects the widget and the SRender signals and slots.
    VISUOGREQT_API virtual void connectToContainer() final;

    /// Disconnects the widget and the SRender signals and slots.
    VISUOGREQT_API virtual void disconnectInteractor() final;

    /// Returns the unique identifier of the widget.
    VISUOGREQT_API virtual int getWidgetId() const final;

    /// Returns current frame number of the render window.
    VISUOGREQT_API virtual int getFrameId() const final;

    /// Makes the OpenGL context as current one on this thread against this window.
    VISUOGREQT_API virtual void makeCurrent() final;

    /// Gets the Ogre render target.
    VISUOGREQT_API virtual ::Ogre::RenderTarget* getRenderTarget() final;

    /// Returns a nullptr. This is due to the fact that this manager doesn't write to a texture.
    VISUOGREQT_API virtual ::Ogre::TexturePtr getRenderTexture() final;

    /// Creates a worker able to handle resources from the window's OpenGL context.
    VISUOGREQT_API ::fwRenderOgre::IGraphicsWorker* createGraphicsWorker() final;

    /// Gets the vertical logical DPI of the monitor on which the window is displayed.
    /// The logical DPI takes accessibility features and desktop zoom into account and is used for font rendering.
    VISUOGREQT_API virtual float getLogicalDotsPerInch() const final;

    /**
     * @brief Sets the fullscreen or windowed rendering mode.
     * @param _fullscreen whether to render in fullscreen mode. Use windowed mode otherwise.
     * @param _screenNumber index of the screen on which to render in fullscreen mode.
     */
    VISUOGREQT_API virtual void setFullscreen(bool _fullscreen, int _screenNumber) final;

private Q_SLOTS:

    /// Called when the user interacts with the scene using the mouse and keyboard, connected to @ref m_qOgreWidget.
    void onInteracted(::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo _info);

    /// Called when the clipping range has to match the last updating of the scene bounding box
    void onCameraClippingComputation();

private:

    /// Disables fullscreen rendering.
    void disableFullscreen();

    /// Contains Qt element of the Widget.
    QPointer< ::visuOgreQt::Window > m_qOgreWidget;

    /// Contains the parent of the widget.
    SPTR(::fwGuiQt::container::QtContainer) m_parentContainer;

    /// Contains the window container.
    QWidget* m_windowContainer {nullptr};

};

//-----------------------------------------------------------------------------

inline ::Ogre::TexturePtr RenderWindowInteractorManager::getRenderTexture()
{
    SLM_ERROR("'RenderWindowInteractorManager' doesn't render in a texture.");
    return ::Ogre::TexturePtr();
}

} // namespace visuOgreQt.
