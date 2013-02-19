/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIMEDDATAQT_WIDGET_SELECTOR_HPP__
#define __UIMEDDATAQT_WIDGET_SELECTOR_HPP__

#include <map>

#include <QString>
#include <QTreeView>
#include <QPointer>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QModelIndex>
#include <QVector>

#include <fwMedData/Series.hpp>
#include <fwMedData/Study.hpp>

#include "uiMedDataQt/config.hpp"
#include "uiMedDataQt/widget/SelectorModel.hpp"

namespace uiMedData
{
namespace widget
{

/**
 * @brief   This selector represents the Series in a hierarchical view (Study/Patient->Series).
 * @class   SSelector.
 * @date    2013.
 */
class UIMEDDATAQT_CLASS_API Selector : public QTreeView
{
    Q_OBJECT

public:

    typedef QVector< ::fwMedData::Series::sptr > SeriesVectorType;
    typedef QVector< ::fwMedData::Study::sptr > StudiesVectorType;

    /// Constructor. Init tree view.
    UIMEDDATAQT_API Selector(QWidget *parent = 0);

    /// Destrucotr
    UIMEDDATAQT_API ~Selector();

    /// Clear all items in the tree.
    UIMEDDATAQT_API void clear();

    /**
     * @brief Add the Series in the tree. If the associated study already exist in the tree, the series is added to
     * this study.
     */
    UIMEDDATAQT_API void addSeries(::fwMedData::Series::sptr series);

    UIMEDDATAQT_API void removeSeries(::fwMedData::Series::sptr series);

    /// Returns the type of the item (SERIES or STUDY)
    UIMEDDATAQT_API SelectorModel::ItemType getItemType(const QModelIndex &index);

    UIMEDDATAQT_API void keyPressEvent(QKeyEvent * event);

Q_SIGNALS:
    /**
     * @brief Signal emitted when the selection change.
     * @param selection contains the new selection of series.
     * @param deselection contains the new deselection of series.
     * @note selection and deselection contain only the change of selection. The series always selected or deselected
     * don't appear in this selection/deselection.
     */
    void selectSeries(QVector< ::fwMedData::Series::sptr > selection, QVector< ::fwMedData::Series::sptr > deselection);

    void removeSeries(QVector< ::fwMedData::Series::sptr > selection);


protected Q_SLOTS:
    /**
     * @brief Slot called when the selection changed. Emits a signal containing the new selected/deselected series. If a
     * Study is selected, no series are added in signal.
     * @param selection contains the new selection.
     * @param deselection contains the new deselection.
     * @note selection and deselection contain only the change of selection. The items always selected or deselected
     * don't appear in this selection/deselection.
     */
    void selectionChanged( const QItemSelection & selected, const QItemSelection & deselected );
    void onDelete();

protected :

    /**
     * @brief Returns the Series associated to the selection.
     * @note If a study is selected, return an empty selection.
     */
    SeriesVectorType getSeries( const QItemSelection & selection );
    SeriesVectorType getSeries(const QModelIndexList& indexList);

    QModelIndexList getStudyIndexes(const QModelIndexList& indexList);
    SeriesVectorType getSeriesFromStudyIndex(const QModelIndex& index);

private:

    /// Tree model
    QPointer<SelectorModel> m_model;
};

} // namespace widget
} // namespace uiMedData

#endif // __UIMEDDATAQT_WIDGET_SELECTOR_HPP__
