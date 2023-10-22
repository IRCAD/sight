/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
 * Copyright (C) 2018-2021 IHU Strasbourg
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

#include "point_list_from_matrices.hpp"

#include <core/com/signal.hxx>

#include <data/string.hpp>

#include <service/macros.hpp>

namespace sight::module::geometry
{

//-----------------------------------------------------------------------------

point_list_from_matrices::point_list_from_matrices()
= default;

//-----------------------------------------------------------------------------

point_list_from_matrices::~point_list_from_matrices()
= default;

//-----------------------------------------------------------------------------

void point_list_from_matrices::configuring()
{
    const config_t config_tree = this->get_config();
    const config_t config      = config_tree.get_child("config.<xmlattr>");
    if(!config.empty())
    {
        m_append = config.get<bool>("append", m_append);
    }
}

//-----------------------------------------------------------------------------

void point_list_from_matrices::starting()
{
}

//-----------------------------------------------------------------------------

void point_list_from_matrices::stopping()
{
}

//-----------------------------------------------------------------------------

void point_list_from_matrices::updating()
{
    const std::size_t num_matrices = m_matrices.size();
    SIGHT_ASSERT("no matrices found", num_matrices != 0);

    auto point_list = m_pointList.lock();
    if(!m_append)
    {
        point_list->getPoints().clear();
    }

    for(std::size_t j = 0 ; j < num_matrices ; ++j)
    {
        const auto mat = m_matrices[j].lock();

        // extract translation
        data::point::sptr p = std::make_shared<data::point>((*mat)[3], (*mat)[7], (*mat)[11]);

        std::string label;
        if(m_append)
        {
            label = std::to_string(point_list->getPoints().size());
        }
        else
        {
            label = std::to_string(j);
        }

        p->setLabel(label);
        point_list->pushBack(p);
    }

    auto sig = point_list->signal<data::point_list::modified_signal_t>(data::point_list::MODIFIED_SIG);
    sig->async_emit();

    m_sigComputed->async_emit();
}

//-----------------------------------------------------------------------------

} // namespace sight::module::geometry
