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

#include "modules/viz/scene3d_qt/offscreen_window_interactor.hpp"

#include <modules/viz/scene3d_qt/init.hpp>
#include <modules/viz/scene3d_qt/OpenGLWorker.hpp>

#include <viz/scene3d/ogre.hpp>
#include <viz/scene3d/registry/macros.hpp>
#include <viz/scene3d/render.hpp>

#define FW_PROFILING_DISABLED
#include <core/profiling.hpp>

#include <OGRE/OgreHardwarePixelBuffer.h>
#include <OGRE/OgreRenderTexture.h>
#include <OGRE/OgreTextureManager.h>

//-----------------------------------------------------------------------------

SIGHT_REGISTER_SCENE3D_OFFSCREEN(
    sight::module::viz::scene3d_qt::offscreen_window_interactor,
    sight::viz::scene3d::window_interactor::OFFSCREEN_REGISTRY_KEY
);

namespace sight::module::viz::scene3d_qt
{

int offscreen_window_interactor::s_counter = 0;

//-----------------------------------------------------------------------------

offscreen_window_interactor::offscreen_window_interactor(
    unsigned int _width,
    unsigned int _height
) :
    m_id(offscreen_window_interactor::s_counter++),
    m_width(_width),
    m_height(_height)
{
}

//-----------------------------------------------------------------------------

offscreen_window_interactor::~offscreen_window_interactor()
= default;

//-----------------------------------------------------------------------------

void offscreen_window_interactor::render_now()
{
    this->render();
}

//------------------------------------------------------------------------------

void offscreen_window_interactor::request_render()
{
    this->render();
}

//-----------------------------------------------------------------------------

void offscreen_window_interactor::create_container(
    sight::ui::container::widget::sptr /*_parent*/,
    bool /*fullscreen*/,
    const std::string& /*id*/
)
{
    m_ogre_root = sight::viz::scene3d::utils::get_ogre_root();

    SIGHT_ASSERT(
        "OpenGL RenderSystem not found",
        m_ogre_root->getRenderSystem()->getName().find("GL") != std::string::npos
    );

    // We share the OpenGL context on all windows. The first window will create the context, the other ones will
    // reuse the current context.
    m_offscreen_surface = std::make_unique<QOffscreenSurface>();
    this->make_current();

    init_resources();

    auto& tex_mgr = Ogre::TextureManager::getSingleton();
    m_ogre_texture = tex_mgr.createManual(
        "OffscreenRT" + std::to_string(m_id),
        sight::viz::scene3d::RESOURCE_GROUP,
        Ogre::TEX_TYPE_2D,
        m_width,
        m_height,
        0,
        Ogre::PF_BYTE_RGBA,
        Ogre::TU_RENDERTARGET
    );
    m_ogre_render_target = m_ogre_texture->getBuffer()->getRenderTarget();

    const auto render_service = std::dynamic_pointer_cast<sight::viz::scene3d::render>(m_render_service.lock());
    SIGHT_ASSERT("RenderService wrongly instantiated. ", render_service);

    std::map<int, sight::viz::scene3d::layer::wptr> ordered_layers;
    for(auto& layer : render_service->get_layers())
    {
        ordered_layers[layer.second->get_order()] = layer.second;
    }

    for(auto& layer : ordered_layers)
    {
        const auto l = layer.second.lock();
        l->set_render_target(m_ogre_render_target);
        if(!l->initialized())
        {
            l->create_scene();
        }
    }
}

//-----------------------------------------------------------------------------

void offscreen_window_interactor::connect_to_container()
{
}

//-----------------------------------------------------------------------------

void offscreen_window_interactor::disconnect_interactor()
{
    m_ogre_render_target = nullptr;

    if(m_ogre_texture)
    {
        auto& tex_mgr = Ogre::TextureManager::getSingleton();
        tex_mgr.remove(m_ogre_texture);
        m_ogre_texture.reset();
    }

    m_offscreen_surface.reset();
}

//-----------------------------------------------------------------------------

void offscreen_window_interactor::make_current()
{
    {
        QOpenGLContext::globalShareContext()->makeCurrent(m_offscreen_surface.get());

        Ogre::RenderSystem* render_system = m_ogre_root->getRenderSystem();

        if(render_system != nullptr)
        {
            // Use this trick to apply the current OpenGL context
            //
            // Actually this method does the following :
            // void GLRenderSystem::postExtraThreadsStarted()
            // {
            //   OGRE_LOCK_MUTEX(mThreadInitMutex);
            //   if(mCurrentContext)
            //     mCurrentContext->setCurrent();
            // }
            //
            // This is actually want we want to do, even if this is not the initial purpose of this method
            //
            render_system->postExtraThreadsStarted();

            // This allows to set the current OpenGL context in Ogre internal state
            render_system->_setRenderTarget(m_ogre_render_target);
        }
    }
}

//------------------------------------------------------------------------------

void offscreen_window_interactor::render()
{
    service::base::sptr render_service                    = m_render_service.lock();
    sight::viz::scene3d::render::sptr ogre_render_service = std::dynamic_pointer_cast<sight::viz::scene3d::render>(
        render_service
    );

    ++m_frame_id;
    /*
       How we tied in the render function for OGre3D with QWindow's render function. This is what gets call
       repeatedly. Note that we don't call this function directly; rather we use the renderNow() function
       to call this method as we don't want to render the Ogre3D scene unless everything is set up first.
       That is what renderNow() does.

       Theoretically you can have one function that does this check but from my experience it seems better
       to keep things separate and keep the render function as simple as possible.
     */

    FW_PROFILE_FRAME_AVG("Ogre", 3);
    FW_PROFILE_AVG("Ogre", 3);
    this->make_current();

    m_ogre_render_target->update(false);
}

//-----------------------------------------------------------------------------

sight::viz::scene3d::graphics_worker* offscreen_window_interactor::create_graphics_worker()
{
    SIGHT_ASSERT("Not supported", false);
    return nullptr;
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d_qt
