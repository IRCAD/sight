/************************************************************************
 *
 * Copyright (C) 2014-2022 IRCAD France
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

#include "data/CalibrationInfo.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Signals.hpp>

#include <data/registry/macros.hpp>

#include <algorithm>
#include <iterator>

namespace sight::data
{

SIGHT_REGISTER_DATA(sight::data::CalibrationInfo);

const core::com::Signals::SignalKeyType CalibrationInfo::s_ADDED_RECORD_SIG   = "addedRecord";
const core::com::Signals::SignalKeyType CalibrationInfo::s_REMOVED_RECORD_SIG = "removedRecord";
const core::com::Signals::SignalKeyType CalibrationInfo::s_RESET_RECORD_SIG   = "resetRecord";
const core::com::Signals::SignalKeyType CalibrationInfo::s_GET_RECORD_SIG     = "getRecord";

using ImageContainerType          = std::list<Image::sptr>;
using PointListContainerType      = std::list<PointList::sptr>;
using constImageContainerType     = std::list<Image::csptr>;
using constPointListContainerType = std::list<PointList::csptr>;

//------------------------------------------------------------------------------

CalibrationInfo::CalibrationInfo(data::Object::Key /*unused*/)
{
    m_sigAddedRecord   = AddedRecordSignalType::New();
    m_sigRemovedRecord = RemovedRecordSignalType::New();
    m_sigResetRecord   = ResetRecordSignalType::New();
    m_sigGetRecord     = GetRecordSignalType::New();

    core::com::HasSignals::m_signals(s_ADDED_RECORD_SIG, m_sigAddedRecord);
    core::com::HasSignals::m_signals(s_REMOVED_RECORD_SIG, m_sigRemovedRecord);
    core::com::HasSignals::m_signals(s_RESET_RECORD_SIG, m_sigResetRecord);
    core::com::HasSignals::m_signals(s_GET_RECORD_SIG, m_sigGetRecord);
}

//------------------------------------------------------------------------------

CalibrationInfo::~CalibrationInfo()
= default;

//------------------------------------------------------------------------------

void CalibrationInfo::shallowCopy(const data::Object::csptr& _source)
{
    CalibrationInfo::csptr other = CalibrationInfo::dynamicConstCast(_source);
    SIGHT_THROW_EXCEPTION_IF(
        data::Exception(
            "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
            + " to " + this->getClassname()
        ),
        !bool(other)
    );

    this->fieldShallowCopy(other);

    m_imageContainer     = other->m_imageContainer;
    m_pointListContainer = other->m_pointListContainer;
}

//------------------------------------------------------------------------------

void CalibrationInfo::cachedDeepCopy(const data::Object::csptr& _source, DeepCopyCacheType& cache)
{
    CalibrationInfo::csptr other = CalibrationInfo::dynamicConstCast(_source);
    SIGHT_THROW_EXCEPTION_IF(
        data::Exception(
            "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
            + " to " + this->getClassname()
        ),
        !bool(other)
    );

    this->fieldDeepCopy(other);

    this->resetRecords();
    SIGHT_ASSERT("Lists have not the same size", other->m_pointListContainer.size() == other->m_imageContainer.size());

    auto imgIter = other->m_imageContainer.begin();
    for(const data::PointList::sptr& pl : other->m_pointListContainer)
    {
        data::Image::sptr otherImg    = data::Object::copy(*imgIter, cache);
        data::PointList::sptr otherPl = data::Object::copy(pl, cache);
        this->addRecord(otherImg, otherPl);

        ++imgIter;
    }
}

//------------------------------------------------------------------------------

void CalibrationInfo::addRecord(const data::Image::sptr& img, const data::PointList::sptr& pl)
{
    m_imageContainer.push_back(img);
    m_pointListContainer.push_back(pl);
}

//------------------------------------------------------------------------------

