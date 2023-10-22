/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "data/matrix4.hpp"

#include "data/exception.hpp"
#include "data/registry/macros.hpp"

SIGHT_REGISTER_DATA(sight::data::matrix4);

namespace sight::data
{

//------------------------------------------------------------------------------

matrix4::matrix4()
{
    *this = s_IDENTITY;
}

//------------------------------------------------------------------------------

matrix4::matrix4(std::initializer_list<value_type> _init_list)
{
    SIGHT_ASSERT("16 values should be provided to the constructor  of the matrix", _init_list.size() == this->size());
    std::copy(_init_list.begin(), _init_list.end(), this->begin());
}

//------------------------------------------------------------------------------
matrix4& matrix4::operator=(std::initializer_list<value_type> _init_list)
{
    SIGHT_ASSERT("16 values should be provided to the constructor  of the matrix", _init_list.size() == this->size());
    std::copy(_init_list.begin(), _init_list.end(), this->begin());
    return *this;
}

//-----------------------------------------------------------------------------

void matrix4::shallow_copy(const object::csptr& _source)
{
    const auto& other = std::dynamic_pointer_cast<const matrix4>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    base_class::shallow_copy(other);
}

//------------------------------------------------------------------------------

void matrix4::deep_copy(const object::csptr& _source, const std::unique_ptr<deep_copy_cache_t>& _cache)
{
    const auto& other = std::dynamic_pointer_cast<const matrix4>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    base_class::deep_copy(other, _cache);
}

//------------------------------------------------------------------------------

} // namespace sight::data
