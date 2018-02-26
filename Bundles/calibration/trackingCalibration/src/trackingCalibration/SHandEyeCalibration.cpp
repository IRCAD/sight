/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "trackingCalibration/SHandEyeCalibration.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>

#include <fwData/TransformationMatrix3D.hpp>
#include <fwData/Vector.hpp>

#include <fwDataTools/TransformationMatrix3D.hpp>

#include <fwServices/macros.hpp>

#include <boost/make_unique.hpp>

namespace trackingCalibration
{

fwServicesRegisterMacro(::fwServices::IRegisterer, ::trackingCalibration::SHandEyeCalibration);

static const ::fwServices::IService::KeyType s_MATRIXVECTOR1_INPUT = "matrixVector1";
static const ::fwServices::IService::KeyType s_MATRIXVECTOR2_INPUT = "matrixVector2";

// -----------------------------------------------------------------------------

SHandEyeCalibration::SHandEyeCalibration() noexcept :
    m_movingCamera(false)
{
}

// -----------------------------------------------------------------------------

SHandEyeCalibration::~SHandEyeCalibration() noexcept
{
}

// -----------------------------------------------------------------------------

void SHandEyeCalibration::configuring()
{
    const auto configTree = this->getConfigTree();
    const auto config     = configTree.get_child_optional("config.<xmlattr>");

    if(config)
    {
        m_movingCamera = config->get<bool>("movingCamera", false);
    }
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

void SHandEyeCalibration::computeRegistration(::fwCore::HiResClock::HiResClockType)
{
    ::fwCore::mt::ScopedLock lock(m_mutex);

    SLM_WARN_IF("Invoking computeHandEye while service is STOPPED", this->isStopped() );
    ::fwData::TransformationMatrix3D::sptr matrixX = this->getInOut< ::fwData::TransformationMatrix3D >("matrix");

    ::fwData::Vector::csptr vector1 = this->getInput< ::fwData::Vector >(s_MATRIXVECTOR1_INPUT);
    SLM_ASSERT("The first vector is null", vector1);

    ::fwData::Vector::csptr vector2 = this->getInput< ::fwData::Vector >(s_MATRIXVECTOR2_INPUT);
    SLM_ASSERT("The second vector is null", vector2);

    SLM_ASSERT("The first and the second matrices vector should have the same size and > 1",
               (vector1->size() == vector2->size()) && (vector1->size() > 1));

    ::fwData::Vector::ConstIteratorType it1, it2;

    it1 = vector1->begin();
    it2 = vector2->begin();

    std::vector< ::fwData::TransformationMatrix3D::csptr > AMatrices, BMatrices;

    for(; it1 != vector1->end()-1 && it2 != vector2->end()-1;
        ++it1, ++it2)
    {
        ::fwData::TransformationMatrix3D::sptr matrixAi,
        matrixAj,
        matrixBi,
        matrixBj;

        ::fwData::TransformationMatrix3D::sptr matrixAiInv = ::fwData::TransformationMatrix3D::New();
        ::fwData::TransformationMatrix3D::sptr matrixA     = ::fwData::TransformationMatrix3D::New();
        ::fwData::TransformationMatrix3D::sptr matrixB     = ::fwData::TransformationMatrix3D::New();

        matrixAi = ::fwData::TransformationMatrix3D::dynamicCast(*it1);
        SLM_ASSERT("This element of the vector is not a TransformationMatrix3D", matrixAi);
        matrixAj = ::fwData::TransformationMatrix3D::dynamicCast(*(it1 + 1));
        SLM_ASSERT("This element of the vector is not a TransformationMatrix3D", matrixAj);

        ::fwDataTools::TransformationMatrix3D::invert(matrixAi, matrixAiInv);
        ::fwDataTools::TransformationMatrix3D::multiply(matrixAj, matrixAiInv, matrixA);

        matrixBi = ::fwData::TransformationMatrix3D::dynamicCast(*it2);
        SLM_ASSERT("This element of the vector is not a TransformationMatrix3D", matrixBi);
        matrixBj = ::fwData::TransformationMatrix3D::dynamicCast(*(it2 + 1));
        SLM_ASSERT("This element of the vector is not a TransformationMatrix3D", matrixBj);

        if(m_movingCamera)
        {
            ::fwData::TransformationMatrix3D::sptr matrixBjInv = ::fwData::TransformationMatrix3D::New();
            ::fwDataTools::TransformationMatrix3D::invert(matrixBj, matrixBjInv);
            ::fwDataTools::TransformationMatrix3D::multiply(matrixBjInv, matrixBi, matrixB);
        }
        else
        {
            ::fwData::TransformationMatrix3D::sptr matrixBiInv = ::fwData::TransformationMatrix3D::New();
            ::fwDataTools::TransformationMatrix3D::invert(matrixBi, matrixBiInv);
            ::fwDataTools::TransformationMatrix3D::multiply(matrixBj, matrixBiInv, matrixB);
        }

        AMatrices.push_back(matrixA);
        BMatrices.push_back(matrixB);
    }

    ::handEyeCalibration::HandEyeApi handEyeApi;

    handEyeApi.setTransformLists(AMatrices, BMatrices);
    ::fwData::TransformationMatrix3D::sptr resultX = handEyeApi.computeHandEye();

    handEyeApi.setTransformLists(BMatrices, AMatrices);
    ::fwData::TransformationMatrix3D::sptr resultZ = handEyeApi.computeHandEye();

    // Result needs to be inverted because we want the position of the camera relative to the tracking system
    if(!m_movingCamera)
    {
        ::fwDataTools::TransformationMatrix3D::invert(resultX, matrixX);
    }
    else
    {
        matrixX->deepCopy(resultX);
    }

    auto sig = matrixX->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
    sig->asyncEmit();
}

} // trackingCalibration
