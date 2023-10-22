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

#include "selector.hpp"

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

selector::selector(const std::string& _display_column, QWidget* _parent) :
    QTreeView(_parent),
    m_model(new selector_model(_display_column, this))
{
    this->setModel(m_model);
    this->setSelectionMode(QAbstractItemView::ExtendedSelection);
    this->setAlternatingRowColors(true);
    this->setDragEnabled(true);

    QObject::connect(m_model, &selector_model::removeStudyInstanceUID, this, &selector::onRemoveStudyInstanceUID);
    QObject::connect(m_model, &selector_model::removeSeriesID, this, &selector::onRemoveSeriesID);

    this->setDragEnabled(false);
    this->setAcceptDrops(false);
}

//-----------------------------------------------------------------------------

void selector::addSeries(data::series::sptr _series)
{
    m_model->addSeries(_series);
    QStandardItem* study_item = m_model->findStudyItem(_series);
    this->expand(m_model->indexFromItem(study_item));

    for(int i = 0 ; i < m_model->columnCount() ; ++i)
    {
        this->resizeColumnToContents(i);
    }
}

//-----------------------------------------------------------------------------

void selector::removeSeries(data::series::sptr _series)
{
    m_model->removeSeries(_series);
}

//-----------------------------------------------------------------------------

void selector::allowRemove(bool _allowed)
{
    m_removeAllowed = _allowed;
    m_model->allowRemove(_allowed);
}

//-----------------------------------------------------------------------------

void selector::selectionChanged(const QItemSelection& _selected, const QItemSelection& _deselected)
{
    QTreeView::selectionChanged(_selected, _deselected);

    series_vector_t selected_series = sight::ui::qt::series::selector::getSeries(_selected);

    series_vector_t deselected_series = sight::ui::qt::series::selector::getSeries(_deselected);

    Q_EMIT seriesSelected(selected_series, deselected_series);
}

//-----------------------------------------------------------------------------

selector::series_vector_t selector::getSeries(const QModelIndexList& _index_list)
{
    series_vector_t v_series;
    for(QModelIndex index : _index_list)
    {
        std::string uid               = index.data(selector_model::UID).toString().toStdString();
        core::tools::object::sptr obj = core::tools::id::get_object(uid);

        if(index.data(selector_model::ITEM_TYPE) == selector_model::SERIES)
        {
            data::series::sptr series = std::dynamic_pointer_cast<data::series>(obj);
            v_series.push_back(series);
        }
    }

    return v_series;
}

//-----------------------------------------------------------------------------

QModelIndexList selector::getStudyIndexes(const QModelIndexList& _index_list)
{
    QModelIndexList studies_index;
    for(QModelIndex index : _index_list)
    {
        if(index.data(selector_model::ITEM_TYPE) == selector_model::STUDY)
        {
            studies_index.push_back(index);
        }
    }

    return studies_index;
}

//-----------------------------------------------------------------------------

selector::series_vector_t selector::getSeriesFromStudyIndex(const QModelIndex& _index) const
{
    series_vector_t v_series;
    QStandardItem* item = m_model->itemFromIndex(_index);
    SIGHT_ASSERT("Item shouldn't be null", item);
    const int nb_row = item->rowCount();
    for(int row = 0 ; row < nb_row ; ++row)
    {
        // Retrieve UID of the series using the DESCRIPTION column.
        QStandardItem* child = item->child(row, 0);
        SIGHT_ASSERT("Child is null", child);
        const std::string uid = child->data(selector_model::UID).toString().toStdString();
        SIGHT_ASSERT("UID must not be empty.", !uid.empty());
        core::tools::object::sptr obj = core::tools::id::get_object(uid);
        data::series::sptr series     = std::dynamic_pointer_cast<data::series>(obj);
        v_series.push_back(series);
    }

    return v_series;
}

//-----------------------------------------------------------------------------

selector_model::item_t selector::get_item_type(const QModelIndex& _index) const
{
    return m_model->get_item_type(_index);
}

//-----------------------------------------------------------------------------

void selector::keyPressEvent(QKeyEvent* _event)
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

void selector::deleteSelection()
{
    QModelIndexList selection = this->selectionModel()->selectedRows(0);

    series_vector_t v_series      = this->getSeries(selection);
    QModelIndexList study_indexes = this->getStudyIndexes(selection);
    for(QModelIndex index : study_indexes)
    {
        series_vector_t series = this->getSeriesFromStudyIndex(index);
        std::copy(series.begin(), series.end(), std::back_inserter(v_series));
    }

    Q_EMIT removeSeries(v_series);

    // Remove item in selector.
    m_model->removeRows(selection);
}

//-----------------------------------------------------------------------------

void selector::onRemoveStudyInstanceUID(const std::string& _uid)
{
    if(m_removeAllowed)
    {
        series_vector_t series_to_remove;
        QModelIndexList selection;

        for(int study_idx = 0 ; study_idx < m_model->rowCount() ; ++study_idx)
        {
            const QStandardItem* const study_item = m_model->item(study_idx);
            if(study_item->index().data(selector_model::UID) == QString::fromStdString(_uid))
            {
                selection.push_back(study_item->index());
                for(int series_idx = 0 ; series_idx < study_item->rowCount() ; ++series_idx)
                {
                    const QStandardItem* const series_item = study_item->child(series_idx, 0);
                    selection.push_back(series_item->index());

                    const std::string series_uid =
                        series_item->index().data(selector_model::UID).toString().toStdString();
                    auto series =
                        std::dynamic_pointer_cast<data::series>(core::tools::id::get_object(series_uid));

                    if(series)
                    {
                        series_to_remove.push_back(series);
                    }
                }

                break;
            }
        }

        Q_EMIT removeSeries(series_to_remove);

        // Remove item in selector.
        m_model->removeRows(selection);
    }
}

//-----------------------------------------------------------------------------

void selector::onRemoveSeriesID(const std::string& _id)
{
    if(m_removeAllowed)
    {
        series_vector_t series_to_remove;
        QModelIndexList selection;

        for(int study_idx = 0 ; study_idx < m_model->rowCount() ; ++study_idx)
        {
            const QStandardItem* const study_item = m_model->item(study_idx);
            for(int series_idx = 0 ; series_idx < study_item->rowCount() ; ++series_idx)
            {
                const QStandardItem* const series_item = study_item->child(series_idx, 0);
                const std::string series_uid           =
                    series_item->index().data(selector_model::UID).toString().toStdString();

                if(series_uid == _id)
                {
                    selection.push_back(series_item->index());
                    auto series = std::dynamic_pointer_cast<data::series>(core::tools::id::get_object(series_uid));

                    if(series)
                    {
                        series_to_remove.push_back(series);
                    }

                    break;
                }
            }
        }

        Q_EMIT removeSeries(series_to_remove);

        // Remove item in selector.
        m_model->removeRows(selection);
    }
}

//-----------------------------------------------------------------------------

void selector::setSeriesIcons(const series_icon_t& _series_icons)
{
    m_model->setSeriesIcons(_series_icons);
}

//------------------------------------------------------------------------------

QSize selector::sizeHint() const
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
