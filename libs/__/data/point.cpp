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

#include "data/point.hpp"

#include "data/exception.hpp"
#include "data/registry/macros.hpp"
#include "data/string.hpp"

#include <core/base.hpp>

SIGHT_REGISTER_DATA(sight::data::point);

namespace sight::data
{

// Note: for compatibility, use the same name as in MedicalImageHelper, to be changed in the future.
static constexpr std::string_view field_label_id = "m_labelId";

//------------------------------------------------------------------------------

point::point()
{
    m_vCoord[0] = 0.0;
    m_vCoord[1] = 0.0;
    m_vCoord[2] = 0.0;
}

//------------------------------------------------------------------------------

point::point(float _x, float _y, float _z)
{
    m_vCoord[0] = _x;
    m_vCoord[1] = _y;
    m_vCoord[2] = _z;
}

//------------------------------------------------------------------------------

point::point(double _x, double _y, double _z)
{
    m_vCoord[0] = _x;
    m_vCoord[1] = _y;
    m_vCoord[2] = _z;
}

//------------------------------------------------------------------------------

point::point(const point_coord_array_t& _coord) :
    m_vCoord(_coord)
{
}

//------------------------------------------------------------------------------

point::point(const point::sptr& _p)
{
    m_vCoord[0] = _p->m_vCoord[0];
    m_vCoord[1] = _p->m_vCoord[1];
    m_vCoord[2] = _p->m_vCoord[2];
}

//------------------------------------------------------------------------------

void point::shallow_copy(const object::csptr& _source)
{
    const auto& other = std::dynamic_pointer_cast<const point>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_vCoord = other->m_vCoord;

    base_class::shallow_copy(other);
}

//------------------------------------------------------------------------------

void point::deep_copy(const object::csptr& _source, const std::unique_ptr<deep_copy_cache_t>& _cache)
{
    const auto& other = std::dynamic_pointer_cast<const point>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_vCoord = other->m_vCoord;

    base_class::deep_copy(other, _cache);
}

//------------------------------------------------------------------------------

std::string point::getLabel() const
{
    //TODO: In the future label should be an attribute of point class, and should be serialized.
    std::string label;
    if(this->get_field(std::string(field_label_id)))
    {
        label = this->get_field<data::string>(std::string(field_label_id))->value();
    }

    return label;
}

//------------------------------------------------------------------------------

void point::setLabel(const std::string& _label)
{
    //TODO: In the future label should be an attribute of point class, and should be serialized.
    this->set_field(std::string(field_label_id), std::make_shared<data::string>(_label));
}

//------------------------------------------------------------------------------

bool point::operator==(const point& _other) const noexcept
{
    if(!core::tools::is_equal(m_vCoord, _other.m_vCoord))
    {
        return false;
    }

    // Super class last
    return base_class::operator==(_other);
}

//------------------------------------------------------------------------------

bool point::operator!=(const point& _other) const noexcept
{
    return !(*this == _other);
}

//------------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& _out, const point& _p)
{
    _out << "{";
    bool first = true;
    for(double e : _p.getCoord())
    {
        if(!first)
        {
            _out << ", ";
        }

        _out << e;
        first = false;
    }

    return _out << "}";
}

} // namespace sight::data
