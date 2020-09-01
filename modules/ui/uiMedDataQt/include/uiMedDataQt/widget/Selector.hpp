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
#include "uiMedDataQt/widget/SelectorModel.hpp"

#include <fwMedData/Series.hpp>
#include <fwMedData/Study.hpp>

#include <QModelIndex>
#include <QPointer>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QString>
#include <QTreeView>
#include <QVector>

#include <map>

namespace uiMedDataQt
{
namespace widget
{

/**
 * @brief This selector represents the Series in a hierarchical view (Study/Patient->Series).
 */
class UIMEDDATAQT_CLASS_API Selector : public QTreeView
{

Q_OBJECT

public:

    /// Defines a map associating icons to series (map\<series classname, icon path\>)
    typedef SelectorModel::SeriesIconType SeriesIconType;

    /// Initializes the tree view.
    UIMEDDATAQT_API Selector(QWidget* _parent = nullptr);

    /// Destroys the selector.
    UIMEDDATAQT_API ~Selector();

    /// Clears all items in the tree.
    UIMEDDATAQT_API void clear();

    /**
     * @brief Adds the Series in the tree. If the associated study already exists in the tree, the series is added to
     * this study.
     * @param _series series to add in the tree.
     */
    UIMEDDATAQT_API void addSeries(::fwMedData::Series::sptr _series);

    /**
     * @brief Removes the Series from the tree. After deletion, if the study is empty, it will be removed.
     * @param _series series to remove from the tree.
     */
    UIMEDDATAQT_API void removeSeries(::fwMedData::Series::sptr _series);

    /// Returns the type of the item (SERIES or STUDY)
    UIMEDDATAQT_API SelectorModel::ItemType getItemType(const QModelIndex& _index);

    /// Catchs the delete key event and remove the selected items.
    UIMEDDATAQT_API void keyPressEvent(QKeyEvent* _event);

    /**
     * @brief Sets the specific icons for series in selector.
     * @param _seriesIcons map\<series classname, icon path\>
     */
    UIMEDDATAQT_API void setSeriesIcons(const SeriesIconType& _seriesIcons);

    /// Allows removing items or not.
    UIMEDDATAQT_API void setAllowedRemove(bool _allowed);

    /// Sets if the selector must be in insert mode.
    UIMEDDATAQT_API void setInsertMode(bool _insert);

Q_SIGNALS:
    /**
     * @brief Signal emitted when the selection change.
     * @param _selection contains the new selection of series.
     * @param _deselection contains the new deselection of series.
     * @note selection and deselection contain only the change of selection. The series always selected or deselected
     * don't appear in this selection/deselection.
     */
    void selectSeries(QVector< ::fwMedData::Series::sptr > _selection,
                      QVector< ::fwMedData::Series::sptr > _deselection);

    /**
     * @brief Signal emitted when series are deleted.
     * @param _selection contains the deleted series.
     */
    void removeSeries(QVector< ::fwMedData::Series::sptr > _selection);

protected Q_SLOTS:
    /**
     * @brief Slot called when the selection changed. Emits a signal containing the new selected/deselected series. If a
     * Study is selected, no series are added in signal.
     * @param _selected contains the new selected items.
     * @param _deselected contains the new deselected items.
     * @note selection and deselection contain only the change of selection. The items always selected or deselected
     * don't appear in this selection/deselection.
     */
    void selectionChanged(const QItemSelection& _selected, const QItemSelection& _deselected);

protected:

    typedef QVector< ::fwMedData::Series::sptr > SeriesVectorType;

    /**
     * @brief Returns all the Series associated to the selection.
     * @note If a study is selected, return an empty selection.
     */
    SeriesVectorType getSeries(const QItemSelection& _selection);

    /**
     * @brief Returns all the Series associated to the selection.
     * @note It ignores the studies.
     */
    SeriesVectorType getSeries(const QModelIndexList& _indexList);

    /// Returns all the study indexes contained in the given indexList
    QModelIndexList getStudyIndexes(const QModelIndexList& _indexList);

    /// Returns all the series associated with the study index
    SeriesVectorType getSeriesFromStudyIndex(const QModelIndex& _index);

    /// Deletes the selected items and notify the deleted series.
    void deleteSelection();

private:

    /// Tree model
    QPointer<SelectorModel> m_model { nullptr };

    /// Allows to remove items.
    bool m_allowedRemove { true };

};

} // namespace widget.
} // namespace uiMedDataQt.
