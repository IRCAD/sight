/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "trackingCalibration/SHandEyeCalibration.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>

#include <fwData/TransformationMatrix3D.hpp>
#include <fwData/Vector.hpp>

#include <fwServices/macros.hpp>

#include <boost/make_unique.hpp>

namespace trackingCalibration
{

fwServicesRegisterMacro(::arServices::IRegisterer, ::trackingCalibration::SHandEyeCalibration);

static const ::fwServices::IService::KeyType s_MATRIXVECTOR1_INPUT = "matrixVector1";
static const ::fwServices::IService::KeyType s_MATRIXVECTOR2_INPUT = "matrixVector2";

// -----------------------------------------------------------------------------

SHandEyeCalibration::SHandEyeCalibration() noexcept :
    m_lastTimestamp(0)
{
    m_handEyeApi = ::boost::make_unique< ::handEyeCalibration::HandEyeApi >();
}

// -----------------------------------------------------------------------------

SHandEyeCalibration::~SHandEyeCalibration() noexcept
{
}

// -----------------------------------------------------------------------------

void SHandEyeCalibration::configuring()
{

}

// -----------------------------------------------------------------------------

void SHandEyeCalibration::starting()
{

}

// -----------------------------------------------------------------------------

void SHandEyeCalibration::stopping()
{

}

// -----------------------------------------------------------------------------

void SHandEyeCalibration::updating()
{

}

// -----------------------------------------------------------------------------

void SHandEyeCalibration::computeRegistration(::fwCore::HiResClock::HiResClockType timestamp)
{
    ::fwCore::mt::ScopedLock lock(m_mutex);

    SLM_WARN_IF("Invoking computeHandEye while service is STOPPED", this->isStopped() );
    ::fwData::TransformationMatrix3D::sptr matrix3D = this->getInOut< ::fwData::TransformationMatrix3D >("matrix");

    ::fwData::Vector::csptr vector1 = this->getInput< ::fwData::Vector >(s_MATRIXVECTOR1_INPUT);
    SLM_ASSERT("The first vector is null", vector1);

    ::fwData::Vector::csptr vector2 = this->getInput< ::fwData::Vector >(s_MATRIXVECTOR2_INPUT);
    SLM_ASSERT("The second vector is null", vector2);

    SLM_ASSERT("The first and the second matrices vector should have the same size and > 1",
               (vector1->size() == vector2->size()) && (vector1->size() > 1));

    ::fwData::Vector::ConstIteratorType it1, it2;

    it1 = vector1->begin();
    it2 = vector2->begin();

    for(; it1 != vector1->end() && it2 != vector2->end();
        ++it1, ++it2)
    {
        ::fwData::TransformationMatrix3D::sptr matrix1, matrix2;

        matrix1 = ::fwData::TransformationMatrix3D::dynamicCast(*it1);

        SLM_ASSERT("This element of the vector is not a TransformationMatrix3D", matrix1);

        matrix2 = ::fwData::TransformationMatrix3D::dynamicCast(*it2);

        SLM_ASSERT("This element of the vector is not a TransformationMatrix3D", matrix2);

        m_handEyeApi->pushMatrix(matrix1, matrix2);
    }

    ::fwData::TransformationMatrix3D::sptr result = m_handEyeApi->computeHandEye();

    matrix3D->deepCopy(result);

    auto sig = matrix3D->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
    sig->asyncEmit();

}

} // trackingCalibration
