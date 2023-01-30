/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

Point::Point(data::Object::Key /*unused*/)
{
    m_vCoord[0] = 0.0;
    m_vCoord[1] = 0.0;
    m_vCoord[2] = 0.0;
}

//------------------------------------------------------------------------------

Point::sptr Point::New(float x, float y, float z)
{
    Point::sptr point = data::Point::New();
    point->m_vCoord[0] = x;
    point->m_vCoord[1] = y;
    point->m_vCoord[2] = z;
    return point;
}

//------------------------------------------------------------------------------

Point::sptr Point::New(double x, double y, double z)
{
    Point::sptr point = data::Point::New();
    point->m_vCoord[0] = x;
    point->m_vCoord[1] = y;
    point->m_vCoord[2] = z;
    return point;
}

//------------------------------------------------------------------------------

Point::sptr Point::New(const PointCoordArrayType& coord)
{
    Point::sptr point = data::Point::New();
    point->m_vCoord = coord;
    return point;
}

//------------------------------------------------------------------------------

Point::sptr Point::New(const Point::sptr& p)
{
    Point::sptr point = data::Point::New();
    point->m_vCoord[0] = p->m_vCoord[0];
    point->m_vCoord[1] = p->m_vCoord[1];
    point->m_vCoord[2] = p->m_vCoord[2];
    return point;
}

//------------------------------------------------------------------------------

void Point::shallowCopy(const Object::csptr& source)
{
    const auto& other = dynamicConstCast(source);

    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Unable to copy " + (source ? source->getClassname() : std::string("<NULL>"))
            + " to " + getClassname()
        ),
        !bool(other)
    );

    m_vCoord = other->m_vCoord;

    BaseClass::shallowCopy(other);
}

//------------------------------------------------------------------------------

void Point::deepCopy(const Object::csptr& source, const std::unique_ptr<DeepCopyCacheType>& cache)
{
    const auto& other = dynamicConstCast(source);

    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Unable to copy " + (source ? source->getClassname() : std::string("<NULL>"))
            + " to " + getClassname()
        ),
        !bool(other)
    );

    m_vCoord = other->m_vCoord;

    BaseClass::deepCopy(other, cache);
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
    this->setField(std::string(field_label_id), data::String::New(_label));
}

//------------------------------------------------------------------------------

bool Point::operator==(const Point& other) const noexcept
{
    if(!core::tools::is_equal(m_vCoord, other.m_vCoord))
    {
        return false;
    }

    // Super class last
    return BaseClass::operator==(other);
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
