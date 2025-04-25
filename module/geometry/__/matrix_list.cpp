/************************************************************************
 *
 * Copyright (C) 2018-2024 IRCAD France
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

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <iomanip>

namespace sight::module::geometry
{

//-----------------------------------------------------------------------------

static const core::com::signals::key_t MATRIX_ADDED_SIG   = "matrixAdded";
static const core::com::signals::key_t MATRIX_REMOVED_SIG = "matrixRemoved";

static const core::com::slots::key_t SELECT_MATRIX_SLOT = "selectMatrix";
static const core::com::slots::key_t REMOVE_MATRIX_SLOT = "removeMatrix";

//-----------------------------------------------------------------------------

matrix_list::matrix_list() noexcept
{
    new_signal<matrix_added_signal_t>(MATRIX_ADDED_SIG);
    new_signal<matrix_removed_signal_t>(MATRIX_REMOVED_SIG);

    new_slot(SELECT_MATRIX_SLOT, &matrix_list::select_matrix, this);
    new_slot(REMOVE_MATRIX_SLOT, &matrix_list::remove_matrix, this);
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
    if(m_input_vector.size() > 0)
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
            auto sig = computed_vector->signal<data::vector::added_signal_t>
                           (data::vector::ADDED_OBJECTS_SIG);
            sig->async_emit(computed_vector->get_content());
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

    this->signal<matrix_added_signal_t>(MATRIX_ADDED_SIG)->async_emit(index, str);
}

//-----------------------------------------------------------------------------

void matrix_list::select_matrix(int _index)
{
    for(std::size_t i = 0 ; i < m_input_vector.size() ; ++i)
    {
        auto selected_matrix = m_selected_vector[i].lock();
        auto output_vector   = m_output_vector[i].lock();
        selected_matrix->deep_copy(std::dynamic_pointer_cast<data::matrix4>((*output_vector)[std::size_t(_index)]));

        auto sig = selected_matrix->signal<data::matrix4::modified_signal_t>(data::matrix4::MODIFIED_SIG);
        sig->async_emit();
    }
}

//-----------------------------------------------------------------------------

void matrix_list::remove_matrix(int _index)
{
    if(m_input_vector.size() > 0)
    {
        for(std::size_t i = 0 ; i < m_input_vector.size() ; ++i)
        {
            auto output_vector = m_output_vector[i].lock();
            output_vector->erase(output_vector->begin() + _index);

            auto sig = output_vector->signal<data::vector::removed_signal_t>
                           (data::vector::REMOVED_OBJECTS_SIG);
            sig->async_emit(output_vector->get_content());
        }

        this->signal<matrix_removed_signal_t>(MATRIX_REMOVED_SIG)->async_emit(_index);
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::geometry
