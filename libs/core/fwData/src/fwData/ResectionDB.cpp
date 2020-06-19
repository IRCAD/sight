/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include "fwData/ResectionDB.hpp"

#include "fwData/Exception.hpp"
#include "fwData/registry/macros.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwCore/base.hpp>

fwDataRegisterMacro( ::fwData::ResectionDB );

namespace fwData
{

const ::fwCom::Signals::SignalKeyType ResectionDB::s_RESECTION_ADDED_SIG = "resectionAdded";
const ::fwCom::Signals::SignalKeyType ResectionDB::s_SAFE_PART_ADDED_SIG = "safePartAdded";

//------------------------------------------------------------------------------

ResectionDB::ResectionDB(::fwData::Object::Key)
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
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldShallowCopy( _source );
}

//------------------------------------------------------------------------------

void ResectionDB::cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache)
{
    ResectionDB::csptr other = ResectionDB::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
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

void ResectionDB::addResection(const ::fwData::Resection::sptr& resection )
{
    m_resections.push_back( resection );
}

//------------------------------------------------------------------------------

} // end namespace fwData
