/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwDataTools/config.hpp"
#include "fwDataTools/helper/ImageGetter.hpp"

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
 * The two slots must be reimplemented by the services managing transfer function
 * - \b updateTFPoints(): called when the transfer function points are modified
 * - \b updateTFWindowing(double window, double level): called when the transfer function windowing is modified
 */
class FWDATATOOLS_CLASS_API TransferFunction
{

public:

    fwCoreBaseClassDefinitionsMacro( (TransferFunction) );

    /// Destructor. Do nothing.
    FWDATATOOLS_API virtual ~TransferFunction();

    /// Set the current TransferFunction
    FWDATATOOLS_API void setTransferFunction( const ::fwData::TransferFunction::sptr& tf );

protected:

    /// Constructor. Do nothing.
    FWDATATOOLS_API TransferFunction(); // this class VISUVTKADAPTOR_CLASS_API must be specialized

    /// Return the image
    FWDATATOOLS_API ::fwData::Image::sptr getImage() const;

    /// Update the image information (slice index, min/max,...)
    FWDATATOOLS_API void updateImageInfos( ::fwData::Image::sptr image  );

    /**
     * @brief Create and set the default transfer function.
     *
     * Create the image's transfer function pool if if does not exist and then create the defaut transfer function.
     * Set the current transfer function to the default grey-level if no transfer function was set (using
     * setTransferFunction() ).
     */
    FWDATATOOLS_API void createTransferFunction( ::fwData::Image::sptr image );

    /**
     * @brief Sets the transfer function, creates one if _tf is null (@see createTransferFunction).
     *
     * Connects the TF's signals to the updateTFPoints and updateTFWindowing slots.
     */
    FWDATATOOLS_API void setOrCreateTF(const ::fwData::TransferFunction::sptr& _tf, const fwData::Image::sptr& _image);

    /// Get the current transfer function
    FWDATATOOLS_API ::fwData::TransferFunction::sptr getTransferFunction() const;

    /**
     * @name Connections to transfer function
     * @{
     */

    /// Install connections to listen TF modifications
    FWDATATOOLS_API void installTFConnections();

    /// Remove the TF connections
    FWDATATOOLS_API void removeTFConnections();

    /**
     * @brief Install the slots to managed TF modifications.
     *
     * Creates slots to listen TF selection Composite and TransferFunction signals.
     *
     * @warning It must be called in the service constructor
     */
    FWDATATOOLS_API void installTFSlots(::fwCom::HasSlots* hasslots);

    /// Slot: called when transfer function points are modified
    FWDATATOOLS_API virtual void updateTFPoints();

    /// Slot: called when transfer function windowing is modified
    FWDATATOOLS_API virtual void updateTFWindowing(double window, double level);

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

    /// Current image
    ::fwData::Image::wptr m_weakImage;

    /// Transfer function selection
    ::fwData::TransferFunction::wptr m_transferFunction;

    /// Connections to the transfer function
    ::fwCom::helper::SigSlotConnection m_tfConnections;

};

} //namespace helper

} //namespace fwDataTools
