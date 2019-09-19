/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#pragma once

#include "fwRuntime/config.hpp"

#include <fwCore/base.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/make_shared.hpp>
#include <boost/utility.hpp>

#include <functional>
#include <vector>

namespace fwRuntime
{

/**
 * @brief       Namespace ::fwRuntime::profile
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
 */
class Profile : public ::fwCore::BaseObject
{
public:

    typedef std::vector< std::string >  ParamsContainer;
    typedef std::function< int () > RunCallbackType;

    fwCoreClassMacro(Profile, BaseObject, std::make_shared< Profile >);

    /**
     * @brief   Constructor : does nothing.
     */
    FWRUNTIME_API Profile();

    FWRUNTIME_API ~Profile();

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
     * @brief       Adds a new stopper.
     *
     * @param[in]   stopper a shared pointer to a stopper
     */
    FWRUNTIME_API void add( SPTR( Stopper ) stopper );

    /**
     * @brief       Adds a new starter.
     *
     * @param[in]   initializer a shared pointer to an initializer
     */
    FWRUNTIME_API void add( SPTR( Initializer ) initializer );

    /**
     * @brief       Adds a new starter.
     *
     * @param[in]   uninitializer a shared pointer to an uninitializer
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
    std::string getName() const
    {
        return m_sName;
    }

    /**
     * @brief       Set profile name.
     *
     * @param[in]   _sName  profile name
     */
    void setName(std::string _sName)
    {
        m_sName = _sName;
    }

    /// Get profile m_filePath
    ::boost::filesystem::path getFilePath() const
    {
        return m_filePath;
    }

    /// Set profile m_filePath
    void setFilePath(const ::boost::filesystem::path& _filePath)
    {
        m_filePath = _filePath;
    }

    /**
     * @brief   Return profile version.
     */
    std::string getVersion() const
    {
        return m_sVersion;
    }

    /**
     * @brief       Set profile version.
     *
     * @param[in]   _sVersion   profile version
     */
    void setVersion(std::string _sVersion)
    {
        m_sVersion = _sVersion;
    }

    /**
     * @brief   Return profile CheckSingleInstance.
     */
    bool getCheckSingleInstance() const
    {
        return m_checkSingleInstance;
    }

    /**
     * @brief       Set profile CheckSingleInstance.
     *
     * @param[in]   _checkSingleInstance    profile CheckSingleInstance
     */
    void setCheckSingleInstance(bool _checkSingleInstance)
    {
        m_checkSingleInstance = _checkSingleInstance;
    }

    //------------------------------------------------------------------------------

    ParamsContainer getParams() const
    {
        return m_params;
    }

    FWRUNTIME_API void setParams(const ParamsContainer& params);
    FWRUNTIME_API void setParams(int argc, char** argv);

    /**
     * @brief Returns internal arg count.
     * The returned int shall not be modified. This is provided for external
     * library needs (QApplication contructor for example)
     */
    int& getRawArgCount()
    {
        return m_argc;
    }

    /**
     * @brief Returns a raw pointer on internal arguments.
     * The returned data shall not be modified. This is provided for external
     * library needs (QApplication contructor for example)
     */
    char** getRawParams() const
    {
        return m_argv;
    }

private:

    typedef std::vector< SPTR(Activater) >     ActivaterContainer;
    typedef std::vector< SPTR(Starter) >       StarterContainer;
    typedef std::vector< SPTR(Stopper) >       StopperContainer;
    typedef std::vector< SPTR(Initializer) >   InitializerContainer;
    typedef std::vector< SPTR(Uninitializer) > UninitializerContainer;

    ActivaterContainer m_activaters;            ///< all managed activators
    StarterContainer m_starters;                ///< all managed starters
    StopperContainer m_stoppers;                ///< all managed stoppers
    InitializerContainer m_initializers;        ///< all managed initializers
    UninitializerContainer m_uninitializers;    ///< all managed uninitializers

    std::string m_sName;                    ///< name profile
    std::string m_sVersion;                 ///< profile app version
    ::boost::filesystem::path m_filePath;   ///< xml parsed file used to generate profile

    bool m_checkSingleInstance;
    RunCallbackType m_run;

    ParamsContainer m_params;
    int m_argc;
    char**          m_argv;
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
