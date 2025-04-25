/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <core/com/slot.hpp>
#include <core/com/slots.hpp>

#include <data/series_set.hpp>
#include <data/vector.hpp>

#include <ui/__/editor.hpp>
#include <ui/qt/series/selector.hpp>

#include <QAbstractItemView>
#include <QObject>
#include <QPointer>
#include <QVector>

namespace sight::module::ui::qt::series
{

/**
 * @brief This editor shows information about the medical data. It allows to manipulate (select, erase, ...)
 *        studies and series.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="sight::module::ui::qt::series::selector">
        <inout key="seriesSet" uid="..." />
        <inout key="selection" uid="..." />
        <icons>
            <icon series="..." icon="..." />
            <icon series="..." icon="..." />
        </icons>
        <config removeStudyIcon="..." removeSerieIcon="..." selectionMode="extended"
            allowedRemove="true" insertMode="false" />
    </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b seriesSet [sight::data::series_set]: series_set on which the editor operates.
 * - \b selection [sight::data::vector]: defines the id of the data::vector where the selection will be put or get.
 *
 * @subsection Configuration Configuration
 * - \b selectionMode (optional, single/extended, default=extended): defines the selection mode for the series, where
 *                    extended means "multiple".
 * - \b allowedRemove (optional, bool, default=true): allows user to remove series.
 * - \b insertMode (optional, bool, default=false): only allows selection of module::ui::qt::InsertSeries.
 * - \b removeStudyIcon (optional, string, default=""): remove study button icon.
 * - \b removeSerieIcon (optional, string, default=""): remove series button icon.
 * - \b icons (optional): defines the icon to associate for a series.
 *     - \b series (mandatory, string): series name, e.g. {data::image_series, data::model_series, ...}.
 *     - \b icon (mandatory, string): icon path.
 */
class selector final :
    public QObject,
    public sight::ui::editor
{
Q_OBJECT

public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(selector, sight::ui::editor);

    /// Creates the signal and slots.
    selector();

    /// Destroys the service.
    ~selector() noexcept override;

protected:

    /// Configures the service according to the xml tags found.
    void configuring() override;

    /// Creates container and add selector.
    void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::series_set::ADDED_OBJECTS_SIG of s_SERIES_SET_INOUT to ADD_SERIES_SLOT
     * Connect data::series_set::REMOVED_OBJECTS_SIG of s_SERIES_SET_INOUT to REMOVE_SERIES_SLOT
     */
    connections_t auto_connections() const override;

    /// Fills selector with the series contained in series_set.
    void updating() override;

    /// Destroys GUI.
    void stopping() override;

protected Q_SLOTS:

    /**
     * @brief Manages the selection vector according to selection/deselection.
     * @param _selection series to add in selection vector.
     * @param _deselection series to remove from selection vector.
     */
    void on_selected_series(
        QVector<data::series::sptr> _selection,
        QVector<data::series::sptr> _deselection
    );

    /**
     * @brief Sends a 'seriesDoubleClicked' signal when the user double click on a series.
     * @param _index index of the clicked item in the selector.
     * @todo Manages double click on a study.
     */
    void on_double_click(const QModelIndex& _index);

    /**
     * @brief Removes series from series_set and notify.
     * @param _selection series to remove from series_set.
     */
    void on_remove_series(QVector<data::series::sptr> _selection);

private:

    using remove_series_slot_t = core::com::slot<void (data::series_set::container_t)>;

    using series_double_clicked_signal_t = core::com::signal<void (std::shared_ptr<data::series>)>;

    /// SLOT: adds series into the selector.
    void add_series(data::series_set::container_t _added_series);

    /// SLOT: removes series from the selector.
    void remove_series(data::series_set::container_t _removed_series);

    /// Contains the slot used to remove series from the selector.
    remove_series_slot_t::sptr m_slot_remove_series;

    /// Contains the selector widget.
    QPointer<sight::ui::qt::series::selector> m_selector_widget {nullptr};

    /// Contains the signal emitted when there is a double click on a series.
    series_double_clicked_signal_t::sptr m_sig_series_double_clicked {nullptr};

    /// Stores a map containing the specified icons for a series (map\<series classname, icon path\>).
    sight::ui::qt::series::selector::series_icon_t m_series_icons;

    /// Defines if series can be removed.
    bool m_remove_allowed {true};

    /// Defines the behaviour of the treeview selection mode.
    QAbstractItemView::SelectionMode m_selection_mode {QAbstractItemView::ExtendedSelection};

    /// Allows selection of module::ui::qt::InsertSeries only.
    bool m_insert_mode {false};

    /// Defines the path of the remove study button icon.
    std::filesystem::path m_remove_study_icon;

    /// Defines the path of the remove series button icon.
    std::filesystem::path m_remove_series_icon;

    /// Defines the columns to be displayed in the widget
    std::string m_displayed_columns =
        "PatientName/SeriesInstanceUID,PatientSex,PatientBirthDate/Icon,Modality,StudyDescription/SeriesDescription,StudyDate/SeriesDate,StudyTime/SeriesTime,PatientAge,BodyPartExamined,PatientPositionString,ContrastBolusAgent,AcquisitionTime,ContrastBolusStartTime";

    static constexpr std::string_view SERIES_SET = "seriesSet";
    static constexpr std::string_view SELECTION  = "selection";

    data::ptr<data::series_set, data::access::inout> m_series_set {this, SERIES_SET};
    data::ptr<data::vector, data::access::inout> m_selection {this, SELECTION};
};

} // namespace sight::module::ui::qt::series