void CalibrationInfo::removeRecord(std::size_t idx)
{
    SIGHT_ASSERT("index out of bound ", idx < m_pointListContainer.size());

    auto plIt  = m_pointListContainer.begin();
    auto imgIt = m_imageContainer.begin();

    std::advance(plIt, static_cast<PointListContainerType::iterator::difference_type>(idx));
    std::advance(imgIt, static_cast<ImageContainerType::iterator::difference_type>(idx));

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

ImageContainerType CalibrationInfo::getImageContainer()
{
    return m_imageContainer;
}

//------------------------------------------------------------------------------

constImageContainerType CalibrationInfo::getImageContainer() const
{
    // Transform to csptr
    constImageContainerType constImageList;
    for(const auto& image : m_imageContainer)
    {
        constImageList.push_back(image);
    }

    return constImageList;
}

//------------------------------------------------------------------------------

PointListContainerType CalibrationInfo::getPointListContainer()
{
    return m_pointListContainer;
}

//------------------------------------------------------------------------------

constPointListContainerType CalibrationInfo::getPointListContainer() const
{
    // Transform to csptr
    constPointListContainerType constPointList;
    for(const auto& pl : m_pointListContainer)
    {
        constPointList.push_back(pl);
    }

    return constPointList;
}

//------------------------------------------------------------------------------

data::PointList::csptr CalibrationInfo::getPointList(const data::Image::csptr& img) const
{
    data::PointList::sptr pl;

    SIGHT_ASSERT("Lists have not the same size", m_imageContainer.size() == m_pointListContainer.size());
    std::size_t dist = 0;
    ImageContainerType::const_iterator it;
    for(it = m_imageContainer.begin() ; it != m_imageContainer.end() && *(it) != img ; ++it, ++dist)
    {
    }

    auto plIt = m_pointListContainer.begin();
    std::advance(plIt, static_cast<PointListContainerType::const_iterator::difference_type>(dist));

    if(it != m_imageContainer.end())
    {
        pl = *(plIt);
    }

    return pl;
}

//------------------------------------------------------------------------------

data::Image::csptr CalibrationInfo::getImage(const data::PointList::csptr& pl) const
{
    data::Image::sptr img;

    SIGHT_ASSERT("Lists have not the same size", m_imageContainer.size() == m_pointListContainer.size());

    std::size_t dist = 0;
    PointListContainerType::const_iterator it;
    for(it = m_pointListContainer.begin() ; it != m_pointListContainer.end() && *(it) != pl ; ++it, ++dist)
    {
    }

    auto imgIt = m_imageContainer.begin();
    std::advance(imgIt, static_cast<ImageContainerType::const_iterator::difference_type>(dist));

    if(it != m_pointListContainer.end())
    {
        img = *(imgIt);
    }

    return img;
}

//------------------------------------------------------------------------------

data::Image::sptr CalibrationInfo::getImage(std::size_t idx)
{
    SIGHT_ASSERT("index out of bound ", idx < m_imageContainer.size());

    auto imgIt = m_imageContainer.begin();

    std::advance(imgIt, static_cast<ImageContainerType::const_iterator::difference_type>(idx));

    return *(imgIt);
}

//------------------------------------------------------------------------------

data::Image::csptr CalibrationInfo::getImage(std::size_t idx) const
{
    SIGHT_ASSERT("index out of bound ", idx < m_imageContainer.size());

    auto imgIt = m_imageContainer.begin();

    std::advance(imgIt, static_cast<ImageContainerType::const_iterator::difference_type>(idx));

    return *(imgIt);
}

//------------------------------------------------------------------------------

bool CalibrationInfo::operator==(const CalibrationInfo& other) const noexcept
{
    if(!core::tools::is_equal(m_imageContainer, other.m_imageContainer)
       || !core::tools::is_equal(m_pointListContainer, other.m_pointListContainer))
    {
        return false;
    }

    // Super class last
    return Object::operator==(other);
}

//------------------------------------------------------------------------------

bool CalibrationInfo::operator!=(const CalibrationInfo& other) const noexcept
{
    return !(*this == other);
}

//------------------------------------------------------------------------------

} //namespace sight::data
