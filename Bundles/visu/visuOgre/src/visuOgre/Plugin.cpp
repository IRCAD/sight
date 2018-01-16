/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuOgre/Plugin.hpp"

#include <fwRenderOgre/Utils.hpp>

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <fwServices/macros.hpp>

#include <OgreLogManager.h>

namespace visuOgre
{

//------------------------------------------------------------------------------

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("visuOgre::Plugin");

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

Plugin::~Plugin() noexcept
{
}

//------------------------------------------------------------------------------

void Plugin::start()
{
    // Redirect Ogre Log to FW4SPL Log
    ::Ogre::LogManager* logMgr = new ::Ogre::LogManager();
    ::Ogre::Log* log           = logMgr->createLog("Ogre.log", true, false, false);
    log->addListener(new F4SOgreListener());
    log->setLogDetail(::Ogre::LL_BOREME);
}

//------------------------------------------------------------------------------

void Plugin::stop() noexcept
{
}

//------------------------------------------------------------------------------

void F4SOgreListener::messageLogged(const::Ogre::String& _message, ::Ogre::LogMessageLevel _lml, bool /*_maskDebug*/,
                                    const::Ogre::String& /*_logName*/, bool& _skipThisMessage)
{
    if (!_skipThisMessage)
    {
        switch(_lml)
        {
            case ::Ogre::LML_TRIVIAL:
                SLM_INFO(_message);
                break;
            case ::Ogre::LML_NORMAL:
                SLM_DEBUG(_message);
                break;
            case ::Ogre::LML_WARNING:
                SLM_WARN(_message);
                break;
            case ::Ogre::LML_CRITICAL:
                SLM_ERROR(_message);
                break;
        }
    }
}

//------------------------------------------------------------------------------

} // namespace operators
