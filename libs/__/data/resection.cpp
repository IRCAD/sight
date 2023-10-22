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

#include "data/resection.hpp"

#include "data/exception.hpp"
#include "data/registry/macros.hpp"

#include <core/base.hpp>
#include <core/com/signal.hpp>
#include <core/com/signal.hxx>

SIGHT_REGISTER_DATA(sight::data::resection);

namespace sight::data
{

const core::com::signals::key_t resection::RECONSTRUCTION_ADDED_SIG = "reconstructionAdded";
const core::com::signals::key_t resection::VISIBILITY_MODIFIED_SIG  = "pointTexCoordsModified";

//------------------------------------------------------------------------------

resection::resection() :
    m_planeList(std::make_shared<data::plane_list>()),
    m_sigReconstructionAdded(std::make_shared<reconstruction_added_signal_t>()),
    m_sigVisibilityModified(std::make_shared<visibility_modified_signal_t>())
{
    m_signals(RECONSTRUCTION_ADDED_SIG, m_sigReconstructionAdded)
        (VISIBILITY_MODIFIED_SIG, m_sigVisibilityModified);
}

//------------------------------------------------------------------------------

void resection::shallow_copy(const object::csptr& _source)
{
    const auto& other = std::dynamic_pointer_cast<const resection>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_name       = other->m_name;
    m_isSafePart = other->m_isSafePart;
    m_isValid    = other->m_isValid;
    m_isVisible  = other->m_isVisible;
    m_planeList  = other->m_planeList;
    m_vInputs    = other->m_vInputs;
    m_vOutputs   = other->m_vOutputs;

    base_class::shallow_copy(other);
}

//------------------------------------------------------------------------------

void resection::deep_copy(const object::csptr& _source, const std::unique_ptr<deep_copy_cache_t>& _cache)
{
    const auto& other = std::dynamic_pointer_cast<const resection>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_name       = other->m_name;
    m_isSafePart = other->m_isSafePart;
    m_isValid    = other->m_isValid;
    m_isVisible  = other->m_isVisible;
    m_planeList  = data::object::copy(other->m_planeList, _cache);

    this->m_vInputs.clear();
    for(const ResectionInputs::value_type& r : other->m_vInputs)
    {
        m_vInputs.push_back(data::object::copy(r, _cache));
    }

    this->m_vOutputs.clear();
    for(const ResectionOutputs::value_type& r : other->m_vOutputs)
    {
        m_vOutputs.push_back(data::object::copy(r, _cache));
    }

    base_class::deep_copy(other, _cache);
}

//------------------------------------------------------------------------------

bool resection::operator==(const resection& _other) const noexcept
{
    if(m_name != _other.m_name
       || m_isSafePart != _other.m_isSafePart
       || m_isValid != _other.m_isValid
       || m_isVisible != _other.m_isVisible
       || !core::tools::is_equal(m_planeList, _other.m_planeList)
       || !core::tools::is_equal(m_vInputs, _other.m_vInputs)
       || !core::tools::is_equal(m_vOutputs, _other.m_vOutputs))
    {
        return false;
    }

    // Super class last
    return base_class::operator==(_other);
}

//------------------------------------------------------------------------------

bool resection::operator!=(const resection& _other) const noexcept
{
    return !(*this == _other);
}

} // namespace sight::data
