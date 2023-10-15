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

#include "data/raw_buffer_tl.hpp"

#include "data/timeline/raw_buffer.hpp"

#include <data/exception.hpp>
#include <data/registry/macros.hpp>

#include <functional>

SIGHT_REGISTER_DATA(sight::data::raw_buffer_tl);

namespace sight::data
{

//------------------------------------------------------------------------------

void raw_buffer_tl::shallow_copy(const object::csptr& /*source*/)
{
    SIGHT_FATAL("shallow_copy not implemented for : " + this->get_classname());
}

//------------------------------------------------------------------------------

void raw_buffer_tl::deep_copy(const object::csptr& source, const std::unique_ptr<deep_copy_cache_t>& cache)
{
    const auto& other = std::dynamic_pointer_cast<const raw_buffer_tl>(source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (source ? source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    this->clearTimeline();
    this->allocPoolSize(other->m_pool->get_requested_size());

    for(const TimelineType::value_type& elt : other->m_timeline)
    {
        SPTR(data::timeline::raw_buffer) tlObj = this->createBuffer(elt.first);
        tlObj->deep_copy(*elt.second);
        m_timeline.insert(TimelineType::value_type(elt.first, tlObj));
    }

    base_class::deep_copy(other, cache);
}

//------------------------------------------------------------------------------

CSPTR(data::timeline::raw_buffer) raw_buffer_tl::getClosestBuffer(
    core::hires_clock::type timestamp,
    timeline::direction_t direction
) const
{
    CSPTR(data::timeline::object) buffer = this->getClosestObject(timestamp, direction);
    return std::dynamic_pointer_cast<const data::timeline::raw_buffer>(buffer);
}

//------------------------------------------------------------------------------

CSPTR(data::timeline::raw_buffer) raw_buffer_tl::get_buffer(core::hires_clock::type timestamp) const
{
    CSPTR(data::timeline::object) buffer = this->getObject(timestamp);
    return std::dynamic_pointer_cast<const data::timeline::raw_buffer>(buffer);
}

//------------------------------------------------------------------------------

void raw_buffer_tl::initPoolSize(std::size_t size)
{
    this->allocPoolSize(size);
}

//------------------------------------------------------------------------------

SPTR(data::timeline::object) raw_buffer_tl::createObject(core::hires_clock::type timestamp)
{
    return this->createBuffer(timestamp);
}

//------------------------------------------------------------------------------

SPTR(data::timeline::raw_buffer) raw_buffer_tl::createBuffer(core::hires_clock::type timestamp)
{
    return std::make_shared<data::timeline::raw_buffer>(
        timestamp,
        reinterpret_cast<data::timeline::buffer::BufferDataType>(m_pool->malloc()),
        m_pool->get_requested_size(),
        [ObjectPtr = m_pool](auto&& PH1, auto&& ...){ObjectPtr->free(std::forward<decltype(PH1)>(PH1));});
}

//------------------------------------------------------------------------------

bool raw_buffer_tl::isObjectValid(const CSPTR(data::timeline::object)& obj) const
{
    CSPTR(data::timeline::raw_buffer) srcObj =
        std::dynamic_pointer_cast<const data::timeline::raw_buffer>(obj);
    return srcObj != nullptr;
}

//------------------------------------------------------------------------------

} // namespace sight::data
