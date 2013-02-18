/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIMEDDATAQT_WIDGET_SELECTORMODEL_HPP__
#define __UIMEDDATAQT_WIDGET_SELECTORMODEL_HPP__

#include <map>

#include <QString>
#include <QTreeView>
#include <QPointer>
#include <QStandardItemModel>
#include <QStandardItem>

#include <fwMedData/Series.hpp>

#include "uiMedDataQt/config.hpp"

namespace uiMedData
{
namespace widget
{
class UIMEDDATAQT_CLASS_API SelectorModel : public QStandardItemModel
{
    Q_OBJECT

public:

    typedef enum
    {
        ITEM_TYPE = Qt::UserRole,
        UID
    }Role;

    typedef enum
    {
        STUDY = 1,
        SERIES
    }ItemType;

    UIMEDDATAQT_API SelectorModel(QWidget *parent = 0);
    UIMEDDATAQT_API ~SelectorModel();

    UIMEDDATAQT_API void addSeries(::fwMedData::Series::sptr series);
    UIMEDDATAQT_API void removeSeries(::fwMedData::Series::sptr series);

    UIMEDDATAQT_API void clear();

    /// returns item flags with non editable flag
    UIMEDDATAQT_API Qt::ItemFlags flags(const QModelIndex& index) const
    {
        return (QStandardItemModel::flags(index) & ~Qt::ItemIsEditable);
    }

    UIMEDDATAQT_API ItemType getItemType(const QModelIndex &index);

private:

    template <typename T>
    QStandardItem* getInfo(T data, QString separator);

    typedef std::map< ::fwMedData::DicomValueType, QStandardItem* > StudyUidItemMapType;

    int m_studyRowCount;
    StudyUidItemMapType m_items;
};


template <typename T>
QStandardItem* SelectorModel::getInfo(T data, QString separator)
{
    QString dataStr;
    if(!data.empty())
    {
        typename T::iterator itr = data.begin();
        std::ostringstream str;
        str << *itr++;
        dataStr = QString::fromStdString(str.str());

        for(;itr!= data.end(); ++itr)
        {
            str.str("");
            str << *itr;
            dataStr.append(separator + QString::fromStdString(str.str()));
        }
    }
    QStandardItem *item = new QStandardItem(dataStr);
    return item;
}


} // namespace widget
} // namespace uiMedData

#endif // __UIMEDDATAQT_WIDGET_SELECTORMODEL_HPP__
