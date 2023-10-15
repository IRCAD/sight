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

#include "data/resection_db.hpp"

#include "data/exception.hpp"
#include "data/registry/macros.hpp"

#include <core/base.hpp>
#include <core/com/signal.hpp>
#include <core/com/signal.hxx>

SIGHT_REGISTER_DATA(sight::data::resection_db);

namespace sight::data
{

const core::com::signals::key_t resection_db::RESECTION_ADDED_SIG = "resectionAdded";
const core::com::signals::key_t resection_db::SAFE_PART_ADDED_SIG = "safePartAdded";

//------------------------------------------------------------------------------

resection_db::resection_db() :
    m_sigSafePartAdded(std::make_shared<SafePartAddedSignalType>()),
    m_sigResectionAdded(std::make_shared<ResectionAddedSignalType>())
{
    m_signals(RESECTION_ADDED_SIG, m_sigResectionAdded)
        (SAFE_PART_ADDED_SIG, m_sigSafePartAdded);
}

//------------------------------------------------------------------------------

void resection_db::shallow_copy(const object::csptr& source)
{
    const auto& other = std::dynamic_pointer_cast<const resection_db>(source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (source ? source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_safeResection = other->m_safeResection;
    m_resections    = other->m_resections;

    base_class::shallow_copy(other);
}

//------------------------------------------------------------------------------

void resection_db::deep_copy(const object::csptr& source, const std::unique_ptr<deep_copy_cache_t>& cache)
{
    const auto& other = std::dynamic_pointer_cast<const resection_db>(source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (source ? source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_safeResection = data::object::copy(other->m_safeResection, cache);

    m_resections.clear();
    for(const auto& resection : other->m_resections)
    {
        m_resections.push_back(data::object::copy(resection, cache));
    }

    base_class::deep_copy(other, cache);
}

//------------------------------------------------------------------------------

resection_db::ResectionContainerType::size_type resection_db::numResections() const
{
    return m_resections.size();
}

//------------------------------------------------------------------------------

void resection_db::addResection(const data::resection::sptr& resection)
{
    m_resections.push_back(resection);
}

//------------------------------------------------------------------------------

bool resection_db::operator==(const resection_db& other) const noexcept
{
    if(!core::tools::is_equal(m_safeResection, other.m_safeResection)
       || !core::tools::is_equal(m_resections, other.m_resections))
    {
        return false;
    }

    // Super class last
    return base_class::operator==(other);
}

//------------------------------------------------------------------------------

bool resection_db::operator!=(const resection_db& other) const noexcept
{
    return !(*this == other);
}

} // end namespace sight::data
