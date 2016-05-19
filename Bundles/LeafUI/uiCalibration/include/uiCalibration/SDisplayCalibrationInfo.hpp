/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UICALIBRATION_SDISPLAYCALIBRATIONINFO_HPP__
#define __UICALIBRATION_SDISPLAYCALIBRATIONINFO_HPP__

#include "uiCalibration/config.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwServices/IController.hpp>
#include <fwServices/AppConfigManager.hpp>
#include <fwServices/helper/SigSlotConnection.hpp>



namespace uiCalibration
{

/**
 * @class   SDisplayCalibrationInfo
 * @brief   Launch an AppConfig to display calibration images.
 *
 *  This service works on a ::fwData::Composite.
 *  It starts/stops a template configuration. This service can display one or two images.
 *
 *  If there is one calibration info in the service parameters, the template configuration used is
 *  "DisplayImageConfig.xml". The parameters are :
 *  - "imageId1" is the image to be displayed.
 *  - GENERIC_UID is replaced by a generated unique identifier when the configuration is launched.
 *
 *  If there are two calibration infos, the template configuration used is "DisplayTwoImagesConfig.xml".
 *  The parameters are :
 *  - "imageId1" is the first image.
 *  - "imageId2" is the second image.
 *  - GENERIC_UID is replaced by a generated unique identifier when the configuration is launched.
 *
 * @note The use of GENERIC_UID ensures that the created object and services in the configuration have a unique uid
 *  even if this configuration is launched several times.
 *
 */
class UICALIBRATION_CLASS_API SDisplayCalibrationInfo : public ::fwServices::IController
{

public:

    fwCoreServiceClassDefinitionsMacro ( (SDisplayCalibrationInfo)(::fwServices::IController) );

    /// Constructor. Does nothing.
    UICALIBRATION_API SDisplayCalibrationInfo() throw();

    /// Destructor. Does nothing.
    UICALIBRATION_API virtual ~SDisplayCalibrationInfo() throw();

    /**
     * @name Slots API
     * @{
     */
    UICALIBRATION_API static const ::fwCom::Slots::SlotKeyType s_DISPLAY_IMAGE_SLOT;
    typedef ::fwCom::Slot<void (size_t)> DisplayImageSlotType;

    /** @} */


protected:

    /// Starts the config
    virtual void starting() throw(::fwTools::Failed);

    /// Stops the config
    virtual void stopping() throw(::fwTools::Failed);

    /// Does nothing
    virtual void updating() throw(::fwTools::Failed);

    /**
     * @brief Configure the service
     *
     * Example of this service configuration with two calibration informations.
     * @code{.xml}
     *
       <service impl="::uiCalibration::SDisplayCalibrationInfo" type="::fwServices::IController">
           <CalInfo1>calInfo1</CalInfo1>
           <CalInfo2>calInfo2</CalInfo2>
       </service>
        @endcode
     * It MUST have at least one calibration tag and at most two calibration tags.
     */

    virtual void configuring() throw(fwTools::Failed);

    /// Overrides
    virtual void info( std::ostream &_sstream );



private:

    /**
     * @name Slots
     * @}
     */
    /// Slot: stop the config.
    void stopConfig();

    /// Launch an appConfig to display an image on an external window.
    void displayImage(size_t idx);
    /**
     * @}
     */

    /// AppConfig manager, used to launch the config
    ::fwServices::IAppConfigManager::sptr m_configMgr;

    /// Calibration information
    std::vector< std::string > m_calInfoKeys;

    std::string m_proxychannel; ///< Name of the channel used to connect stopConfig slot to the config frame closing.
};

} // uiCalibration

#endif // __UICALIBRATION_SDISPLAYCALIBRATIONINFO_HPP__
