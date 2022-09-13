/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

Matrix4::Matrix4() :
    m_vCoefficients(s_IDENTITY)
{
}

//------------------------------------------------------------------------------

Matrix4::Matrix4(data::Object::Key /*unused*/) :
    m_vCoefficients(s_IDENTITY)
{
}

//-----------------------------------------------------------------------------

void Matrix4::shallowCopy(const Object::csptr& source)
{
    const auto& other = dynamicConstCast(source);

    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Unable to copy " + (source ? source->getClassname() : std::string("<NULL>"))
            + " to " + getClassname()
        ),
        !bool(other)
    );

    m_vCoefficients = other->m_vCoefficients;

    BaseClass::shallowCopy(other);
}

//------------------------------------------------------------------------------

void Matrix4::deepCopy(const Object::csptr& source, const std::unique_ptr<DeepCopyCacheType>& cache)
{
    const auto& other = dynamicConstCast(source);

    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Unable to copy " + (source ? source->getClassname() : std::string("<NULL>"))
            + " to " + getClassname()
        ),
        !bool(other)
    );

    m_vCoefficients = other->m_vCoefficients;

    BaseClass::deepCopy(other, cache);
}

//------------------------------------------------------------------------------

bool Matrix4::operator==(const Matrix4& other) const noexcept
{
    if(!core::tools::is_equal(m_vCoefficients, other.m_vCoefficients))
    {
        return false;
    }

    // Super class last
    return BaseClass::operator==(other);
}

//------------------------------------------------------------------------------

bool Matrix4::operator!=(const Matrix4& other) const noexcept
{
    return !(*this == other);
}

} // namespace sight::data
