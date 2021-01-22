/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
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

#include "syncTimeline/SMatrixTLSynchronizer.hpp"

#include <arData/MatrixTL.hpp>
#include <arData/timeline/Buffer.hpp>

#include <core/com/Signal.hxx>
#include <core/tools/fwID.hpp>

#include <fwData/mt/ObjectWriteLock.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwServices/macros.hpp>

#include <functional>
#include <sstream>

namespace syncTimeline
{

// -----------------------------------------------------------------------------
fwServicesRegisterMacro(::arServices::ISynchronizer, ::syncTimeline::SMatrixTLSynchronizer, ::arData::MatrixTL)

static const core::com::Signals::SignalKeyType MATRIX_SYNCHRONIZED_SIG = "matrixSynchronized";
static const core::com::Signals::SignalKeyType MATRIX_UNSYNCHRONIZED_SIG = "matrixUnsynchronized";

static const ::fwServices::IService::KeyType s_MATRIXTL_INPUT = "matrixTL";
static const ::fwServices::IService::KeyType s_MATRICES_INOUT = "matrices";

// ----------------------------------------------------------------------------

SMatrixTLSynchronizer::SMatrixTLSynchronizer() noexcept
{
    newSignal< MatrixSynchronizedSignalType>(MATRIX_SYNCHRONIZED_SIG);
    newSignal< MatrixUnsynchronizedSignalType>(MATRIX_UNSYNCHRONIZED_SIG);
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
    core::HiResClock::HiResClockType currentTimestamp =
        std::numeric_limits< core::HiResClock::HiResClockType >::max();

    ::arData::MatrixTL::csptr matrixTL           = this->getInput< ::arData::MatrixTL >(s_MATRIXTL_INPUT);
    CSPTR(::arData::MatrixTL::BufferType) buffer = matrixTL->getClosestBuffer(currentTimestamp);

    if(buffer)
    {
        std::stringstream matrixPrint;

        for(size_t matrixIndex = 0; matrixIndex < this->getKeyGroupSize(s_MATRICES_INOUT); ++matrixIndex)
        {
            const unsigned int index = static_cast< unsigned int >(matrixIndex);

            if(buffer->isPresent(index))
            {
                const float* values = buffer->getElement(index);
                auto matrix         = this->getInOut< ::fwData::TransformationMatrix3D >(s_MATRICES_INOUT, matrixIndex);

                {
                    ::fwData::mt::ObjectWriteLock lock(matrix);
                    SLM_ASSERT("Matrix['" << matrixIndex << "] not found.", matrix);

                    matrixPrint << std::endl << "Matrix[" << matrixIndex << "]" << std::endl;

                    for(std::uint8_t i = 0; i < 4; ++i)
                    {
                        for(std::uint8_t j = 0; j < 4; ++j)
                        {
                            matrix->setCoefficient(i, j, static_cast<double>(values[i*4+j]));
                            matrixPrint << values[i*4+j] << " ; ";

                        }
                        matrixPrint << std::endl;
                    }
                }
                this->signal<MatrixSynchronizedSignalType>(MATRIX_SYNCHRONIZED_SIG)->asyncEmit(index);
                auto sig = matrix->signal< ::fwData::Object::ModifiedSignalType >(
                    ::fwData::Object::s_MODIFIED_SIG);
                sig->asyncEmit();
            }
            else
            {
                this->signal<MatrixUnsynchronizedSignalType>(MATRIX_UNSYNCHRONIZED_SIG)->asyncEmit(index);
            }
        }

        SLM_DEBUG( std::endl <<matrixPrint.str());
    }
}

// ----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SMatrixTLSynchronizer::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push( s_MATRIXTL_INPUT, ::arData::MatrixTL::s_OBJECT_PUSHED_SIG, s_UPDATE_SLOT );

    return connections;
}

// ----------------------------------------------------------------------------
}  // namespace syncTimeline
