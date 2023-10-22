/************************************************************************
 *
 * Copyright (C) 2019-2023 IRCAD France
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

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

namespace sight::module::geometry
{

static const core::com::slots::key_t PICK_SLOT         = "pick";
static const core::com::slots::key_t CLEAR_POINTS_SLOT = "clearPoints";

static const std::string s_MAX_CONFIG       = "max";
static const std::string s_REMOVABLE_CONFIG = "removable";
static const std::string s_LABEL_CONFIG     = "label";
static const std::string s_TOLERANCE_CONFIG = "tolerance";

//------------------------------------------------------------------------------

manage_point_list::manage_point_list() noexcept
{
    new_slot(PICK_SLOT, &manage_point_list::pick, this);
    new_slot(CLEAR_POINTS_SLOT, &manage_point_list::clearPoints, this);
}

//------------------------------------------------------------------------------

manage_point_list::~manage_point_list() noexcept =
    default;

//------------------------------------------------------------------------------

void manage_point_list::configuring()
{
    const auto tree   = this->get_config();
    const auto config = tree.get_child_optional("config.<xmlattr>");

    if(config)
    {
        m_max       = config->get<std::size_t>(s_MAX_CONFIG, m_max);
        m_removable = config->get<bool>(s_REMOVABLE_CONFIG, m_removable);
        m_label     = config->get<bool>(s_LABEL_CONFIG, m_label);
        m_tolerance = config->get<float>(s_TOLERANCE_CONFIG, m_tolerance);
    }
}

//------------------------------------------------------------------------------

void manage_point_list::starting()
{
}

//------------------------------------------------------------------------------

void manage_point_list::updating()
{
}

//------------------------------------------------------------------------------

void manage_point_list::stopping()
{
}

//------------------------------------------------------------------------------

void manage_point_list::pick(data::tools::picking_info _info) const
{
    if((_info.m_modifierMask & data::tools::picking_info::CTRL) != 0)
    {
        const data::point::sptr point = std::make_shared<data::point>();

        const auto matrix = m_transform.lock();

        if(matrix)
        {
            const std::array<double, 3>& picked_coord = _info.m_worldPos;
            const glm::dvec4 picked_point             =
                glm::dvec4 {picked_coord[0], picked_coord[1], picked_coord[2], 1.0
            };
            const glm::dmat4x4 mat = sight::geometry::data::to_glm_mat(*matrix);

            const glm::dvec4 modified_picked_point = mat * picked_point;
            point->setCoord({modified_picked_point[0], modified_picked_point[1], modified_picked_point[2]});
        }
        else
        {
            point->setCoord({_info.m_worldPos[0], _info.m_worldPos[1], _info.m_worldPos[2]});
        }

        if(_info.m_eventId == data::tools::picking_info::Event::MOUSE_LEFT_UP)
        {
            this->addPoint(point);
        }
        else if(_info.m_eventId == data::tools::picking_info::Event::MOUSE_RIGHT_UP)
        {
            this->removePoint(point);
        }
    }
}

//------------------------------------------------------------------------------

void manage_point_list::addPoint(const data::point::sptr _point) const
{
    const auto point_list = m_pointList.lock();

    if(m_label)
    {
        const auto counter = point_list->getPoints().size();
        _point->setLabel(std::to_string(counter));
    }

    point_list->pushBack(_point);
    const auto& sig_added = point_list->signal<data::point_list::point_added_signal_t>(
        data::point_list::POINT_ADDED_SIG
    );
    sig_added->async_emit(_point);

    if(m_max != 0 && point_list->getPoints().size() > m_max)
    {
        const data::point::sptr removed_point = point_list->getPoints().front();
        point_list->remove(0);
        const auto& sig_removed = point_list->signal<data::point_list::point_removed_signal_t>(
            data::point_list::POINT_REMOVED_SIG
        );
        sig_removed->async_emit(removed_point);
    }
}

//------------------------------------------------------------------------------

void manage_point_list::removePoint(const data::point::csptr _point) const
{
    if(m_removable)
    {
        const auto point_list             = m_pointList.lock();
        const data::point::sptr point_res =
            sight::geometry::data::point_list::removeClosestPoint(point_list.get_shared(), _point, m_tolerance);

        if(point_res != nullptr)
        {
            const auto& sig_removed = point_list->signal<data::point_list::point_removed_signal_t>(
                data::point_list::POINT_REMOVED_SIG
            );
            sig_removed->async_emit(point_res);
        }
    }
}

//------------------------------------------------------------------------------

void manage_point_list::clearPoints() const
{
    const auto point_list = m_pointList.lock();

    using PLContainer = data::point_list::PointListContainer;
    const PLContainer container = point_list->getPoints();
    point_list->clear();

    for(const auto& point : container)
    {
        const auto& sig_removed = point_list->signal<data::point_list::point_removed_signal_t>(
            data::point_list::POINT_REMOVED_SIG
        );
        sig_removed->async_emit(point);
    }
}

} // namespace sight::module::geometry
