/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UICALIBRATION_SCAMERASERIESEDITOR_HPP__
#define __UICALIBRATION_SCAMERASERIESEDITOR_HPP__

#include "uiCalibration/config.hpp"

#include <QPointer>
#include <QComboBox>
#include <QLabel>
#include <QVector>

#include <gui/editor/IEditor.hpp>

namespace uiCalibration
{
/**
 * @brief   SCameraSeriesEditor service is used to display the extrinsic calibration of a camera series.
 * @class   SCameraSeriesEditor
 */
class UICALIBRATION_CLASS_API SCameraSeriesEditor : public QObject,
                                                    public ::gui::editor::IEditor
{
Q_OBJECT;

public:
    fwCoreServiceClassDefinitionsMacro((SCameraSeriesEditor)(::gui::editor::IEditor));

    static const ::fwCom::Slots::SlotKeyType s_UPDATE_INFOS_SLOT;
    typedef ::fwCom::Slot<void ()> UpdateInfosSlotType;

    /**
     * @brief Constructor.
     */
    UICALIBRATION_API SCameraSeriesEditor() throw();

    /**
     * @brief Destructor.
     */
    UICALIBRATION_API ~SCameraSeriesEditor() throw()
    {
    }

    /**
     * @brief Configuring method : This method is used to configure the service.
     * @code{.xml}
       <service impl="::uiCalibration::SCameraSeriesEditor" >
            <camIndex>...</camIndex>
       </service>
       @endcode
     * - \b camIndex (optional, default: 1): index of the camera in cameraSeries used to display extrinsic matrix
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
    UICALIBRATION_API void updating() throw(fwTools::Failed)
    {
    }

    /**
     * @brief Swapping method : This method is used to swap the service.
     */
    UICALIBRATION_API void swapping() throw ( ::fwTools::Failed );

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     */
    ::fwServices::IService::KeyConnectionsType getObjSrvConnections() const;

protected:

    /**
     * @brief Updates the informations of the intrinsic calibration.
     */
    UICALIBRATION_API void updateInformations();

    /**
     * @brief Clear all the labels.
     */
    UICALIBRATION_API void clearLabels();

    /// Slots used when the camera is intrinsically calibrated
    UpdateInfosSlotType::sptr m_slotUpdateInfos;

    QPointer< QLabel > m_description; ///< description of camera series

    QVector< QPointer< QLabel > > m_matrixLabels; ///< Labels for matrix's elements

    /// Index of the camera in cameraSeries used to display extrinsic matrix.
    size_t m_camIndex;

};
} // namespace uiCalibration

#endif // __UICALIBRATION_SCAMERASERIESEDITOR_HPP__
