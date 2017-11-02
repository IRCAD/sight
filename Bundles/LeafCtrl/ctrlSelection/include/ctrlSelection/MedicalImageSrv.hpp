/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __CTRLSELECTION_MEDICALIMAGESRV_HPP__
#define __CTRLSELECTION_MEDICALIMAGESRV_HPP__

#include "ctrlSelection/config.hpp"

#include <fwServices/IController.hpp>
#include <fwServices/IService.hpp>

namespace ctrlSelection
{

/**
 * @brief  This service convert its attached image to a medical image by adding specific fields.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
       <service type="::ctrlSelection::MedicalImageSrv">
           <inout key="image" uid="..."/>
       </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b image [::fwData::Image]: image to be converted into a medical image.
 */
class CTRLSELECTION_CLASS_API MedicalImageSrv : public ::fwServices::IController
{

public:

    fwCoreServiceClassDefinitionsMacro( (MedicalImageSrv)(::fwServices::IController) );

    CTRLSELECTION_API MedicalImageSrv() noexcept;

    CTRLSELECTION_API virtual ~MedicalImageSrv() noexcept;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect Image::s_BUFFER_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    CTRLSELECTION_API virtual KeyConnectionsType getObjSrvConnections() const override;

protected:

    /// Implements starting method derived from IService. Convert the image.
    CTRLSELECTION_API virtual void starting() override;

    /// Implements stopping method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void stopping() override;

    /// Implements swapping method derived from IService. Convert the image.
    CTRLSELECTION_API virtual void swapping() override;

    /// Implements configuring method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void configuring() override;

    /// Implements reconfiguring method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void reconfiguring() override;

    /// Implements updating method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void updating() override;

    /// Implements info method derived from IService. Print classname.
    CTRLSELECTION_API virtual void info( std::ostream& _sstream ) override;

private:

    void convertImage();

    std::vector< std::string > m_imageCompositeKeys;
};

} // ctrlSelection

#endif // __CTRLSELECTION_MEDICALIMAGESRV_HPP__
