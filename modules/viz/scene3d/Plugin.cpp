/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "modules/viz/scene3d/Plugin.hpp"

#include <OgreLogManager.h>

namespace sight::module::viz::scene3d
{

//------------------------------------------------------------------------------

SIGHT_REGISTER_PLUGIN("::sight::module::viz::scene3d::Plugin");

//------------------------------------------------------------------------------

Plugin::~Plugin() noexcept
{
}

//------------------------------------------------------------------------------

void Plugin::start()
{
    // Redirect Ogre Log to Sight Log.
    m_logManager = ::Ogre::LogManager::getSingletonPtr();
    m_log        = m_logManager->createLog("Ogre.log", true, false, false);
    m_listener   = new SightOgreListener();
    m_log->addListener(m_listener);
    m_log->setLogDetail(::Ogre::LL_BOREME);
}

//------------------------------------------------------------------------------

void Plugin::stop() noexcept
{
    m_log->removeListener(m_listener);
    delete m_listener;
    delete m_logManager;
}

//------------------------------------------------------------------------------

void SightOgreListener::messageLogged(const ::Ogre::String& _message,
                                      ::Ogre::LogMessageLevel _lml,
                                      bool,
                                      const ::Ogre::String&,
                                      bool& _skipThisMessage)
{
    if (!_skipThisMessage)
    {
        switch(_lml)
        {
            case ::Ogre::LML_TRIVIAL:
                SIGHT_INFO(_message);
                break;
            case ::Ogre::LML_NORMAL:
                SIGHT_DEBUG(_message);
                break;
            case ::Ogre::LML_WARNING:
                SIGHT_WARN(_message);
                break;
            case ::Ogre::LML_CRITICAL:
                SIGHT_ERROR(_message);
                break;
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d.
