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

#include "data/Resection.hpp"

#include "data/Exception.hpp"
#include "data/registry/macros.hpp"

#include <core/base.hpp>
#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>

SIGHT_REGISTER_DATA(sight::data::Resection);

namespace sight::data
{

const core::com::Signals::SignalKeyType Resection::s_RECONSTRUCTION_ADDED_SIG = "reconstructionAdded";
const core::com::Signals::SignalKeyType Resection::s_VISIBILITY_MODIFIED_SIG  = "pointTexCoordsModified";

//------------------------------------------------------------------------------

Resection::Resection(data::Object::Key /*unused*/) :
    m_planeList(data::PlaneList::New()),
    m_sigReconstructionAdded(ReconstructionAddedSignalType::New()),
    m_sigVisibilityModified(VisibilityModifiedSignalType::New())
{
    m_signals(s_RECONSTRUCTION_ADDED_SIG, m_sigReconstructionAdded)
        (s_VISIBILITY_MODIFIED_SIG, m_sigVisibilityModified);
}

//------------------------------------------------------------------------------

void Resection::shallowCopy(const Object::csptr& source)
{
    const auto& other = dynamicConstCast(source);

    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Unable to copy " + (source ? source->getClassname() : std::string("<NULL>"))
            + " to " + getClassname()
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

    BaseClass::shallowCopy(other);
}

//------------------------------------------------------------------------------

void Resection::deepCopy(const Object::csptr& source, const std::unique_ptr<DeepCopyCacheType>& cache)
{
    const auto& other = dynamicConstCast(source);

    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Unable to copy " + (source ? source->getClassname() : std::string("<NULL>"))
            + " to " + getClassname()
        ),
        !bool(other)
    );

    m_name       = other->m_name;
    m_isSafePart = other->m_isSafePart;
    m_isValid    = other->m_isValid;
    m_isVisible  = other->m_isVisible;
    m_planeList  = data::Object::copy(other->m_planeList, cache);

    this->m_vInputs.clear();
    for(const ResectionInputs::value_type& r : other->m_vInputs)
    {
        m_vInputs.push_back(data::Object::copy(r, cache));
    }

    this->m_vOutputs.clear();
    for(const ResectionOutputs::value_type& r : other->m_vOutputs)
    {
        m_vOutputs.push_back(data::Object::copy(r, cache));
    }

    BaseClass::deepCopy(other, cache);
}

//------------------------------------------------------------------------------

bool Resection::operator==(const Resection& other) const noexcept
{
    if(m_name != other.m_name
       || m_isSafePart != other.m_isSafePart
       || m_isValid != other.m_isValid
       || m_isVisible != other.m_isVisible
       || !core::tools::is_equal(m_planeList, other.m_planeList)
       || !core::tools::is_equal(m_vInputs, other.m_vInputs)
       || !core::tools::is_equal(m_vOutputs, other.m_vOutputs))
    {
        return false;
    }

    // Super class last
    return BaseClass::operator==(other);
}

//------------------------------------------------------------------------------

bool Resection::operator!=(const Resection& other) const noexcept
{
    return !(*this == other);
}

} // namespace sight::data
