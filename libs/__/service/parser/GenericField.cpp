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

#include "service/parser/GenericField.hpp"

#include "service/macros.hpp"

#include <data/Boolean.hpp>
#include <data/Float.hpp>
#include <data/GenericFieldBase.hpp>
#include <data/Integer.hpp>
#include <data/String.hpp>

namespace sight::service::parser
{

//------------------------------------------------------------------------------

void GenericField::updating()
{
    SIGHT_FATAL("This method is deprecated, and thus shouldn't be used.");
}

//------------------------------------------------------------------------------

void GenericField::createConfig(core::tools::object::sptr _obj)
{
    const auto input = m_cfg.get_optional<std::string>("value");

    if(input.has_value())
    {
        const auto field = std::dynamic_pointer_cast<data::GenericFieldBase>(_obj);
        SIGHT_ASSERT("GenericField not instanced", field);
        field->fromString(input.value());
    }
}

//------------------------------------------------------------------------------

} // namespace sight::service::parser
