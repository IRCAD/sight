/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#ifndef __UIMEDDATAQT_WIDGET_SELECTOR_HPP__
#define __UIMEDDATAQT_WIDGET_SELECTOR_HPP__

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
 * @brief   This selector represents the Series in a hierarchical view (Study/Patient->Series).
 */
class UIMEDDATAQT_CLASS_API Selector : public QTreeView
{
Q_OBJECT

public:

    typedef QVector< ::fwMedData::Series::sptr > SeriesVectorType;

    /// Map associating icons to series (map\<series classname, icon path\>)
    typedef SelectorModel::SeriesIconType SeriesIconType;

    /// Constructor. Init tree view.
    UIMEDDATAQT_API Selector(QWidget* parent = 0);

    /// Destrucotr
    UIMEDDATAQT_API ~Selector();

    /// Clear all items in the tree.
    UIMEDDATAQT_API void clear();

    /**
     * @brief Adds the Series in the tree. If the associated study already exists in the tree, the series is added to
     * this study.
     * @param[in] series series to add in the tree.
     */
    UIMEDDATAQT_API void addSeries(::fwMedData::Series::sptr series);

    /**
     * @brief Removes the Series from the tree. After deletion, if the study is empty, it will be removed.
     * @param[in] series series to remove from the tree.
     */
    UIMEDDATAQT_API void removeSeries(::fwMedData::Series::sptr series);

    /// Returns the type of the item (SERIES or STUDY)
    UIMEDDATAQT_API SelectorModel::ItemType getItemType(const QModelIndex& index);

    /// Catch the delete key event and remove the selected items.
    UIMEDDATAQT_API void keyPressEvent(QKeyEvent* event);

    /**
     * @brief Sets the specific icons for series in selector.
     * @param[in]  seriesIcons map\<series classname, icon path\>
     */
    UIMEDDATAQT_API void setSeriesIcons(const SeriesIconType& seriesIcons);

    /// Allows removing items or not.
    UIMEDDATAQT_API void setAllowedRemove(bool allowed);

    /// Sets if the selector must be in insert mode.
    UIMEDDATAQT_API void setInsertMode(bool insert);

Q_SIGNALS:
    /**
     * @brief Signal emitted when the selection change.
     * @param selection contains the new selection of series.
     * @param deselection contains the new deselection of series.
     * @note selection and deselection contain only the change of selection. The series always selected or deselected
     * don't appear in this selection/deselection.
     */
    void selectSeries(QVector< ::fwMedData::Series::sptr > selection, QVector< ::fwMedData::Series::sptr > deselection);

    /**
     * @brief Signal emitted when series are deleted.
     * @param selection contains the deleted series.
     */
    void removeSeries(QVector< ::fwMedData::Series::sptr > selection);

protected Q_SLOTS:
    /**
     * @brief Slot called when the selection changed. Emits a signal containing the new selected/deselected series. If a
     * Study is selected, no series are added in signal.
     * @param selected contains the new selected items.
     * @param deselected contains the new deselected items.
     * @note selection and deselection contain only the change of selection. The items always selected or deselected
     * don't appear in this selection/deselection.
     */
    void selectionChanged( const QItemSelection& selected, const QItemSelection& deselected );

protected:

    /**
     * @brief Returns all the Series associated to the selection.
     * @note If a study is selected, return an empty selection.
     */
    SeriesVectorType getSeries( const QItemSelection& selection );

    /**
     * @brief Returns all the Series associated to the selection.
     * @note It ignores the studies.
     */
    SeriesVectorType getSeries(const QModelIndexList& indexList);

    /// Returns all the study indexes contained in the given indexList
    QModelIndexList getStudyIndexes(const QModelIndexList& indexList);

    /// Returns all the series associated with the study index
    SeriesVectorType getSeriesFromStudyIndex(const QModelIndex& index);

    /// Deletes the selected items and notify the deleted series.
    void deleteSelection();

private:

    /// Tree model
    QPointer<SelectorModel> m_model;

    /// Allows to remove items.
    bool m_allowedRemove;

};

} // namespace widget
} // namespace uiMedDataQt

#endif // __UIMEDDATAQT_WIDGET_SELECTOR_HPP__
