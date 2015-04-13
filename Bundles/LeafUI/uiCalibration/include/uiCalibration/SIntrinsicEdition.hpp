/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UICALIBRATION_SINTRINSICEDITION_HPP__
#define __UICALIBRATION_SINTRINSICEDITION_HPP__

#include "uiCalibration/SUpdateIntrinsicDialog.hpp"
#include "uiCalibration/config.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwServices/IService.hpp>

#include <QObject>

#include <array>
#include <list>

namespace uiCalibration
{
/**
 * @brief   SCameraEditor service is used to set the intrinsic parameter infos.
 * @class   SCameraEditor
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
    UICALIBRATION_API void configuring() throw(fwTools::Failed);

    /**
     * @brief Starting method : This method is used to initialize the service.
     */
    UICALIBRATION_API void starting() throw(fwTools::Failed);

    /**
     * @brief Stopping method : This method is used to stop the service.
     */
    UICALIBRATION_API void stopping() throw(fwTools::Failed);

    /**
     * @brief Updating method : This method is used to update the service.
     */
    UICALIBRATION_API void updating() throw(fwTools::Failed);

    /**
     * @brief Swapping method : This method is used to swap the service.
     */
    UICALIBRATION_API void swapping() throw ( ::fwTools::Failed );


private Q_SLOTS:

    /**
     * @brief onNewCalibration
     * @param cal
     */
    void onNewCalibration( std::array< double, 12 > & cal);

private:

    void updateCalibration();

    void readCalibration();


    std::array<double,4>    m_intrinsic;
    std::array<double,5>    m_distParameters;

    double m_skew;
    std::array< double, 2 > m_resolution;

    std::array< double, 12 > m_calibration;

    SUpdateIntrinsicDialog* m_dialog;
};

} // namespace uiCalibration

#endif // __UICALIBRATION_SINTRINSICEDITION_HPP__
