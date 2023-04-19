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

#include "modules/viz/scene3dQt/OffScreenWindowInteractor.hpp"

#include <modules/viz/scene3dQt/init.hpp>
#include <modules/viz/scene3dQt/OpenGLWorker.hpp>

#include <viz/scene3d/ogre.hpp>
#include <viz/scene3d/registry/macros.hpp>
#include <viz/scene3d/SRender.hpp>

#define FW_PROFILING_DISABLED
#include <core/Profiling.hpp>

#include <OGRE/OgreHardwarePixelBuffer.h>
#include <OGRE/OgreRenderTexture.h>
#include <OGRE/OgreTextureManager.h>

//-----------------------------------------------------------------------------

SIGHT_REGISTER_SCENE3D_OFFSCREEN(
    sight::module::viz::scene3dQt::OffScreenWindowInteractor,
    sight::viz::scene3d::IWindowInteractor::OFFSCREEN_REGISTRY_KEY
);

namespace sight::module::viz::scene3dQt
{

int OffScreenWindowInteractor::m_counter = 0;

//-----------------------------------------------------------------------------

OffScreenWindowInteractor::OffScreenWindowInteractor(
    sight::viz::scene3d::IWindowInteractor::OffscreenMgrKey /*unused*/,
    unsigned int _width,
    unsigned int _height
) :
    m_id(OffScreenWindowInteractor::m_counter++),
    m_width(_width),
    m_height(_height)
{
}

//-----------------------------------------------------------------------------

OffScreenWindowInteractor::~OffScreenWindowInteractor()
= default;

//-----------------------------------------------------------------------------

void OffScreenWindowInteractor::renderNow()
{
    this->render();
}

//------------------------------------------------------------------------------

void OffScreenWindowInteractor::requestRender()
{
    this->render();
}

//-----------------------------------------------------------------------------

void OffScreenWindowInteractor::createContainer(
    sight::ui::base::container::fwContainer::sptr /*_parent*/,
    bool /*fullscreen*/,
    const std::string& /*id*/
)
{
    m_ogreRoot = sight::viz::scene3d::Utils::getOgreRoot();

    SIGHT_ASSERT(
        "OpenGL RenderSystem not found",
        m_ogreRoot->getRenderSystem()->getName().find("GL") != std::string::npos
    );

    // We share the OpenGL context on all windows. The first window will create the context, the other ones will
    // reuse the current context.
    m_offscreenSurface = std::make_unique<QOffscreenSurface>();
    this->makeCurrent();

    initResources();

    auto& texMgr = Ogre::TextureManager::getSingleton();
    m_ogreTexture = texMgr.createManual(
        "OffscreenRT" + std::to_string(m_id),
        sight::viz::scene3d::RESOURCE_GROUP,
        Ogre::TEX_TYPE_2D,
        m_width,
        m_height,
        0,
        Ogre::PF_BYTE_RGBA,
        Ogre::TU_RENDERTARGET
    );
    m_ogreRenderTarget = m_ogreTexture->getBuffer()->getRenderTarget();

    const auto renderService = sight::viz::scene3d::SRender::dynamicCast(m_renderService.lock());
    SIGHT_ASSERT("RenderService wrongly instantiated. ", renderService);

    std::map<int, sight::viz::scene3d::Layer::wptr> orderedLayers;
    for(auto& layer : renderService->getLayers())
    {
        orderedLayers[layer.second->getOrder()] = layer.second;
    }

    for(auto& layer : orderedLayers)
    {
        const auto l = layer.second.lock();
        l->setRenderTarget(m_ogreRenderTarget);
        if(!l->initialized())
        {
            l->createScene();
        }
    }
}

//-----------------------------------------------------------------------------

void OffScreenWindowInteractor::connectToContainer()
{
}

//-----------------------------------------------------------------------------

void OffScreenWindowInteractor::disconnectInteractor()
{
    m_ogreRenderTarget = nullptr;

    if(m_ogreTexture)
    {
        auto& texMgr = Ogre::TextureManager::getSingleton();
        texMgr.remove(m_ogreTexture);
        m_ogreTexture.reset();
    }

    m_offscreenSurface.reset();
}

//-----------------------------------------------------------------------------

void OffScreenWindowInteractor::makeCurrent()
{
    {
        QOpenGLContext::globalShareContext()->makeCurrent(m_offscreenSurface.get());

        Ogre::RenderSystem* renderSystem = m_ogreRoot->getRenderSystem();

        if(renderSystem != nullptr)
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
            renderSystem->postExtraThreadsStarted();

            // This allows to set the current OpenGL context in Ogre internal state
            renderSystem->_setRenderTarget(m_ogreRenderTarget);
        }
    }
}

//------------------------------------------------------------------------------

void OffScreenWindowInteractor::render()
{
    service::IService::sptr renderService                = m_renderService.lock();
    sight::viz::scene3d::SRender::sptr ogreRenderService = sight::viz::scene3d::SRender::dynamicCast(renderService);
    ogreRenderService->slot(sight::viz::scene3d::SRender::s_COMPUTE_CAMERA_CLIPPING_SLOT)->asyncRun();

    ++m_frameId;
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
    this->makeCurrent();

    m_ogreRenderTarget->update(false);
}

//-----------------------------------------------------------------------------

sight::viz::scene3d::IGraphicsWorker* OffScreenWindowInteractor::createGraphicsWorker()
{
    SIGHT_ASSERT("Not supported", false);
    return nullptr;
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3dQt
