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

#pragma once

#include "fwRuntime/config.hpp"
#include "fwRuntime/Profile.hpp"

#include <filesystem>
#include <functional>
#include <vector>

namespace fwRuntime
{

namespace impl
{

namespace profile
{

class Activater;
class Starter;
class Stopper;
class Initializer;
class Uninitializer;

/**
 * @brief   Implements a module set profile.
 */
class Profile : public ::fwRuntime::Profile
{
public:

    typedef std::vector< std::string >  ParamsContainer;
    typedef std::function< int () > RunCallbackType;

    fwCoreClassMacro(Profile, BaseObject, std::make_shared< Profile >)

    /**
     * @brief   Constructor : does nothing.
     */
    Profile();

    virtual ~Profile();

    /**
     * @brief       Adds a new activator.
     *
     * @param[in]   activater   a shared pointer to an activator
     */
    void add( SPTR( Activater ) activater );

    /**
     * @brief       Adds a new starter.
     *
     * @param[in]   starter a shared pointer to a starter
     */
    void add( SPTR( Starter ) starter );

    /**
     * @brief       Adds a new stopper.
     *
     * @param[in]   stopper a shared pointer to a stopper
     */
    void add( SPTR( Stopper ) stopper );

    /**
     * @brief       Adds a new starter.
     *
     * @param[in]   initializer a shared pointer to an initializer
     */
    void add( SPTR( Initializer ) initializer );

    /**
     * @brief       Adds a new starter.
     *
     * @param[in]   uninitializer a shared pointer to an uninitializer
     */
    void add( SPTR( Uninitializer ) uninitializer );

    /**
     * @brief   Starts the profile.
     */
    virtual void start() final;
    virtual void stop() final;

    /**
     * @brief   Once started, setup the profile.
     */
    virtual void setup() final;
    virtual void cleanup() final;

    /**
     * @brief   Run the profile.
     */
    virtual int run() final;
    virtual void setRunCallback(RunCallbackType callback) final;

    int defaultRun();

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

    RunCallbackType m_run;
};

/**
 * @brief       Set current profile.
 *
 * @param       prof profile
 */
void setCurrentProfile(Profile::sptr prof);

/**
 * @brief       Get current profile.
 */
Profile::sptr getCurrentProfile();

} // namespace profile

} // namespace impl

} // namespace fwRuntime
