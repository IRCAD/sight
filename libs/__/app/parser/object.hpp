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
 * @brief Specific core::tools::object parser. Does nothing as no base method is specialized: object_parser base class
 *  methods are therefore considered.
 *
 * @note This implementation is only written to avoid the mix between the service type object_parser and the
 * core::tools::object implementation (for homogeneity).
 *
 */
class APP_CLASS_API object : public service::object_parser
{
public:

    SIGHT_DECLARE_SERVICE(object, service::object_parser);

    APP_API object()           = default;
    APP_API ~object() override = default;

    APP_API void createConfig(core::tools::object::sptr _obj) override;
    APP_API void startConfig() override;
    APP_API void updateConfig() override;
    APP_API void stopConfig() override;
    APP_API void destroyConfig() override;

protected:

    void updating() override;

    std::vector<app::config_manager::sptr> m_ctmContainer;
};

} // namespace sight::app::parser
