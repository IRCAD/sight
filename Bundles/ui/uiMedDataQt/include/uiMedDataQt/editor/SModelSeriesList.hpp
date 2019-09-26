/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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
 * It is mostly associated to ::uiReconstructionQt::OrganMaterialEditor and
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
 *
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
 * @subsection In-Out In-Out
 * - \b modelSeries [::fwMedData::ModelSeries]: model series containing the organs to list
 * @subsection Configuration Configuration
 * \b enable_hide_all: if 'true', allows to hide all models through a single checkbox displayed in UI (default
 * value is 'true', allowed values are 'true' and 'false').
 *
 * \b column: defines colums to be shown in reconstruction list. XML child element names follow
 * ::fwData::Reconstruction serialization attribute names. The name of the tag will be used as the column name.
 * The attribute 'view' is optional and has the following values:
 *  - positive: a numeric value is displayed only if it is positive. Otherwise, 'Unknown' is displayed.
 */
class UIMEDDATAQT_CLASS_API SModelSeriesList :  public QObject,
                                                public ::fwGui::editor::IEditor
{
Q_OBJECT
public:

    fwCoreServiceMacro(SModelSeriesList, ::fwGui::editor::IEditor);

    /// Constructor. Do nothing.
    UIMEDDATAQT_API SModelSeriesList() noexcept;

    /// Destructor. Do nothing.
    UIMEDDATAQT_API virtual ~SModelSeriesList() noexcept;

protected:

    ///This method launches the IEditor::starting method.
    virtual void starting() override;

    ///This method launches the IEditor::stopping method.
    virtual void stopping() override;

    virtual void updating() override;

    virtual void swapping() override;

    /// Configures the editor.
    virtual void configuring() override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect ModelSeries::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect ModelSeries::s_RECONSTRUCTIONS_ADDED_SIG to this::s_UPDATE_SLOT
     * Connect ModelSeries::s_RECONSTRUCTIONS_REMOVED_SIG to this::s_UPDATE_SLOT
     */
    UIMEDDATAQT_API virtual KeyConnectionsMap getAutoConnections() const override;

    typedef std::map< std::string, ValueView* > DisplayedInformation;

    void updateReconstructions();

    void fillTree();

    static const ::fwCom::Signals::SignalKeyType s_RECONSTRUCTION_SELECTED_SIG;
    typedef ::fwCom::Signal< void (::fwData::Object::sptr) > ReconstructionSelectedSignalType;
    static const ::fwCom::Signals::SignalKeyType s_EMPTIED_SELECTION_SIG;
    typedef ::fwCom::Signal< void () > EmptiedSelectionSignalType;
    static const ::fwCom::Slots::SlotKeyType s_SHOW_RECONSTRUCTIONS_SLOT;
    typedef ::fwCom::Slot< void (bool) > ShowReconstructionsSlotType;

protected Q_SLOTS:

    /// Slot called when new current item is setted in m_organChoice
    void onCurrentItemChanged ( QTreeWidgetItem* current, QTreeWidgetItem* previous );

    void onCurrentItemChanged ( QTreeWidgetItem* current, int column );

    void onShowReconstructions(int state);

    void onOrganChoiceVisibility(QTreeWidgetItem* item, int column);

    void onCheckAllCheckBox();
    void onUnCheckAllCheckBox();

private:

    /// SLOT: Show (or hide) reconstructions
    void showReconstructions(bool show);

    void refreshVisibility();

    void onCheckAllBoxes(bool visible);

    QPointer<QPushButton> m_checkAllButton;
    QPointer<QPushButton> m_unCheckAllButton;

    QPointer< QCheckBox > m_showCheckBox;
    QPointer< QTreeWidget > m_tree;
    DisplayedInformation m_displayedInfo;

    bool m_enableHideAll;

    QStringList m_headers;

    /// Signal emitted when a reconstruction is selected
    ReconstructionSelectedSignalType::sptr m_sigReconstructionSelected;

    /// Signal emitted when we clean the list
    EmptiedSelectionSignalType::sptr m_sigEmptiedSelection;

    /// Slot to show (or hide) reconstructions
    ShowReconstructionsSlotType::sptr m_slotShowReconstuctions;
};

} // namespace editor
} // namespace uiMedDataQt
