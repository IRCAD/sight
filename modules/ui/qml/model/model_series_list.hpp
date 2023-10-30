/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
 * Copyright (C) 2018-2019 IHU Strasbourg
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

#include "modules/ui/qml/config.hpp"
#include "modules/ui/qml/model/organ_list_model.hpp"

#include <ui/qml/editor.hpp>

#include <QObject>

namespace sight::module::ui::qml::model
{

/**
 * @brief   model_series_list service allows to manage the ModelSeries organs.
 *
 * @section Signals Signals
 * - \b reconstructionSelected(data::object::sptr): this signal emits the selected reconstruction
 * - \b emptiedSelection(): this signal is emitted when no reconstruction is selected
 *
 * @section QSlots Qt Slots
 * - \b onOrganSelected(int): called when an organ is selected, the index represents the index of the selected
 *      reconstruction into the modelSeries
 * - \b onShowReconstructions(int): called to show/hide all the reconstructions
 * - \b onOrganVisibilityChanged(int, bool): called to show/hide a reconstruction
 * - \b onCheckAllBoxes(bool): called when the organ's check box are checked/unchecked and thus shown/hidden
 *
 * @section QML Qml Configuration
 *
 * @code{.qml}
    model_series_list {
        id: modelSeriesList

        // @disable-check M16
        listModel: organListModel

        // @disable-check M16
        onStarted: {
            modelList.enabled = true
        }
    }
   @endcode
 *
 * @section Objects Required objects
 *
 * @subsection In-Out In-Out
 * - \b modelSeries [sight::data::model_series]: model series containing the organs to list
 */
class MODULE_UI_QML_CLASS_API model_series_list : public sight::ui::qml::editor
{
Q_OBJECT
Q_PROPERTY(organ_list_model * list_model MEMBER m_list_model)

public:

    SIGHT_DECLARE_SERVICE(model_series_list, sight::ui::qml::editor);

    /// Constructor. Do nothing.
    MODULE_UI_QML_API model_series_list() noexcept;

    /// Destructor. Do nothing.
    MODULE_UI_QML_API ~model_series_list() noexcept override;

protected:

    /// Call the editor::starting method.
    void starting() override;

    /// Call the editor::stopping method.
    void stopping() override;

    /// Update the list model with the modelSeries items
    void updating() override;

    /// Do nothing
    void configuring() override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect ModelSeries::MODIFIED_SIG to this::service::slots::UPDATE
     * Connect ModelSeries::RECONSTRUCTIONS_ADDED_SIG to this::service::slots::UPDATE
     * Connect ModelSeries::RECONSTRUCTIONS_REMOVED_SIG to this::service::slots::UPDATE
     */
    connections_t auto_connections() const override;

    static const core::com::signals::key_t RECONSTRUCTION_SELECTED_SIG;
    using reconstruction_selected_signal_t = core::com::signal<void (data::object::sptr)>;

    static const core::com::signals::key_t EMPTIED_SELECTION_SIG;
    using emptied_selection_signal_t = core::com::signal<void ()>;

protected Q_SLOTS:

    /// Called when an organ is selected, the index represents the index of the selected reconstruction into the
    // modelSeries
    void on_organ_selected(int _index);

    /// Called when the reconstruction should be shown/hidden
    void on_show_reconstructions(int _state);

    /// Called when the visibility of an organ should change
    void on_organ_visibility_changed(int _index, bool _visible);

    /// Called when all the reconstruction are checked/unchecked, and
    void on_check_all_boxes(bool _checked);

private:

    /// Signal emitted when a reconstruction is selected
    reconstruction_selected_signal_t::sptr m_sig_reconstruction_selected;

    /// Signal emitted when we clean the list
    emptied_selection_signal_t::sptr m_sig_emptied_selection;

    organ_list_model* m_list_model {nullptr};

    /// Model series data
    static constexpr std::string_view MODEL_SERIES_INOUT = "modelSeries";
    data::ptr<data::model_series, data::access::inout> m_model_series {this, MODEL_SERIES_INOUT, true};
};

} // namespace sight::module::ui::qml::model
