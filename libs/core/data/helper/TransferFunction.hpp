/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
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

#pragma once

#include "data/config.hpp"

#include <core/com/Connection.hpp>
#include <core/com/helper/SigSlotConnection.hpp>
#include <core/com/Slot.hpp>

#include <data/Image.hpp>
#include <data/TransferFunction.hpp>

namespace sight::data
{

namespace helper
{

/**
 * @brief   Helpers for transfer function.
 *
 * @section Slots Slots
 *
 * One function must be implemented by the service managing the transfer function. This function will be
 * called when the transfer function points are modified. If the second function is implemented, it will
 * be called when the transfer function windowing is modified, else, the first function will also be called.
 */
class DATA_CLASS_API TransferFunction
{

public:

    SIGHT_DECLARE_CLASS(TransferFunction)

    /// Constructor.
    DATA_API TransferFunction(const std::function<void()>&);

    /// Constructor.
    DATA_API TransferFunction(const std::function<void()>&, const std::function<void(double, double)>&);

    /// Destructor. Do nothing.
    DATA_API virtual ~TransferFunction();

    /**
     * @brief Create and set the default transfer function.
     *
     * Create the image's transfer function pool if if does not exist and then create the defaut transfer function.
     * Set the current transfer function to the default grey-level if no transfer function was set (using
     * setTransferFunction() ).
     */
    DATA_API void createTransferFunction( data::Image::sptr image );

    /// Set the current TransferFunction
    DATA_API void setTransferFunction(const data::TransferFunction::sptr&);

    /**
     * @brief Sets the transfer function, creates one if _tf is null (@see createTransferFunction).
     *
     * Connects the TF's signals to the updateTFPoints and updateTFWindowing slots.
     */
    DATA_API void setOrCreateTF(const data::TransferFunction::sptr&, const data::Image::sptr&);

    /// Get the current transfer function
    DATA_API data::TransferFunction::sptr getTransferFunction() const;

    /// Install connections to listen TF modifications
    DATA_API void installTFConnections();

    /// Remove the TF connections
    DATA_API void removeTFConnections();

    DATA_API core::com::Connection getTFUpdateConnection() const;

    DATA_API core::com::Connection getTFWindowingConnection() const;

protected:

    /**
     * @name Connections to transfer function
     * @{
     */

    /// Slot: called when transfer function are modified
    DATA_API void updateTFPoints();

    /// Slot: called when transfer function windowing is modified
    DATA_API void updateTFWindowing(double, double);

    typedef core::com::Slot<void ()> UpdateTFPointsSlotType;

    typedef core::com::Slot<void (double, double)> UpdateTFWindowingSlotType;

    /// Slot called when transfer function points are modified
    UpdateTFPointsSlotType::sptr m_slotUpdateTFPoints;

    /// Slot called when transfer function windowing is modified
    UpdateTFWindowingSlotType::sptr m_slotUpdateTFWindowing;
    /**
     * @}
     */

private:

    /// Transfer function selection
    data::TransferFunction::wptr m_transferFunction;

    /// Connections to the transfer function
    core::com::helper::SigSlotConnection m_tfConnections;

    std::function<void()> m_updateTFPoints;

    std::function<void(double, double)> m_updateTFWindowing;

};

} //namespace helper

} //namespace sight::data
