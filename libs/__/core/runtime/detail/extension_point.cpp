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

#include "core/runtime/detail/extension_point.hpp"

#include "core/runtime/detail/io/validator.hpp"
#include "core/runtime/module.hpp"
#include "core/runtime/path.hpp"
#include "core/runtime/runtime.hpp"
#include "core/runtime/runtime_exception.hpp"

#include <utility>

namespace sight::core::runtime::detail
{

//------------------------------------------------------------------------------

extension_point::extension_point(
    const std::shared_ptr<core::runtime::module> _module,
    const std::string& _id,
    std::filesystem::path _schema
) :
    module_element(_module),
    M_ID(filter_id(_id)),
    M_SCHEMA(std::move(_schema))
{
}

//------------------------------------------------------------------------------

const std::string& extension_point::identifier() const
{
    return M_ID;
}

//------------------------------------------------------------------------------

std::shared_ptr<io::validator> extension_point::get_extension_validator() const
{
    if(!M_SCHEMA.empty() && !m_validator)
    {
        try
        {
            std::filesystem::path schema_path = get_module()->get_resources_location() / M_SCHEMA;
            SIGHT_DEBUG("Use this schema : " << schema_path << " for this id : " << M_ID);
            if(!std::filesystem::exists(schema_path))
            {
                // Allow to specify a schema defined elsewhere than this module
                schema_path = core::runtime::get_resource_file_path(M_SCHEMA);
            }

            m_validator = std::make_shared<io::validator>(schema_path);
        }
        catch(const std::exception& e)
        {
            throw runtime_exception("Error while creating a validator. " + std::string(e.what()));
        }
    }

    return m_validator;
}

//------------------------------------------------------------------------------

} // namespace sight::core::runtime::detail
