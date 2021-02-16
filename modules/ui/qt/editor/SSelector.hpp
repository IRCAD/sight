/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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
#include "modules/ui/qt/widget/Selector.hpp"

#include <core/com/Slot.hpp>
#include <core/com/Slots.hpp>

#include <data/SeriesDB.hpp>
#include <data/Vector.hpp>

#include <QAbstractItemView>
#include <QObject>
#include <QPointer>
#include <QVector>

#include <ui/base/IEditor.hpp>

namespace sight::modules::ui::qt
{
namespace editor
{
/**
 * @brief This editor shows information about the medical data. It allows to manipulate (select, erase, ...)
 *        studies and series.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="::sight::modules::ui::qt::editor::SSelector">
        <inout key="seriesDB" uid="..." />
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
 * - \b seriesDB [sight::data::SeriesDB]: seriesDB on which the editor operates.
 * - \b selection [sight::data::Vector]: defines the id of the data::Vector where the selection will be put or get.
 *
 * @subsection Configuration Configuration
 * - \b selectionMode (optional, single/extended, default=extended): defines the selection mode for the series, where
 *                    extended means "multiple".
 * - \b allowedRemove (optional, bool, default=true): allows user to remove series.
 * - \b insertMode (optional, bool, default=false): only allows selection of modules::ui::qt::InsertSeries.
 * - \b removeStudyIcon (optional, string, default=""): remove study button icon.
 * - \b removeSerieIcon (optional, string, default=""): remove serie button icon.
 * - \b icons (optional): defines the icon to associate for a series.
 *     - \b series (mandatory, string): series name, e.g. {data::ImageSeries, data::ModelSeries, ...}.
 *     - \b icon (mandatory, string): icon path.
 */
class MODULE_UI_QT_CLASS_API SSelector final :
    public QObject,
    public sight::ui::base::IEditor
{

Q_OBJECT

public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(SSelector, sight::ui::base::IEditor)

    /// Creates the signal and slots.
    MODULE_UI_QT_API SSelector();

    /// Destroys the service.
    MODULE_UI_QT_API virtual ~SSelector() noexcept;

protected:

    /// Configures the service according to the xml tags found.
    MODULE_UI_QT_API void configuring() override;

    /// Creates container and add selector.
    MODULE_UI_QT_API void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::SeriesDB::s_ADDED_SERIES_SIG of s_SERIES_DB_INOUT to s_ADD_SERIES_SLOT
     * Connect data::SeriesDB::s_REMOVED_SERIES_SIG of s_SERIES_DB_INOUT to s_REMOVE_SERIES_SLOT
     */
    MODULE_UI_QT_API virtual KeyConnectionsMap getAutoConnections() const override;

    /// Fills selector with the series contained in SeriesDB.
    MODULE_UI_QT_API void updating() override;

    /// Destroys GUI.
    MODULE_UI_QT_API void stopping() override;

protected Q_SLOTS:

    /**
     * @brief Manages the selection vector according to selection/deselection.
     * @param _selection series to add in selection vector.
     * @param _deselection series to remove from selection vector.
     */
    void onSelectedSeries(QVector< data::Series::sptr > _selection,
                          QVector< data::Series::sptr > _deselection);

    /**
     * @brief Sends a 'seriesDoubleClicked' signal when the user double click on a series.
     * @param _index index of the clicked item in the selector.
     * @todo Manages double click on a study.
     */
    void onDoubleClick(const QModelIndex& _index);

    /**
     * @brief Removes series from seriesDB and notify.
     * @param _selection series to remove from seriesDB.
     */
    void onRemoveSeries(QVector< data::Series::sptr > _selection);

private:

    typedef core::com::Slot<void (data::SeriesDB::ContainerType)> RemoveSeriesSlotType;

    typedef core::com::Signal< void ( SPTR( data::Series ) ) > SeriesDoubleClickedSignalType;

    /// SLOT: adds series into the selector.
    void addSeries(data::SeriesDB::ContainerType addedSeries);

    /// SLOT: removes series from the selector.
    void removeSeries(data::SeriesDB::ContainerType removedSeries);

    /// Contains the slot used to remove series from the selector.
    RemoveSeriesSlotType::sptr m_slotRemoveSeries;

    /// Contains the selector widget.
    QPointer< modules::ui::qt::widget::Selector > m_selectorWidget { nullptr };

    /// Contains the signal emitted when there is a double click on a series.
    SeriesDoubleClickedSignalType::sptr m_sigSeriesDoubleClicked { nullptr };

    /// Stores a map containing the specified icons for a series (map\<series classname, icon path\>).
    modules::ui::qt::widget::Selector::SeriesIconType m_seriesIcons;

    /// Defines if series can be removed.
    bool m_allowedRemove { true };

    /// Defines the behaviour of the treeview selection mode.
    QAbstractItemView::SelectionMode m_selectionMode { QAbstractItemView::ExtendedSelection };

    /// Allows selection of modules::ui::qt::InsertSeries only.
    bool m_insertMode { false };

    /// Defines the path of the remove study button icon.
    std::filesystem::path m_removeStudyIcon;

    /// Defines the path of the remove serie button icon.
    std::filesystem::path m_removeSerieIcon;

};
} // namespace editor.
} // namespace sight::modules::ui::qt.
