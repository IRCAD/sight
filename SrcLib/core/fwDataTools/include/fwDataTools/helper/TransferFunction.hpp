/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwDataTools/config.hpp"
#include "fwDataTools/helper/ImageGetter.hpp"

#include <fwCom/Connection.hpp>
#include <fwCom/helper/SigSlotConnection.hpp>
#include <fwCom/Slot.hpp>

#include <fwData/TransferFunction.hpp>

namespace fwDataTools
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
class FWDATATOOLS_CLASS_API TransferFunction
{

public:

    fwCoreBaseClassDefinitionsMacro( (TransferFunction) );

    /// Constructor.
    FWDATATOOLS_API TransferFunction(const std::function<void()>&);

    /// Constructor.
    FWDATATOOLS_API TransferFunction(const std::function<void()>&, const std::function<void(double, double)>&);

    /// Destructor. Do nothing.
    FWDATATOOLS_API virtual ~TransferFunction();

    /**
     * @brief Create and set the default transfer function.
     *
     * Create the image's transfer function pool if if does not exist and then create the defaut transfer function.
     * Set the current transfer function to the default grey-level if no transfer function was set (using
     * setTransferFunction() ).
     */
    FWDATATOOLS_API void createTransferFunction( ::fwData::Image::sptr image );

    /// Set the current TransferFunction
    FWDATATOOLS_API void setTransferFunction(const fwData::TransferFunction::sptr&);

    /**
     * @brief Sets the transfer function, creates one if _tf is null (@see createTransferFunction).
     *
     * Connects the TF's signals to the updateTFPoints and updateTFWindowing slots.
     */
    FWDATATOOLS_API void setOrCreateTF(const ::fwData::TransferFunction::sptr&, const fwData::Image::sptr&);

    /// Get the current transfer function
    FWDATATOOLS_API ::fwData::TransferFunction::sptr getTransferFunction() const;

    /// Install connections to listen TF modifications
    FWDATATOOLS_API void installTFConnections();

    /// Remove the TF connections
    FWDATATOOLS_API void removeTFConnections();

    FWDATATOOLS_API ::fwCom::Connection getTFUpdateConnection() const;

    FWDATATOOLS_API ::fwCom::Connection getTFWindowingConnection() const;

protected:

    /**
     * @name Connections to transfer function
     * @{
     */

    /// Slot: called when transfer function are modified
    FWDATATOOLS_API void updateTFPoints();

    /// Slot: called when transfer function windowing is modified
    FWDATATOOLS_API void updateTFWindowing(double, double);

    typedef ::fwCom::Slot<void ()> UpdateTFPointsSlotType;

    typedef ::fwCom::Slot<void (double, double)> UpdateTFWindowingSlotType;

    /// Slot called when transfer function points are modified
    UpdateTFPointsSlotType::sptr m_slotUpdateTFPoints;

    /// Slot called when transfer function windowing is modified
    UpdateTFWindowingSlotType::sptr m_slotUpdateTFWindowing;
    /**
     * @}
     */

private:

    /// Transfer function selection
    ::fwData::TransferFunction::wptr m_transferFunction;

    /// Connections to the transfer function
    ::fwCom::helper::SigSlotConnection m_tfConnections;

    std::function<void()> m_updateTFPoints;

    std::function<void(double, double)> m_updateTFWindowing;

};

} //namespace helper

} //namespace fwDataTools
