/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
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

#include "modules/geometry/__/matrix_list.hpp"

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
    new_signal<MatrixAddedSignalType>(MATRIX_ADDED_SIG);
    new_signal<MatrixRemovedSignalType>(MATRIX_REMOVED_SIG);

    new_slot(SELECT_MATRIX_SLOT, &matrix_list::selectMatrix, this);
    new_slot(REMOVE_MATRIX_SLOT, &matrix_list::removeMatrix, this);
}

//-----------------------------------------------------------------------------

matrix_list::~matrix_list() noexcept =
    default;

//-----------------------------------------------------------------------------

void matrix_list::starting()
{
    // get inputs
    [[maybe_unused]] const std::size_t numMatrices = m_inputVector.size();
    [[maybe_unused]] const std::size_t numSelected = m_selectedVector.size();
    [[maybe_unused]] const std::size_t numOutput   = m_outputVector.size();

    SIGHT_ASSERT(
        "the numbers of matrices, vectors and selected matrices should be the same",
        numMatrices == numOutput && numMatrices == numSelected
    );
    SIGHT_ASSERT(
        "the numbers of matrices, vectors and selected matrices should be superior to one",
        numMatrices > 0 && numOutput > 0 && numSelected > 0
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
    data::vector::sptr computedVector;
    if(m_inputVector.size() > 0)
    {
        for(std::size_t i = 0 ; i < m_inputVector.size() ; ++i)
        {
            data::matrix4::sptr computedMatrix = std::make_shared<data::matrix4>();

            {
                const auto input = m_inputVector[i].lock();
                computedMatrix->deep_copy(input.get_shared());
            }

            // Fill the output vector group with the matrix
            auto computedVectorPtr = m_outputVector[i].lock();
            computedVector = computedVectorPtr.get_shared();

            if(nullptr == computedVector)
            {
                computedVector = std::make_shared<data::vector>();
            }

            computedVector->push_back(computedMatrix);
            this->set_output(s_VECTOR_INOUT, computedVector, i);
            auto sig = computedVector->signal<data::vector::added_signal_t>
                           (data::vector::ADDED_OBJECTS_SIG);
            sig->async_emit(computedVector->get_content());
        }
    }

    // create string containing matrix values
    std::string str;
    data::matrix4::sptr computedMatrix = std::make_shared<data::matrix4>();
    {
        const auto input = m_inputVector[0].lock();
        computedMatrix->deep_copy(input.get_shared());
    }

    for(std::size_t i = 0 ; i < 4 ; ++i)
    {
        str += "[ ";
        for(std::size_t j = 0 ; j < 4 ; j++)
        {
            std::ostringstream out;
            out << std::setprecision(3) << (*computedMatrix)[i * 4 + j];
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
    const int index = static_cast<int>(computedVector->size() - 1);

    // push the selected matrix
    this->selectMatrix(index);

    this->signal<MatrixAddedSignalType>(MATRIX_ADDED_SIG)->async_emit(index, str);
}

//-----------------------------------------------------------------------------

void matrix_list::selectMatrix(int index)
{
    for(std::size_t i = 0 ; i < m_inputVector.size() ; ++i)
    {
        auto selectedMatrix = m_selectedVector[i].lock();
        auto outputVector   = m_outputVector[i].lock();
        selectedMatrix->deep_copy(std::dynamic_pointer_cast<data::matrix4>((*outputVector)[std::size_t(index)]));

        auto sig = selectedMatrix->signal<data::matrix4::ModifiedSignalType>(data::matrix4::MODIFIED_SIG);
        sig->async_emit();
    }
}

//-----------------------------------------------------------------------------

void matrix_list::removeMatrix(int _index)
{
    if(m_inputVector.size() > 0)
    {
        for(std::size_t i = 0 ; i < m_inputVector.size() ; ++i)
        {
            auto outputVector = m_outputVector[i].lock();
            outputVector->erase(outputVector->begin() + _index);

            auto sig = outputVector->signal<data::vector::removed_signal_t>
                           (data::vector::REMOVED_OBJECTS_SIG);
            sig->async_emit(outputVector->get_content());
        }

        this->signal<MatrixRemovedSignalType>(MATRIX_REMOVED_SIG)->async_emit(_index);
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::geometry
