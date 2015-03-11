/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UICALIBRATION_SCALIBRATIONINFOEDITOR_HPP__
#define __UICALIBRATION_SCALIBRATIONINFOEDITOR_HPP__

#include <QPointer>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>

#include <fwData/List.hpp>
#include <fwData/PointList.hpp>
#include <fwData/Composite.hpp>

#include <gui/editor/IEditor.hpp>

#include "uiCalibration/config.hpp"

namespace uiCalibration
{
/**
 * @brief   SCalibrationInfoEditor service is used to handle the calibration points acquisition.
 * @class   SCalibrationInfoEditor
 *
 * Service registered details : \n
 * fwServicesRegisterMacro(::gui::editor::IEditor, ::uiCalibration::SCalibrationInfoEditor, ::fwData::Composite)
 */
class UICALIBRATION_CLASS_API SCalibrationInfoEditor : public QObject,
                                                       public ::gui::editor::IEditor
{
Q_OBJECT;

public:
    fwCoreServiceClassDefinitionsMacro((SCalibrationInfoEditor)(::gui::editor::IEditor));

    /**
     * @name Slots API
     * @{
     */
    UICALIBRATION_API static const ::fwCom::Slots::SlotKeyType s_REMOVE_SLOT;
    typedef ::fwCom::Slot<void ()> RemoveSlotType;

    UICALIBRATION_API static const ::fwCom::Slots::SlotKeyType s_RESET_SLOT;
    typedef ::fwCom::Slot<void ()> ResetSlotType;

    UICALIBRATION_API static const ::fwCom::Slots::SlotKeyType s_GET_SELECTION_SLOT;
    typedef ::fwCom::Slot<void ()> GetSelectionSlotType;
    ///@}

    /**
     * @brief Constructor.
     */
    UICALIBRATION_API SCalibrationInfoEditor() throw ();

    /**
     * @brief Destructor.
     */
    UICALIBRATION_API ~SCalibrationInfoEditor() throw ()
    {
    }

protected:

    /**
     * @brief Configuring method : This method is used to configure the service.
     * @verbatim
        <service uid="CalInfoEditor" type="::gui::editor::IEditor" impl="::uiCalibration::SCalibrationInfoEditor" autoConnect="no">
            <calInfo1>...</calInfo1>
            <calInfo2>...</calInfo2>
        </service>
       @endverbatim
     *
     * - \b calInfo1 uid of Camera1 CalibrationInfo
     * - \b calInfo2 uid of Camera2 calibrationInfo
     */
    UICALIBRATION_API void configuring() throw (fwTools::Failed);

    /**
     * @brief Starting method : This method is used to initialize the service.
     */
    UICALIBRATION_API void starting() throw (fwTools::Failed);

    /**
     * @brief Stopping method : This method is used to stop the service.
     */
    UICALIBRATION_API void stopping() throw (fwTools::Failed);

    /**
     * @brief Updating method : This method is used to update the service.
     */
    UICALIBRATION_API void updating() throw (fwTools::Failed);

    /**
     * @brief Receiving method : This method is used to update the service.
     * @param[in] msg Information message for modification.
     */
    UICALIBRATION_API void receiving(fwServices::ObjectMsg::csptr msg) throw (fwTools::Failed);

    /**
     * @brief Slot called when the user presses the remove acquisition button.
     */
    UICALIBRATION_API void remove();

    /**
     * @brief Slot called when the user presses the reset button.
     */
    UICALIBRATION_API void reset();

    /**
     * @brief Slot called when the user presses the display button.
     */
    UICALIBRATION_API void getSelection();

private Q_SLOTS:
    /**
     * @brief Slot called when an element is double-clicked in the list widget.
     */
    void onItemDoubleClicked(QListWidgetItem*);

private:

    /**
     * @brief Index of the last acquisition.
     */
    int m_captureIdx;

    /**
     * @brief Label displaying the number of point acquisitions.
     */
    QPointer< QLabel > m_nbCapturesLabel;

    /**
     * @brief Calibration point list.
     */
    QPointer< QListWidget > m_capturesListWidget;

    RemoveSlotType::sptr m_slotRemove;

    ResetSlotType::sptr m_slotReset;

    GetSelectionSlotType::sptr m_slotGetSelection;

    std::string m_calInfo1Key;

    std::string m_calInfo2Key;

};
} // namespace uiCalibration

#endif // __UICALIBRATION_SCALIBRATIONINFOEDITOR_HPP__
