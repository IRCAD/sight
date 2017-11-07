/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "trackingCalibration/SHandEyeCalibration.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>

#include <fwData/List.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwServices/macros.hpp>

#include <boost/make_unique.hpp>

namespace trackingCalibration
{

fwServicesRegisterMacro(::arServices::IRegisterer, ::trackingCalibration::SHandEyeCalibration);

static const ::fwServices::IService::KeyType s_MATRIXLIST1_INPUT = "matrixList1";
static const ::fwServices::IService::KeyType s_MATRIXLIST2_INPUT = "matrixList2";

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

    ::fwData::List::csptr list1 = this->getInput< ::fwData::List>("s_MATRIXLIST1_INPUT");
    SLM_ASSERT("The second list is null", list1);

    ::fwData::List::csptr list2 = this->getInput< ::fwData::List>("s_MATRIXLIST2_INPUT");
    SLM_ASSERT("The second list is null", list2);

    SLM_ASSERT("The first and the second matrices list should have the same size", list1->size() == list2->size());

    ::fwData::List::ConstIteratorType it1, it2;

    it1 = list1->begin();
    it2 = list2->begin();

    for(; it1 != list1->end() && it2 != list2->end();
        ++it1, ++it2)
    {
        ::fwData::TransformationMatrix3D::sptr matrix1, matrix2;

        matrix1 = ::fwData::TransformationMatrix3D::dynamicCast(*it1);

        SLM_ASSERT("This element of the list is not a TransformationMatrix3D", matrix1);

        matrix2 = ::fwData::TransformationMatrix3D::dynamicCast(*it2);

        SLM_ASSERT("This element of the list is not a TransformationMatrix3D", matrix2);

        m_handEyeApi->pushMatrix(matrix1, matrix2);
    }

    ::fwData::TransformationMatrix3D::sptr result = m_handEyeApi->computeHandEye();

    matrix3D->deepCopy(result);

    auto sig = matrix3D->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
    sig->asyncEmit();

}

} // trackingCalibration
