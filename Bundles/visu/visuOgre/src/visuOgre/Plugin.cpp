/************************************************************************
 *
 * Copyright (C) 2014-2018 IRCAD France
 * Copyright (C) 2014-2018 IHU Strasbourg
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

#include "visuOgre/Plugin.hpp"

#include <fwRenderOgre/Utils.hpp>

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <fwServices/macros.hpp>

#include <OgreLogManager.h>

namespace visuOgre
{

//------------------------------------------------------------------------------

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::visuOgre::Plugin");

//------------------------------------------------------------------------------

Plugin::~Plugin() noexcept
{
}

//------------------------------------------------------------------------------

void Plugin::start()
{
    // Redirect Ogre Log to Sight Log
    ::Ogre::LogManager* logMgr = new ::Ogre::LogManager();
    ::Ogre::Log* log           = logMgr->createLog("Ogre.log", true, false, false);
    log->addListener(new SightOgreListener());
    log->setLogDetail(::Ogre::LL_BOREME);
}

//------------------------------------------------------------------------------

void Plugin::stop() noexcept
{
}

//------------------------------------------------------------------------------

void SightOgreListener::messageLogged(const ::Ogre::String& _message, ::Ogre::LogMessageLevel _lml, bool /*_maskDebug*/,
                                      const ::Ogre::String& /*_logName*/, bool& _skipThisMessage)
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
