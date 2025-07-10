/************************************************************************
 *
 * Copyright (C) 2024-2025 IRCAD France
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

#include <sight/app/config.hpp>

#include "service/base.hpp"

namespace sight::app
{

/**
 * @brief   This interface defines control service API.
 * Does nothing particularly, can be considered as a default service type to be implemented by unclassified services.
 */
class SIGHT_APP_CLASS_API updater : public service::base
{
public:

    SIGHT_DECLARE_CLASS(updater, service::base);

protected:

    enum class type_t : std::uint8_t
    {
        SERVICE,
        UPDATER
    };

    struct update_element_t
    {
        std::string uid;
        std::string slot;
        type_t type {type_t::SERVICE};
        bool ignore_stopped {false};
    };

    SIGHT_APP_API ~updater() override = default;

    /// Does nothing
    SIGHT_APP_API void configuring(const config_t& _config) final;

    /// Keep track of received signals
    std::vector<update_element_t> m_elements;
    /// Keep track of received signals
    std::vector<std::string> m_services;
    std::string m_parent;

    bool m_loop {false};
    bool m_run_loop {false};
};

} // namespace sight::app
