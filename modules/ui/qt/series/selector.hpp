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

#include <core/com/slot.hpp>
#include <core/com/slots.hpp>

#include <data/series_set.hpp>
#include <data/vector.hpp>

#include <ui/__/editor.hpp>
#include <ui/qt/series/Selector.hpp>

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
class MODULE_UI_QT_CLASS_API selector final :
    public QObject,
    public sight::ui::editor
{
Q_OBJECT

public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(selector, sight::ui::editor);

    /// Creates the signal and slots.
    MODULE_UI_QT_API selector();

    /// Destroys the service.
    MODULE_UI_QT_API ~selector() noexcept override;

protected:

    /// Configures the service according to the xml tags found.
    MODULE_UI_QT_API void configuring() override;

    /// Creates container and add selector.
    MODULE_UI_QT_API void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::series_set::ADDED_OBJECTS_SIG of s_SERIES_SET_INOUT to ADD_SERIES_SLOT
     * Connect data::series_set::REMOVED_OBJECTS_SIG of s_SERIES_SET_INOUT to REMOVE_SERIES_SLOT
     */
    MODULE_UI_QT_API connections_t auto_connections() const override;

    /// Fills selector with the series contained in series_set.
    MODULE_UI_QT_API void updating() override;

    /// Destroys GUI.
    MODULE_UI_QT_API void stopping() override;

protected Q_SLOTS:

    /**
     * @brief Manages the selection vector according to selection/deselection.
     * @param _selection series to add in selection vector.
     * @param _deselection series to remove from selection vector.
     */
    void onSelectedSeries(
        QVector<data::series::sptr> _selection,
        QVector<data::series::sptr> _deselection
    );

    /**
     * @brief Sends a 'seriesDoubleClicked' signal when the user double click on a series.
     * @param _index index of the clicked item in the selector.
     * @todo Manages double click on a study.
     */
    void onDoubleClick(const QModelIndex& _index);

    /**
     * @brief Removes series from series_set and notify.
     * @param _selection series to remove from series_set.
     */
    void onRemoveSeries(QVector<data::series::sptr> _selection);

private:

    typedef core::com::slot<void (data::series_set::container_type)> RemoveSeriesSlotType;

    typedef core::com::signal<void (SPTR(data::series))> SeriesDoubleClickedSignalType;

    /// SLOT: adds series into the selector.
    void addSeries(data::series_set::container_type addedSeries);

    /// SLOT: removes series from the selector.
    void removeSeries(data::series_set::container_type removedSeries);

    /// Contains the slot used to remove series from the selector.
    RemoveSeriesSlotType::sptr m_slotRemoveSeries;

    /// Contains the selector widget.
    QPointer<sight::ui::qt::series::Selector> m_selectorWidget {nullptr};

    /// Contains the signal emitted when there is a double click on a series.
    SeriesDoubleClickedSignalType::sptr m_sigSeriesDoubleClicked {nullptr};

    /// Stores a map containing the specified icons for a series (map\<series classname, icon path\>).
    sight::ui::qt::series::Selector::SeriesIconType m_seriesIcons;

    /// Defines if series can be removed.
    bool m_removeAllowed {true};

    /// Defines the behaviour of the treeview selection mode.
    QAbstractItemView::SelectionMode m_selectionMode {QAbstractItemView::ExtendedSelection};

    /// Allows selection of module::ui::qt::InsertSeries only.
    bool m_insertMode {false};

    /// Defines the path of the remove study button icon.
    std::filesystem::path m_removeStudyIcon;

    /// Defines the path of the remove series button icon.
    std::filesystem::path m_removeSeriesIcon;

    /// Defines the columns to be displayed in the widget
    std::string m_displayedColumns =
        "PatientName/SeriesInstanceUID,PatientSex,PatientBirthDate/Icon,Modality,StudyDescription/SeriesDescription,StudyDate/SeriesDate,StudyTime/SeriesTime,PatientAge,BodyPartExamined,PatientPositionString,ContrastBolusAgent,AcquisitionTime,ContrastBolusStartTime";

    static constexpr std::string_view s_SERIES_SET = "seriesSet";
    static constexpr std::string_view s_SELECTION  = "selection";

    data::ptr<data::series_set, data::Access::inout> m_series_set {this, s_SERIES_SET, true};
    data::ptr<data::vector, data::Access::inout> m_selection {this, s_SELECTION};
};

} // namespace sight::module::ui::qt::series
