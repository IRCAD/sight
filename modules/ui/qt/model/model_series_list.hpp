/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "modules/ui/qt/config.hpp"

#include <core/com/signal.hpp>
#include <core/com/signals.hpp>
#include <core/com/slot.hpp>
#include <core/com/slots.hpp>
#include <core/tools/failed.hpp>

#include <data/model_series.hpp>
#include <data/reconstruction.hpp>

#include <ui/__/editor.hpp>

#include <QObject>
#include <QPointer>
#include <QPushButton>

class QTreeWidget;
class QCheckBox;
class QListWidgetItem;
class QTreeWidgetItem;

namespace sight::module::ui::qt::model
{

/**
 * @brief Editor displaying the list of the organs in a ModelSeries.
 *
 * It allows to show/hide a Reconstruction when it is associated to a render scene with a Model series adaptor
 * (for example fwRenderVTK::render with ::visuVTKAdaptor::model_series).
 * It also allows to select a Reconstruction: associated to the module::data::select_object, the
 * reconstruction is available to be used by other services.
 * It is mostly associated to module::ui::qt::reconstruction::organ_material_editor and
 * module::ui::qt::reconstruction::RepresentationEditor to update the reconstrution color, transparency, ....
 *
 * @section Signals Signals
 * - \b reconstructionSelected(data::object::sptr): this signal emits the selected reconstruction
 * - \b emptiedSelection(): this signal is emitted when no reconstruction is selected
 *
 * @section Slots Slots
 * - \b showReconstructions(bool): slot called to show or hide all the reconstructions
 *
 * @section XML XML Configuration
 * @code{.xml}
   <service module::ui::qt::editor::model_series_list">
       <inout key="modelSeries" uid="..." auto_connect="true" />
       <enable_hide_all>true</enable_hide_all>
       <columns>
         <organ_name/>
         <structure_type/>
         <volume/>
       </columns>
   </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b modelSeries [sight::data::model_series]: model series containing the organs to list
 *
 * @subsection Configuration Configuration
 * - \b enable_hide_all (optional, bool, default=true): if 'true', allows to hide all models through a single checkbox
 *      displayed in UI.
 * - \b enableDelete (optional, bool, default=false): if 'true', allows to delete models through a single button
 *      displayed in UI.
 * - \b column (optional, string, default=""): defines columns to be shown in reconstruction list. XML child element
 *      names follow data::reconstruction serialization attribute names.
 *      The name of the tag will be used as the column name.
 *      The attribute 'view' is optional and has the following values:
 *  - positive: a numeric value is displayed only if it is positive. Otherwise, 'Unknown' is displayed.
 */
class MODULE_UI_QT_CLASS_API model_series_list final :
    public QObject,
    public sight::ui::editor
{
Q_OBJECT

public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(model_series_list, sight::ui::editor);

    /// Initializes the slot and signals.
    MODULE_UI_QT_API model_series_list() noexcept;

    /// Cleans ressources.
    MODULE_UI_QT_API ~model_series_list() noexcept final = default;

protected:

    /// Configures the editor.
    void configuring() final;

    /// Creates layouts.
    void starting() final;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::model_series::MODIFIED_SIG of s_MODEL_SERIES_INOUT to service::slots::UPDATE.
     * Connect data::model_series::RECONSTRUCTIONS_ADDED_SIG of s_MODEL_SERIES_INOUT to service::slots::UPDATE.
     * Connect data::model_series::RECONSTRUCTIONS_REMOVED_SIG of s_MODEL_SERIES_INOUT to service::slots::UPDATE.
     */
    connections_t auto_connections() const final;

    /**
     * @brief Refreshes the editor.
     * @see updateReconstructions().
     * @see refreshVisibility().
     */
    void updating() final;

    /// Disconnects connections.
    void stopping() final;

private:

    /// Updates reconstructions.
    void updateReconstructions();

    /// Fills the editor tree.
    void fillTree(const data::mt::locked_ptr<data::model_series>& _model_series);

    /// SLOT: Shows (or hide) reconstructions.
    void showReconstructions(bool _show);

    /// Refreshes reconstructions visibility on the editor.
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
    void onCurrentItemChanged(QTreeWidgetItem* _current, QTreeWidgetItem* /*unused*/);

    /// Changes the current item, called when new current item is setted in m_organChoice.
    static void onCurrentItemChanged(QTreeWidgetItem* _current, int _column);

    /// Shows reconstructions, called when m_showCheckBox is clicked.
    void onShowReconstructions(int _state);

    /// Shows a reconstruction, called when new current item is setted in m_organChoice.
    static void onOrganChoiceVisibility(QTreeWidgetItem* _item, int /*unused*/);

    /// Shows reconstructions, called when m_checkAllButton is clicked.
    void onCheckAllCheckBox();

    /// Shows reconstructions, called when m_unCheckAllButton is clicked.
    void onUnCheckAllCheckBox();

    /// Deletes all reconstructions, called when m_deleteAllButton is clicked.
    void onDeleteAllCheckBox();

    /// Opens a context menu to deletes a specific reconstruction.
    void onCustomContextMenuRequested(const QPoint& _pos);

private:

    /// Contains the button to check all reconstructions.
    QPointer<QPushButton> m_checkAllButton;

    /// Contains the button to uncheck all reconstructions.
    QPointer<QPushButton> m_unCheckAllButton;

    /// Contains the button to delete all reconstructions.
    QPointer<QPushButton> m_deleteAllButton;

    /// Contains the button to hide or show all reconstructions.
    QPointer<QCheckBox> m_showCheckBox;

    /// Contains the reconstructions tree:
    QPointer<QTreeWidget> m_tree;

    /// Enables m_showCheckBox.
    bool m_enableHideAll {true};

    /// Enables m_deleteAllButton.
    bool m_enableDelete {false};

    /// Defines the header of the tree.
    QStringList m_headers;

    /// Contains the signal emitted when a reconstruction is selected.
    typedef core::com::signal<void (data::object::sptr)> reconstruction_selected_signal_t;
    reconstruction_selected_signal_t::sptr m_sigReconstructionSelected;

    /// Contains the signal emitted when we clean the list.
    typedef core::com::signal<void ()> emptied_selection_signal_t;
    emptied_selection_signal_t::sptr m_sigEmptiedSelection;

    /// Contains the slot to show (or hide) reconstructions.
    typedef core::com::slot<void (bool)> show_reconstructions_slot_t;
    show_reconstructions_slot_t::sptr m_slotShowReconstructions;

    static constexpr std::string_view s_MODEL_SERIES = "modelSeries";
    data::ptr<data::model_series, data::Access::inout> m_modelSeries {this, "modelSeries", true};
};

} // namespace sight::module::ui::qt::model
