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

#include "SPointListFromMatrices.hpp"

#include <core/com/signal.hxx>

#include <data/String.hpp>

#include <service/macros.hpp>

namespace sight::module::geometry
{

//-----------------------------------------------------------------------------

SPointListFromMatrices::SPointListFromMatrices()
= default;

//-----------------------------------------------------------------------------

SPointListFromMatrices::~SPointListFromMatrices()
= default;

//-----------------------------------------------------------------------------

void SPointListFromMatrices::configuring()
{
    const ConfigType configTree = this->getConfiguration();
    const ConfigType config     = configTree.get_child("config.<xmlattr>");
    if(!config.empty())
    {
        m_append = config.get<bool>("append", m_append);
    }
}

//-----------------------------------------------------------------------------

void SPointListFromMatrices::starting()
{
}

//-----------------------------------------------------------------------------

void SPointListFromMatrices::stopping()
{
}

//-----------------------------------------------------------------------------

void SPointListFromMatrices::updating()
{
    const std::size_t numMatrices = m_matrices.size();
    SIGHT_ASSERT("no matrices found", numMatrices != 0);

    auto pointList = m_pointList.lock();
    if(!m_append)
    {
        pointList->getPoints().clear();
    }

    for(std::size_t j = 0 ; j < numMatrices ; ++j)
    {
        const auto mat = m_matrices[j].lock();

        // extract translation
        data::Point::sptr p = std::make_shared<data::Point>((*mat)[3], (*mat)[7], (*mat)[11]);

        std::string label;
        if(m_append)
        {
            label = std::to_string(pointList->getPoints().size());
        }
        else
        {
            label = std::to_string(j);
        }

        p->setLabel(label);
        pointList->pushBack(p);
    }

    auto sig = pointList->signal<data::PointList::ModifiedSignalType>(data::PointList::MODIFIED_SIG);
    sig->async_emit();

    m_sigComputed->async_emit();
}

//-----------------------------------------------------------------------------

} // namespace sight::module::geometry
