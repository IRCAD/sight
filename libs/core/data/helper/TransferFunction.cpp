/************************************************************************
 *
 * Copyright (C) 2018-2022 IRCAD France
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

#include "data/helper/TransferFunction.hpp"

#include "data/helper/Composite.hpp"
#include "data/helper/MedicalImage.hpp"
#include "data/Image.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>

namespace sight::data
{

namespace helper
{

//------------------------------------------------------------------------------

TransferFunction::TransferFunction(const std::function<void()>& _function) :
    m_updateTFPoints(_function)
{
    auto defaultWorker = core::thread::getDefaultWorker();
    m_slotUpdateTFPoints = core::com::newSlot(&TransferFunction::updateTFPoints, this);
    m_slotUpdateTFPoints->setWorker(defaultWorker);
    m_slotUpdateTFWindowing = core::com::newSlot(&TransferFunction::updateTFWindowing, this);
    m_slotUpdateTFWindowing->setWorker(defaultWorker);
}

TransferFunction::TransferFunction(
    const std::function<void()>& _functionPoints,
    const std::function<void(double,
                             double)>& _functionWindow
) :
    m_updateTFPoints(_functionPoints),
    m_updateTFWindowing(_functionWindow)
{
    auto defaultWorker = core::thread::getDefaultWorker();
    m_slotUpdateTFPoints = core::com::newSlot(&TransferFunction::updateTFPoints, this);
    m_slotUpdateTFPoints->setWorker(defaultWorker);
    m_slotUpdateTFWindowing = core::com::newSlot(&TransferFunction::updateTFWindowing, this);
    m_slotUpdateTFWindowing->setWorker(defaultWorker);
}

//------------------------------------------------------------------------------

TransferFunction::~TransferFunction()
{
}

//------------------------------------------------------------------------------

void TransferFunction::createTransferFunction(data::Image::sptr image)
{
    namespace medImHelper = data::helper::MedicalImage;

    data::Composite::sptr tfPool = medImHelper::getTransferFunction(*image);

    if(!tfPool)
    {
        tfPool = data::Composite::New();
        medImHelper::setTransferFunction(*image, tfPool);
    }

    // create the default transfer function in the image tf field if it does not exist
    if(tfPool->find(data::TransferFunction::s_DEFAULT_TF_NAME) == tfPool->end())
    {
        data::TransferFunction::sptr tfGreyLevel = data::TransferFunction::createDefaultTF();
        if(image->getWindowWidth() != 0)
        {
            tfGreyLevel->setWindow(image->getWindowWidth());
            tfGreyLevel->setLevel(image->getWindowCenter());
        }
        else if(data::helper::MedicalImage::checkImageValidity(image))
        {
            double min, max;
            data::helper::MedicalImage::getMinMax(image, min, max);
            data::TransferFunction::TFValuePairType wlMinMax(min, max);
            tfGreyLevel->setWLMinMax(wlMinMax);
        }

        data::helper::Composite compositeHelper(tfPool);
        compositeHelper.add(data::TransferFunction::s_DEFAULT_TF_NAME, tfGreyLevel);
        compositeHelper.notify();
    }

    if(m_transferFunction.expired())
    {
        data::TransferFunction::sptr tfGreyLevel =
            tfPool->at<data::TransferFunction>(data::TransferFunction::s_DEFAULT_TF_NAME);
        m_transferFunction = tfGreyLevel;
    }
    else if(m_transferFunction.lock()->getTFValues().empty())
    {
        data::TransferFunction::sptr tfGreyLevel =
            tfPool->at<data::TransferFunction>(data::TransferFunction::s_DEFAULT_TF_NAME);
        m_transferFunction.lock()->deepCopy(tfGreyLevel);
    }
}

//------------------------------------------------------------------------------

void TransferFunction::setOrCreateTF(const data::TransferFunction::sptr& _tf, const data::Image::sptr& _image)
{
    SIGHT_ASSERT("Image should not be null", _image);

    this->removeTFConnections();
    if(_tf)
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

data::TransferFunction::sptr TransferFunction::getTransferFunction() const
{
    SIGHT_ASSERT(
        "Transfer funtion is not defined, you must call setTransferFunction() or createTransferFunction() first."
        ,
        !m_transferFunction.expired()
    );
    return m_transferFunction.lock();
}

//------------------------------------------------------------------------------

void TransferFunction::setTransferFunction(const data::TransferFunction::sptr& _tf)
{
    m_transferFunction = _tf;
}

//------------------------------------------------------------------------------

void TransferFunction::installTFConnections()
{
    core::com::Connection connection;

    data::TransferFunction::sptr tf = this->getTransferFunction();

    connection = tf->signal(data::Object::s_MODIFIED_SIG)->connect(m_slotUpdateTFPoints);
    m_tfConnections.addConnection(connection);
    connection = tf->signal(data::TransferFunction::s_POINTS_MODIFIED_SIG)->connect(m_slotUpdateTFPoints);
    m_tfConnections.addConnection(connection);
    connection = tf->signal(data::TransferFunction::s_WINDOWING_MODIFIED_SIG)->connect(m_slotUpdateTFWindowing);
    m_tfConnections.addConnection(connection);
}

//------------------------------------------------------------------------------

void TransferFunction::removeTFConnections()
{
    m_tfConnections.disconnect();
}

//------------------------------------------------------------------------------

core::com::Connection TransferFunction::getTFUpdateConnection() const
{
    const data::TransferFunction::sptr tf = this->getTransferFunction();
    const auto sig                        = tf->signal<data::TransferFunction::PointsModifiedSignalType>(
        data::TransferFunction::s_POINTS_MODIFIED_SIG
    );

    return sig->getConnection(m_slotUpdateTFPoints);
}

//------------------------------------------------------------------------------

core::com::Connection TransferFunction::getTFWindowingConnection() const
{
    const data::TransferFunction::sptr tf = this->getTransferFunction();
    const auto sig                        = tf->signal<data::TransferFunction::WindowingModifiedSignalType>(
        data::TransferFunction::s_WINDOWING_MODIFIED_SIG
    );

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

} //namespace sight::data
