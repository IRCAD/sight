/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "data/Point.hpp"

#include "data/Exception.hpp"
#include "data/registry/macros.hpp"
#include "data/String.hpp"

#include <core/base.hpp>

SIGHT_REGISTER_DATA(sight::data::Point);

namespace sight::data
{

// Note: for compatibility, use the same name as in MedicalImageHelper, to be changed in the future.
static constexpr std::string_view field_label_id = "m_labelId";

//------------------------------------------------------------------------------

Point::Point()
{
    m_vCoord[0] = 0.0;
    m_vCoord[1] = 0.0;
    m_vCoord[2] = 0.0;
}

//------------------------------------------------------------------------------

Point::Point(float x, float y, float z)
{
    m_vCoord[0] = x;
    m_vCoord[1] = y;
    m_vCoord[2] = z;
}

//------------------------------------------------------------------------------

Point::Point(double x, double y, double z)
{
    m_vCoord[0] = x;
    m_vCoord[1] = y;
    m_vCoord[2] = z;
}

//------------------------------------------------------------------------------

Point::Point(const PointCoordArrayType& coord) :
    m_vCoord(coord)
{
}

//------------------------------------------------------------------------------

Point::Point(const Point::sptr& p)
{
    m_vCoord[0] = p->m_vCoord[0];
    m_vCoord[1] = p->m_vCoord[1];
    m_vCoord[2] = p->m_vCoord[2];
}

//------------------------------------------------------------------------------

void Point::shallow_copy(const Object::csptr& source)
{
    const auto& other = std::dynamic_pointer_cast<const Point>(source);

    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Unable to copy " + (source ? source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_vCoord = other->m_vCoord;

    base_class::shallow_copy(other);
}

//------------------------------------------------------------------------------

void Point::deep_copy(const Object::csptr& source, const std::unique_ptr<DeepCopyCacheType>& cache)
{
    const auto& other = std::dynamic_pointer_cast<const Point>(source);

    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Unable to copy " + (source ? source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_vCoord = other->m_vCoord;

    base_class::deep_copy(other, cache);
}

//------------------------------------------------------------------------------

std::string Point::getLabel() const
{
    //TODO: In the future label should be an attribute of Point class, and should be serialized.
    std::string label;
    if(this->getField(std::string(field_label_id)))
    {
        label = this->getField<data::String>(std::string(field_label_id))->value();
    }

    return label;
}

//------------------------------------------------------------------------------

void Point::setLabel(const std::string& _label)
{
    //TODO: In the future label should be an attribute of Point class, and should be serialized.
    this->setField(std::string(field_label_id), std::make_shared<data::String>(_label));
}

//------------------------------------------------------------------------------

bool Point::operator==(const Point& other) const noexcept
{
    if(!core::tools::is_equal(m_vCoord, other.m_vCoord))
    {
        return false;
    }

    // Super class last
    return base_class::operator==(other);
}

//------------------------------------------------------------------------------

bool Point::operator!=(const Point& other) const noexcept
{
    return !(*this == other);
}

//------------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& out, const Point& p)
{
    out << "{";
    bool first = true;
    for(double e : p.getCoord())
    {
        if(!first)
        {
            out << ", ";
        }

        out << e;
        first = false;
    }

    return out << "}";
}

} // namespace sight::data
