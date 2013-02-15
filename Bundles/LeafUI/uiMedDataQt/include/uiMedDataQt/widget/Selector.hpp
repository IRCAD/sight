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

#include <fwMedData/Series.hpp>

#include "uiMedDataQt/config.hpp"

namespace uiMedData
{
namespace widget
{
class UIMEDDATAQT_CLASS_API Selector : public QTreeView
{
    Q_OBJECT

public:
    UIMEDDATAQT_API Selector(QWidget *parent = 0);
    UIMEDDATAQT_API ~Selector();

    UIMEDDATAQT_API void addSeries(::fwMedData::Series::sptr series);

private:

    template <typename T>
    QStandardItem* getInfo(T data, QString separator);

    typedef std::map< ::fwMedData::DicomValueType, QStandardItem* > StudyUidItemMapType;

    int m_studyRowCount;
    QPointer<QStandardItemModel> m_model;
    StudyUidItemMapType m_items;
};


template <typename T>
QStandardItem* Selector::getInfo(T data, QString separator)
{
    QString dataStr;
    if(!data.empty())
    {
        T::iterator itr = data.begin();
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

#endif // __UIMEDDATAQT_WIDGET_SELECTOR_HPP__
