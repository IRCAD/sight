/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwData/registry/macros.hpp"
#include "fwData/Exception.hpp"
#include "fwData/ResectionDB.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwCore/base.hpp>

fwDataRegisterMacro( ::fwData::ResectionDB );

namespace fwData
{

const ::fwCom::Signals::SignalKeyType ResectionDB::s_RESECTION_ADDED_SIG = "resectionAdded";
const ::fwCom::Signals::SignalKeyType ResectionDB::s_SAFE_PART_ADDED_SIG = "safePartAdded";

//------------------------------------------------------------------------------

ResectionDB::ResectionDB(::fwData::Object::Key key)
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

void ResectionDB::shallowCopy(const Object::csptr &_source )
{
    ResectionDB::csptr other = ResectionDB::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldShallowCopy( _source );
}

//------------------------------------------------------------------------------

void ResectionDB::cachedDeepCopy(const Object::csptr &_source, DeepCopyCacheType &cache)
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
