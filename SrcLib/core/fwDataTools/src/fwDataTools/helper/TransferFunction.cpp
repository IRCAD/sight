/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDataTools/helper/TransferFunction.hpp"

#include "fwDataTools/fieldHelper/Image.hpp"
#include "fwDataTools/fieldHelper/MedicalImageHelpers.hpp"
#include "fwDataTools/helper/Composite.hpp"

#include <fwCom/Slots.hxx>

#include <fwData/Image.hpp>

#include <fwServices/registry/ActiveWorkers.hpp>

namespace fwDataTools
{

namespace helper
{

static const ::fwCom::Slots::SlotKeyType s_UPDATE_TF_POINTS_SLOT    = "updateTFPoints";
static const ::fwCom::Slots::SlotKeyType s_UPDATE_TF_WINDOWING_SLOT = "updateTFWindowing";

//------------------------------------------------------------------------------

TransferFunction::TransferFunction()
{
    auto defaultWorker = ::fwServices::registry::ActiveWorkers::getDefaultWorker();
    m_slotUpdateTFPoints = ::fwCom::newSlot(&TransferFunction::updateTFPoints, this);
    m_slotUpdateTFPoints->setWorker(defaultWorker);
    m_slotUpdateTFWindowing = ::fwCom::newSlot(&TransferFunction::updateTFWindowing, this);
    m_slotUpdateTFWindowing->setWorker(defaultWorker);
}

//------------------------------------------------------------------------------

TransferFunction::~TransferFunction()
{
}

//------------------------------------------------------------------------------

void TransferFunction::createTransferFunction( ::fwData::Image::sptr image )
{
    ::fwData::Composite::sptr tfPool =
        image->setDefaultField(::fwDataTools::fieldHelper::Image::m_transferFunctionCompositeId,
                               ::fwData::Composite::New());

    // create the default transfer function in the image tf field if it does not exist
    if (tfPool->find(::fwData::TransferFunction::s_DEFAULT_TF_NAME) == tfPool->end())
    {
        ::fwData::TransferFunction::sptr tfGreyLevel = ::fwData::TransferFunction::createDefaultTF();
        if (image->getWindowWidth() != 0 )
        {
            tfGreyLevel->setWindow( image->getWindowWidth() );
            tfGreyLevel->setLevel( image->getWindowCenter() );
        }
        else if(::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(image))
        {
            double min, max;
            ::fwDataTools::fieldHelper::MedicalImageHelpers::getMinMax(image, min, max);
            ::fwData::TransferFunction::TFValuePairType wlMinMax(min, max);
            tfGreyLevel->setWLMinMax(wlMinMax);
        }

        ::fwDataTools::helper::Composite compositeHelper(tfPool);
        compositeHelper.add(::fwData::TransferFunction::s_DEFAULT_TF_NAME, tfGreyLevel);
        compositeHelper.notify();
    }

    if (m_transferFunction.expired())
    {
        ::fwData::TransferFunction::sptr tfGreyLevel =
            tfPool->at< ::fwData::TransferFunction >(::fwData::TransferFunction::s_DEFAULT_TF_NAME);
        m_transferFunction = tfGreyLevel;
    }
    else if (m_transferFunction.lock()->getTFValues().empty())
    {
        ::fwData::TransferFunction::sptr tfGreyLevel =
            tfPool->at< ::fwData::TransferFunction >(::fwData::TransferFunction::s_DEFAULT_TF_NAME);
        m_transferFunction.lock()->deepCopy(tfGreyLevel);
    }
}

//------------------------------------------------------------------------------v

void TransferFunction::setOrCreateTF(const fwData::TransferFunction::sptr& _tf, const fwData::Image::sptr& _image)
{
    this->removeTFConnections();
    if (_tf)
    {
        this->setTransferFunction(_tf);
    }
    else
    {
        this->createTransferFunction(_image);
    }
    this->installTFConnections();
}

//------------------------------------------------------------------------------

::fwData::TransferFunction::sptr TransferFunction::getTransferFunction() const
{
    SLM_ASSERT("Transfer funtion is not defined, you must call setTransferFunction() or createTransferFunction() first."
               , !m_transferFunction.expired());
    return m_transferFunction.lock();
}

//------------------------------------------------------------------------------

void TransferFunction::setTransferFunction(const ::fwData::TransferFunction::sptr& tf )
{
    m_transferFunction = tf;
}

//------------------------------------------------------------------------------

void TransferFunction::installTFConnections()
{
    ::fwCom::Connection connection;

    ::fwData::TransferFunction::sptr tf = this->getTransferFunction();

    connection = tf->signal(::fwData::TransferFunction::s_POINTS_MODIFIED_SIG)->connect(m_slotUpdateTFPoints);
    m_tfConnections.addConnection(connection);
    connection = tf->signal(::fwData::TransferFunction::s_WINDOWING_MODIFIED_SIG)->connect(m_slotUpdateTFWindowing);
    m_tfConnections.addConnection(connection);
}

//------------------------------------------------------------------------------

void TransferFunction::removeTFConnections()
{
    m_tfConnections.disconnect();
}

//------------------------------------------------------------------------------

void TransferFunction::updateTFPoints()
{
    SLM_ASSERT("This methods (updateTFPoints) must be reimplemented in subclass to manage TF modifications", false);
}

//------------------------------------------------------------------------------

void TransferFunction::updateTFWindowing(double /*window*/, double /*level*/)
{
    SLM_ASSERT("This methods (updateTFWindowing) must be reimplemented in subclass to manage TF modifications",
               false);
}

//------------------------------------------------------------------------------

} //namespace helper

} //namespace fwDataTools
