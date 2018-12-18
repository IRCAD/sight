/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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
 * @brief   This editor shows information about the medical data. It allows to manipulate
 *          (select, erase, ...) studies and series.
 *
 * @section XML XML Configuration
 *
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
 * @subsection In-Out In-Out
 * - \b seriesDB [::fwMedData::SeriesDB]: seriesDB on which the editor operates.
 * - \b selection [::fwData::Vector]: defines the id of the ::fwData::Vector where the selection will be put or get.
 * @subsection Configuration Configuration
 * - \b selectionMode : defines the selection mode for the series, among {"single", "extended"}, where extended means
 *"multiple"
 * - \b allowedRemove : allows user to remove series, among {"yes", "no"}
 * - \b insertMode : only allows selection of uiMedDataQt::InsertSeries, among {"yes", "no"}
 * - \b icons : defines the icon to associate for a series
 *    - \b series : the series classname, e.g. {::fwMedData::ImageSeries, ::fwMedData::ModelSeries, ...}
 *    - \b icon : the icon path
 */
class UIMEDDATAQT_CLASS_API SSelector : public QObject,
                                        public ::fwGui::editor::IEditor
{
Q_OBJECT
public:
    fwCoreServiceClassDefinitionsMacro( (SSelector)(::fwGui::editor::IEditor) );

    /// Constructor
    UIMEDDATAQT_API SSelector();

    /// Destructor
    UIMEDDATAQT_API virtual ~SSelector() noexcept;

    typedef ::fwCom::Signal< void ( SPTR( ::fwMedData::Series ) ) > SeriesDoubleClickedSignalType;

    /// Key in m_signals map of signal m_sigSeriesDoubleClicked
    UIMEDDATAQT_API static const ::fwCom::Signals::SignalKeyType s_SERIES_DOUBLE_CLICKED_SIG;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect SeriesDB::s_ADDED_SERIES_SIG to this::s_ADD_SERIES_SLOT
     * Connect SeriesDB::s_REMOVED_SERIES_SIG to this::s_REMOVE_SERIES_SLOT
     */
    UIMEDDATAQT_API virtual KeyConnectionsMap getAutoConnections() const override;

protected:

    /// Installs GUI : create container and add selector.
    virtual void starting() override;

    /// Destroys GUI.
    virtual void stopping() override;

    /// Configures the service according to the xml tags found.
    virtual void configuring() override;

    /// Fill selector with the series contained in SeriesDB.
    virtual void updating() override;

    virtual void info( std::ostream& _sstream ) override;

protected Q_SLOTS:

    /**
     * @brief Manages the selection vector according to selection/deselection.
     * @param[in] selection series to add in selection vector.
     * @param[in] deselection series to remove from selection vector.
     */
    void onSelectedSeries(QVector< ::fwMedData::Series::sptr > selection,
                          QVector< ::fwMedData::Series::sptr > deselection);

    /**
     * @brief Send a 'seriesDoubleClicked' signal when the user double click on a series. This signal holds the
     * clicked series.
     * @param[in] index index of the clicked item in the selector.
     * @todo  Manages double click on a study.
     */
    void onDoubleClick(const QModelIndex& index);

    /**
     * @brief Removes series from seriesDB and notify.
     * @param[in] selection series to remove from seriesDB.
     */
    void onRemoveSeries(QVector< ::fwMedData::Series::sptr > selection);

private:

    /**
     * @name Slots
     * @{
     */
    static const ::fwCom::Slots::SlotKeyType s_ADD_SERIES_SLOT;
    static const ::fwCom::Slots::SlotKeyType s_REMOVE_SERIES_SLOT;
    typedef ::fwCom::Slot<void (::fwMedData::SeriesDB::ContainerType)> RemoveSeriesSlotType;

    /// Slot: add series into the selector
    void addSeries(::fwMedData::SeriesDB::ContainerType addedSeries);
    /// Slot: remove series from the selector
    void removeSeries(::fwMedData::SeriesDB::ContainerType removedSeries);

    /// Slot used to remove series from the selector
    RemoveSeriesSlotType::sptr m_slotRemoveSeries;
    /**
     * @}
     */

    /// Returns current selection vector given by its fwID m_selectionId.
    ::fwData::Vector::sptr getSelection();

    /// fwID of the vector of selections
    std::string m_selectionId;

    /// Selector widget.
    QPointer< ::uiMedDataQt::widget::Selector > m_selectorWidget;

    /// Signal emitted when there is a double click on a series
    SeriesDoubleClickedSignalType::sptr m_sigSeriesDoubleClicked;

    /// Map containing the specified icons for a series (map\<series classname, icon path\>)
    ::uiMedDataQt::widget::Selector::SeriesIconType m_seriesIcons;

    /// Permits the user to remove series. Default is yes
    bool m_allowedRemove;

    /// Change the behaviour of the treeview selection mode
    QAbstractItemView::SelectionMode m_selectionMode;

    /// If true, allows selection of uiMedDataQt::InsertSeries only
    bool m_insertMode;

};
} // namespace editor
} // namespace uiMedDataQt
