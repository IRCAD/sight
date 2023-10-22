/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

#include "data/calibration_info.hpp"

#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/com/signals.hpp>

#include <data/registry/macros.hpp>

#include <algorithm>
#include <iterator>

namespace sight::data
{

SIGHT_REGISTER_DATA(sight::data::calibration_info);

const core::com::signals::key_t calibration_info::ADDED_RECORD_SIG   = "addedRecord";
const core::com::signals::key_t calibration_info::REMOVED_RECORD_SIG = "removedRecord";
const core::com::signals::key_t calibration_info::RESET_RECORD_SIG   = "resetRecord";
const core::com::signals::key_t calibration_info::GET_RECORD_SIG     = "getRecord";

using image_container_t           = std::list<image::sptr>;
using point_list_container_t      = std::list<point_list::sptr>;
using constImageContainerType     = std::list<image::csptr>;
using constPointListContainerType = std::list<point_list::csptr>;

//------------------------------------------------------------------------------

calibration_info::calibration_info() :
    m_sigAddedRecord(std::make_shared<added_record_signal_t>()),
    m_sigRemovedRecord(std::make_shared<removed_record_signal_t>()),
    m_sigResetRecord(std::make_shared<Reset_record_signal_type>()),
    m_sigGetRecord(std::make_shared<get_record_signal_t>())
{
    core::com::has_signals::m_signals(ADDED_RECORD_SIG, m_sigAddedRecord);
    core::com::has_signals::m_signals(REMOVED_RECORD_SIG, m_sigRemovedRecord);
    core::com::has_signals::m_signals(RESET_RECORD_SIG, m_sigResetRecord);
    core::com::has_signals::m_signals(GET_RECORD_SIG, m_sigGetRecord);
}

//------------------------------------------------------------------------------

void calibration_info::shallow_copy(const object::csptr& _source)
{
    const auto& other = std::dynamic_pointer_cast<const calibration_info>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_imageContainer     = other->m_imageContainer;
    m_pointListContainer = other->m_pointListContainer;

    base_class::shallow_copy(other);
}

//------------------------------------------------------------------------------

void calibration_info::deep_copy(const object::csptr& _source, const std::unique_ptr<deep_copy_cache_t>& _cache)
{
    const auto& other = std::dynamic_pointer_cast<const calibration_info>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    this->resetRecords();
    SIGHT_ASSERT("Lists have not the same size", other->m_pointListContainer.size() == other->m_imageContainer.size());

    auto img_iter = other->m_imageContainer.begin();
    for(const data::point_list::sptr& pl : other->m_pointListContainer)
    {
        data::image::sptr other_img     = data::object::copy(*img_iter, _cache);
        data::point_list::sptr other_pl = data::object::copy(pl, _cache);
        this->addRecord(other_img, other_pl);

        ++img_iter;
    }

    base_class::deep_copy(other, _cache);
}

//------------------------------------------------------------------------------

void calibration_info::addRecord(const data::image::sptr& _img, const data::point_list::sptr& _pl)
{
    m_imageContainer.push_back(_img);
    m_pointListContainer.push_back(_pl);
}

//------------------------------------------------------------------------------

void calibration_info::removeRecord(std::size_t _idx)
{
    SIGHT_ASSERT("index out of bound ", _idx < m_pointListContainer.size());

    auto pl_it  = m_pointListContainer.begin();
    auto img_it = m_imageContainer.begin();

    std::advance(pl_it, static_cast<point_list_container_t::iterator::difference_type>(_idx));
    std::advance(img_it, static_cast<image_container_t::iterator::difference_type>(_idx));

    m_pointListContainer.erase(pl_it);
    m_imageContainer.erase(img_it);
}

//------------------------------------------------------------------------------

void calibration_info::resetRecords()
{
    m_pointListContainer.clear();
    m_imageContainer.clear();
}

//------------------------------------------------------------------------------

image_container_t calibration_info::getImageContainer()
{
    return m_imageContainer;
}

//------------------------------------------------------------------------------

constImageContainerType calibration_info::getImageContainer() const
{
    // Transform to csptr
    constImageContainerType const_image_list;
    for(const auto& image : m_imageContainer)
    {
        const_image_list.push_back(image);
    }

    return const_image_list;
}

//------------------------------------------------------------------------------

point_list_container_t calibration_info::getPointListContainer()
{
    return m_pointListContainer;
}

//------------------------------------------------------------------------------

constPointListContainerType calibration_info::getPointListContainer() const
{
    // Transform to csptr
    constPointListContainerType const_point_list;
    for(const auto& pl : m_pointListContainer)
    {
        const_point_list.push_back(pl);
    }

    return const_point_list;
}

//------------------------------------------------------------------------------

data::point_list::csptr calibration_info::getPointList(const data::image::csptr& _img) const
{
    data::point_list::sptr pl;

    SIGHT_ASSERT("Lists have not the same size", m_imageContainer.size() == m_pointListContainer.size());
    std::size_t dist = 0;
    image_container_t::const_iterator it;
    for(it = m_imageContainer.begin() ; it != m_imageContainer.end() && *(it) != _img ; ++it, ++dist)
    {
    }

    auto pl_it = m_pointListContainer.begin();
    std::advance(pl_it, static_cast<point_list_container_t::const_iterator::difference_type>(dist));

    if(it != m_imageContainer.end())
    {
        pl = *(pl_it);
    }

    return pl;
}

//------------------------------------------------------------------------------

data::image::csptr calibration_info::getImage(const data::point_list::csptr& _pl) const
{
    data::image::sptr img;

    SIGHT_ASSERT("Lists have not the same size", m_imageContainer.size() == m_pointListContainer.size());

    std::size_t dist = 0;
    point_list_container_t::const_iterator it;
    for(it = m_pointListContainer.begin() ; it != m_pointListContainer.end() && *(it) != _pl ; ++it, ++dist)
    {
    }

    auto img_it = m_imageContainer.begin();
    std::advance(img_it, static_cast<image_container_t::const_iterator::difference_type>(dist));

    if(it != m_pointListContainer.end())
    {
        img = *(img_it);
    }

    return img;
}

//------------------------------------------------------------------------------

data::image::sptr calibration_info::getImage(std::size_t _idx)
{
    SIGHT_ASSERT("index out of bound ", _idx < m_imageContainer.size());

    auto img_it = m_imageContainer.begin();

    std::advance(img_it, static_cast<image_container_t::const_iterator::difference_type>(_idx));

    return *(img_it);
}

//------------------------------------------------------------------------------

data::image::csptr calibration_info::getImage(std::size_t _idx) const
{
    SIGHT_ASSERT("index out of bound ", _idx < m_imageContainer.size());

    auto img_it = m_imageContainer.begin();

    std::advance(img_it, static_cast<image_container_t::const_iterator::difference_type>(_idx));

    return *(img_it);
}

//------------------------------------------------------------------------------

bool calibration_info::operator==(const calibration_info& _other) const noexcept
{
    if(!core::tools::is_equal(m_imageContainer, _other.m_imageContainer)
       || !core::tools::is_equal(m_pointListContainer, _other.m_pointListContainer))
    {
        return false;
    }

    // Super class last
    return base_class::operator==(_other);
}

//------------------------------------------------------------------------------

bool calibration_info::operator!=(const calibration_info& _other) const noexcept
{
    return !(*this == _other);
}

//------------------------------------------------------------------------------

} //namespace sight::data
