/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
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

#include "modules/viz/scene3dQt/config.hpp"

#include <viz/scene3d/IWindowInteractor.hpp>

#include <QOffscreenSurface>
#include <QOpenGLContext>

#include <memory>

namespace sight::module::viz::scene3dQt
{

/// Defines a class to manage an offscreen render window that renders in an Ogre texture.
class MODULE_VIZ_SCENE3DQT_CLASS_API OffScreenWindowInteractor final :
    public sight::viz::scene3d::IWindowInteractor
{
public:

    SIGHT_DECLARE_CLASS(OffScreenWindowInteractor, sight::viz::scene3d::IWindowInteractor);

    /**
     * @brief Initialize members.
     * @param _width width of the render window.
     * @param _height height of the render window.
     */
    MODULE_VIZ_SCENE3DQT_API OffScreenWindowInteractor(
        sight::viz::scene3d::IWindowInteractor::OffscreenMgrKey /*unused*/,
        unsigned int _width,
        unsigned int _height
    );

    /// Destroys the render window.
    MODULE_VIZ_SCENE3DQT_API ~OffScreenWindowInteractor() final;

    /// Renders immediately the frame in the render window.
    MODULE_VIZ_SCENE3DQT_API void renderNow() override;

    /// Renders immediately the frame in the render window.
    MODULE_VIZ_SCENE3DQT_API void requestRender() override;

    /// Creates the render window in the global OpenGL context, @see module::viz::scene3dQt::OpenGLContext.
    MODULE_VIZ_SCENE3DQT_API void createContainer(ui::base::container::fwContainer::sptr /*_parent*/,
                                                  bool /*fullscreen*/,
                                                  const std::string& /*id*/) override;

    /// Does nothing.
    MODULE_VIZ_SCENE3DQT_API void connectToContainer() override;

    /// Deletes the render window and the render texture.
    MODULE_VIZ_SCENE3DQT_API void disconnectInteractor() override;

    /// Returns the unique identifier of the widget.
    MODULE_VIZ_SCENE3DQT_API int getWidgetId() const override;

    /// Returns current frame number of the render window.
    MODULE_VIZ_SCENE3DQT_API int getFrameId() const override;

    /// Makes the OpenGL context as current one on this thread against this window.
    MODULE_VIZ_SCENE3DQT_API void makeCurrent() override;

    /// Gets the Ogre render target.
    MODULE_VIZ_SCENE3DQT_API Ogre::RenderTarget* getRenderTarget();

    /// Gets the Ogre render texture attached to the render target.
    MODULE_VIZ_SCENE3DQT_API Ogre::TexturePtr getRenderTexture() override;

    /// Creates a worker able to handle resources from the window's OpenGL context.
    MODULE_VIZ_SCENE3DQT_API sight::viz::scene3d::IGraphicsWorker* createGraphicsWorker() override;

    /// Returns a DPI of 220 to permit offscreen font rendering.
    /// @warning Doesn't make much sense when not rendering on a screen.
    MODULE_VIZ_SCENE3DQT_API float getLogicalDotsPerInch() const override
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
    int m_frameId {0};

    /// Contains the Ogre root.
    Ogre::Root* m_ogreRoot {nullptr};

    /// Contains the Ogre render target.
    Ogre::RenderTarget* m_ogreRenderTarget {nullptr};

    /// Contains the Ogre render texture attached to the render target
    Ogre::TexturePtr m_ogreTexture;

    /// Defines the window width.
    unsigned int m_width {0};

    /// Defines the window height.
    unsigned int m_height {0};

    /// Contains the offscreen surface, needed to enable the OpenGL context on the current thread.
    std::unique_ptr<QOffscreenSurface> m_offscreenSurface;
};

//-----------------------------------------------------------------------------

inline int OffScreenWindowInteractor::getWidgetId() const
{
    return m_id;
}

//-----------------------------------------------------------------------------

inline int OffScreenWindowInteractor::getFrameId() const
{
    return m_frameId;
}

//-----------------------------------------------------------------------------

inline Ogre::RenderTarget* OffScreenWindowInteractor::getRenderTarget()
{
    return m_ogreRenderTarget;
}

//-----------------------------------------------------------------------------

inline Ogre::TexturePtr OffScreenWindowInteractor::getRenderTexture()
{
    return m_ogreTexture;
}

} // namespace sight::module::viz::scene3dQt.
