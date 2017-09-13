/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRuntime/profile/Profile.hpp"

#include "fwRuntime/Bundle.hpp"
#include "fwRuntime/Extension.hpp"
#include "fwRuntime/profile/Activater.hpp"
#include "fwRuntime/profile/Initializer.hpp"
#include "fwRuntime/profile/Starter.hpp"
#include "fwRuntime/profile/Stopper.hpp"
#include "fwRuntime/profile/Uninitializer.hpp"
#include "fwRuntime/Runtime.hpp"

#include <algorithm>
#include <cstring>
#include <functional>

namespace fwRuntime
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

Profile::wptr current_profile;

//------------------------------------------------------------------------------

void setCurrentProfile(Profile::sptr prof)
{
    current_profile = prof;
}

//------------------------------------------------------------------------------

Profile::sptr getCurrentProfile()
{
    return current_profile.lock();
}

//------------------------------------------------------------------------------

Profile::Profile() :
    m_checkSingleInstance(false),
#ifdef ANDROID
    m_app(nullptr),
#endif
    m_argc(0),
    m_argv(NULL)
{
    m_run = std::bind(&Profile::defaultRun, this);
}

//------------------------------------------------------------------------------

Profile::~Profile()
{
    if (m_argv)
    {
        delete[] m_argv;
    }
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
    Runtime* rntm( Runtime::getDefault() );
    for( Runtime::ExtensionIterator i = rntm->extensionsBegin(); i != rntm->extensionsEnd(); ++i )
    {
        SPTR( Extension ) extension( *i );
        OSLM_FATAL_IF(
            "Validation not ok for bundle = '" << extension->getBundle()->getIdentifier() << "'  (extension id = '" << extension->getIdentifier() << "' )",
            extension->getBundle()->isEnable() && extension->validate() == Extension::Invalid );
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

void Profile::setParams(int argc, char** argv)
{
    ParamsContainer params;
    for(int i = 0; i < argc; ++i)
    {
        params.push_back( std::string(argv[i]) );
    }
    this->setParams(params);
}

//------------------------------------------------------------------------------

void Profile::setParams(const Profile::ParamsContainer& params)
{
    m_params = params;

    if (m_argv)
    {
        delete[] m_argv;
    }

    m_argc = static_cast<int>(m_params.size());
    // allocate memory for an array of character strings
    m_argv = new char*[m_params.size()];

    // for each string, allocate memory in the character array and copy
    for(size_t i = 0; i < m_params.size(); i++)
    {
        size_t paramSize = m_params[i].size();
        m_argv[i] = new char[paramSize+1];
#ifndef _WIN32
        strncpy(m_argv[i], m_params[i].c_str(), paramSize);
        m_argv[i][paramSize] = '\0';
#else
        strncpy_s(m_argv[i], paramSize+1, m_params[i].c_str(), paramSize);
#endif
    }
}

//------------------------------------------------------------------------------

} // namespace profile

} // namespace fwRuntime
