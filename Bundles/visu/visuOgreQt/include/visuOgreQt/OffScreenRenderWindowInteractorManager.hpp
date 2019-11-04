/************************************************************************
 *
 * Copyright (C) 2018-2019 IRCAD France
 * Copyright (C) 2018-2019 IHU Strasbourg
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

/**
 * @brief   Defines a class to manage ogreRenderWindowInteractor in a window.
 */
class VISUOGREQT_CLASS_API OffScreenRenderWindowInteractorManager :
    public ::fwRenderOgre::IRenderWindowInteractorManager
{
public:
    fwCoreClassMacro(OffScreenRenderWindowInteractorManager, ::fwRenderOgre::IRenderWindowInteractorManager)

    /// Constructor
    VISUOGREQT_API OffScreenRenderWindowInteractorManager(
        ::fwRenderOgre::IRenderWindowInteractorManager::OffscreenMgrKey,
        unsigned int _width, unsigned int _height);
    /// Destructor
    VISUOGREQT_API virtual ~OffScreenRenderWindowInteractorManager() final;

    /// Call Widget render immediately
    VISUOGREQT_API virtual void renderNow() final;

    /// Call Widget render
    VISUOGREQT_API virtual void requestRender() final;

    /// Create the container that holds the QtWidget.
    VISUOGREQT_API virtual void createContainer(::fwGui::container::fwContainer::sptr _parent,
                                                bool renderOnDemand, bool fullscreen) final;

    /// Connects widget and SRender signals and slots.
    VISUOGREQT_API virtual void connectToContainer() final;

    /// Not implemented yet
    /// Deletes interactor and manages correctly the window (removing layout).
    VISUOGREQT_API virtual void disconnectInteractor() final;

    /// Returns Ogre widget
    VISUOGREQT_API virtual int getWidgetId() const final;

    /// Returns frame ID
    VISUOGREQT_API virtual int getFrameId() const final;

    /// Enables visuOgreQt's shared gl context on this thread against this window.
    VISUOGREQT_API virtual void makeCurrent() final;

    /// Get Ogre RenderWindow
    VISUOGREQT_API virtual ::Ogre::RenderTarget* getRenderTarget() final;

    /// Get the Ogre texture attached to the target
    VISUOGREQT_API virtual ::Ogre::TexturePtr getRenderTexture() final;

    /// Creates a worker able to handle resources from the window's OpenGL context.
    VISUOGREQT_API ::fwRenderOgre::IGraphicsWorker* createGraphicsWorker() final;

    /// Doesn't make much sense when not rendering on a screen.
    /// Returns a DPI of 220 to permit offscreen font rendering.
    VISUOGREQT_API virtual float getLogicalDotsPerInch() const final
    {
        // Return this value which is kind of common for HiDPI monitors.
        return 220;
    }

private:
    void render();

    /// Needed to get the widget Id
    static int m_counter;

    /// Used to instantiate the managers related to this instance with a proper name.
    int m_id;

    /// Frame identifier
    int m_frameId { 0 };

    /// Ogre root
    ::Ogre::Root* m_ogreRoot { nullptr };

    /// Ogre window
    ::Ogre::RenderWindow* m_ogreRenderWindow { nullptr };

    /// Ogre window
    ::Ogre::RenderTarget* m_ogreRenderTarget { nullptr };

    /// Ogre window
    ::Ogre::TexturePtr m_ogreTexture;

    /// OpenGL context used for offscreen rendering.
    std::shared_ptr<QOpenGLContext> m_glContext;

    /// Window width
    unsigned int m_width { 0 };

    /// Window height
    unsigned int m_height { 0 };

    /// Offscreen surface, needed to enable the OpenGL context on the current thread.
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

} // namespace visuOgreQt
