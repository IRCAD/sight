/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/OffScreenRenderWindowInteractorManager.hpp"

#include "fwRenderOgre/SRender.hpp"
#include "fwRenderOgre/WindowManager.hpp"

#define FW_PROFILING_DISABLED
#include <fwCore/Profiling.hpp>

#include <OGRE/OgreHardwarePixelBuffer.h>
#include <OGRE/OgreRenderTexture.h>
#include <OGRE/OgreTextureManager.h>

//-----------------------------------------------------------------------------

namespace fwRenderOgre
{

int OffScreenRenderWindowInteractorManager::m_counter = 0;

//-----------------------------------------------------------------------------

OffScreenRenderWindowInteractorManager::OffScreenRenderWindowInteractorManager(unsigned int _width,
                                                                               unsigned int _height) :
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

void OffScreenRenderWindowInteractorManager::requestRender()
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

    m_ogreRoot->renderOneFrame();
}

//-----------------------------------------------------------------------------

void OffScreenRenderWindowInteractorManager::createContainer( ::fwGui::container::fwContainer::sptr, bool, bool, bool)
{
    m_ogreRoot = ::fwRenderOgre::Utils::getOgreRoot();

    SLM_ASSERT("OpenGL RenderSystem not found",
               m_ogreRoot->getRenderSystem()->getName().find("GL") != std::string::npos);

    Ogre::NameValuePairList parameters;

    ::fwRenderOgre::WindowManager::sptr mgr = ::fwRenderOgre::WindowManager::get();

    // We share the OpenGL context on all windows. The first Ogre window will create the context, the other ones will
    // reuse the current context
    if(!mgr->hasWindow())
    {
        parameters["currentGLContext"] = Ogre::String("false");
    }
    else
    {
        parameters["currentGLContext"] = Ogre::String("true");
    }

#if defined(Q_OS_MAC)
    parameters["macAPI"]               = Ogre::String("cocoa");
    parameters["macAPICocoaUseNSView"] = Ogre::String("true");

    // We set the contextProfile to GLNativeSupport::CONTEXT_CORE by default otherwise ogre will initialize the default
    // context to "compatibility" which wil set openGL version to 2.1 which do not support all the feature we want
    parameters["contextProfile"] = Ogre::String("1");
#endif

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
    m_ogreTexture.reset();
    m_ogreRenderTarget = nullptr;

    OffScreenRenderWindowInteractorManager::m_counter--;

    if(m_ogreRenderWindow)
    {
        ::fwRenderOgre::WindowManager::sptr mgr = ::fwRenderOgre::WindowManager::get();
        mgr->unregisterWindow(m_ogreRenderWindow);
        m_ogreRenderWindow = nullptr;
    }
}

//-----------------------------------------------------------------------------

void OffScreenRenderWindowInteractorManager::makeCurrent()
{
    if(m_ogreRenderWindow)
    {
        ::Ogre::RenderSystem* renderSystem = m_ogreRoot->getRenderSystem();

        if(renderSystem)
        {
            // This allows to set the current OpengGL context in Ogre internal state
            renderSystem->_setRenderTarget(m_ogreRenderTarget);

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
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace fwRenderOgre
