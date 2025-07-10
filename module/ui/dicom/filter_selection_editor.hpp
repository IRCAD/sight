/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

#include <core/base.hpp>

#include <data/series_set.hpp>
#include <data/vector.hpp>

#include <filter/dicom/filter.hpp>

#include <ui/__/editor.hpp>

#include <QCheckBox>
#include <QComboBox>
#include <QListWidget>
#include <QPointer>
#include <QPushButton>
#include <QShortcut>

namespace sight::module::ui::dicom
{

/**
 * @brief   This editor service is used to select and apply filters to Dicom series.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="sight::module::ui::dicom::filter_selection_editor">
            <in key="selection" uid="..." />
            <inout key="target" uid="..." />
       </service>
   @endcode
 * @subsection Input Input:
 * - \b selection [sight::data::vector]: selection.
 * @subsection In-Out In-Out:
 * - \b target [sight::data::series_set]: series_set where to put the selection.
 */

class filter_selection_editor : public QObject,
                                public sight::ui::editor
{
Q_OBJECT;

public:

    SIGHT_DECLARE_SERVICE(filter_selection_editor, sight::ui::editor);

    using filter_map_t = std::map<std::string, sight::filter::dicom::filter::sptr>;

    /**
     * @brief Constructor
     */
    filter_selection_editor() noexcept;

    /**
     * @brief Destructor
     */
    ~filter_selection_editor() noexcept override;

private Q_SLOTS:

    /// Slot called when a new filter type is selected in the context menu
    void add_filter(int _filter_type_index);

    /// Slot called when the add filter button is pressed
    void add_filter_at_the_end();

    /// Slot called when the remove filter button is pressed
    void remove_filter();

    /// Slot called when a new filter is selected
    void update_button_status(int _filter_index);

    /// Slot called when the configure filter button is pressed
    void configure_filter();

    /// Slot called when the split filter button is pressed
    void split_filter();

    /// Slot called when the apply filters button is pressed
    void apply_filters();

    /// Slot used to display the context menu on a filter
    void show_context_menu_for_selected_filter(const QPoint& _pos);

    /// Slot used to display a warning when ignore errors checkbox is checked
    static void on_force_checked(int _state);

protected:

    /// Do nothing.
    void configuring() override;

    /// Override
    void starting() override;

    /// Override
    void stopping() override;

    /// Override
    void updating() override;

    /// Override
    void info(std::ostream& _sstream) override;

    /// Fill the combobox with the list of available filters
    void fill_available_filters();

    /**
     * @brief Create and add a filter in a specific position
     * @param[in] _filter_type_index filter type index in the combobox
     * @param[in] _position Position in the list
     */
    void add_filter(int _filter_type_index, int _position);

    /**
     * @brief Sort filters
     * @param[in] _a First filter
     * @param[in] _b Second filter
     */
    static bool sort_filters(
        const sight::filter::dicom::filter::sptr& _a,
        const sight::filter::dicom::filter::sptr& _b
    );

    static QIcon get_filter_icon(sight::filter::dicom::filter::sptr _filter);

    static std::string get_filter_description(sight::filter::dicom::filter::sptr _filter);

    /// Combo box displaying the available filters
    QPointer<QComboBox> m_available_filter_list_widget;

    /// Selected filters list widget
    QPointer<QListWidget> m_selected_filter_list_widget;

    /// Button used to add a filter in the list
    QPointer<QPushButton> m_add_filter_button;

    /// Button used to remove a filter in the list
    QPointer<QPushButton> m_remove_filter_button;

    /// Button used to configure a filter in the list
    QPointer<QPushButton> m_configure_filter_button;

    /// Button used to apply filters of the list
    QPointer<QPushButton> m_apply_filters_button;

    /// Button used to split a composite filter
    QPointer<QPushButton> m_split_filter_button;

    /// Delete key shortcut
    QPointer<QShortcut> m_delete_shortcut;

    /// Forced apply check box
    QPointer<QCheckBox> m_forced_apply_check_box;

    /// Map used to store filters and their keys
    filter_map_t m_filters_map;

    /// Destination series_set ID
    std::string m_dest_series_set_id;

    data::ptr<data::series_set, sight::data::access::inout> m_dest_series_set {this, "target"};
    data::ptr<data::vector, sight::data::access::in> m_selected_dicom_series {this, "selection"};
};

} // namespace sight::module::ui::dicom
