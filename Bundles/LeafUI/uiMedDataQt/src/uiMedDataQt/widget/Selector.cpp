/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QStandardItem>
#include <QString>

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
    QTreeView(parent)
{
    m_model = new SelectorModel();
    this->setModel(m_model);

    this->setSelectionMode(QAbstractItemView::ExtendedSelection);
    this->setAlternatingRowColors( true );
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

void Selector::addSeries(::fwMedData::Series::sptr series)
{
    m_model->addSeries(series);
}

//-----------------------------------------------------------------------------

void Selector::removeSeries(::fwMedData::Series::sptr series)
{

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
    if (!selectedIndexes.empty())
    {
        for (int i=0 ; i < selectedIndexes.count() ; ++i)
        {
            QModelIndex index = selectedIndexes[i];
            std::string uid = index.data(SelectorModel::UID).toString().toStdString();
            ::fwTools::Object::sptr obj = ::fwTools::fwID::getObject(uid);

            if (index.data(SelectorModel::ITEM_TYPE)  == SelectorModel::SERIES)
            {
                ::fwMedData::Series::sptr series = ::fwMedData::Series::dynamicCast(obj);
                vSeries.push_back(series);
            }
        }
    }

    return vSeries;
}

//-----------------------------------------------------------------------------

SelectorModel::ItemType Selector::getItemType(const QModelIndex &index)
{
    return m_model->getItemType(index);
}

//-----------------------------------------------------------------------------

} // namespace widget
} // namespace uiMedData
