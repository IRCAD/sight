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

#include "Selector.hpp"

#include <data/image.hpp>
#include <data/image_series.hpp>
#include <data/series.hpp>

#include <QItemSelectionModel>
#include <QKeyEvent>
#include <QModelIndexList>
#include <QStandardItem>
#include <QString>

namespace sight::ui::qt::series
{

Selector::Selector(const std::string& displayColumn, QWidget* _parent) :
    QTreeView(_parent),
    m_model(new SelectorModel(displayColumn, this))
{
    this->setModel(m_model);
    this->setSelectionMode(QAbstractItemView::ExtendedSelection);
    this->setAlternatingRowColors(true);
    this->setDragEnabled(true);

    QObject::connect(m_model, &SelectorModel::removeStudyInstanceUID, this, &Selector::onRemoveStudyInstanceUID);
    QObject::connect(m_model, &SelectorModel::removeSeriesID, this, &Selector::onRemoveSeriesID);

    this->setDragEnabled(false);
    this->setAcceptDrops(false);
}

//-----------------------------------------------------------------------------

void Selector::addSeries(data::series::sptr _series)
{
    m_model->addSeries(_series);
    QStandardItem* studyItem = m_model->findStudyItem(_series);
    this->expand(m_model->indexFromItem(studyItem));

    for(int i = 0 ; i < m_model->columnCount() ; ++i)
    {
        this->resizeColumnToContents(i);
    }
}

//-----------------------------------------------------------------------------

void Selector::removeSeries(data::series::sptr _series)
{
    m_model->removeSeries(_series);
}

//-----------------------------------------------------------------------------

void Selector::allowRemove(bool _allowed)
{
    m_removeAllowed = _allowed;
    m_model->allowRemove(_allowed);
}

//-----------------------------------------------------------------------------

void Selector::selectionChanged(const QItemSelection& _selected, const QItemSelection& _deselected)
{
    QTreeView::selectionChanged(_selected, _deselected);

    SeriesVectorType selectedSeries = sight::ui::qt::series::Selector::getSeries(_selected);

    SeriesVectorType deselectedSeries = sight::ui::qt::series::Selector::getSeries(_deselected);

    Q_EMIT seriesSelected(selectedSeries, deselectedSeries);
}

//-----------------------------------------------------------------------------

Selector::SeriesVectorType Selector::getSeries(const QModelIndexList& _indexList)
{
    SeriesVectorType vSeries;
    for(QModelIndex index : _indexList)
    {
        std::string uid               = index.data(SelectorModel::UID).toString().toStdString();
        core::tools::object::sptr obj = core::tools::id::get_object(uid);

        if(index.data(SelectorModel::ITEM_TYPE) == SelectorModel::SERIES)
        {
            data::series::sptr series = std::dynamic_pointer_cast<data::series>(obj);
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
        if(index.data(SelectorModel::ITEM_TYPE) == SelectorModel::STUDY)
        {
            studiesIndex.push_back(index);
        }
    }

    return studiesIndex;
}

//-----------------------------------------------------------------------------

Selector::SeriesVectorType Selector::getSeriesFromStudyIndex(const QModelIndex& _index) const
{
    SeriesVectorType vSeries;
    QStandardItem* item = m_model->itemFromIndex(_index);
    SIGHT_ASSERT("Item shouldn't be null", item);
    const int nbRow = item->rowCount();
    for(int row = 0 ; row < nbRow ; ++row)
    {
        // Retrieve UID of the series using the DESCRIPTION column.
        QStandardItem* child = item->child(row, 0);
        SIGHT_ASSERT("Child is null", child);
        const std::string uid = child->data(SelectorModel::UID).toString().toStdString();
        SIGHT_ASSERT("UID must not be empty.", !uid.empty());
        core::tools::object::sptr obj = core::tools::id::get_object(uid);
        data::series::sptr series     = std::dynamic_pointer_cast<data::series>(obj);
        vSeries.push_back(series);
    }

    return vSeries;
}

//-----------------------------------------------------------------------------

SelectorModel::ItemType Selector::getItemType(const QModelIndex& _index) const
{
    return m_model->getItemType(_index);
}

//-----------------------------------------------------------------------------

void Selector::keyPressEvent(QKeyEvent* _event)
{
    if(_event->matches(QKeySequence::Delete) && m_removeAllowed)
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
    QModelIndexList selection = this->selectionModel()->selectedRows(0);

    SeriesVectorType vSeries     = this->getSeries(selection);
    QModelIndexList studyIndexes = this->getStudyIndexes(selection);
    for(QModelIndex index : studyIndexes)
    {
        SeriesVectorType series = this->getSeriesFromStudyIndex(index);
        std::copy(series.begin(), series.end(), std::back_inserter(vSeries));
    }

    Q_EMIT removeSeries(vSeries);

    // Remove item in Selector.
    m_model->removeRows(selection);
}

//-----------------------------------------------------------------------------

void Selector::onRemoveStudyInstanceUID(const std::string& _uid)
{
    if(m_removeAllowed)
    {
        SeriesVectorType series_to_remove;
        QModelIndexList selection;

        for(int studyIdx = 0 ; studyIdx < m_model->rowCount() ; ++studyIdx)
        {
            const QStandardItem* const studyItem = m_model->item(studyIdx);
            if(studyItem->index().data(SelectorModel::UID) == QString::fromStdString(_uid))
            {
                selection.push_back(studyItem->index());
                for(int seriesIdx = 0 ; seriesIdx < studyItem->rowCount() ; ++seriesIdx)
                {
                    const QStandardItem* const seriesItem = studyItem->child(seriesIdx, 0);
                    selection.push_back(seriesItem->index());

                    const std::string seriesUID = seriesItem->index().data(SelectorModel::UID).toString().toStdString();
                    auto series                 =
                        std::dynamic_pointer_cast<data::series>(core::tools::id::get_object(seriesUID));

                    if(series)
                    {
                        series_to_remove.push_back(series);
                    }
                }

                break;
            }
        }

        Q_EMIT removeSeries(series_to_remove);

        // Remove item in Selector.
        m_model->removeRows(selection);
    }
}

//-----------------------------------------------------------------------------

void Selector::onRemoveSeriesID(const std::string& _id)
{
    if(m_removeAllowed)
    {
        SeriesVectorType series_to_remove;
        QModelIndexList selection;

        for(int studyIdx = 0 ; studyIdx < m_model->rowCount() ; ++studyIdx)
        {
            const QStandardItem* const studyItem = m_model->item(studyIdx);
            for(int seriesIdx = 0 ; seriesIdx < studyItem->rowCount() ; ++seriesIdx)
            {
                const QStandardItem* const seriesItem = studyItem->child(seriesIdx, 0);
                const std::string seriesUID           =
                    seriesItem->index().data(SelectorModel::UID).toString().toStdString();

                if(seriesUID == _id)
                {
                    selection.push_back(seriesItem->index());
                    auto series = std::dynamic_pointer_cast<data::series>(core::tools::id::get_object(seriesUID));

                    if(series)
                    {
                        series_to_remove.push_back(series);
                    }

                    break;
                }
            }
        }

        Q_EMIT removeSeries(series_to_remove);

        // Remove item in Selector.
        m_model->removeRows(selection);
    }
}

//-----------------------------------------------------------------------------

void Selector::setSeriesIcons(const SeriesIconType& _seriesIcons)
{
    m_model->setSeriesIcons(_seriesIcons);
}

//------------------------------------------------------------------------------

QSize Selector::sizeHint() const
{
    static constexpr auto GOLD_NUMBER = 1.618;

    // Take the size of the scrollbar into account.
    QSize preferred_size(style()->pixelMetric(QStyle::PixelMetric::PM_ScrollBarExtent), 0);

    // Take the size of the columns into account
    for(int i = 0, end = std::min(4, m_model->columnCount()) ; i < end ; ++i)
    {
        preferred_size.setWidth(preferred_size.width() + sizeHintForColumn(i));
    }

    preferred_size.setHeight(int(preferred_size.width() / GOLD_NUMBER));

    return QTreeView::sizeHint().expandedTo(preferred_size);
}

} // namespace sight::ui::qt::series
