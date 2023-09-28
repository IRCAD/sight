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

#include "data/Matrix4.hpp"

#include "data/Exception.hpp"
#include "data/registry/macros.hpp"

SIGHT_REGISTER_DATA(sight::data::Matrix4);

namespace sight::data
{

//------------------------------------------------------------------------------

Matrix4::Matrix4()
{
    *this = s_IDENTITY;
}

//------------------------------------------------------------------------------

Matrix4::Matrix4(std::initializer_list<value_type> init_list)
{
    SIGHT_ASSERT("16 values should be provided to the constructor  of the matrix", init_list.size() == this->size());
    std::copy(init_list.begin(), init_list.end(), this->begin());
}

//------------------------------------------------------------------------------
Matrix4& Matrix4::operator=(std::initializer_list<value_type> init_list)
{
    SIGHT_ASSERT("16 values should be provided to the constructor  of the matrix", init_list.size() == this->size());
    std::copy(init_list.begin(), init_list.end(), this->begin());
    return *this;
}

//-----------------------------------------------------------------------------

void Matrix4::shallow_copy(const Object::csptr& source)
{
    const auto& other = std::dynamic_pointer_cast<const Matrix4>(source);

    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Unable to copy " + (source ? source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    base_class::shallow_copy(other);
}

//------------------------------------------------------------------------------

void Matrix4::deep_copy(const Object::csptr& source, const std::unique_ptr<DeepCopyCacheType>& cache)
{
    const auto& other = std::dynamic_pointer_cast<const Matrix4>(source);

    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Unable to copy " + (source ? source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    base_class::deep_copy(other, cache);
}

//------------------------------------------------------------------------------

} // namespace sight::data
