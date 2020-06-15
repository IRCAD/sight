/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include "uiMedDataQt/config.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signals.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwData/Reconstruction.hpp>

#include <fwMedData/ModelSeries.hpp>

#include <fwGui/editor/IEditor.hpp>

#include <fwTools/Failed.hpp>

#include <QObject>
#include <QPointer>
#include <QPushButton>

class QTreeWidget;
class QCheckBox;
class QListWidgetItem;
class QTreeWidgetItem;

namespace uiMedDataQt
{
namespace editor
{

class ValueView;

/**
 * @brief Editor displaying the list of the organs in a ModelSeries.
 *
 * It allows to show/hide a Reconstruction when it is associated to a render scene with a Model series adaptor
 * (for example fwRenderVTK::SRender with ::visuVTKAdaptor::SModelSeries).
 * It also allows to select a Reconstruction: associated to the ::ctrlSelection::updater::SObjFromSlot, the
 * reconstruction is available to be used by other services.
 * It is mostly associated to ::uiReconstructionQt::SOrganMaterialEditor and
 * ::uiReconstructionQt::RepresentationEditor to update the reconstrution color, transparency, ....
 *
 * @section Signals Signals
 * - \b reconstructionSelected(::fwData::Object::sptr): this signal emits the selected reconstruction
 * - \b emptiedSelection(): this signal is emitted when no reconstruction is selected
 *
 * @section Slots Slots
 * - \b showReconstructions(bool): slot called to show or hide all the reconstructions
 *
 * @section XML XML Configuration
 * @code{.xml}
   <service ::uiMedDataQt::editor::SModelSeriesList">
       <inout key="modelSeries" uid="..." autoConnect="yes" />
       <enable_hide_all>true</enable_hide_all>
       <columns>
         <organ_name>@organ_name</organ_name>
         <volume_cc view="positive" >@volume</volume_cc>
       </columns>
   </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b modelSeries [::fwMedData::ModelSeries]: model series containing the organs to list
 *
 * @subsection Configuration Configuration
 * \b enable_hide_all: if 'true', allows to hide all models through a single checkbox displayed in UI (default
 * value is 'true', allowed values are 'true' and 'false').
 * \b column: defines colums to be shown in reconstruction list. XML child element names follow
 * ::fwData::Reconstruction serialization attribute names. The name of the tag will be used as the column name.
 * The attribute 'view' is optional and has the following values:
 *  - positive: a numeric value is displayed only if it is positive. Otherwise, 'Unknown' is displayed.
 */
class UIMEDDATAQT_CLASS_API SModelSeriesList final :
    public QObject,
    public ::fwGui::editor::IEditor
{

Q_OBJECT

public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(SModelSeriesList, ::fwGui::editor::IEditor)

    /// Initializes the slot and signals.
    UIMEDDATAQT_API SModelSeriesList() noexcept;

    /// Cleans ressources.
    UIMEDDATAQT_API ~SModelSeriesList() noexcept override;

private:

    /// Configures the editor.
    void configuring() override;

    /// Creates layouts.
    void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect ::fwMedData::ModelSeries::s_MODIFIED_SIG of s_MODEL_SERIES_INOUT to s_UPDATE_SLOT.
     * Connect ::fwMedData::ModelSeries::s_RECONSTRUCTIONS_ADDED_SIG of s_MODEL_SERIES_INOUT to s_UPDATE_SLOT.
     * Connect ::fwMedData::ModelSeries::s_RECONSTRUCTIONS_REMOVED_SIG of s_MODEL_SERIES_INOUT to s_UPDATE_SLOT.
     */
    KeyConnectionsMap getAutoConnections() const override;

    /**
     * @brief Refreshs the editor.
     * @see updateReconstructions().
     * @see refreshVisibility().
     */
    void updating() override;

    /// Disconnects connections.
    void stopping() override;

    /// Updates reconstructions.
    void updateReconstructions();

    /// Fills the editor tree.
    void fillTree(const ::fwData::mt::locked_ptr< ::fwMedData::ModelSeries >& _modelSeries);

    /// SLOT: Shows (or hide) reconstructions.
    void showReconstructions(bool _show);

    /// Refreshs reconstructions visibility on the editor.
    void refreshVisibility();

    /**
     * @brief Checks or unchecks reconstructions.
     * @param _visible the checked status.
     * @see onCheckAllCheckBox().
     * @see onUnCheckAllCheckBox().
     */
    void onCheckAllBoxes(bool _visible);

private Q_SLOTS:

    /// Changes the current item, called when new current item is setted in m_organChoice.
    void onCurrentItemChanged(QTreeWidgetItem* _current, QTreeWidgetItem*);

    /// Changes the current item, called when new current item is setted in m_organChoice.
    void onCurrentItemChanged(QTreeWidgetItem* _current, int _column);

    /// Shows reconstructions, called when m_showCheckBox is clicked.
    void onShowReconstructions(int state);

    /// Shows a reconstruction, called when new current item is setted in m_organChoice.
    void onOrganChoiceVisibility(QTreeWidgetItem* _item, int);

    /// Shows reconstructions, called when m_checkAllButton is clicked.
    void onCheckAllCheckBox();

    /// Shows reconstructions, called when m_unCheckAllButton is clicked.
    void onUnCheckAllCheckBox();

private:

    /// Contains the button to check all reconstructions.
    QPointer<QPushButton> m_checkAllButton;

    /// Contains the button to uncheck all reconstructions.
    QPointer<QPushButton> m_unCheckAllButton;

    /// Contains the button to hide or show all reconstructions.
    QPointer< QCheckBox > m_showCheckBox;

    /// Contains the reconstructions tree:
    QPointer< QTreeWidget > m_tree;

    /// Defines informations.
    typedef std::map< std::string, ValueView* > DisplayedInformation;
    DisplayedInformation m_displayedInfo;

    /// Enables m_showCheckBox.
    bool m_enableHideAll;

    /// Defines the header of the tree.
    QStringList m_headers;

    /// Contains the signal emitted when a reconstruction is selected.
    typedef ::fwCom::Signal< void (::fwData::Object::sptr) > ReconstructionSelectedSignalType;
    ReconstructionSelectedSignalType::sptr m_sigReconstructionSelected;

    /// Contains the signal emitted when we clean the list.
    typedef ::fwCom::Signal< void () > EmptiedSelectionSignalType;
    EmptiedSelectionSignalType::sptr m_sigEmptiedSelection;

    /// Contains the slot to show (or hide) reconstructions.
    typedef ::fwCom::Slot< void (bool) > ShowReconstructionsSlotType;
    ShowReconstructionsSlotType::sptr m_slotShowReconstuctions;

};

} // namespace editor.
} // namespace uiMedDataQt.
