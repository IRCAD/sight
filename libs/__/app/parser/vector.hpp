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

#include "app/config.hpp"

#include <app/config_manager.hpp>

#include <service/object_parser.hpp>

namespace sight::app::parser
{

/**
 * @brief   Specific service for the construction of a vector and its associated services from an XML-based description.
 * @see     service::object_parser
 */
class APP_CLASS_API vector : public service::object_parser
{
public:

    SIGHT_DECLARE_SERVICE(vector, service::object_parser);

    vector()           = default;
    ~vector() override = default;

    APP_API void create_config(core::tools::object::sptr _obj) override;

    APP_API void start_config() override;

    APP_API void update_config() override;

    APP_API void stop_config() override;

    APP_API void destroy_config() override;

protected:

    /**
     * @brief   Updating method : create vector object.
     *
     * Parse the vector configuration element to configure and add its objects.
     */
    APP_API void updating() override;

private:

    std::vector<app::config_manager::sptr> m_ctm_container;
};

} // namespace sight::app::parser
