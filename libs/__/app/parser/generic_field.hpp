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

#include <sight/app/config.hpp>

#include <core/tools/failed.hpp>
#include <core/tools/object.hpp>

#include <service/object_parser.hpp>

namespace sight::app::parser
{

/**
 * @brief   Specific service for the construction of a generic_field and its associated services from an XML-based
 * description.
 * @see     service::object_parser
 */
class SIGHT_APP_CLASS_API generic_field : public service::object_parser
{
public:

    SIGHT_DECLARE_SERVICE(generic_field, service::object_parser);

    /// Constructor : does nothing.
    generic_field()
    = default;

    /// Destructor : does nothing.
    ~generic_field() override
    = default;

    SIGHT_APP_API void create_config(core::tools::object::sptr _obj) override;

protected:

    /**
     * @brief Updating method : create the process object.
     *
     * Parse the configuration element to configure inputs and outputs and add
     * them in the process object.
     */
    SIGHT_APP_API void updating() override;
};

} // namespace sight::app::parser
