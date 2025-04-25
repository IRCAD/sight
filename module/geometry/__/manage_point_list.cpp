/************************************************************************
 *
 * Copyright (C) 2019-2025 IRCAD France
 * Copyright (C) 2019-2020 IHU Strasbourg
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

#include "manage_point_list.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <data/string.hpp>

#include <boost/algorithm/string.hpp>

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
namespace sight::module::geometry
{

//------------------------------------------------------------------------------

manage_point_list::manage_point_list() noexcept
{
    new_slot(slots::PICK, &manage_point_list::pick, this);
    new_slot(slots::CLEAR, &manage_point_list::clear, this);
}

//------------------------------------------------------------------------------

void manage_point_list::configuring(const config_t& _config)
{
    const auto config = _config.get_child_optional("config.<xmlattr>");

    if(config)
    {
        m_max       = config->get<std::size_t>("max", m_max);
        m_removable = config->get<bool>("removable", m_removable);
        m_label     = config->get<bool>("label", m_label);
        m_tolerance = config->get<float>("tolerance", m_tolerance);
        auto modifier = boost::to_lower_copy(config->get<std::string>("modifier", ""));
        if(modifier == "ctrl")
        {
            m_modifier = data::tools::picking_info::ctrl;
        }
        else if(modifier == "shift")
        {
            m_modifier = data::tools::picking_info::shift;
        }
    }
}

//------------------------------------------------------------------------------

void manage_point_list::starting()
{
}

//------------------------------------------------------------------------------

void manage_point_list::updating()
{
    const auto input_position = m_position.lock();
    if(input_position == nullptr)
    {
        return;
    }

    const data::point::sptr new_point = std::make_shared<data::point>();
    const auto matrix                 = m_transform.lock();
    if(matrix)
    {
        const glm::dmat4x4 mat = sight::geometry::data::to_glm_mat(*matrix);

        const glm::dvec4 point          = {(*input_position)[3], (*input_position)[7], (*input_position)[11], 1.0};
        const glm::dvec4 modified_point = mat * point;
        *new_point = {modified_point[0], modified_point[1], modified_point[2]};
    }
    else
    {
        *new_point = {(*input_position)[3], (*input_position)[7], (*input_position)[11]};
    }

    this->add_point(new_point);
}

//------------------------------------------------------------------------------

void manage_point_list::stopping()
{
}

//------------------------------------------------------------------------------

void manage_point_list::pick(data::tools::picking_info _info) const
{
    if(_info.m_modifier_mask == m_modifier)
    {
        const data::point::sptr new_point = std::make_shared<data::point>();

        const auto matrix = m_transform.lock();
        if(matrix)
        {
            const glm::dvec4 picked_point = {_info.m_world_pos[0], _info.m_world_pos[1], _info.m_world_pos[2], 1.0};
            const glm::dmat4x4 mat        = sight::geometry::data::to_glm_mat(*matrix);

            const glm::dvec4 modified_picked_point = mat * picked_point;
            *new_point = {modified_picked_point[0], modified_picked_point[1], modified_picked_point[2]};
        }
        else
        {
            *new_point = {_info.m_world_pos[0], _info.m_world_pos[1], _info.m_world_pos[2]};
        }

        if(_info.m_event_id == data::tools::picking_info::event::mouse_left_up)
        {
            this->add_point(new_point);
        }
        else if(_info.m_event_id == data::tools::picking_info::event::mouse_right_up)
        {
            this->remove_point(new_point);
        }
    }
}

//------------------------------------------------------------------------------

void manage_point_list::add_point(const data::point::sptr _point) const
{
    const auto point_list = m_point_list.lock();

    if(m_label)
    {
        const auto counter = point_list->get_points().size();
        _point->set_label(std::to_string(counter));
    }

    point_list->push_back(_point);
    point_list->async_emit(data::point_list::POINT_ADDED_SIG, _point);

    if(m_max != 0 && point_list->get_points().size() > m_max)
    {
        const data::point::sptr removed_point = point_list->get_points().front();
        point_list->remove(0);
        point_list->async_emit(data::point_list::POINT_REMOVED_SIG, removed_point);
    }
}

//------------------------------------------------------------------------------

void manage_point_list::remove_point(const data::point::csptr _point) const
{
    if(m_removable)
    {
        const auto point_list             = m_point_list.lock();
        const data::point::sptr point_res =
            sight::geometry::data::point_list::remove_closest_point(point_list.get_shared(), _point, m_tolerance);

        if(point_res != nullptr)
        {
            point_list->async_emit(data::point_list::POINT_REMOVED_SIG, point_res);
        }
    }
}

//------------------------------------------------------------------------------

void manage_point_list::clear() const
{
    const auto point_list = m_point_list.lock();

    using pl_container_t = data::point_list::container_t;
    const pl_container_t container = point_list->get_points();
    point_list->clear();

    for(const auto& point : container)
    {
        point_list->async_emit(data::point_list::POINT_REMOVED_SIG, point);
    }
}

} // namespace sight::module::geometry
