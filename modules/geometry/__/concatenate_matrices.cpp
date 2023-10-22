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
    const auto config                     = this->get_config();
    const auto in_cfg                     = config.get_child("in");
    [[maybe_unused]] const auto group_cfg = in_cfg.get_child_optional("<xmlattr>.group");
    SIGHT_ASSERT("config must contain one input group named 'matrix'.", group_cfg.has_value());
    SIGHT_ASSERT("Missing 'in group=\"matrix\"'", group_cfg->get_value<std::string>() == s_MATRIX_GROUP_INPUT);

    for(const auto& cfg : boost::make_iterator_range(in_cfg.equal_range("key")))
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
    auto output_matrix = m_output.lock();
    SIGHT_ASSERT("inout '" << s_OUTPUT << "' is not defined", output_matrix);
    {
        sight::geometry::data::identity(*output_matrix);

        data::matrix4 inverse;

        std::size_t index = 0;
        for(const bool invert_current_matrix : m_invertVector)
        {
            auto input_matrix = m_matrices[index++].lock();

            if(invert_current_matrix)
            {
                sight::geometry::data::invert(*input_matrix, inverse);
                sight::geometry::data::multiply(*output_matrix, inverse, *output_matrix);
            }
            else
            {
                sight::geometry::data::multiply(*output_matrix, *input_matrix, *output_matrix);
            }
        }
    }

    auto sig = output_matrix->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
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
