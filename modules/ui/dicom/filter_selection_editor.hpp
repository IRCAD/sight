/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "modules/ui/dicom/config.hpp"

#include <core/base.hpp>

#include <data/series_set.hpp>
#include <data/vector.hpp>

#include <filter/dicom/filter.hpp>

#include <ui/__/editor.hpp>

#include <QCheckBox>
#include <QComboBox>
#include <QListWidget>
#include <QObject>
#include <QPointer>
#include <QPushButton>
#include <QShortcut>
#include <QWidget>

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

class MODULE_UI_DICOM_CLASS_API filter_selection_editor : public QObject,
                                                          public sight::ui::editor
{
Q_OBJECT;

public:

    SIGHT_DECLARE_SERVICE(filter_selection_editor, sight::ui::editor);

    typedef std::map<std::string, sight::filter::dicom::filter::sptr> FilterMapType;

    /**
     * @brief Constructor
     */
    MODULE_UI_DICOM_API filter_selection_editor() noexcept;

    /**
     * @brief Destructor
     */
    MODULE_UI_DICOM_API ~filter_selection_editor() noexcept override;

private Q_SLOTS:

    /// Slot called when a new filter type is selected in the context menu
    MODULE_UI_DICOM_API void addFilter(int filterTypeIndex);

    /// Slot called when the add filter button is pressed
    MODULE_UI_DICOM_API void addFilterAtTheEnd();

    /// Slot called when the remove filter button is pressed
    MODULE_UI_DICOM_API void removeFilter();

    /// Slot called when a new filter is selected
    MODULE_UI_DICOM_API void updateButtonStatus(int filterIndex);

    /// Slot called when the configure filter button is pressed
    MODULE_UI_DICOM_API void configureFilter();

    /// Slot called when the split filter button is pressed
    MODULE_UI_DICOM_API void splitFilter();

    /// Slot called when the apply filters button is pressed
    MODULE_UI_DICOM_API void applyFilters();

    /// Slot used to display the context menu on a filter
    MODULE_UI_DICOM_API void showContextMenuForSelectedFilter(const QPoint& pos);

    /// Slot used to display a warning when ignore errors checkbox is checked
    static MODULE_UI_DICOM_API void onForceChecked(int state);

protected:

    /// Do nothing.
    MODULE_UI_DICOM_API void configuring() override;

    /// Override
    MODULE_UI_DICOM_API void starting() override;

    /// Override
    MODULE_UI_DICOM_API void stopping() override;

    /// Override
    MODULE_UI_DICOM_API void updating() override;

    /// Override
    MODULE_UI_DICOM_API void info(std::ostream& _sstream) override;

    /// Fill the combobox with the list of available filters
    MODULE_UI_DICOM_API void fillAvailableFilters();

    /**
     * @brief Create and add a filter in a specific position
     * @param[in] filterTypeIndex Filter type index in the combobox
     * @param[in] position Position in the list
     */
    MODULE_UI_DICOM_API void addFilter(int filterTypeIndex, int position);

    /**
     * @brief Sort filters
     * @param[in] a First filter
     * @param[in] b Second filter
     */
    static bool sortFilters(const sight::filter::dicom::filter::sptr& a, const sight::filter::dicom::filter::sptr& b);

    static QIcon getFilterIcon(sight::filter::dicom::filter::sptr filter);

    static std::string getFilterDescription(sight::filter::dicom::filter::sptr filter);

    /// Combo box displaying the available filters
    QPointer<QComboBox> m_availableFilterListWidget;

    /// Selected filters list widget
    QPointer<QListWidget> m_selectedFilterListWidget;

    /// Button used to add a filter in the list
    QPointer<QPushButton> m_addFilterButton;

    /// Button used to remove a filter in the list
    QPointer<QPushButton> m_removeFilterButton;

    /// Button used to configure a filter in the list
    QPointer<QPushButton> m_configureFilterButton;

    /// Button used to apply filters of the list
    QPointer<QPushButton> m_applyFiltersButton;

    /// Button used to split a composite filter
    QPointer<QPushButton> m_splitFilterButton;

    /// Delete key shortcut
    QPointer<QShortcut> m_deleteShortcut;

    /// Forced apply check box
    QPointer<QCheckBox> m_forcedApplyCheckBox;

    /// Map used to store filters and their keys
    FilterMapType m_filtersMap;

    /// Destination series_set ID
    std::string m_dest_series_setID;

    data::ptr<data::series_set, sight::data::Access::inout> m_dest_series_set {this, "target"};
    data::ptr<data::vector, sight::data::Access::in> m_selectedDicomSeries {this, "selection"};
};

} // namespace sight::module::ui::dicom
