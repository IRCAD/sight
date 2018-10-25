/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "ctrlSelection/config.hpp"

#include <fwDataTools/helper/MedicalImage.hpp>

#include <fwServices/IController.hpp>
#include <fwServices/IService.hpp>

namespace ctrlSelection
{

/**
 * @brief  This service updates a fwData::Float given in configuration with the image axis.
 * @deprecated This service will be removed
 */
class CTRLSELECTION_CLASS_API ImageUpdateAxis : public ::fwServices::IController
{

public:

    fwCoreServiceClassDefinitionsMacro( (ImageUpdateAxis)(::fwServices::IController) );

    CTRLSELECTION_API ImageUpdateAxis() noexcept;

    CTRLSELECTION_API virtual ~ImageUpdateAxis() noexcept;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_SLICE_TYPE_MODIFIED_SIG to this::s_UPDATE_SLICE_TYPE_SLOT
     */
    CTRLSELECTION_API virtual KeyConnectionsType getObjSrvConnections() const override;

protected:

    /// Calls updating().
    CTRLSELECTION_API virtual void starting() override;

    /// Does nothing.
    CTRLSELECTION_API virtual void stopping() override;

    /// Does nothing.
    CTRLSELECTION_API virtual void swapping() override;

    /**
     * @brief Configures the service.
     *
     * @code{.xml}
       <service type="::fwServices::IController" impl="::ctrlSelection::ImageUpdateAxis" autoConnect="yes">
           <axis uid="imageAxis" orientation="axial" />
       </service>
       @endcode
     * - \b uid : fwID of the ::fwData::Float to update
     * - \b orientation : image orientation (axial, frontal or sagittal)
     */
    CTRLSELECTION_API virtual void configuring() override;

    /// Does nothing.
    CTRLSELECTION_API virtual void reconfiguring() override;

    /// Updates the float from the image axis orientation.
    CTRLSELECTION_API virtual void updating() override;

    /// Implements info method derived from IService. Print classname.
    CTRLSELECTION_API virtual void info( std::ostream& _sstream ) override;

private:

    /**
     * @name Slots
     * @{
     */
    /// Slot: update image slice type
    void updateSliceType(int from, int to);
    /**
     * @}
     */

    /// fwID of the float to update.
    std::string m_floatID;

    /// Orientation of the image axis.
    ::fwDataTools::helper::MedicalImage::Orientation m_orientation;
};

} // ctrlSelection
