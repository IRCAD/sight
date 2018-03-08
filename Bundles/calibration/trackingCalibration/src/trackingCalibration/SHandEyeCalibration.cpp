/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "trackingCalibration/SHandEyeCalibration.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/TransformationMatrix3D.hpp>
#include <fwData/Vector.hpp>

#include <fwDataTools/TransformationMatrix3D.hpp>

#include <fwGuiQt/dialog/MessageDialog.hpp>

#include <fwServices/macros.hpp>

namespace trackingCalibration
{

fwServicesRegisterMacro(::fwServices::IRegisterer, ::trackingCalibration::SHandEyeCalibration);

static const ::fwCom::Slots::SlotKeyType s_SET_MOVING_CAMERA = "setMovingCamera";

static const ::fwServices::IService::KeyType s_MATRIXVECTOR1_INPUT = "matrixVector1";
static const ::fwServices::IService::KeyType s_MATRIXVECTOR2_INPUT = "matrixVector2";

// -----------------------------------------------------------------------------

SHandEyeCalibration::SHandEyeCalibration() noexcept :
    m_movingCamera(false)
{
    newSlot(s_SET_MOVING_CAMERA, &SHandEyeCalibration::setMovingCamera, this);
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
    ::fwData::TransformationMatrix3D::sptr matrixX = this->getInOut< ::fwData::TransformationMatrix3D >("matX");
    ::fwData::TransformationMatrix3D::sptr matrixZ = this->getInOut< ::fwData::TransformationMatrix3D >("matZ");
    SLM_ASSERT("Need Z or X output matrix", matrixX || matrixZ);

    ::fwData::Vector::csptr vector1 = this->getInput< ::fwData::Vector >(s_MATRIXVECTOR1_INPUT);
    SLM_ASSERT("The first vector is null", vector1);

    ::fwData::Vector::csptr vector2 = this->getInput< ::fwData::Vector >(s_MATRIXVECTOR2_INPUT);
    SLM_ASSERT("The second vector is null", vector2);

    SLM_ASSERT("The first and the second matrices vector should have the same size",
               (vector1->size() == vector2->size()));

    if(vector1->size() < 2)
    {
        ::fwGuiQt::dialog::MessageDialog::sptr dialog = ::fwGuiQt::dialog::MessageDialog::New();
        dialog->setTitle("Hand-eye Calibration Error");
        dialog->setIcon(::fwGui::dialog::IMessageDialog::Icons::WARNING);
        dialog->setMessage("Hand-eye calibration needs at least 2 pairs of matrices.");
        dialog->show();
        return;
    }

    ::fwData::TransformationMatrix3D::sptr matrixAi = ::fwData::TransformationMatrix3D::dynamicCast(vector1->at(0));
    SLM_ASSERT("This element of the vector is not a TransformationMatrix3D", matrixAi);
    ::fwData::TransformationMatrix3D::sptr matrixBi = ::fwData::TransformationMatrix3D::dynamicCast(vector2->at(0));
    SLM_ASSERT("This element of the vector is not a TransformationMatrix3D", matrixBi);

    ::fwData::TransformationMatrix3D::sptr matrixAiInv = ::fwData::TransformationMatrix3D::New();
    ::fwData::TransformationMatrix3D::sptr matrixBiInv = ::fwData::TransformationMatrix3D::New();

    ::fwDataTools::TransformationMatrix3D::invert(matrixAi, matrixAiInv);
    ::fwDataTools::TransformationMatrix3D::invert(matrixBi, matrixBiInv);

    std::vector< ::fwData::TransformationMatrix3D::csptr > AMatrices, BMatrices, AMatricesCoB, BMatricesCoB;

    for(size_t index = 1; index < vector1->size(); ++index)
    {
        ::fwData::TransformationMatrix3D::sptr matrixAj, matrixBj;

        ::fwData::TransformationMatrix3D::sptr matrixAjInv = ::fwData::TransformationMatrix3D::New();
        ::fwData::TransformationMatrix3D::sptr matrixBjInv = ::fwData::TransformationMatrix3D::New();
        //Displacement from the other point of view (i.e. a Change of Base)
        //ACoB is the displacement matrix in A's destination basis
        ::fwData::TransformationMatrix3D::sptr matrixACoB = ::fwData::TransformationMatrix3D::New();
        ::fwData::TransformationMatrix3D::sptr matrixBCoB = ::fwData::TransformationMatrix3D::New();
        //A is the displacement matrix in A's source basis
        ::fwData::TransformationMatrix3D::sptr matrixA = ::fwData::TransformationMatrix3D::New();
        ::fwData::TransformationMatrix3D::sptr matrixB = ::fwData::TransformationMatrix3D::New();

        matrixAj = ::fwData::TransformationMatrix3D::dynamicCast(vector1->at(index));
        SLM_ASSERT("This element of the vector is not a TransformationMatrix3D", matrixAj);

        ::fwDataTools::TransformationMatrix3D::invert(matrixAj, matrixAjInv);

        ::fwDataTools::TransformationMatrix3D::multiply(matrixAjInv, matrixAi, matrixA);
        ::fwDataTools::TransformationMatrix3D::multiply(matrixAj, matrixAiInv, matrixACoB);

        matrixBj = ::fwData::TransformationMatrix3D::dynamicCast(vector2->at(index));
        SLM_ASSERT("This element of the vector is not a TransformationMatrix3D", matrixBj);

        ::fwDataTools::TransformationMatrix3D::invert(matrixBj, matrixBjInv);

        if(m_movingCamera)
        {
            //B is the displacement matrix in B's destination basis
            ::fwDataTools::TransformationMatrix3D::multiply(matrixBj, matrixBiInv, matrixB);

            //BCoB is the displacement matrix in B's source basis
            ::fwDataTools::TransformationMatrix3D::multiply(matrixBjInv, matrixBi, matrixBCoB);
        }
        else
        {
            //B is the displacement matrix in B's source basis
            ::fwDataTools::TransformationMatrix3D::multiply(matrixBjInv, matrixBi, matrixB);

            //BCoB is the displacement matrix in B's destination basis
            ::fwDataTools::TransformationMatrix3D::multiply(matrixBj, matrixBiInv, matrixBCoB);
        }

        AMatrices.push_back(matrixA);
        BMatrices.push_back(matrixB);

        AMatricesCoB.push_back(matrixACoB);
        BMatricesCoB.push_back(matrixBCoB);
    }

    ::handEyeCalibration::HandEyeApi handEyeApi;

    if(matrixX)
    {
        handEyeApi.setTransformLists(AMatrices, BMatrices);
        ::fwData::TransformationMatrix3D::sptr resultX = handEyeApi.computeHandEye();

        matrixX->deepCopy(resultX);

        auto sig = matrixX->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
        sig->asyncEmit();
    }

    if(matrixZ)
    {
        handEyeApi.setTransformLists(AMatricesCoB, BMatricesCoB);
        ::fwData::TransformationMatrix3D::sptr resultZ = handEyeApi.computeHandEye();

        matrixZ->deepCopy(resultZ);

        auto sig = matrixZ->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
        sig->asyncEmit();
    }

}

// -----------------------------------------------------------------------------

void SHandEyeCalibration::setMovingCamera(bool movingCamera)
{
    m_movingCamera = movingCamera;
}

} // trackingCalibration
