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

#include "uiMedDataQt/config.hpp"
#include "uiMedDataQt/widget/SelectorModel.hpp"

namespace uiMedData
{
namespace widget
{
class UIMEDDATAQT_CLASS_API Selector : public QTreeView
{
    Q_OBJECT

public:

    typedef QVector< ::fwMedData::Series::sptr > SeriesVectorType;

    UIMEDDATAQT_API Selector(QWidget *parent = 0);
    UIMEDDATAQT_API ~Selector();

    UIMEDDATAQT_API void clear();

    UIMEDDATAQT_API void addSeries(::fwMedData::Series::sptr series);

    UIMEDDATAQT_API void removeSeries(::fwMedData::Series::sptr series);

    SelectorModel::ItemType getItemType(const QModelIndex &index);

Q_SIGNALS:
    void selectSeries(QVector< ::fwMedData::Series::sptr > selection, QVector< ::fwMedData::Series::sptr > deselection);


protected Q_SLOTS:
    void selectionChanged( const QItemSelection & selected, const QItemSelection & deselected );

protected :

    /**
     * @brief Returns the Series associated to the selection.
     * @todo If a study is selected : returns the first series associated to this study.
     */
    SeriesVectorType getSeries( const QItemSelection & selection );

private:

    QPointer<SelectorModel> m_model;
};

} // namespace widget
} // namespace uiMedData

#endif // __UIMEDDATAQT_WIDGET_SELECTOR_HPP__
