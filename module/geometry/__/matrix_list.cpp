/************************************************************************
 *
 * Copyright (C) 2018-2026 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

#include "module/geometry/__/matrix_list.hpp"

#include <iomanip>

namespace sight::module::geometry
{

//-----------------------------------------------------------------------------

matrix_list::matrix_list() noexcept
{
    new_signal<signals::matrix_added_t>(signals::MATRIX_ADDED);
    new_signal<signals::matrix_removed_t>(signals::MATRIX_REMOVED);

    new_slot(slots::SELECT_MATRIX, &matrix_list::select_matrix, this);
    new_slot(slots::REMOVE_MATRIX, &matrix_list::remove_matrix, this);
}

//-----------------------------------------------------------------------------

matrix_list::~matrix_list() noexcept =
    default;

//-----------------------------------------------------------------------------

void matrix_list::starting()
{
    // get inputs
    [[maybe_unused]] const std::size_t num_matrices = m_input_vector.size();
    [[maybe_unused]] const std::size_t num_selected = m_selected_vector.size();
    [[maybe_unused]] const std::size_t num_output   = m_output_vector.size();

    SIGHT_ASSERT(
        "the numbers of matrices, vectors and selected matrices should be the same",
        num_matrices == num_output && num_matrices == num_selected
    );
    SIGHT_ASSERT(
        "the numbers of matrices, vectors and selected matrices should be superior to one",
        num_matrices > 0 && num_output > 0 && num_selected > 0
    );
}

//-----------------------------------------------------------------------------

void matrix_list::stopping()
{
}

//-----------------------------------------------------------------------------

void matrix_list::configuring()
{
}

//-----------------------------------------------------------------------------

void matrix_list::updating()
{
    // Get the computed matrix from input group vector
    data::vector::sptr computed_vector;
    if(!m_input_vector.empty())
    {
        for(std::size_t i = 0 ; i < m_input_vector.size() ; ++i)
        {
            data::matrix4::sptr computed_matrix = std::make_shared<data::matrix4>();

            {
                const auto input = m_input_vector[i].lock();
                computed_matrix->deep_copy(input.get_shared());
            }

            // Fill the output vector group with the matrix
            auto computed_vector_ptr = m_output_vector[i].lock();
            computed_vector = computed_vector_ptr.get_shared();

            if(nullptr == computed_vector)
            {
                computed_vector = std::make_shared<data::vector>();
            }

            computed_vector->push_back(computed_matrix);
            this->set_output(computed_vector, VECTOR_INOUT, i);
            computed_vector->async_emit(data::vector::signals::ADDED_OBJECTS, computed_vector->get_content());
        }
    }

    // create string containing matrix values
    std::string str;
    data::matrix4::sptr computed_matrix = std::make_shared<data::matrix4>();
    {
        const auto input = m_input_vector[0].lock();
        computed_matrix->deep_copy(input.get_shared());
    }

    for(std::size_t i = 0 ; i < 4 ; ++i)
    {
        str += "[ ";
        for(std::size_t j = 0 ; j < 4 ; j++)
        {
            std::ostringstream out;
            out << std::setprecision(3) << (*computed_matrix)[i * 4 + j];
            str += out.str();
            str += " ";
        }

        str += "]";
        if(i < 3)
        {
            str += ", ";
        }
    }

    // notify
    const int index = static_cast<int>(computed_vector->size() - 1);

    // push the selected matrix
    this->select_matrix(index);
    this->async_emit(signals::MATRIX_ADDED, index, str);
}

//-----------------------------------------------------------------------------

void matrix_list::select_matrix(int _index)
{
    for(std::size_t i = 0 ; i < m_input_vector.size() ; ++i)
    {
        auto selected_matrix = m_selected_vector[i].lock();
        auto output_vector   = m_output_vector[i].lock();
        selected_matrix->deep_copy(
            std::dynamic_pointer_cast<data::matrix4>(
                (*output_vector)[static_cast<std::size_t>(_index)]
            )
        );

        selected_matrix->async_emit(data::signals::MODIFIED);
    }
}

//-----------------------------------------------------------------------------

void matrix_list::remove_matrix(int _index)
{
    if(!m_input_vector.empty())
    {
        for(std::size_t i = 0 ; i < m_input_vector.size() ; ++i)
        {
            auto output_vector = m_output_vector[i].lock();
            output_vector->erase(output_vector->begin() + _index);
            output_vector->async_emit(data::vector::signals::REMOVED_OBJECTS, output_vector->get_content());
        }

        this->async_emit(signals::MATRIX_REMOVED, _index);
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::geometry
