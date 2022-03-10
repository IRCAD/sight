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
#include "core/runtime/detail/profile/Starter.hpp"
#include "core/runtime/detail/profile/Stopper.hpp"
#include "core/runtime/detail/Runtime.hpp"
#include "core/runtime/Extension.hpp"

#include <algorithm>
#include <cstring>
#include <functional>

namespace sight::core::runtime
{

namespace detail
{

namespace profile
{

namespace
{

template<typename E>
struct Apply
{
    //------------------------------------------------------------------------------

    void operator()(E e)
    {
        e->apply();
    }
};

}

//------------------------------------------------------------------------------

Profile::Profile() :
    m_checkSingleInstance(false)
{
    m_run = std::bind(&Profile::defaultRun, this);
}

//------------------------------------------------------------------------------

Profile::~Profile()
{
}

//------------------------------------------------------------------------------

void Profile::add(SPTR(Activater)activater)
{
    m_activaters.push_back(activater);
}

//------------------------------------------------------------------------------

void Profile::add(SPTR(Starter)starter)
{
    m_starters.push_back(starter);
}

//------------------------------------------------------------------------------

void Profile::add(SPTR(Stopper)stopper)
{
    m_stoppers.push_back(stopper);
}

//------------------------------------------------------------------------------

void Profile::start()
{
    std::for_each(m_activaters.begin(), m_activaters.end(), Apply<ActivaterContainer::value_type>());

    // Check validity of extension

    detail::Runtime& runtime = detail::Runtime::get();
    for(auto& extension : runtime.getExtensions())
    {
        auto bundle = std::dynamic_pointer_cast<detail::Module>(extension->getModule());

        SIGHT_FATAL_IF(
            "Validation not ok for bundle = '" << extension->getModule()->getIdentifier()
            << "'  (extension id = '" << extension->getIdentifier() << "' )",
            bundle->isEnabled() && extension->validate() == Extension::Invalid
        );
    }

    std::for_each(m_starters.begin(), m_starters.end(), Apply<StarterContainer::value_type>());
}

//------------------------------------------------------------------------------

int Profile::run()
{
    SIGHT_ASSERT("the 'run' callback is missing", m_run);
    int result;
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
    std::for_each(m_stoppers.rbegin(), m_stoppers.rend(), Apply<StopperContainer::value_type>());
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

} // namespace profile

} // namespace detail

} // namespace sight::core::runtime
