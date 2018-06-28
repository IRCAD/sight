/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "uiCalibration/config.hpp"
#include "uiCalibration/SUpdateIntrinsicDialog.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwServices/IService.hpp>

#include <QObject>

#include <array>
#include <list>

namespace uiCalibration
{

/**
 * @brief   SIntrinsicEdition service is used to set the intrinsic parameter infos.
 *
 * This services displays a dialog to change the resolution of your calibration, it computes the new intrinic parameters
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::uiCalibration::SIntrinsicEdition">
       <inout key="camera" uid="..." />
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b camera [::arData::Camera]: camera on which the intrinsic parameters wil be modified.
 */
class UICALIBRATION_CLASS_API SIntrinsicEdition : public QObject,
                                                  public ::fwServices::IService
{

Q_OBJECT;

public:
    fwCoreServiceClassDefinitionsMacro((SIntrinsicEdition)(::fwServices::IService));

    /**
     * @brief Constructor.
     */
    UICALIBRATION_API SIntrinsicEdition();

    /**
     * @brief Destructor.
     */
    UICALIBRATION_API ~SIntrinsicEdition();

    /**
     * @brief Configuring method : This method is used to configure the service.
     */
    UICALIBRATION_API void configuring() override;

    /**
     * @brief Starting method : This method is used to initialize the service.
     */
    UICALIBRATION_API void starting() override;

    /**
     * @brief Stopping method : This method is used to stop the service.
     */
    UICALIBRATION_API void stopping() override;

    /**
     * @brief Updating method : This method is used to update the service.
     */
    UICALIBRATION_API void updating() override;

    /**
     * @brief Swapping method : This method is used to swap the service.
     */
    UICALIBRATION_API void swapping() override;

private Q_SLOTS:

    /**
     * @brief onNewCalibration
     * @param cal
     */
    void onNewCalibration( std::array< double, 12 >& cal);

private:

    void updateCalibration();

    void readCalibration();

    std::array<double, 4>    m_intrinsic;
    std::array<double, 5>    m_distParameters;

    double m_skew;
    std::array< double, 2 > m_resolution;

    std::array< double, 12 > m_calibration;

    SUpdateIntrinsicDialog* m_dialog;
};

} // namespace uiCalibration
