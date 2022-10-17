/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "core/runtime/detail/ExtensionPoint.hpp"

#include "core/runtime/detail/io/Validator.hpp"
#include "core/runtime/Module.hpp"
#include "core/runtime/path.hpp"
#include "core/runtime/runtime.hpp"
#include "core/runtime/RuntimeException.hpp"

#include <utility>

namespace sight::core::runtime::detail
{

//------------------------------------------------------------------------------

ExtensionPoint::ExtensionPoint(
    const std::shared_ptr<core::runtime::Module> module,
    const std::string& id,
    std::filesystem::path schema
) :
    ModuleElement(module),
    m_id(filterID(id)),
    m_schema(std::move(schema))
{
}

//------------------------------------------------------------------------------

const std::string& ExtensionPoint::getIdentifier() const
{
    return m_id;
}

//------------------------------------------------------------------------------

std::shared_ptr<io::Validator> ExtensionPoint::getExtensionValidator() const
{
    if(!m_schema.empty() && !m_validator)
    {
        try
        {
            std::filesystem::path schemaPath = getModule()->getResourcesLocation() / m_schema;
            SIGHT_DEBUG("Use this schema : " << schemaPath << " for this id : " << m_id);
            if(!std::filesystem::exists(schemaPath))
            {
                // Allow to specify a schema defined elsewhere than this module
                schemaPath = core::runtime::getResourceFilePath(m_schema);
            }

            m_validator = std::make_shared<io::Validator>(schemaPath);
        }
        catch(const std::exception& e)
        {
            throw RuntimeException("Error while creating a validator. " + std::string(e.what()));
        }
    }

    return m_validator;
}

//------------------------------------------------------------------------------

} // namespace sight::core::runtime::detail
