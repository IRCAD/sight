/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
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

#include "fwRuntime/impl/profile/Profile.hpp"

#include "fwRuntime/Extension.hpp"
#include "fwRuntime/impl/Module.hpp"
#include "fwRuntime/impl/profile/Activater.hpp"
#include "fwRuntime/impl/profile/Initializer.hpp"
#include "fwRuntime/impl/profile/Starter.hpp"
#include "fwRuntime/impl/profile/Stopper.hpp"
#include "fwRuntime/impl/profile/Uninitializer.hpp"
#include "fwRuntime/impl/Runtime.hpp"

#include <algorithm>
#include <cstring>
#include <functional>

namespace fwRuntime
{

namespace impl
{

namespace profile
{

namespace
{
template< typename E >
struct Apply
{
    //------------------------------------------------------------------------------

    void operator() ( E e )
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

void Profile::add( SPTR( Activater )activater )
{
    m_activaters.push_back( activater );
}

//------------------------------------------------------------------------------

void Profile::add( SPTR( Starter )starter )
{
    m_starters.push_back( starter );
}

//------------------------------------------------------------------------------

void Profile::add( SPTR( Stopper )stopper )
{
    m_stoppers.push_back( stopper );
}

//------------------------------------------------------------------------------

void Profile::add( SPTR( Initializer )initializer )
{
    m_initializers.push_back(initializer);
}

//------------------------------------------------------------------------------

void Profile::add( SPTR( Uninitializer )uninitializer )
{
    m_uninitializers.push_back(uninitializer);
}

//------------------------------------------------------------------------------

void Profile::start()
{
    std::for_each( m_activaters.begin(), m_activaters.end(), Apply< ActivaterContainer::value_type >() );

    // Check validity of extension

    impl::Runtime& runtime = impl::Runtime::get();
    for( auto& extension : runtime.getExtensions() )
    {
        auto bundle = std::dynamic_pointer_cast< impl::Module >(extension->getModule());

        OSLM_FATAL_IF( "Validation not ok for bundle = '" << extension->getModule()->getIdentifier() <<
                       "'  (extension id = '" << extension->getIdentifier() << "' )",
                       bundle->isEnabled() && extension->validate() == Extension::Invalid );
    }

    std::for_each( m_starters.begin(), m_starters.end(), Apply< StarterContainer::value_type >() );
    OSLM_TRACE( "NB INITIALIZERS: " <<  m_initializers.size() );
}

//------------------------------------------------------------------------------

int Profile::run()
{
    SLM_ASSERT("the 'run' callback is missing", m_run);
    int result;
    result = m_run();
    return result;
}

//------------------------------------------------------------------------------

int Profile::defaultRun()
{
    SLM_TRACE_FUNC();
    this->setup();
    this->cleanup();
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
    std::for_each( m_stoppers.rbegin(), m_stoppers.rend(), Apply< StopperContainer::value_type >() );
}

//------------------------------------------------------------------------------

void Profile::setup()
{
    InitializerContainer initializers;
    initializers = m_initializers;
    m_initializers.clear();
    std::for_each( initializers.begin(), initializers.end(), Apply< InitializerContainer::value_type >() );
}

//------------------------------------------------------------------------------

void Profile::cleanup()
{
    std::for_each( m_uninitializers.rbegin(), m_uninitializers.rend(), Apply< UninitializerContainer::value_type >() );
    m_uninitializers.clear();
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

} // namespace impl

} // namespace fwRuntime
