/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
 * Copyright (C) 2014-2018 IHU Strasbourg
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

#include "modules/geometry/base/SConcatenateMatrices.hpp"

#include <core/com/Signal.hxx>

#include <geometry/data/Matrix4.hpp>

#include <boost/range/iterator_range_core.hpp>

namespace sight::module::geometry::base
{

// ----------------------------------------------------------------------------

void SConcatenateMatrices::configuring()
{
    const auto config                    = this->getConfiguration();
    const auto inCfg                     = config.get_child("in");
    [[maybe_unused]] const auto groupCfg = inCfg.get_child_optional("<xmlattr>.group");
    SIGHT_ASSERT("Config must contain one input group named 'matrix'.", groupCfg.has_value());
    SIGHT_ASSERT("Missing 'in group=\"matrix\"'", groupCfg->get_value<std::string>() == s_MATRIX_GROUP_INPUT);

    for(const auto& cfg : boost::make_iterator_range(inCfg.equal_range("key")))
    {
        const auto inverse = cfg.second.get<bool>("<xmlattr>.inverse", false);
        m_invertVector.push_back(inverse);
    }
}

// ----------------------------------------------------------------------------

void SConcatenateMatrices::starting()
{
}

// ----------------------------------------------------------------------------

void SConcatenateMatrices::stopping()
{
}

// ----------------------------------------------------------------------------

void SConcatenateMatrices::updating()
{
    auto outputMatrix = m_output.lock();
    SIGHT_ASSERT("inout '" << s_OUTPUT << "' is not defined", outputMatrix);
    {
        sight::geometry::data::identity(*outputMatrix);

        data::Matrix4 inverse;

        std::size_t index = 0;
        for(const bool invertCurrentMatrix : m_invertVector)
        {
            auto inputMatrix = m_matrices[index++].lock();

            if(invertCurrentMatrix)
            {
                sight::geometry::data::invert(*inputMatrix, inverse);
                sight::geometry::data::multiply(*outputMatrix, inverse, *outputMatrix);
            }
            else
            {
                sight::geometry::data::multiply(*outputMatrix, *inputMatrix, *outputMatrix);
            }
        }
    }

    auto sig = outputMatrix->signal<data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);
    {
        core::com::Connection::Blocker block(sig->getConnection(slot(IService::slots::s_UPDATE)));
        sig->asyncEmit();
    }
}

// ----------------------------------------------------------------------------

service::IService::KeyConnectionsMap SConcatenateMatrices::getAutoConnections() const
{
    return {{s_MATRIX_GROUP_INPUT, data::Object::s_MODIFIED_SIG, IService::slots::s_UPDATE}};
}

// ----------------------------------------------------------------------------

} // namespace sight::module::geometry::base
