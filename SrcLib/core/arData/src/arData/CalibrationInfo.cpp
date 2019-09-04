/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "arData/CalibrationInfo.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>

#include <fwData/registry/macros.hpp>

#include <algorithm>
#include <iterator>

namespace arData
{

fwDataRegisterMacro( ::arData::CalibrationInfo );

const ::fwCom::Signals::SignalKeyType CalibrationInfo::s_ADDED_RECORD_SIG   = "addedRecord";
const ::fwCom::Signals::SignalKeyType CalibrationInfo::s_REMOVED_RECORD_SIG = "removedRecord";
const ::fwCom::Signals::SignalKeyType CalibrationInfo::s_RESET_RECORD_SIG   = "resetRecord";
const ::fwCom::Signals::SignalKeyType CalibrationInfo::s_GET_RECORD_SIG     = "getRecord";

//------------------------------------------------------------------------------

CalibrationInfo::CalibrationInfo(::fwData::Object::Key)
{
    m_sigAddedRecord   = AddedRecordSignalType::New();
    m_sigRemovedRecord = RemovedRecordSignalType::New();
    m_sigResetRecord   = ResetRecordSignalType::New();
    m_sigGetRecord     = GetRecordSignalType::New();

    ::fwCom::HasSignals::m_signals(s_ADDED_RECORD_SIG, m_sigAddedRecord);
    ::fwCom::HasSignals::m_signals(s_REMOVED_RECORD_SIG, m_sigRemovedRecord);
    ::fwCom::HasSignals::m_signals(s_RESET_RECORD_SIG, m_sigResetRecord);
    ::fwCom::HasSignals::m_signals(s_GET_RECORD_SIG, m_sigGetRecord);
}

//------------------------------------------------------------------------------

CalibrationInfo::~CalibrationInfo()
{

}

//------------------------------------------------------------------------------

void CalibrationInfo::shallowCopy( const ::fwData::Object::csptr& _source )
{
    CalibrationInfo::csptr other = CalibrationInfo::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );

    this->fieldShallowCopy(other);

    m_imageContainer     = other->m_imageContainer;
    m_pointListContainer = other->m_pointListContainer;
}

//------------------------------------------------------------------------------

void CalibrationInfo::cachedDeepCopy(const ::fwData::Object::csptr& _source, DeepCopyCacheType& cache)
{
    CalibrationInfo::csptr other = CalibrationInfo::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );

    this->fieldDeepCopy( other );

    this->resetRecords();
    SLM_ASSERT("Lists have not the same size", other->m_pointListContainer.size() == other->m_imageContainer.size());

    ImageContainerType::const_iterator imgIter = other->m_imageContainer.begin();
    for(const ::fwData::PointList::sptr& pl : other->m_pointListContainer)
    {
        ::fwData::Image::sptr otherImg    = ::fwData::Object::copy(*imgIter, cache);
        ::fwData::PointList::sptr otherPl = ::fwData::Object::copy(pl, cache);
        this->addRecord(otherImg, otherPl);

        ++imgIter;
    }
}

//------------------------------------------------------------------------------

void CalibrationInfo::addRecord( const ::fwData::Image::sptr& img, const ::fwData::PointList::sptr& pl )
{
    m_imageContainer.push_back(img);
    m_pointListContainer.push_back(pl);
}

//------------------------------------------------------------------------------

void CalibrationInfo::removeRecord(size_t idx)
{
    SLM_ASSERT("index out of bound ", idx < m_pointListContainer.size() );

    PointListContainerType::iterator plIt = m_pointListContainer.begin();
    ImageContainerType::iterator imgIt    = m_imageContainer.begin();

    std::advance(plIt, idx);
    std::advance(imgIt, idx);

    m_pointListContainer.erase(plIt);
    m_imageContainer.erase(imgIt);
}

//------------------------------------------------------------------------------

void CalibrationInfo::resetRecords()
{
    m_pointListContainer.clear();
    m_imageContainer.clear();
}

//------------------------------------------------------------------------------

CalibrationInfo::ImageContainerType CalibrationInfo::getImageContainer() const
{
    return m_imageContainer;
}

//------------------------------------------------------------------------------

CalibrationInfo::PointListContainerType CalibrationInfo::getPointListContainer() const
{
    return m_pointListContainer;
}

//------------------------------------------------------------------------------

::fwData::PointList::sptr CalibrationInfo::getPointList( const ::fwData::Image::sptr& img) const
{
    ::fwData::PointList::sptr pl;

    SLM_ASSERT("Lists have not the same size", m_imageContainer.size() == m_pointListContainer.size() );
    size_t dist = 0;
    ImageContainerType::const_iterator it;
    for(it = m_imageContainer.begin(); it != m_imageContainer.end() && *(it) != img; ++it, ++dist)
    {
    }

    PointListContainerType::const_iterator plIt = m_pointListContainer.begin();
    std::advance(plIt, dist);

    if(it != m_imageContainer.end())
    {
        pl = *(plIt);
    }

    return pl;
}

//------------------------------------------------------------------------------

::fwData::Image::sptr CalibrationInfo::getImage( const ::fwData::PointList::sptr& pl) const
{
    ::fwData::Image::sptr img;

    SLM_ASSERT("Lists have not the same size", m_imageContainer.size() == m_pointListContainer.size() );

    size_t dist = 0;
    PointListContainerType::const_iterator it;
    for(it = m_pointListContainer.begin(); it != m_pointListContainer.end() && *(it) != pl; ++it, ++dist)
    {
    }

    ImageContainerType::const_iterator imgIt = m_imageContainer.begin();
    std::advance(imgIt, dist);

    if(it != m_pointListContainer.end())
    {
        img = *(imgIt);
    }

    return img;
}

//------------------------------------------------------------------------------

::fwData::Image::sptr CalibrationInfo::getImage(size_t idx) const
{
    SLM_ASSERT("index out of bound ", idx < m_imageContainer.size() );

    ImageContainerType::const_iterator imgIt = m_imageContainer.begin();

    std::advance(imgIt, idx);

    return *(imgIt);
}

//------------------------------------------------------------------------------

} //namespace arData
