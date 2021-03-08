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

#include "data/ResectionDB.hpp"

#include "data/Exception.hpp"
#include "data/registry/macros.hpp"

#include <core/base.hpp>
#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>

SIGHT_REGISTER_DATA( sight::data::ResectionDB );

namespace sight::data
{

const core::com::Signals::SignalKeyType ResectionDB::s_RESECTION_ADDED_SIG = "resectionAdded";
const core::com::Signals::SignalKeyType ResectionDB::s_SAFE_PART_ADDED_SIG = "safePartAdded";

//------------------------------------------------------------------------------

ResectionDB::ResectionDB(data::Object::Key)
{
    m_sigResectionAdded = ResectionAddedSignalType::New();
    m_sigSafePartAdded  = SafePartAddedSignalType::New();

    m_signals( s_RESECTION_ADDED_SIG,  m_sigResectionAdded)
        ( s_SAFE_PART_ADDED_SIG,  m_sigSafePartAdded);
}

//------------------------------------------------------------------------------

ResectionDB::~ResectionDB()
{
}

//------------------------------------------------------------------------------

void ResectionDB::shallowCopy(const Object::csptr& _source )
{
    ResectionDB::csptr other = ResectionDB::dynamicConstCast(_source);
    SIGHT_THROW_EXCEPTION_IF( data::Exception(
                                  "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                                  + " to " + this->getClassname()), !bool(other) );
    this->fieldShallowCopy( _source );
}

//------------------------------------------------------------------------------

void ResectionDB::cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache)
{
    ResectionDB::csptr other = ResectionDB::dynamicConstCast(_source);
    SIGHT_THROW_EXCEPTION_IF( data::Exception(
                                  "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                                  + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( _source, cache );
}

//------------------------------------------------------------------------------

ResectionDB::ResectionContainerType::size_type ResectionDB::getNumberOfResections() const
{
    return m_resections.size();
}

//------------------------------------------------------------------------------

void ResectionDB::addResection(const data::Resection::sptr& resection )
{
    m_resections.push_back( resection );
}

//------------------------------------------------------------------------------

} // end namespace sight::data
