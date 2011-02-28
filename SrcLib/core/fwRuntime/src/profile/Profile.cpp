/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRuntime/profile/Profile.hpp"

#include <algorithm>
#include <cstring>

#include <boost/bind.hpp>

#include "fwRuntime/Runtime.hpp"
#include "fwRuntime/profile/Activater.hpp"
#include "fwRuntime/profile/Starter.hpp"
#include "fwRuntime/profile/Stopper.hpp"
#include "fwRuntime/profile/Initializer.hpp"
#include "fwRuntime/profile/Uninitializer.hpp"

#include "fwRuntime/Extension.hpp"
#include "fwRuntime/Bundle.hpp"

namespace fwRuntime
{

namespace profile
{

namespace
{

    template< typename E >
    struct Apply
    {
        void operator() ( E e )
        {
            e->apply();
        }
    };

}

// =========================================================
Profile::wptr current_profile;

void setCurrentProfile(Profile::sptr prof)
{
    current_profile = prof;
}

Profile::sptr getCurrentProfile()
{
    return current_profile.lock();
}

// =========================================================

Profile::Profile(): m_checkSingleInstance(false)
{
    m_run = ::boost::bind(&Profile::defaultRun, this);
}

//------------------------------------------------------------------------------

void Profile::add( SPTR( Activater ) activater )
{
    m_activaters.push_back( activater );
}

//------------------------------------------------------------------------------

void Profile::add( SPTR( Starter ) starter )
{
    m_starters.push_back( starter );
    m_stoppers.push_back( SPTR( Stopper ) ( new Stopper(starter) ));
}


//------------------------------------------------------------------------------
void Profile::add( SPTR( Initializer ) initializer )
{
    m_initializers.push_back(initializer);
}

//------------------------------------------------------------------------------
void Profile::add( SPTR( Uninitializer ) uninitializer )
{
    m_uninitializers.push_back(uninitializer);
}

//------------------------------------------------------------------------------

void Profile::start()
{
    std::for_each( m_activaters.begin(), m_activaters.end(), Apply< ActivaterContainer::value_type >() );


    // Check validity of extension
    Runtime * rntm( Runtime::getDefault() );
    for( Runtime::ExtensionIterator i = rntm->extensionsBegin(); i != rntm->extensionsEnd(); ++i )
    {
        SPTR( Extension ) extension( *i );
        OSLM_FATAL_IF( "Validation not ok for bundle = '" << extension->getBundle()->getIdentifier() << "'  (extension id = '" << extension->getIdentifier() << "' )",
                extension->getBundle()->isEnable() && extension->validate() == Extension::Invalid );
    }

    std::for_each( m_starters.begin(), m_starters.end(), Apply< StarterContainer::value_type >() );
    OSLM_TRACE( "NB INITIALIZERS" <<  m_initializers.size() );
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
    std::for_each( m_initializers.begin(), m_initializers.end(), Apply< InitializerContainer::value_type >() );
}

//------------------------------------------------------------------------------
void Profile::cleanup()
{
    std::for_each( m_uninitializers.rbegin(), m_uninitializers.rend(), Apply< UninitializerContainer::value_type >() );
    m_uninitializers.clear();
}

//------------------------------------------------------------------------------

Profile::ParamsContainer Profile::getParams()
{
    return m_params;
}

//------------------------------------------------------------------------------

char** Profile::getRawParams()
{
    // allocate memory for an array of character strings
    char** rawParams = new char*[m_params.size()];

    // for each string, allocate memory in the character array and copy
    for (unsigned long i=0; i<m_params.size(); i++)
    {
        int paramSize = m_params[i].size();
        rawParams[i] = new char[paramSize+1];
        strncpy(rawParams[i], m_params[i].c_str(), paramSize);
        rawParams[i][paramSize] = '\0';
    }
    return rawParams;
}

//------------------------------------------------------------------------------

void Profile::setParams(int argc, char** argv)
{
    m_params.clear();
    for(int i = 0; i < argc; i++)
    {
        std::string arg = argv[i];
        m_params.push_back( arg );
    }
}

//------------------------------------------------------------------------------

} // namespace profile

} // namespace fwRuntime
