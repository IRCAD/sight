/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "core/config.hpp"
#include "core/runtime/Profile.hpp"

#include <filesystem>
#include <functional>
#include <vector>

namespace sight::core::runtime
{

namespace detail
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
class Profile : public core::runtime::Profile
{
public:

    typedef std::vector<std::string> ParamsContainer;
    typedef std::function<int ()> RunCallbackType;

    SIGHT_DECLARE_CLASS(Profile, BaseObject, std::make_shared<Profile>);

    /**
     * @brief   Constructor : does nothing.
     */
    Profile();

    ~Profile() override;

    /**
     * @brief       Adds a new activator.
     *
     * @param[in]   activater   a shared pointer to an activator
     */
    void add(SPTR(Activater) activater);

    /**
     * @brief       Adds a new starter.
     *
     * @param[in]   starter a shared pointer to a starter
     */
    void add(SPTR(Starter) starter);

    /**
     * @brief       Adds a new stopper.
     *
     * @param[in]   stopper a shared pointer to a stopper
     */
    void add(SPTR(Stopper) stopper);

    /**
     * @brief       Adds a new starter.
     *
     * @param[in]   initializer a shared pointer to an initializer
     */
    void add(SPTR(Initializer) initializer);

    /**
     * @brief       Adds a new starter.
     *
     * @param[in]   uninitializer a shared pointer to an uninitializer
     */
    void add(SPTR(Uninitializer) uninitializer);

    /**
     * @brief   Starts the profile.
     */
    void start() final;
    void stop() final;

    /**
     * @brief   Once started, setup the profile.
     */
    void setup() final;
    void cleanup() final;

    /**
     * @brief   Run the profile.
     */
    int run() final;
    void setRunCallback(RunCallbackType callback) final;

    int defaultRun();

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

private:

    typedef std::vector<SPTR(Activater)> ActivaterContainer;
    typedef std::vector<SPTR(Starter)> StarterContainer;
    typedef std::vector<SPTR(Stopper)> StopperContainer;
    typedef std::vector<SPTR(Initializer)> InitializerContainer;
    typedef std::vector<SPTR(Uninitializer)> UninitializerContainer;

    ActivaterContainer m_activaters;         ///< all managed activators
    StarterContainer m_starters;             ///< all managed starters
    StopperContainer m_stoppers;             ///< all managed stoppers
    InitializerContainer m_initializers;     ///< all managed initializers
    UninitializerContainer m_uninitializers; ///< all managed uninitializers

    RunCallbackType m_run;

    bool m_checkSingleInstance;
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

} // namespace detail

} // namespace sight::core::runtime
