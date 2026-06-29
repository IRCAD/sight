/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include "data/model_series.hpp"

#include <data/exception.hpp>
#include <data/reconstruction.hpp>
#include <data/registry/macros.hpp>

SIGHT_REGISTER_DATA(sight::data::model_series)

namespace sight::data
{

model_series::model_series() :
    has_fiducials(has_signals::signals())
{
    new_signal<signals::reconstructions_added_t>(signals::RECONSTRUCTIONS_ADDED);
    new_signal<signals::reconstructions_removed_t>(signals::RECONSTRUCTIONS_REMOVED);
}

//------------------------------------------------------------------------------

void model_series::shallow_copy(const object::csptr& _source)
{
    const auto& other = std::dynamic_pointer_cast<const model_series>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_reconstruction_db = other->m_reconstruction_db;

    base_class_t::shallow_copy(other);
}

//------------------------------------------------------------------------------

void model_series::deep_copy(const object::csptr& _source, const std::unique_ptr<deep_copy_cache_t>& _cache)
{
    const auto& other = std::dynamic_pointer_cast<const model_series>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_reconstruction_db.clear();
    for(const data::reconstruction::sptr& rec : other->m_reconstruction_db)
    {
        m_reconstruction_db.push_back(data::object::copy(rec, _cache));
    }

    base_class_t::deep_copy(other, _cache);
}

//------------------------------------------------------------------------------

bool model_series::operator==(const model_series& _other) const noexcept
{
    if(!core::is_equal(m_reconstruction_db, _other.m_reconstruction_db))
    {
        return false;
    }

    // Super class last
    return base_class_t::operator==(_other);
}

//------------------------------------------------------------------------------

bool model_series::operator!=(const model_series& _other) const noexcept
{
    return !(*this == _other);
}

} // namespace sight::data
