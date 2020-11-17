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
#include "uiMedDataQt/widget/Selector.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwData/Vector.hpp>

#include <fwGui/editor/IEditor.hpp>

#include <fwMedData/SeriesDB.hpp>

#include <QAbstractItemView>
#include <QObject>
#include <QPointer>
#include <QVector>

namespace uiMedDataQt
{
namespace editor
{
/**
 * @brief This editor shows information about the medical data. It allows to manipulate (select, erase, ...)
 *        studies and series.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="::uiMedDataQt::editor::SSelector">
        <inout key="seriesDB" uid="..." />
        <inout key="selection" uid="..." />
        <selectionMode>single|extended</selectionMode>
        <allowedRemove>yes|no</allowedRemove>
        <insertMode>yes|no</insertMode>
        <icons>
            <icon series="..." icon="..." />
            <icon series="..." icon="..." />
        </icons>
    </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b seriesDB [::fwMedData::SeriesDB]: seriesDB on which the editor operates.
 * - \b selection [::fwData::Vector]: defines the id of the ::fwData::Vector where the selection will be put or get.
 *
 * @subsection Configuration Configuration
 * - \b selectionMode (optional, single/extended, default=extended): defines the selection mode for the series, where
 *                    extended means "multiple".
 * - \b allowedRemove (optional, yes/no, default=yes): allows user to remove series.
 * - \b insertMode (optional, yes/no, default=no): only allows selection of uiMedDataQt::InsertSeries.
 * - \b icons (optional): defines the icon to associate for a series.
 *     - \b series (mandatory, string): series name, e.g. {::fwMedData::ImageSeries, ::fwMedData::ModelSeries, ...}.
 *     - \b icon (mandatory, string): icon path.
 */
class UIMEDDATAQT_CLASS_API SSelector final :
    public QObject,
    public ::fwGui::editor::IEditor
{

Q_OBJECT

public:

    fwCoreServiceMacro(SSelector, ::fwGui::editor::IEditor)

    /// Creates the signal and slots.
    UIMEDDATAQT_API SSelector();

    /// Destroys the service.
    UIMEDDATAQT_API virtual ~SSelector() noexcept;

protected:

    /// Configures the service according to the xml tags found.
    UIMEDDATAQT_API void configuring() override;

    /// Creates container and add selector.
    UIMEDDATAQT_API void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect ::fwMedData::SeriesDB::s_ADDED_SERIES_SIG of s_SERIES_DB_INOUT to s_ADD_SERIES_SLOT
     * Connect ::fwMedData::SeriesDB::s_REMOVED_SERIES_SIG of s_SERIES_DB_INOUT to s_REMOVE_SERIES_SLOT
     */
    UIMEDDATAQT_API virtual KeyConnectionsMap getAutoConnections() const override;

    /// Fills selector with the series contained in SeriesDB.
    UIMEDDATAQT_API void updating() override;

    /// Destroys GUI.
    UIMEDDATAQT_API void stopping() override;

protected Q_SLOTS:

    /**
     * @brief Manages the selection vector according to selection/deselection.
     * @param _selection series to add in selection vector.
     * @param _deselection series to remove from selection vector.
     */
    void onSelectedSeries(QVector< ::fwMedData::Series::sptr > _selection,
                          QVector< ::fwMedData::Series::sptr > _deselection);

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
    void onRemoveSeries(QVector< ::fwMedData::Series::sptr > _selection);

private:

    typedef ::fwCom::Slot<void (::fwMedData::SeriesDB::ContainerType)> RemoveSeriesSlotType;
    typedef ::fwCom::Signal< void ( SPTR( ::fwMedData::Series ) ) > SeriesDoubleClickedSignalType;

    /// SLOT: adds series into the selector.
    void addSeries(::fwMedData::SeriesDB::ContainerType addedSeries);

    /// SLOT: removes series from the selector.
    void removeSeries(::fwMedData::SeriesDB::ContainerType removedSeries);

    /// Contains the slot used to remove series from the selector.
    RemoveSeriesSlotType::sptr m_slotRemoveSeries;

    /// Contains the selector widget.
    QPointer< ::uiMedDataQt::widget::Selector > m_selectorWidget { nullptr };

    /// Contains the signal emitted when there is a double click on a series.
    SeriesDoubleClickedSignalType::sptr m_sigSeriesDoubleClicked { nullptr };

    /// Stores a map containing the specified icons for a series (map\<series classname, icon path\>).
    ::uiMedDataQt::widget::Selector::SeriesIconType m_seriesIcons;

    /// Defines if series can be removed.
    bool m_allowedRemove { true };

    /// Defines the behaviour of the treeview selection mode.
    QAbstractItemView::SelectionMode m_selectionMode { QAbstractItemView::ExtendedSelection };

    /// Allows selection of uiMedDataQt::InsertSeries only.
    bool m_insertMode { false };

};
} // namespace editor.
} // namespace uiMedDataQt.
