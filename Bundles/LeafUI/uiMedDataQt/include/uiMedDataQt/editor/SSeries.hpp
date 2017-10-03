/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIMEDDATAQT_EDITOR_SSERIES_HPP__
#define __UIMEDDATAQT_EDITOR_SSERIES_HPP__

#include "uiMedDataQt/config.hpp"

#include <fwCom/Signal.hpp>

#include <gui/editor/IEditor.hpp>

#include <QObject>
#include <QPointer>

class QPushButton;

namespace uiMedDataQt
{

namespace widget
{
class PatientEditor;
class StudyEditor;
class EquipmentEditor;
class SeriesEditor;
}

namespace editor
{
/**
 * @brief Shows and edit fwMedData::Series information
 */
class UIMEDDATAQT_CLASS_API SSeries : public QObject,
                                      public ::gui::editor::IEditor
{
Q_OBJECT
public:

    fwCoreServiceClassDefinitionsMacro( (SSeries)(::gui::editor::IEditor) );

    /// Constructor
    UIMEDDATAQT_API SSeries();

    /// Destructor
    UIMEDDATAQT_API virtual ~SSeries() noexcept;

    /**
     * @brief Signal type and key triggered when the series has been exported to related series DB.
     * @{ */
    typedef ::fwCom::Signal< void () > SeriesExportedSignalType;
    UIMEDDATAQT_API static const ::fwCom::Signals::SignalKeyType s_SERIES_EXPORTED_SIG;
    /**  @} */

    /**
     * @brief Signal type and key triggered when the service export ability changes
     * @{ */
    typedef ::fwCom::Signal< void ( bool ) > CanExportSignalType;
    UIMEDDATAQT_API static const ::fwCom::Signals::SignalKeyType s_CAN_EXPORT_SIG;
    /**  @} */

    /**
     * @brief Slot triggering export
     * @{ */
    typedef ::fwCom::Slot<void ()> ExportSlotType;
    UIMEDDATAQT_API static const ::fwCom::Slots::SlotKeyType s_EXPORT_SLOT;
    /**  @} */

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect SeriesDB::s_ADDED_SERIES_SIG to this::s_UPDATE_SLOT
     * Connect SeriesDB::s_REMOVED_SERIES_SIG to this::s_UPDATE_SLOT
     */
    UIMEDDATAQT_API virtual KeyConnectionsType getObjSrvConnections() const override;

protected:

    /// Installs GUI : create container and add selector.
    virtual void starting() override;

    /// Destroys GUI.
    virtual void stopping() override;

    /**
     *
     * @code{.xml}
       <service uid="selector" impl="::uiMedDataQt::editor::SSeries" type="::gui::editor::IEditor" autoConnect="yes">
        <selectionId>selections</selectionId>
        <selectionMode>single|extended</selectionMode>
        <allowedRemove>yes|no</allowedRemove>
       </service>
       @endcode
     * - \b selectionId : defines the fwId of the ::fwData::Vector where the selection will be put or get.
     * - \b selectionMode : defines the selection mode for the series
     * - \b allowedRemove : allows user to remove series
     */
    virtual void configuring() override;

    /// Fill selector with the series contained in SeriesDB.
    virtual void updating() override;

protected Q_SLOTS:

    /// Triggered when export button is clicked
    void onExportClicked();

private:

    /// Widget to allow patient information editing
    QPointer< ::uiMedDataQt::widget::PatientEditor > m_patientEditor;

    /// Widget for study edition/creation
    QPointer< ::uiMedDataQt::widget::StudyEditor > m_studyEditor;

    /// Widget for study edition/creation
    QPointer< ::uiMedDataQt::widget::EquipmentEditor > m_equipmentEditor;

    /// Widget for series edition/creation
    QPointer< ::uiMedDataQt::widget::SeriesEditor > m_seriesEditor;

    /// Widget to export information into target series
    QPointer< QPushButton > m_btnExport;

    /// UID of series to be exported
    std::string m_seriesId;

    /// UID of destination series DB
    std::string m_seriesDBId;

    /// Signal triggered when the series has been exported to the series DB
    SeriesExportedSignalType::sptr m_sigSeriesExported;

    /// Signal triggered when the service export ability changes
    CanExportSignalType::sptr m_sigCanExport;

    /// Slot triggering the export
    ExportSlotType::sptr m_slotExport;
};
} // namespace editor
} // namespace uiMedDataQt

#endif // __UIMEDDATAQT_EDITOR_SSERIES_HPP__

