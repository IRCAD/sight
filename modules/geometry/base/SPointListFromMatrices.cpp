/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
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

#include <core/com/Signal.hxx>

#include <data/fieldHelper/Image.hpp>
#include <data/String.hpp>

#include <service/macros.hpp>

namespace sight::module::geometry::base
{

//-----------------------------------------------------------------------------

SPointListFromMatrices::SPointListFromMatrices() :
    m_append(false)
{
}

//-----------------------------------------------------------------------------

SPointListFromMatrices::~SPointListFromMatrices()
{
}

//-----------------------------------------------------------------------------

void SPointListFromMatrices::configuring()
{
    const ConfigType configTree = this->getConfigTree();
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
    const size_t numMatrices = m_matrices.size();
    SIGHT_ASSERT("no matrices found", numMatrices != 0);

    auto pointList = m_pointList.lock();
    if(!m_append)
    {
        pointList->getPoints().clear();
    }

    for(size_t j = 0 ; j < numMatrices ; ++j)
    {
        const auto mat = m_matrices[j].lock();

        data::Matrix4::TMCoefArray coefs = mat->getCoefficients();

        //extract translation
        data::Point::sptr p = data::Point::New(coefs[3], coefs[7], coefs[11]);

        std::string label;
        if(m_append)
        {
            label = std::to_string(pointList->getPoints().size());
        }
        else
        {
            label = std::to_string(j);
        }

        data::String::sptr fwLabel = data::String::New(label);
        p->setField(data::fieldHelper::Image::m_labelId, fwLabel);
        pointList->pushBack(p);
    }

    auto sig = pointList->signal<data::PointList::ModifiedSignalType>(data::PointList::s_MODIFIED_SIG);
    sig->asyncEmit();

    m_sigComputed->asyncEmit();
}

//-----------------------------------------------------------------------------

} // namespace sight::module
