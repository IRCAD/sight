/************************************************************************
 *
 * Copyright (C) 2018-2020 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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
#include "visuOgreQt/OpenGLContext.hpp"

#include <fwRenderOgre/IRenderWindowInteractorManager.hpp>

#include <QOffscreenSurface>

#include <memory>

namespace visuOgreQt
{

/// Defines a class to manage an offscreen render window that renders in an Ogre texture.
class VISUOGREQT_CLASS_API OffScreenRenderWindowInteractorManager final :
    public ::fwRenderOgre::IRenderWindowInteractorManager
{

public:

    fwCoreClassMacro(OffScreenRenderWindowInteractorManager, ::fwRenderOgre::IRenderWindowInteractorManager)

    /**
     * @brief Initialize members.
     * @param _width width of the render window.
     * @param _height height of the render window.
     */
    VISUOGREQT_API OffScreenRenderWindowInteractorManager(
        ::fwRenderOgre::IRenderWindowInteractorManager::OffscreenMgrKey,
        unsigned int _width,
        unsigned int _height);

    /// Destroys the render window.
    VISUOGREQT_API virtual ~OffScreenRenderWindowInteractorManager() final;

    /// Renders immediately the frame in the render window.
    VISUOGREQT_API virtual void renderNow() override;

    /// Renders immediately the frame in the render window.
    VISUOGREQT_API virtual void requestRender() override;

    /// Creates the render window in the global OpenGL context, @see ::visuOgreQt::OpenGLContext.
    VISUOGREQT_API virtual void createContainer(::fwGui::container::fwContainer::sptr, bool, bool) override;

    /// Does nothing.
    VISUOGREQT_API virtual void connectToContainer() override;

    /// Deletes the render window and the render texture.
    VISUOGREQT_API virtual void disconnectInteractor() override;

    /// Returns the unique identifier of the widget.
    VISUOGREQT_API virtual int getWidgetId() const override;

    /// Returns current frame number of the render window.
    VISUOGREQT_API virtual int getFrameId() const override;

    /// Makes the OpenGL context as current one on this thread against this window.
    VISUOGREQT_API virtual void makeCurrent() override;

    /// Gets the Ogre render target.
    VISUOGREQT_API virtual ::Ogre::RenderTarget* getRenderTarget() override;

    /// Gets the Ogre render texture attached to the render target.
    VISUOGREQT_API virtual ::Ogre::TexturePtr getRenderTexture() override;

    /// Creates a worker able to handle resources from the window's OpenGL context.
    VISUOGREQT_API ::fwRenderOgre::IGraphicsWorker* createGraphicsWorker() override;

    /// Returns a DPI of 220 to permit offscreen font rendering.
    /// @warning Doesn't make much sense when not rendering on a screen.
    VISUOGREQT_API virtual float getLogicalDotsPerInch() const override
    {
        // Return this value which is kind of common for HiDPI monitors.
        return 220;
    }

private:

    /// Updates the Ogre render target.
    void render();

    /// Defines a counter to get the widget ID.
    static int m_counter;

    /// Defines the unique identifier of the widget, used to instantiate the managers related to this instance with a
    // proper name.
    int m_id;

    /// Defines the number of render frame.
    int m_frameId { 0 };

    /// Contains the Ogre root.
    ::Ogre::Root* m_ogreRoot { nullptr };

    /// Contains the Ogre render window.
    ::Ogre::RenderWindow* m_ogreRenderWindow { nullptr };

    /// Contains the Ogre render target.
    ::Ogre::RenderTarget* m_ogreRenderTarget { nullptr };

    /// Contains the Ogre render texture attached to the render target
    ::Ogre::TexturePtr m_ogreTexture;

    /// Contains the OpenGL context used for offscreen rendering.
    std::shared_ptr<QOpenGLContext> m_glContext;

    /// Defines the window width.
    unsigned int m_width { 0 };

    /// Defines the window height.
    unsigned int m_height { 0 };

    /// Contains the offscreen surface, needed to enable the OpenGL context on the current thread.
    std::unique_ptr<QOffscreenSurface> m_offscreenSurface;

};

//-----------------------------------------------------------------------------

inline int OffScreenRenderWindowInteractorManager::getWidgetId() const
{
    return m_id;
}

//-----------------------------------------------------------------------------

inline int OffScreenRenderWindowInteractorManager::getFrameId() const
{
    return m_frameId;
}

//-----------------------------------------------------------------------------

inline ::Ogre::RenderTarget* OffScreenRenderWindowInteractorManager::getRenderTarget()
{
    return m_ogreRenderTarget;
}

//-----------------------------------------------------------------------------

inline ::Ogre::TexturePtr OffScreenRenderWindowInteractorManager::getRenderTexture()
{
    return m_ogreTexture;
}

} // namespace visuOgreQt.
