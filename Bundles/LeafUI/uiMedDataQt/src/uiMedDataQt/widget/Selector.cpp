/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QStandardItem>
#include <QString>
#include <QItemSelectionModel>
#include <QModelIndexList>
#include <QKeyEvent>

#include <fwData/Image.hpp>

#include <fwMedData/Patient.hpp>
#include <fwMedData/Study.hpp>
#include <fwMedData/Series.hpp>
#include <fwMedData/Equipment.hpp>
#include <fwMedData/ImageSeries.hpp>

#include "uiMedDataQt/widget/Selector.hpp"

namespace uiMedData
{
namespace widget
{

//-----------------------------------------------------------------------------

Selector::Selector(QWidget *parent) :
    QTreeView(parent), m_allowedRemove(true)
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

void Selector::setInsertMode(bool insert)
{
    m_model->setInsertMode(insert);
}

//-----------------------------------------------------------------------------

void Selector::addSeries(::fwMedData::Series::sptr series)
{
    m_model->addSeries(series);
    QStandardItem * studyItem = m_model->findStudyItem(series->getStudy());
    this->expand(m_model->indexFromItem(studyItem));

    for (int i = 0; i < m_model->columnCount(); ++i)
    {
        this->resizeColumnToContents(i);
    }
}

//-----------------------------------------------------------------------------

void Selector::removeSeries(::fwMedData::Series::sptr series)
{
    m_model->removeSeries(series);
}

//-----------------------------------------------------------------------------

void Selector::setAllowedRemove(bool allowed)
{
    m_allowedRemove = allowed;
}

//-----------------------------------------------------------------------------

void Selector::selectionChanged( const QItemSelection & selected, const QItemSelection & deselected )
{
    QTreeView::selectionChanged(selected, deselected);

    SeriesVectorType selectedSeries = this->getSeries(selected);

    SeriesVectorType deselectedSeries = this->getSeries(deselected);

    Q_EMIT selectSeries(selectedSeries, deselectedSeries);
}

//-----------------------------------------------------------------------------

Selector::SeriesVectorType Selector::getSeries( const QItemSelection & selection  )
{
    SeriesVectorType vSeries;

    QModelIndexList selectedIndexes = selection.indexes();
    vSeries = this->getSeries(selectedIndexes);
    return vSeries;
}

//-----------------------------------------------------------------------------

Selector::SeriesVectorType Selector::getSeries(const QModelIndexList& indexList)
{
    SeriesVectorType vSeries;
    for(QModelIndex index :  indexList)
    {
        std::string uid = index.data(SelectorModel::UID).toString().toStdString();
        ::fwTools::Object::sptr obj = ::fwTools::fwID::getObject(uid);

        if (index.data(SelectorModel::ITEM_TYPE)  == SelectorModel::SERIES)
        {
            ::fwMedData::Series::sptr series = ::fwMedData::Series::dynamicCast(obj);
            vSeries.push_back(series);
        }
    }
    return vSeries;
}

//-----------------------------------------------------------------------------

QModelIndexList Selector::getStudyIndexes(const QModelIndexList& indexList)
{
    QModelIndexList studiesIndex;
    for(QModelIndex index :  indexList)
    {
        if (index.data(SelectorModel::ITEM_TYPE)  == SelectorModel::STUDY)
        {
            studiesIndex.push_back(index);
        }
    }
    return studiesIndex;
}

//-----------------------------------------------------------------------------

Selector::SeriesVectorType Selector::getSeriesFromStudyIndex(const QModelIndex& index )
{
    SeriesVectorType vSeries;
    QStandardItem* item = m_model->itemFromIndex(index);
    int nbRow           = item->rowCount();
    for(int row = 0; row < nbRow; ++row)
    {
        QStandardItem *child = item->child(row);
        std::string uid      = child->data(SelectorModel::UID).toString().toStdString();
        SLM_ASSERT("UID must not be empty.", !uid.empty());
        ::fwTools::Object::sptr obj      = ::fwTools::fwID::getObject(uid);
        ::fwMedData::Series::sptr series = ::fwMedData::Series::dynamicCast(obj);
        vSeries.push_back(series);
    }
    return vSeries;
}

//-----------------------------------------------------------------------------

SelectorModel::ItemType Selector::getItemType(const QModelIndex &index)
{
    return m_model->getItemType(index);
}

//-----------------------------------------------------------------------------

void Selector::keyPressEvent(QKeyEvent * event)
{
    if(event->matches(QKeySequence::Delete) && m_allowedRemove)
    {
        this->deleteSelection();
        event->accept();
    }
    else
    {
        QTreeView::keyPressEvent(event);
    }
}

//-----------------------------------------------------------------------------

void Selector::deleteSelection()
{
    QModelIndexList selection = this->selectionModel()->selectedRows(0);

    SeriesVectorType vSeries     = this->getSeries(selection);
    QModelIndexList studyIndexes = this->getStudyIndexes(selection);
    for(QModelIndex index :  studyIndexes)
    {
        SeriesVectorType series = getSeriesFromStudyIndex(index);
        std::copy(series.begin(), series.end(), std::back_inserter(vSeries));
    }

    Q_EMIT removeSeries(vSeries);

    // Remove item in Selector.
    m_model->removeRows(selection);
}

//-----------------------------------------------------------------------------

void Selector::setSeriesIcons(const SeriesIconType &seriesIcons)
{
    m_model->setSeriesIcons(seriesIcons);
}

//-----------------------------------------------------------------------------

} // namespace widget
} // namespace uiMedData
