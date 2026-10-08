/************************************************************************
 *
 * Copyright (C) 2014-2026 IRCAD France
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

#include "module/geometry/__/concatenate_matrices.hpp"

#include <geometry/data/matrix4.hpp>

#include <boost/range/iterator_range_core.hpp>

namespace sight::module::geometry
{

// ----------------------------------------------------------------------------

void concatenate_matrices::configuring()
{
    m_invert_vector.clear();
    for(const auto& cfg : boost::make_iterator_range(this->get_config().equal_range("data")))
    {
        if(auto input = cfg.second.get_optional<std::string>("<xmlattr>.input"); input.has_value() && !input->empty())
        {
            m_invert_vector.push_back(cfg.second.get<bool>("<xmlattr>.inverse", false));
        }
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
    SIGHT_ASSERT("inout '" << OUTPUT << "' is not defined", output_matrix);
    {
        sight::geometry::data::identity(*output_matrix);

        data::matrix4 inverse;

        std::size_t index = 0;
        for(const bool invert_current_matrix : m_invert_vector)
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

    output_matrix->async_emit(this, data::signals::MODIFIED);
}

// ----------------------------------------------------------------------------

service::connections_t concatenate_matrices::auto_connections() const
{
    return {{MATRIX_INPUT, data::signals::MODIFIED, service::slots::UPDATE}};
}

// ----------------------------------------------------------------------------

} // namespace sight::module::geometry
