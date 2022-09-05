/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include <data/Equipment.hpp>
#include <data/Image.hpp>
#include <data/ImageSeries.hpp>
#include <data/Patient.hpp>
#include <data/Series.hpp>
#include <data/Study.hpp>

#include <QItemSelectionModel>
#include <QKeyEvent>
#include <QModelIndexList>
#include <QStandardItem>
#include <QString>

namespace sight::module::ui::qt::series
{

//-----------------------------------------------------------------------------

Selector::Selector(QWidget* _parent) :
    QTreeView(_parent),
    m_model(new SelectorModel(this))
{
    this->setModel(m_model);

    this->setSelectionMode(QAbstractItemView::ExtendedSelection);
    this->setAlternatingRowColors(true);
    this->setDragEnabled(true);

    QObject::connect(m_model, &SelectorModel::removeStudyInstanceUID, this, &Selector::onRemoveStudyInstanceUID);
    QObject::connect(m_model, &SelectorModel::removeSerieID, this, &Selector::onRemoveSerieID);

    this->setDragEnabled(false);
    this->setAcceptDrops(false);
}

//-----------------------------------------------------------------------------

Selector::~Selector()
= default;

//-----------------------------------------------------------------------------

void Selector::addSeries(data::Series::sptr _series)
{
    m_model->addSeries(_series);
    QStandardItem* studyItem = m_model->findStudyItem(_series->getStudy());
    this->expand(m_model->indexFromItem(studyItem));

    for(int i = 0 ; i < m_model->columnCount() ; ++i)
    {
        this->resizeColumnToContents(i);
    }
}

//-----------------------------------------------------------------------------

void Selector::removeSeries(data::Series::sptr _series)
{
    m_model->removeSeries(_series);
}

//-----------------------------------------------------------------------------

void Selector::setAllowedRemove(bool _allowed)
{
    m_allowedRemove = _allowed;
    m_model->setAllowedRemove(_allowed);
}

//-----------------------------------------------------------------------------

void Selector::selectionChanged(const QItemSelection& _selected, const QItemSelection& _deselected)
{
    QTreeView::selectionChanged(_selected, _deselected);

    SeriesVectorType selectedSeries = sight::module::ui::qt::series::Selector::getSeries(_selected);

    SeriesVectorType deselectedSeries = sight::module::ui::qt::series::Selector::getSeries(_deselected);

    Q_EMIT selectSeries(selectedSeries, deselectedSeries);
}

//-----------------------------------------------------------------------------

Selector::SeriesVectorType Selector::getSeries(const QItemSelection& _selection)
{
    SeriesVectorType vSeries;

    QModelIndexList selectedIndexes = _selection.indexes();
    vSeries = sight::module::ui::qt::series::Selector::getSeries(selectedIndexes);
    return vSeries;
}

//-----------------------------------------------------------------------------

Selector::SeriesVectorType Selector::getSeries(const QModelIndexList& _indexList)
{
    SeriesVectorType vSeries;
    for(QModelIndex index : _indexList)
    {
        std::string uid               = index.data(SelectorModel::UID).toString().toStdString();
        core::tools::Object::sptr obj = core::tools::fwID::getObject(uid);

        if(index.data(SelectorModel::ITEM_TYPE) == SelectorModel::SERIES)
        {
            data::Series::sptr series = data::Series::dynamicCast(obj);
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

Selector::SeriesVectorType Selector::getSeriesFromStudyIndex(const QModelIndex& _index)
{
    SeriesVectorType vSeries;
    QStandardItem* item = m_model->itemFromIndex(_index);
    SIGHT_ASSERT("Item shouldn't be null", item);
    const int nbRow = item->rowCount();
    for(int row = 0 ; row < nbRow ; ++row)
    {
        // Retrieve UID of the series using the DESCRIPTION column.
        QStandardItem* child = item->child(row, static_cast<int>(SelectorModel::ColumnSeriesType::NAME));
        SIGHT_ASSERT("Child is null", child);
        const std::string uid = child->data(SelectorModel::UID).toString().toStdString();
        SIGHT_ASSERT("UID must not be empty.", !uid.empty());
        core::tools::Object::sptr obj = core::tools::fwID::getObject(uid);
        data::Series::sptr series     = data::Series::dynamicCast(obj);
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
    if(m_allowedRemove)
    {
        SeriesVectorType series;
        QModelIndexList selection;

        for(int studyIdx = 0 ; studyIdx < m_model->rowCount() ; ++studyIdx)
        {
            const QStandardItem* const studyItem = m_model->item(studyIdx);
            if(studyItem->index().data(SelectorModel::UID) == QString::fromStdString(_uid))
            {
                selection.push_back(studyItem->index());
                for(int serieIdx = 0 ; serieIdx < studyItem->rowCount() ; ++serieIdx)
                {
                    const QStandardItem* const serieItem =
                        studyItem->child(serieIdx, int(SelectorModel::ColumnSeriesType::NAME));
                    selection.push_back(serieItem->index());
                    const std::string serieUID =
                        serieItem->index().data(SelectorModel::UID).toString().toStdString();
                    core::tools::Object::sptr obj = core::tools::fwID::getObject(serieUID);
                    data::Series::sptr serie      = data::Series::dynamicCast(obj);

                    series.push_back(serie);
                }

                break;
            }
        }

        Q_EMIT removeSeries(series);

        // Remove item in Selector.
        m_model->removeRows(selection);
    }
}

//-----------------------------------------------------------------------------

void Selector::onRemoveSerieID(const std::string& _id)
{
    if(m_allowedRemove)
    {
        SeriesVectorType series;
        QModelIndexList selection;

        for(int studyIdx = 0 ; studyIdx < m_model->rowCount() ; ++studyIdx)
        {
            const QStandardItem* const studyItem = m_model->item(studyIdx);
            for(int serieIdx = 0 ; serieIdx < studyItem->rowCount() ; ++serieIdx)
            {
                const QStandardItem* const serieItem =
                    studyItem->child(serieIdx, int(SelectorModel::ColumnSeriesType::NAME));
                const std::string serieUID = serieItem->index().data(SelectorModel::UID).toString().toStdString();

                if(serieUID == _id)
                {
                    selection.push_back(serieItem->index());
                    core::tools::Object::sptr obj = core::tools::fwID::getObject(serieUID);
                    data::Series::sptr serie      = data::Series::dynamicCast(obj);

                    series.push_back(serie);
                    break;
                }
            }
        }

        Q_EMIT removeSeries(series);

        // Remove item in Selector.
        m_model->removeRows(selection);
    }
}

//-----------------------------------------------------------------------------

void Selector::setSeriesIcons(const SeriesIconType& _seriesIcons)
{
    m_model->setSeriesIcons(_seriesIcons);
}

//-----------------------------------------------------------------------------

} // namespace sight::module::ui::qt::series
