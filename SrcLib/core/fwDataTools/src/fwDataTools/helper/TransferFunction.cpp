/************************************************************************
 *
 * Copyright (C) 2018 IRCAD France
 * Copyright (C) 2018 IHU Strasbourg
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

#include "fwDataTools/helper/TransferFunction.hpp"

#include "fwDataTools/fieldHelper/Image.hpp"
#include "fwDataTools/fieldHelper/MedicalImageHelpers.hpp"
#include "fwDataTools/helper/Composite.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/Image.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwServices/registry/ActiveWorkers.hpp>

namespace fwDataTools
{

namespace helper
{

//------------------------------------------------------------------------------

TransferFunction::TransferFunction(const std::function<void()>& _function) :
    m_updateTFPoints(_function)
{
    auto defaultWorker = ::fwServices::registry::ActiveWorkers::getDefaultWorker();
    m_slotUpdateTFPoints = ::fwCom::newSlot(&TransferFunction::updateTFPoints, this);
    m_slotUpdateTFPoints->setWorker(defaultWorker);
    m_slotUpdateTFWindowing = ::fwCom::newSlot(&TransferFunction::updateTFWindowing, this);
    m_slotUpdateTFWindowing->setWorker(defaultWorker);
}

TransferFunction::TransferFunction(
    const std::function<void()>& _functionPoints, const std::function<void(double,
                                                                           double)>& _functionWindow) :
    m_updateTFPoints(_functionPoints),
    m_updateTFWindowing(_functionWindow)
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
    ::fwData::mt::ObjectWriteLock tfLock(image);
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

//------------------------------------------------------------------------------

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

void TransferFunction::setTransferFunction(const ::fwData::TransferFunction::sptr& _tf)
{
    m_transferFunction = _tf;
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

::fwCom::Connection TransferFunction::getTFUpdateConnection() const
{
    const ::fwData::TransferFunction::sptr tf = this->getTransferFunction();
    const auto sig                            = tf->signal< ::fwData::TransferFunction::PointsModifiedSignalType >(
        ::fwData::TransferFunction::s_POINTS_MODIFIED_SIG);

    return sig->getConnection(m_slotUpdateTFPoints);
}

//------------------------------------------------------------------------------

::fwCom::Connection TransferFunction::getTFWindowingConnection() const
{
    const ::fwData::TransferFunction::sptr tf = this->getTransferFunction();
    const auto sig                            = tf->signal< ::fwData::TransferFunction::WindowingModifiedSignalType >(
        ::fwData::TransferFunction::s_WINDOWING_MODIFIED_SIG);

    return sig->getConnection(m_slotUpdateTFWindowing);
}

//------------------------------------------------------------------------------

void TransferFunction::updateTFPoints()
{
    m_updateTFPoints();
}

//------------------------------------------------------------------------------

void TransferFunction::updateTFWindowing(double _window, double _level)
{
    m_updateTFWindowing ? m_updateTFWindowing(_window, _level) : m_updateTFPoints();
}

//------------------------------------------------------------------------------

} //namespace helper

} //namespace fwDataTools
