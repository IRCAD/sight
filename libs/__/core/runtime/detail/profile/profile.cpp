/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "core/runtime/detail/profile/profile.hpp"

#include "core/runtime/detail/module.hpp"
#include "core/runtime/detail/profile/activater.hpp"
#include "core/runtime/detail/runtime.hpp"
#include "core/runtime/extension.hpp"

#include <algorithm>
#include <cstring>
#include <functional>

namespace sight::core::runtime::detail::profile
{

//------------------------------------------------------------------------------

profile::profile() :
    m_run(default_run)
{
}

//------------------------------------------------------------------------------

profile::~profile()
= default;

//------------------------------------------------------------------------------

void profile::add(SPTR(activater)activater)
{
    m_activaters.push_back(activater);
}

//------------------------------------------------------------------------------

void profile::add_starter(const std::string& identifier)
{
    const detail::runtime& runtime = detail::runtime::get();
    auto module                    = std::dynamic_pointer_cast<detail::module>(runtime.find_module(identifier));
    m_starters.insert({module->priority(), identifier});
}

//------------------------------------------------------------------------------

void profile::add_stopper(const std::string& identifier, int priority)
{
    m_stoppers.insert({priority, identifier});
}

//------------------------------------------------------------------------------

void profile::start()
{
    std::for_each(m_activaters.begin(), m_activaters.end(), [](auto& activater){activater->apply();});

    // Check validity of extension

    detail::runtime& runtime = detail::runtime::get();
    for(const auto& extension : runtime.extensions())
    {
        auto bundle         = std::dynamic_pointer_cast<detail::module>(extension->get_module());
        auto extension_impl = std::dynamic_pointer_cast<detail::extension>(extension);

        SIGHT_FATAL_IF(
            "Validation not ok for bundle = '" << extension->get_module()->identifier()
            << "'  (extension id = '" << extension->identifier() << "' )",
            bundle->enabled() && extension_impl->validate() == extension::invalid
        );
    }

    std::ranges::for_each(
        m_starters,
        [](auto& s)
        {
            auto identifier = s.second;
            auto module     = detail::runtime::get().find_enabled_module(identifier);
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

int profile::run()
{
    SIGHT_ASSERT("the 'run' callback is missing", m_run);
    int result = 0;
    result = m_run();
    return result;
}

//------------------------------------------------------------------------------

int profile::default_run()
{
    return 0;
}

//------------------------------------------------------------------------------

void profile::set_run_callback(run_callback_type callback)
{
    m_run = callback;
}

//------------------------------------------------------------------------------

void profile::stop()
{
    std::for_each(
        m_stoppers.rbegin(),
        m_stoppers.rend(),
        [](auto& s)
        {
            auto identifier = s.second;
            auto module     = detail::runtime::get().find_enabled_module(identifier);
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

static profile::sptr current_profile;

//------------------------------------------------------------------------------

void set_current_profile(profile::sptr prof)
{
    current_profile = prof;
}

//------------------------------------------------------------------------------

profile::sptr get_current_profile()
{
    return current_profile;
}

//------------------------------------------------------------------------------

} // namespace sight::core::runtime::detail::profile
