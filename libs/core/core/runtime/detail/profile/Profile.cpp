/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "core/runtime/detail/profile/Profile.hpp"

#include "core/runtime/detail/Module.hpp"
#include "core/runtime/detail/profile/Activater.hpp"
#include "core/runtime/detail/Runtime.hpp"
#include "core/runtime/Extension.hpp"

#include <algorithm>
#include <cstring>
#include <functional>

namespace sight::core::runtime::detail::profile
{

//------------------------------------------------------------------------------

Profile::Profile() :
    m_run(defaultRun)
{
}

//------------------------------------------------------------------------------

Profile::~Profile()
= default;

//------------------------------------------------------------------------------

void Profile::add(SPTR(Activater)activater)
{
    m_activaters.push_back(activater);
}

//------------------------------------------------------------------------------

void Profile::addStarter(const std::string& identifier)
{
    detail::Runtime& runtime = detail::Runtime::get();
    auto module              = std::dynamic_pointer_cast<detail::Module>(runtime.findModule(identifier));
    m_starters.insert({module->priority(), identifier});
}

//------------------------------------------------------------------------------

void Profile::addStopper(const std::string& identifier, int priority)
{
    m_stoppers.insert({priority, identifier});
}

//------------------------------------------------------------------------------

void Profile::start()
{
    std::for_each(m_activaters.begin(), m_activaters.end(), [](auto& activater){activater->apply();});

    // Check validity of extension

    detail::Runtime& runtime = detail::Runtime::get();
    for(const auto& extension : runtime.getExtensions())
    {
        auto bundle = std::dynamic_pointer_cast<detail::Module>(extension->getModule());

        SIGHT_FATAL_IF(
            "Validation not ok for bundle = '" << extension->getModule()->getIdentifier()
            << "'  (extension id = '" << extension->getIdentifier() << "' )",
            bundle->isEnabled() && extension->validate() == Extension::Invalid
        );
    }

    std::ranges::for_each(
        m_starters,
        [](auto& s)
        {
            auto identifier = s.second;
            auto module     = detail::Runtime::get().findEnabledModule(identifier);
            SIGHT_FATAL_IF("Unable to start module " + identifier + ": not found.", module == nullptr);
            try
            {
                module->start();
            }
            catch(const std::exception& e)
            {
                SIGHT_FATAL("Unable to start module " + identifier + ". " + e.what());
            }
        });
}

//------------------------------------------------------------------------------

int Profile::run()
{
    SIGHT_ASSERT("the 'run' callback is missing", m_run);
    int result = 0;
    result = m_run();
    return result;
}

//------------------------------------------------------------------------------

int Profile::defaultRun()
{
    return 0;
}

//------------------------------------------------------------------------------

void Profile::setRunCallback(RunCallbackType callback)
{
    m_run = callback;
}

//------------------------------------------------------------------------------

void Profile::stop()
{
    std::for_each(
        m_stoppers.rbegin(),
        m_stoppers.rend(),
        [](auto& s)
        {
            auto identifier = s.second;
            auto module     = detail::Runtime::get().findEnabledModule(identifier);
            SIGHT_FATAL_IF(
                "Unable to stop module " + identifier + ". Not found.",
                module == nullptr
            );
            try
            {
                SIGHT_INFO("Stopping module : " + identifier);
                module->stop();
            }
            catch(const std::exception& e)
            {
                SIGHT_ERROR("Unable to stop module " + identifier + ". " + e.what());
            }
        });
}

//------------------------------------------------------------------------------

static Profile::sptr current_profile;

//------------------------------------------------------------------------------

void setCurrentProfile(Profile::sptr prof)
{
    current_profile = prof;
}

//------------------------------------------------------------------------------

Profile::sptr getCurrentProfile()
{
    return current_profile;
}

//------------------------------------------------------------------------------

} // namespace sight::core::runtime::detail::profile
