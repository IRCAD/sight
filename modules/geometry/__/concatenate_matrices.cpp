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

#include "modules/geometry/__/concatenate_matrices.hpp"

#include <core/com/signal.hxx>

#include <geometry/data/matrix4.hpp>

#include <boost/range/iterator_range_core.hpp>

namespace sight::module::geometry
{

// ----------------------------------------------------------------------------

void concatenate_matrices::configuring()
{
    const auto config                    = this->get_config();
    const auto inCfg                     = config.get_child("in");
    [[maybe_unused]] const auto groupCfg = inCfg.get_child_optional("<xmlattr>.group");
    SIGHT_ASSERT("config must contain one input group named 'matrix'.", groupCfg.has_value());
    SIGHT_ASSERT("Missing 'in group=\"matrix\"'", groupCfg->get_value<std::string>() == s_MATRIX_GROUP_INPUT);

    for(const auto& cfg : boost::make_iterator_range(inCfg.equal_range("key")))
    {
        const auto inverse = cfg.second.get<bool>("<xmlattr>.inverse", false);
        m_invertVector.push_back(inverse);
    }
}

// ----------------------------------------------------------------------------

void concatenate_matrices::starting()
{
}

// ----------------------------------------------------------------------------

void concatenate_matrices::stopping()
{
}

// ----------------------------------------------------------------------------

void concatenate_matrices::updating()
{
    auto outputMatrix = m_output.lock();
    SIGHT_ASSERT("inout '" << s_OUTPUT << "' is not defined", outputMatrix);
    {
        sight::geometry::data::identity(*outputMatrix);

        data::matrix4 inverse;

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

    auto sig = outputMatrix->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG);
    {
        core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));
        sig->async_emit();
    }
}

// ----------------------------------------------------------------------------

service::connections_t concatenate_matrices::auto_connections() const
{
    return {{s_MATRIX_GROUP_INPUT, data::object::MODIFIED_SIG, service::slots::UPDATE}};
}

// ----------------------------------------------------------------------------

} // namespace sight::module::geometry
