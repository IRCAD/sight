/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

#include "viz/scene3d/Utils.hpp"

#include <core/runtime/Profile.hpp>
#include <core/tools/Os.hpp>

#include <boost/tokenizer.hpp>

#include <OgreLogManager.h>

/**
 * Do not mark `BOREME` as incorrect.
 * cspell:ignore BOREME
 */

namespace sight::module::viz::scene3d
{

//------------------------------------------------------------------------------

SIGHT_REGISTER_PLUGIN("sight::module::viz::scene3d::Plugin");

//------------------------------------------------------------------------------

Plugin::~Plugin() noexcept =
    default;

//------------------------------------------------------------------------------

void Plugin::start()
{
    // Find log dir
    const auto& current_profile = core::runtime::getCurrentProfile();
    const auto& profile_name    = current_profile ? current_profile->getName() : std::string();
    const auto& ogre_log        = core::tools::os::getUserCacheDir(profile_name) / "Ogre.log";

    // Start Ogre Log
    m_logManager = new Ogre::LogManager();
    m_log        = m_logManager->createLog(ogre_log.string(), true, false, true);
    m_listener   = new SightOgreListener();
    m_log->addListener(m_listener);
    m_log->setLogDetail(Ogre::LL_BOREME);

    // Ogre plugins to load
    if(this->getModule()->hasParameter("plugins"))
    {
        const std::string pluginsStr = this->getModule()->getParameterValue("plugins");
        const std::size_t count      = std::size_t(std::count(pluginsStr.begin(), pluginsStr.end(), ' '));

        std::vector<std::string> plugins;
        plugins.reserve(count);

        // Split the string into individual ones (one per plugin)
        {
            using separator = boost::char_separator<std::string::value_type>;
            using tokenizer = boost::tokenizer<separator>;

            const tokenizer t(pluginsStr, separator {" ", "", boost::drop_empty_tokens});
            plugins.assign(t.begin(), t.end());
        }

        sight::viz::scene3d::Utils::addPlugins(plugins);
    }
}

//------------------------------------------------------------------------------

void Plugin::stop() noexcept
{
    m_log->removeListener(m_listener);
    delete m_listener;
    delete m_logManager;
}

//------------------------------------------------------------------------------

void SightOgreListener::messageLogged(
    const Ogre::String& _message,
    Ogre::LogMessageLevel _lml,
    bool /*maskDebug*/,
    const Ogre::String& /*logName*/,
    bool& _skipThisMessage
)
{
    if(!_skipThisMessage)
    {
        switch(_lml)
        {
            case Ogre::LML_TRIVIAL:
                SIGHT_DEBUG(_message);
                break;

            case Ogre::LML_NORMAL:
                SIGHT_INFO(_message);
                break;

            case Ogre::LML_WARNING:
                SIGHT_WARN(_message);
                break;

            case Ogre::LML_CRITICAL:
                SIGHT_ERROR(_message);
                break;
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d.
