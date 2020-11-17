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

#include "uiMedDataQt/widget/Selector.hpp"

#include <fwData/Image.hpp>

#include <fwMedData/Equipment.hpp>
#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/Patient.hpp>
#include <fwMedData/Series.hpp>
#include <fwMedData/Study.hpp>

#include <QItemSelectionModel>
#include <QKeyEvent>
#include <QModelIndexList>
#include <QStandardItem>
#include <QString>

namespace uiMedDataQt
{
namespace widget
{

//-----------------------------------------------------------------------------

Selector::Selector(QWidget* _parent) :
    QTreeView(_parent)
{
    m_model = new SelectorModel();
    this->setModel(m_model);

    this->setSelectionMode(QAbstractItemView::ExtendedSelection);
    this->setAlternatingRowColors( true );
    this->setDragEnabled(true);
}

//-----------------------------------------------------------------------------

Selector::~Selector()
{
}

//-----------------------------------------------------------------------------

void Selector::clear()
{
    m_model->clear();
}

//-----------------------------------------------------------------------------

void Selector::setInsertMode(bool _insert)
{
    m_model->setInsertMode(_insert);
}

//-----------------------------------------------------------------------------

void Selector::addSeries(::fwMedData::Series::sptr _series)
{
    m_model->addSeries(_series);
    QStandardItem* studyItem = m_model->findStudyItem(_series->getStudy());
    this->expand(m_model->indexFromItem(studyItem));

    for (int i = 0; i < m_model->columnCount(); ++i)
    {
        this->resizeColumnToContents(i);
    }
}

//-----------------------------------------------------------------------------

void Selector::removeSeries(::fwMedData::Series::sptr _series)
{
    m_model->removeSeries(_series);
}

//-----------------------------------------------------------------------------

void Selector::setAllowedRemove(bool _allowed)
{
    m_allowedRemove = _allowed;
}

//-----------------------------------------------------------------------------

void Selector::selectionChanged(const QItemSelection& _selected, const QItemSelection& _deselected)
{
    QTreeView::selectionChanged(_selected, _deselected);

    SeriesVectorType selectedSeries = this->getSeries(_selected);

    SeriesVectorType deselectedSeries = this->getSeries(_deselected);

    Q_EMIT selectSeries(selectedSeries, deselectedSeries);
}

//-----------------------------------------------------------------------------

Selector::SeriesVectorType Selector::getSeries(const QItemSelection& _selection)
{
    SeriesVectorType vSeries;

    QModelIndexList selectedIndexes = _selection.indexes();
    vSeries = this->getSeries(selectedIndexes);
    return vSeries;
}

//-----------------------------------------------------------------------------

Selector::SeriesVectorType Selector::getSeries(const QModelIndexList& _indexList)
{
    SeriesVectorType vSeries;
    for(QModelIndex index : _indexList)
    {
        std::string uid = index.data(SelectorModel::UID).toString().toStdString();
        ::fwTools::Object::sptr obj = ::fwTools::fwID::getObject(uid);

        if (index.data(SelectorModel::ITEM_TYPE) == SelectorModel::SERIES)
        {
            ::fwMedData::Series::sptr series = ::fwMedData::Series::dynamicCast(obj);
            vSeries.push_back(series);
        }
    }
    return vSeries;
}

//-----------------------------------------------------------------------------

QModelIndexList Selector::getStudyIndexes(const QModelIndexList& _indexList)
{
    QModelIndexList studiesIndex;
    for(QModelIndex index : _indexList)
    {
        if (index.data(SelectorModel::ITEM_TYPE) == SelectorModel::STUDY)
        {
            studiesIndex.push_back(index);
        }
    }
    return studiesIndex;
}

//-----------------------------------------------------------------------------

Selector::SeriesVectorType Selector::getSeriesFromStudyIndex(const QModelIndex& _index)
{
    SeriesVectorType vSeries;
    QStandardItem* item = m_model->itemFromIndex(_index);
    SLM_ASSERT("Item shouldn't be null", item);
    const int nbRow = item->rowCount();
    for(int row = 0; row < nbRow; ++row)
    {
        // Retrieve UID of the series using the DESCRIPTION column.
        QStandardItem* child = item->child(row, static_cast<int>(SelectorModel::ColumnSeriesType::NAME));
        SLM_ASSERT("Child is null", child);
        const std::string uid = child->data(SelectorModel::UID).toString().toStdString();
        SLM_ASSERT("UID must not be empty.", !uid.empty());
        ::fwTools::Object::sptr obj      = ::fwTools::fwID::getObject(uid);
        ::fwMedData::Series::sptr series = ::fwMedData::Series::dynamicCast(obj);
        vSeries.push_back(series);
    }
    return vSeries;
}

//-----------------------------------------------------------------------------

SelectorModel::ItemType Selector::getItemType(const QModelIndex& _index)
{
    return m_model->getItemType(_index);
}

//-----------------------------------------------------------------------------

void Selector::keyPressEvent(QKeyEvent* _event)
{
    if(_event->matches(QKeySequence::Delete) && m_allowedRemove)
    {
        this->deleteSelection();
        _event->accept();
    }
    else
    {
        QTreeView::keyPressEvent(_event);
    }
}

//-----------------------------------------------------------------------------

void Selector::deleteSelection()
{
    QModelIndexList selection =
        this->selectionModel()->selectedRows(static_cast<int>(SelectorModel::ColumnSeriesType::NAME));

    SeriesVectorType vSeries     = this->getSeries(selection);
    QModelIndexList studyIndexes = this->getStudyIndexes(selection);
    for(QModelIndex index :  studyIndexes)
    {
        SeriesVectorType series = this->getSeriesFromStudyIndex(index);
        std::copy(series.begin(), series.end(), std::back_inserter(vSeries));
    }

    Q_EMIT removeSeries(vSeries);

    // Remove item in Selector.
    m_model->removeRows(selection);
}

//-----------------------------------------------------------------------------

void Selector::setSeriesIcons(const SeriesIconType& _seriesIcons)
{
    m_model->setSeriesIcons(_seriesIcons);
}

//-----------------------------------------------------------------------------

} // namespace widget.
} // namespace uiMedDataQt.
