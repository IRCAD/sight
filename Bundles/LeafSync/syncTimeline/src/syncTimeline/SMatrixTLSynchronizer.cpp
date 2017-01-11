/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "syncTimeline/SMatrixTLSynchronizer.hpp"

#include <arData/MatrixTL.hpp>
#include <arData/timeline/Buffer.hpp>

#include <fwCom/Signal.hxx>

#include <fwData/TransformationMatrix3D.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwDataTools/helper/Array.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwServices/macros.hpp>

#include <fwTools/fwID.hpp>

#include <functional>
#include <sstream>

fwServicesRegisterMacro(::arServices::ISynchronizer, ::syncTimeline::SMatrixTLSynchronizer, ::arData::MatrixTL);

namespace syncTimeline
{

static const ::fwServices::IService::KeyType s_MATRIXTL_INPUT = "matrixTL";
static const ::fwServices::IService::KeyType s_MATRICES_INOUT = "matrices";

// ----------------------------------------------------------------------------

SMatrixTLSynchronizer::SMatrixTLSynchronizer() throw ()
{
}

// ----------------------------------------------------------------------------

void SMatrixTLSynchronizer::configuring() throw (::fwTools::Failed)
{
}

// ----------------------------------------------------------------------------

void SMatrixTLSynchronizer::starting() throw (::fwTools::Failed)
{
}

// ----------------------------------------------------------------------------

void SMatrixTLSynchronizer::stopping() throw (::fwTools::Failed)
{
}

// ----------------------------------------------------------------------------

void SMatrixTLSynchronizer::updating() throw (::fwTools::Failed)
{
    this->synchronize();
}

// ----------------------------------------------------------------------------

void SMatrixTLSynchronizer::synchronize()
{
    ::fwCore::HiResClock::HiResClockType currentTimestamp =
        std::numeric_limits< ::fwCore::HiResClock::HiResClockType >::max();

    ::arData::MatrixTL::csptr matrixTL           = this->getInput< ::arData::MatrixTL >(s_MATRIXTL_INPUT);
    CSPTR(::arData::MatrixTL::BufferType) buffer = matrixTL->getClosestBuffer(currentTimestamp);

    if(buffer)
    {
        std::stringstream matrixPrint;

        for (size_t i = 0; i< this->getKeyGroupSize(s_MATRICES_INOUT); ++i)
        {
            unsigned int index = static_cast< unsigned int >(i);
            if(buffer->isPresent(index))
            {
                const float* values = buffer->getElement(index);

                auto matrix = this->getInOut< ::fwData::TransformationMatrix3D >(s_MATRICES_INOUT, i);
                {
                    ::fwData::mt::ObjectWriteLock lock(matrix);

                    OSLM_ASSERT("Matrix['" << i << "] not found.", matrix);
                    matrixPrint << std::endl << "Matrix[" << i << "]" << std::endl;

                    for(unsigned int i = 0; i < 4; ++i)
                    {
                        for(unsigned int j = 0; j < 4; ++j)
                        {
                            matrix->setCoefficient(i,j,values[i*4+j]);
                            matrixPrint << values[i*4+j] << " ; ";

                        }
                        matrixPrint << std::endl;
                    }
                }
                auto sig = matrix->signal< ::fwData::Object::ModifiedSignalType >(
                    ::fwData::Object::s_MODIFIED_SIG);
                sig->asyncEmit();
            }
        }

        OSLM_DEBUG( std::endl <<matrixPrint.str());
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
