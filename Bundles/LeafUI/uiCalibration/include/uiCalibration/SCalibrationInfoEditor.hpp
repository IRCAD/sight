/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UICALIBRATION_SCALIBRATIONINFOEDITOR_HPP__
#define __UICALIBRATION_SCALIBRATIONINFOEDITOR_HPP__

#include "uiCalibration/config.hpp"

#include <fwData/List.hpp>
#include <fwData/PointList.hpp>

#include <gui/editor/IEditor.hpp>

#include <QPointer>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>

namespace uiCalibration
{
/**
 * @brief   SCalibrationInfoEditor service is used to handle the calibration points acquisition.
 * *
 * @section Slots Slots
 * - \b remove() : removes the current selected image.
 * - \b reset() : clears all the calibration information.
 * - \b getSelection() : emits the CalibrationInfo signal 'getRecord(index)' with the current selection index.
 *
 * @section XML XML Configuration
 * @code{.xml}
   <service type="::uiCalibration::SDisplayCalibrationInfo">
       <inout key="calInfo1" uid"..." />
       <inout key="calInfo2" uid="..." />
   </service>
   @endcode
 * @subsection Input Input
 * - \b calInfo1 [::arData::CalibrationInfo]: calibration information for first camera.
 * - \b calInfo2 [::arData::CalibrationInfo] (optional): calibration information for optional second camera.
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
    UICALIBRATION_API static const ::fwCom::Slots::SlotKeyType s_RESET_SLOT;
    UICALIBRATION_API static const ::fwCom::Slots::SlotKeyType s_GET_SELECTION_SLOT;
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

    /// Initializes the editor
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
     * @brief Slot: removes the current selected image.
     */
    UICALIBRATION_API void remove();

    /**
     * @brief Slot:clears all the calibration information.
     */
    UICALIBRATION_API void reset();

    /**
     * @brief Slot: emits the CalibrationInfo signal 'getRecord(index)' with the current selection index.
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
};
} // namespace uiCalibration

#endif // __UICALIBRATION_SCALIBRATIONINFOEDITOR_HPP__
