/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWRUNTIME_PROFILE_PROFILE_HPP_
#define _FWRUNTIME_PROFILE_PROFILE_HPP_

#include <vector>

#include <boost/function.hpp>
#include <boost/utility.hpp>

#include "fwCore/base.hpp"

#include "fwRuntime/config.hpp"


namespace fwRuntime
{

/**
 * @namespace   ::fwRuntime::profile
 * @brief       Namespace ::fwRuntime::profile
 * @date    2007-2009
 * @author  IRCAD (Research and Development Team).
 */
namespace profile
{

class Activater;
class Starter;
class Stopper;
class Initializer;
class Uninitializer;


/**
 * @brief   Implements a bundle set profile.
 * @class  Profile
 * @date    2007-2009
 * @author  IRCAD (Research and Development Team).
 */
class Profile : public ::fwCore::BaseObject
{
public:

    typedef std::vector< std::string >  ParamsContainer;
    typedef ::boost::function< int () > RunCallbackType;

    fwCoreClassDefinitionsWithFactoryMacro( (Profile)(BaseObject), (()), new Profile) ;

    /**
     * @brief   Constructor : does nothing.
     */
    FWRUNTIME_API Profile();

    /**
     * @brief       Adds a new activator.
     *
     * @param[in]   activater   a shared pointer to an activator
     */
    FWRUNTIME_API void add( SPTR( Activater ) activater );

    /**
     * @brief       Adds a new starter.
     *
     * @param[in]   starter a shared pointer to a starter
     */
    FWRUNTIME_API void add( SPTR( Starter ) starter );

    /**
     * @brief       Adds a new starter.
     *
     * @param[in]   starter a shared pointer to a starter
     */
    FWRUNTIME_API void add( SPTR( Initializer ) initializer );

    /**
     * @brief       Adds a new starter.
     *
     * @param[in]   starter a shared pointer to a starter
     */
    FWRUNTIME_API void add( SPTR( Uninitializer ) uninitializer );

    /**
     * @brief   Starts the profile.
     */
    FWRUNTIME_API void start();
    FWRUNTIME_API void stop();


    /**
     * @brief   Run the profile.
     */
    FWRUNTIME_API int run();
    FWRUNTIME_API int defaultRun();
    FWRUNTIME_API void setRunCallback(RunCallbackType callback);


    /**
     * @brief   Once started, setup the profile.
     */
    FWRUNTIME_API void setup();
    FWRUNTIME_API void cleanup();

    /**
     * @brief   Return profile name.
     */
    FWRUNTIME_API std::string getName() { return m_sName; }

    /**
     * @brief       Set profile name.
     *
     * @param[in]   _sName  profile name
     */
    FWRUNTIME_API void setName(std::string _sName) { m_sName = _sName; }


    /**
     * @brief   Return profile version.
     */
    FWRUNTIME_API std::string getVersion() { return m_sVersion; }

    /**
     * @brief       Set profile version.
     *
     * @param[in]   _sVersion   profile version
     */
    FWRUNTIME_API void setVersion(std::string _sVersion) { m_sVersion = _sVersion; }



    /**
     * @brief   Return profile CheckSingleInstance.
     */
    FWRUNTIME_API bool getCheckSingleInstance() { return m_checkSingleInstance; }

    /**
     * @brief       Set profile CheckSingleInstance.
     *
     * @param[in]   _checkSingleInstance    profile CheckSingleInstance
     */
    FWRUNTIME_API void setCheckSingleInstance(bool _checkSingleInstance) { m_checkSingleInstance = _checkSingleInstance; }

    FWRUNTIME_API ParamsContainer getParams();

    FWRUNTIME_API void setParams(int argc, char** argv);

    FWRUNTIME_API char** getRawParams();

private:

    typedef std::vector< SPTR(Activater) >     ActivaterContainer;
    typedef std::vector< SPTR(Starter) >       StarterContainer;
    typedef std::vector< SPTR(Stopper) >       StopperContainer;
    typedef std::vector< SPTR(Initializer) >   InitializerContainer;
    typedef std::vector< SPTR(Uninitializer) > UninitializerContainer;

    ActivaterContainer        m_activaters;     ///< all managed activators
    StarterContainer          m_starters;       ///< all managed starters
    StopperContainer          m_stoppers;       ///< all managed stoppers
    InitializerContainer      m_initializers;   ///< all managed initializers
    UninitializerContainer    m_uninitializers; ///< all managed uninitializers

    std::string         m_sName;        ///< name profile
    std::string         m_sVersion;     ///< profile app version
    bool                m_checkSingleInstance;
    ParamsContainer     m_params;

    RunCallbackType m_run;
};


/**
 * @brief       Set current profile.
 *
 * @param       prof profile
 */
FWRUNTIME_API void setCurrentProfile(Profile::sptr prof);

/**
 * @brief       Get current profile.
 */
FWRUNTIME_API Profile::sptr getCurrentProfile();

} // namespace profile

} // namespace fwRuntime




#endif /*_FWRUNTIME_PROFILE_PROFILE_HPP_*/
