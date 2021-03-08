/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
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

#include "modules/geometry/base/SMatrixList.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>

#include <service/macros.hpp>

#include <iomanip>

namespace sight::module::geometry::base
{
static const core::com::Signals::SignalKeyType s_MATRIX_ADDED_SIG   = "matrixAdded";
static const core::com::Signals::SignalKeyType s_MATRIX_REMOVED_SIG = "matrixRemoved";

static const core::com::Slots::SlotKeyType s_SELECT_MATRIX_SLOT = "selectMatrix";
static const core::com::Slots::SlotKeyType s_REMOVE_MATRIX_SLOT = "removeMatrix";

const service::IService::KeyType s_MATRICES_INOUT = "matrices";
const service::IService::KeyType s_VECTOR_INOUT   = "vector";
const service::IService::KeyType s_SELECTED_INOUT = "selectedMatrix";

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

SMatrixList::SMatrixList() noexcept
{
    newSignal<MatrixAddedSignalType>(s_MATRIX_ADDED_SIG);
    newSignal<MatrixRemovedSignalType>(s_MATRIX_REMOVED_SIG);

    newSlot(s_SELECT_MATRIX_SLOT, &SMatrixList::selectMatrix, this);
    newSlot(s_REMOVE_MATRIX_SLOT, &SMatrixList::removeMatrix, this);
}

//-----------------------------------------------------------------------------

SMatrixList::~SMatrixList() noexcept
{
}

//-----------------------------------------------------------------------------

void SMatrixList::starting()
{
    m_inputVector.clear();
    m_outputVector.clear();
    m_selectedVector.clear();

    // get inputs
    const size_t numMatrices = this->getKeyGroupSize(s_MATRICES_INOUT);
    const size_t numOutput   = this->getKeyGroupSize(s_VECTOR_INOUT);
    const size_t numSelected = this->getKeyGroupSize(s_SELECTED_INOUT);

    SIGHT_ASSERT("the numbers of matrices, vectors and selected matrices should be the same",
                 numMatrices == numOutput && numMatrices == numSelected );
    SIGHT_ASSERT("the numbers of matrices, vectors and selected matrices should be superior to one",
                 numMatrices > 0 && numOutput > 0 && numSelected > 0 );

    for(size_t j = 0; j < numMatrices; ++j)
    {
        m_inputVector.push_back(this->getLockedInOut< data::Matrix4 >(s_MATRICES_INOUT,
                                                                      j).get_shared());
        // create vector and push it back into the main vector
        m_outputVector.push_back(data::Vector::New());
        m_selectedVector.push_back(this->getLockedInOut< data::Matrix4 >(s_SELECTED_INOUT,
                                                                         j).get_shared());
    }

    if(m_inputVector.empty() || m_selectedVector.empty() || m_outputVector.empty() )
    {
        SIGHT_ERROR("No input matrices found!");
        return;
    }
}

//-----------------------------------------------------------------------------

void SMatrixList::stopping()
{
}

//-----------------------------------------------------------------------------

void SMatrixList::configuring()
{
}

//-----------------------------------------------------------------------------

void SMatrixList::updating()
{
    // Get the computed matrix from input group vector

    data::Vector::sptr computedVector;
    if(m_inputVector.size() > 0)
    {
        for(int i = 0; i < m_inputVector.size(); ++i)
        {
            data::Matrix4::sptr computedMatrix = data::Matrix4::New();
            computedMatrix->deepCopy(m_inputVector[i]);

            // Fill the output vector group with the matrix
            computedVector = m_outputVector[i];

            if( nullptr == computedVector )
            {
                computedVector = data::Vector::New();
            }

            computedVector->getContainer().push_back(computedMatrix);
            this->setOutput(s_VECTOR_INOUT, computedVector, i );
            auto sig = m_outputVector[i]->signal< data::Vector::AddedObjectsSignalType >
                           (data::Vector::s_ADDED_OBJECTS_SIG);
            sig->asyncEmit(m_outputVector[i]->getContainer());
        }
    }

    // create string containing matrix values
    std::string str;
    data::Matrix4::sptr computedMatrix = data::Matrix4::New();
    computedMatrix->deepCopy(m_inputVector[0]);
    const data::Matrix4::TMCoefArray& coef = computedMatrix->getCoefficients();
    for(int i = 0; i < 4; ++i)
    {
        str += "[ ";
        for(int j = 0; j < 4; j++)
        {
            std::ostringstream out;
            out << std::setprecision(3) << coef[i*4 +j];
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
    const int index = static_cast<int>(computedVector->size() -1);

    // push the selected matrix
    this->selectMatrix(index);

    this->signal<MatrixAddedSignalType>(s_MATRIX_ADDED_SIG)->asyncEmit(index, str);

}

//-----------------------------------------------------------------------------

void SMatrixList::selectMatrix(int index)
{
    for(int i = 0; i < m_inputVector.size(); ++i)
    {
        data::Matrix4::sptr selectedMatrix = m_selectedVector[i];
        selectedMatrix->deepCopy(data::Matrix4::dynamicCast(m_outputVector[i]->getContainer()[
                                                                index]));

        auto sig = selectedMatrix->signal< data::Matrix4::ModifiedSignalType >(
            data::Matrix4::s_MODIFIED_SIG);
        sig->asyncEmit();
    }
}

//-----------------------------------------------------------------------------

void SMatrixList::removeMatrix(int _index)
{

    if(m_inputVector.size() > 0)
    {
        for(int i = 0; i < m_inputVector.size(); ++i)
        {
            data::Vector::ContainerType& vec = m_outputVector[i]->getContainer();
            vec.erase(vec.begin() + _index);

            auto sig = m_outputVector[i]->signal< data::Vector::RemovedObjectsSignalType >
                           (data::Vector::s_REMOVED_OBJECTS_SIG);
            sig->asyncEmit(m_outputVector[i]->getContainer());
        }
        this->signal<MatrixRemovedSignalType>(s_MATRIX_REMOVED_SIG)->asyncEmit(_index);
    }

}

//-----------------------------------------------------------------------------

} // maths
