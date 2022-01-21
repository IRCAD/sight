/************************************************************************
 *
 * Copyright (C) 2014-2022 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "SMatrixTLSynchronizer.hpp"

#include <core/com/Signal.hxx>

namespace sight::module::sync
{

// -----------------------------------------------------------------------------

static const core::com::Signals::SignalKeyType MATRIX_SYNCHRONIZED_SIG   = "matrixSynchronized";
static const core::com::Signals::SignalKeyType MATRIX_UNSYNCHRONIZED_SIG = "matrixUnsynchronized";

// ----------------------------------------------------------------------------

SMatrixTLSynchronizer::SMatrixTLSynchronizer() noexcept
{
    newSignal<MatrixSynchronizedSignalType>(MATRIX_SYNCHRONIZED_SIG);
    newSignal<MatrixUnsynchronizedSignalType>(MATRIX_UNSYNCHRONIZED_SIG);
}

// ----------------------------------------------------------------------------

void SMatrixTLSynchronizer::configuring()
{
}

// ----------------------------------------------------------------------------

void SMatrixTLSynchronizer::starting()
{
}

// ----------------------------------------------------------------------------

void SMatrixTLSynchronizer::stopping()
{
}

// ----------------------------------------------------------------------------

void SMatrixTLSynchronizer::updating()
{
    this->synchronize();
}

// ----------------------------------------------------------------------------

void SMatrixTLSynchronizer::synchronize()
{
    const auto currentTimestamp = std::numeric_limits<core::HiResClock::HiResClockType>::max();

    const auto matrixTL = m_matrixTL.lock();
    CSPTR(data::MatrixTL::BufferType) buffer = matrixTL->getClosestBuffer(currentTimestamp);

    if(buffer)
    {
        std::stringstream matrixPrint;

        for(std::size_t matrixIndex = 0 ; matrixIndex < m_matrices.size() ; ++matrixIndex)
        {
            const unsigned int index = static_cast<unsigned int>(matrixIndex);

            if(buffer->isPresent(index))
            {
                const float* values = buffer->getElement(index);

                auto matrix = m_matrices[matrixIndex].lock();
                SIGHT_ASSERT("Matrix['" << matrixIndex << "] not found.", matrix);

                matrixPrint << std::endl << "Matrix[" << matrixIndex << "]" << std::endl;

                for(std::uint8_t i = 0 ; i < 4 ; ++i)
                {
                    for(std::uint8_t j = 0 ; j < 4 ; ++j)
                    {
                        matrix->setCoefficient(i, j, static_cast<double>(values[i * 4 + j]));
                        matrixPrint << values[i * 4 + j] << " ; ";
                    }

                    matrixPrint << std::endl;
                }

                this->signal<MatrixSynchronizedSignalType>(MATRIX_SYNCHRONIZED_SIG)->asyncEmit(int(index));
                auto sig = matrix->signal<data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);
                sig->asyncEmit();
            }
            else
            {
                this->signal<MatrixUnsynchronizedSignalType>(MATRIX_UNSYNCHRONIZED_SIG)->asyncEmit(int(index));
            }
        }

        SIGHT_DEBUG(std::endl << matrixPrint.str());
    }
}

// ----------------------------------------------------------------------------

service::IService::KeyConnectionsMap SMatrixTLSynchronizer::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_MATRIXTL_INPUT, data::MatrixTL::s_OBJECT_PUSHED_SIG, s_UPDATE_SLOT);
    return connections;
}

// ----------------------------------------------------------------------------

} // namespace sight::module::sync
