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

#pragma once

#include "core/config.hpp"
#include "core/runtime/profile.hpp"

#include <filesystem>
#include <functional>
#include <map>
#include <vector>
namespace sight::core::runtime::detail::profile
{

class activater;
class starter;

/**
 * @brief   Implements a module set profile.
 */
class profile : public core::runtime::profile
{
public:

    typedef std::vector<std::string> params_container;
    typedef std::function<int ()> run_callback_type;

    SIGHT_DECLARE_CLASS(profile, base_object);

    /**
     * @brief   Constructor : does nothing.
     */
    profile();

    ~profile() override;

    /**
     * @brief       Adds a new activator.
     *
     * @param[in]   activater   a shared pointer to an activator
     */
    void add(SPTR(activater) activater);

    /**
     * @brief       Adds a new starter.
     *
     * @param[in]   starter a shared pointer to a starter
     */
    void add_starter(const std::string& identifier);

    /**
     * @brief       Adds a new stopper.
     *
     * @param[in]   stopper a shared pointer to a stopper
     */
    void add_stopper(const std::string& identifier, int priority);

    /**
     * @brief   Starts the profile.
     */
    void start() final;
    void stop() final;

    /**
     * @brief   Run the profile.
     */
    int run() final;
    void set_run_callback(run_callback_type callback) final;

    static int default_run();

    /**
     * @brief   Return profile CheckSingleInstance.
     */
    bool get_check_single_instance() const
    {
        return m_check_single_instance;
    }

    /**
     * @brief       Set profile CheckSingleInstance.
     *
     * @param[in]   _checkSingleInstance    profile CheckSingleInstance
     */
    void set_check_single_instance(bool _check_single_instance)
    {
        m_check_single_instance = _check_single_instance;
    }

private:

    typedef std::vector<SPTR(activater)> activater_container;
    typedef std::multimap<int, std::string> starter_container;
    typedef std::multimap<int, std::string> stopper_container;

    activater_container m_activaters; ///< all managed activators
    starter_container m_starters;     ///< all managed starters
    stopper_container m_stoppers;     ///< all managed stoppers

    run_callback_type m_run;

    bool m_check_single_instance {false};
};

/**
 * @brief       Set current profile.
 *
 * @param       prof profile
 */
void set_current_profile(profile::sptr prof);

/**
 * @brief       Get current profile.
 */
profile::sptr get_current_profile();

} // namespace sight::core::runtime::detail::profile
