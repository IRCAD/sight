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

#include "visuOgreQt/OffScreenRenderWindowInteractorManager.hpp"

#include "visuOgreQt/OpenGLWorker.hpp"

#include <fwRenderOgre/registry/macros.hpp>
#include <fwRenderOgre/SRender.hpp>
#include <fwRenderOgre/WindowManager.hpp>

#define FW_PROFILING_DISABLED
#include <fwCore/Profiling.hpp>

#include <OGRE/OgreHardwarePixelBuffer.h>
#include <OGRE/OgreRenderTexture.h>
#include <OGRE/OgreTextureManager.h>

#include <boost/make_unique.hpp>

//-----------------------------------------------------------------------------

fwRenderOgreRegisterOffscreenMgrMacro(::visuOgreQt::OffScreenRenderWindowInteractorManager,
                                      ::fwRenderOgre::IRenderWindowInteractorManager::OFFSCREEN_REGISTRY_KEY);

namespace visuOgreQt
{

int OffScreenRenderWindowInteractorManager::m_counter = 0;

//-----------------------------------------------------------------------------

OffScreenRenderWindowInteractorManager::OffScreenRenderWindowInteractorManager(
    ::fwRenderOgre::IRenderWindowInteractorManager::OffscreenMgrKey, unsigned int _width, unsigned int _height) :
    m_id(OffScreenRenderWindowInteractorManager::m_counter++),
    m_width(_width),
    m_height(_height)
{
}

//-----------------------------------------------------------------------------

OffScreenRenderWindowInteractorManager::~OffScreenRenderWindowInteractorManager()
{
}

//-----------------------------------------------------------------------------

void OffScreenRenderWindowInteractorManager::renderNow()
{
    this->render();
}

//------------------------------------------------------------------------------

void OffScreenRenderWindowInteractorManager::requestRender()
{
    this->render();
}

//-----------------------------------------------------------------------------

void OffScreenRenderWindowInteractorManager::createContainer( ::fwGui::container::fwContainer::sptr, bool, bool, bool)
{
    m_ogreRoot = ::fwRenderOgre::Utils::getOgreRoot();

    SLM_ASSERT("OpenGL RenderSystem not found",
               m_ogreRoot->getRenderSystem()->getName().find("GL") != std::string::npos);

    Ogre::NameValuePairList parameters;

    ::fwRenderOgre::WindowManager::sptr mgr = ::fwRenderOgre::WindowManager::get();

    // We share the OpenGL context on all windows. The first window will create the context, the other ones will
    // reuse the current context.
    m_offscreenSurface = ::boost::make_unique<QOffscreenSurface>();
    m_glContext        = OpenGLContext::getGlobalOgreOpenGLContext();
    this->makeCurrent();

    parameters["currentGLContext"] = "true";

    // We create the renderWindow with a dummy size of 1 by 1
    m_ogreRenderWindow = m_ogreRoot->createRenderWindow("OffScreenWindow_" + std::to_string(m_id),
                                                        1,
                                                        1,
                                                        false,
                                                        &parameters);
    mgr->registerWindow(m_ogreRenderWindow);

    m_ogreRenderWindow->setHidden(true);
    m_ogreRenderWindow->setAutoUpdated(false);

    auto& texMgr = ::Ogre::TextureManager::getSingleton();
    m_ogreTexture = texMgr.createManual("OffscreenRT" + std::to_string(m_id),
                                        ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                                        ::Ogre::TEX_TYPE_2D,
                                        m_width, m_height, 0,
                                        ::Ogre::PF_BYTE_RGBA,
                                        ::Ogre::TU_RENDERTARGET);
    m_ogreRenderTarget = m_ogreTexture->getBuffer()->getRenderTarget();

}

//-----------------------------------------------------------------------------

void OffScreenRenderWindowInteractorManager::connectToContainer()
{

}

//-----------------------------------------------------------------------------

void OffScreenRenderWindowInteractorManager::disconnectInteractor()
{
    m_ogreRenderTarget = nullptr;

    OffScreenRenderWindowInteractorManager::m_counter--;

    if(m_ogreRenderWindow)
    {
        ::fwRenderOgre::WindowManager::sptr mgr = ::fwRenderOgre::WindowManager::get();
        mgr->unregisterWindow(m_ogreRenderWindow);
        m_ogreRenderWindow = nullptr;
    }

    if(m_ogreTexture)
    {
        auto& texMgr = ::Ogre::TextureManager::getSingleton();
        texMgr.remove(m_ogreTexture);
        m_ogreTexture.reset();
    }

    m_offscreenSurface.reset();
}

//-----------------------------------------------------------------------------

void OffScreenRenderWindowInteractorManager::makeCurrent()
{
    if(m_glContext)
    {
        m_glContext->makeCurrent(m_offscreenSurface.get());
    }
}

//------------------------------------------------------------------------------

void OffScreenRenderWindowInteractorManager::render()
{
    if(m_ogreRenderWindow == nullptr)
    {
        return;
    }

    ::fwServices::IService::sptr renderService      = m_renderService.lock();
    ::fwRenderOgre::SRender::sptr ogreRenderService = ::fwRenderOgre::SRender::dynamicCast( renderService );
    ogreRenderService->slot(::fwRenderOgre::SRender::s_COMPUTE_CAMERA_CLIPPING_SLOT)->asyncRun();

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

    m_ogreRenderTarget->update();
}

//-----------------------------------------------------------------------------

::fwRenderOgre::IGraphicsWorker* OffScreenRenderWindowInteractorManager::createGraphicsWorker()
{
    return new OpenGLWorker(m_offscreenSurface.get());
}

//-----------------------------------------------------------------------------

} // namespace visuOgreQt
