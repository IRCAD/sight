/************************************************************************
 *
 * Copyright (C) 2018-2024 IRCAD France
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

#include <viz/scene3d/window_interactor.hpp>

#include <QOffscreenSurface>
#include <QOpenGLContext>

#include <memory>

namespace sight::module::viz::scene3d_qt
{

/// Defines a class to manage an offscreen render window that renders in an Ogre texture.
class offscreen_window_interactor final :
    public sight::viz::scene3d::window_interactor
{
public:

    SIGHT_DECLARE_CLASS(offscreen_window_interactor, sight::viz::scene3d::window_interactor);

    /**
     * @brief Initialize members.
     * @param _width width of the render window.
     * @param _height height of the render window.
     */
    offscreen_window_interactor(
        unsigned int _width,
        unsigned int _height
    );

    /// Destroys the render window.
    ~offscreen_window_interactor() final;

    /// Renders immediately the frame in the render window.
    void render_now() override;

    /// Renders immediately the frame in the render window.
    void request_render() override;

    /// Creates the render window in the global OpenGL context, @see module::viz::scene3d_qt::OpenGLContext.
    void create_container(ui::container::widget::sptr /*_parent*/,
                          bool /*fullscreen*/,
                          const std::string& /*id*/) override;

    /// Does nothing.
    void connect_to_container() override;

    /// Deletes the render window and the render texture.
    void disconnect_interactor() override;

    /// Returns the unique identifier of the widget.
    int get_widget_id() const override;

    /// Returns current frame number of the render window.
    int get_frame_id() const override;

    /// Makes the OpenGL context as current one on this thread against this window.
    void make_current() override;

    /// Gets the Ogre render target.
    Ogre::RenderTarget* get_render_target();

    /// Gets the Ogre render texture attached to the render target.
    Ogre::TexturePtr get_render_texture() override;

    /// Creates a worker able to handle resources from the window's OpenGL context.
    sight::viz::scene3d::graphics_worker* create_graphics_worker() override;

    /// Returns a DPI of 220 to permit offscreen font rendering.
    /// @warning Doesn't make much sense when not rendering on a screen.
    float get_logical_dots_per_inch() const override
    {
        // Return this value which is kind of common for HiDPI monitors.
        return 220;
    }

private:

    /// Updates the Ogre render target.
    void render();

    /// Defines a counter to get the widget ID.
    static int s_counter;

    /// Defines the unique identifier of the widget, used to instantiate the managers related to this instance with a
    // proper name.
    int m_id;

    /// Defines the number of render frame.
    int m_frame_id {0};

    /// Contains the Ogre root.
    Ogre::Root* m_ogre_root {nullptr};

    /// Contains the Ogre render target.
    Ogre::RenderTarget* m_ogre_render_target {nullptr};

    /// Contains the Ogre render texture attached to the render target
    Ogre::TexturePtr m_ogre_texture;

    /// Defines the window width.
    unsigned int m_width {0};

    /// Defines the window height.
    unsigned int m_height {0};

    /// Contains the offscreen surface, needed to enable the OpenGL context on the current thread.
    std::unique_ptr<QOffscreenSurface> m_offscreen_surface;
};

//-----------------------------------------------------------------------------

inline int offscreen_window_interactor::get_widget_id() const
{
    return m_id;
}

//-----------------------------------------------------------------------------

inline int offscreen_window_interactor::get_frame_id() const
{
    return m_frame_id;
}

//-----------------------------------------------------------------------------

inline Ogre::RenderTarget* offscreen_window_interactor::get_render_target()
{
    return m_ogre_render_target;
}

//-----------------------------------------------------------------------------

inline Ogre::TexturePtr offscreen_window_interactor::get_render_texture()
{
    return m_ogre_texture;
}

} // namespace sight::module::viz::scene3d_qt.
