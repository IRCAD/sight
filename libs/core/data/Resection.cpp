/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

Resection::Resection(data::Object::Key) :
    m_isSafePart(true),
    m_isValid(false),
    m_isVisible(true)
{
    m_planeList              = data::PlaneList::New();
    m_sigReconstructionAdded = ReconstructionAddedSignalType::New();
    m_sigVisibilityModified  = VisibilityModifiedSignalType::New();

    m_signals(s_RECONSTRUCTION_ADDED_SIG, m_sigReconstructionAdded)
        (s_VISIBILITY_MODIFIED_SIG, m_sigVisibilityModified);
}

//------------------------------------------------------------------------------

Resection::~Resection()
{
}

//------------------------------------------------------------------------------

void Resection::shallowCopy(const Object::csptr& _source)
{
    Resection::csptr other = Resection::dynamicConstCast(_source);
    SIGHT_THROW_EXCEPTION_IF(
        data::Exception(
            "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
            + " to " + this->getClassname()
        ),
        !bool(other)
    );
    this->fieldShallowCopy(_source);

    m_name       = other->m_name;
    m_isSafePart = other->m_isSafePart;
    m_isValid    = other->m_isValid;
    m_isVisible  = other->m_isVisible;
    m_planeList  = other->m_planeList;
    m_vInputs    = other->m_vInputs;
    m_vOutputs   = other->m_vOutputs;
}

//------------------------------------------------------------------------------

void Resection::cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache)
{
    Resection::csptr other = Resection::dynamicConstCast(_source);
    SIGHT_THROW_EXCEPTION_IF(
        data::Exception(
            "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
            + " to " + this->getClassname()
        ),
        !bool(other)
    );
    this->fieldDeepCopy(_source, cache);

    m_name       = other->m_name;
    m_isSafePart = other->m_isSafePart;
    m_isValid    = other->m_isValid;
    m_isVisible  = other->m_isVisible;
    m_planeList  = data::Object::copy(other->m_planeList, cache);

    this->m_vInputs.clear();
    for(const ResectionInputs::value_type& resec : other->m_vInputs)
    {
        m_vInputs.push_back(data::Object::copy(resec, cache));
    }

    this->m_vOutputs.clear();
    for(const ResectionOutputs::value_type& resec : other->m_vOutputs)
    {
        m_vOutputs.push_back(data::Object::copy(resec, cache));
    }
}

//------------------------------------------------------------------------------

} // namespace sight::data
