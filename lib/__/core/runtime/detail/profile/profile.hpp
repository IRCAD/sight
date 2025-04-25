/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <sight/core/config.hpp>

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

    using params_container  = std::vector<std::string>;
    using run_callback_type = std::function<int ()>;

    /**
     * @brief   Constructor : does nothing.
     */
    profile();

    ~profile() override;

    /**
     * @brief       Adds a new activator.
     *
     * @param[in]   _activater   a shared pointer to an activator
     */
    void add(SPTR(activater) _activater);

    /**
     * @brief       Adds a new starter.
     *
     * @param[in]   _starterIdentifier  identifier of the starter to add
     */
    void add_starter(const std::string& _starterIdentifier);

    /**
     * @brief       Adds a new stopper.
     *
     * @param[in]   _stopperIdentifier identifier of the stopper to add
     */
    void add_stopper(const std::string& _stopperIdentifier, int _priority);

    /**
     * @brief   Starts the profile.
     */
    void start() final;
    void stop() final;

    /**
     * @brief   Run the profile.
     */
    int run() final;
    void set_run_callback(run_callback_type _callback) final;

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
     * @param[in]   _check_single_instance    profile CheckSingleInstance
     */
    void set_check_single_instance(bool _check_single_instance)
    {
        m_check_single_instance = _check_single_instance;
    }

private:

    using activater_container = std::vector<std::shared_ptr<activater> >;
    using starter_container   = std::multimap<int, std::string>;
    using stopper_container   = std::multimap<int, std::string>;

    activater_container m_activaters; ///< all managed activators
    starter_container m_starters;     ///< all managed starters
    stopper_container m_stoppers;     ///< all managed stoppers

    run_callback_type m_run;

    bool m_check_single_instance {false};
};

/**
 * @brief       Set current profile.
 *
 * @param       _prof profile
 */
void set_current_profile(profile::sptr _prof);

/**
 * @brief       Get current profile.
 */
profile::sptr get_current_profile();

} // namespace sight::core::runtime::detail::profile
