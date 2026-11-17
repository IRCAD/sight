/************************************************************************
 *
 * Copyright (C) 2022-2026 IRCAD France
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

#include "series_set.hpp"

#include "data/registry/macros.hpp"

#include <algorithm>
#include <ranges>

SIGHT_REGISTER_DATA(sight::data::series_set);

namespace sight::data
{

//------------------------------------------------------------------------------

void series_set::shallow_copy(const object::csptr& _source)
{
    const auto& other = std::dynamic_pointer_cast<const series_set>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>")) + " to " + get_classname()
        ),
        !other
    );

    base_class_t::shallow_copy(other);
}

//------------------------------------------------------------------------------

bool series_set::operator==(const series_set& _other) const noexcept
{
    return base_class_t::operator==(_other);
}

//------------------------------------------------------------------------------

bool series_set::operator!=(const series_set& _other) const noexcept
{
    return base_class_t::operator!=(_other);
}

//------------------------------------------------------------------------------

std::size_t series_set::append_unique(const series_set& _source)
{
    std::size_t duplicate_count = 0;
    for(const auto& candidate : _source)
    {
        const std::size_t instance_count = candidate ? candidate->num_instances() : 0;
        const bool already_loaded        = instance_count > 0 && std::ranges::any_of(
            *this,
            [&candidate, instance_count](const series::sptr& _loaded)
            {
                return _loaded
                       && _loaded->get_classname() == candidate->get_classname()
                       && _loaded->num_instances() == instance_count
                       && std::ranges::all_of(
                    std::views::iota(std::size_t {0}, instance_count),
                    [&candidate, &_loaded](std::size_t _instance)
                {
                    return _loaded->get_file(_instance) == candidate->get_file(_instance);
                });
            });
        if(!already_loaded)
        {
            this->push_back(candidate);
        }
        else
        {
            ++duplicate_count;
        }
    }

    return duplicate_count;
}

//------------------------------------------------------------------------------

void series_set::deep_copy(const object::csptr& _source, const std::unique_ptr<deep_copy_cache_t>& _cache)
{
    const auto& other = std::dynamic_pointer_cast<const series_set>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>")) + " to " + get_classname()
        ),
        !other
    );

    base_class_t::deep_copy(other, _cache);
}

} // namespace sight::data
