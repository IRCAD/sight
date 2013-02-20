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

/**
 * @brief   This class represents the Selector Model.
 * @class   SSelector.
 * @date    2013.
 */
class UIMEDDATAQT_CLASS_API SelectorModel : public QStandardItemModel
{
    Q_OBJECT

public:

    typedef enum
    {
        ITEM_TYPE = Qt::UserRole, /// Role for the item type (STUDY or SERIES)
        UID                       /// Role for the fwID od the object
    }Role;

    typedef enum
    {
        STUDY = 1,  /// Type to represent Study/Patient
        SERIES      /// Type to represent Series
    }ItemType;

    /// Constructor. Inits the model.
    UIMEDDATAQT_API SelectorModel(QWidget *parent = 0);

    /// Destructor. Does nothing
    UIMEDDATAQT_API ~SelectorModel();

    /**
     * @brief Add the Series in the tree. If the associated study already exist in the tree, the series is added to
     * this study.
     */
    UIMEDDATAQT_API void addSeries(::fwMedData::Series::sptr series);
    UIMEDDATAQT_API void removeSeries(::fwMedData::Series::sptr series);

    /// Clear all items in the model.
    UIMEDDATAQT_API void clear();

    /// Returns item flags with non editable flag
    UIMEDDATAQT_API Qt::ItemFlags flags(const QModelIndex& index) const
    {
        return (QStandardItemModel::flags(index) & ~Qt::ItemIsEditable);
    }

    /// Returns the type of the item (SERIES or STUDY) associates to the ITEM_TYPE role.
    UIMEDDATAQT_API ItemType getItemType(const QModelIndex &index);

    /**
     * @brief Return an index in the same row of the given index and in the column.
     * @param[in] index index used to get the associated row.
     * @param[in] column the column of the index to return.
     */
    UIMEDDATAQT_API QModelIndex getIndex(const QModelIndex& index, int column );

    UIMEDDATAQT_API void removeRows(const QModelIndexList indexes);

private:

    /**
     * @brief Returns the informations contained in the data container as a string, all items are separated with the
     * separator.
     */
    template <typename T>
    QStandardItem* getInfo(T data, QString separator);

    bool removeStudyItem(QStandardItem *item);
    bool removeSeriesItem(QStandardItem *item);

    /// Initialize model. Set headers.
    void init();

    typedef std::map< ::fwMedData::DicomValueType, QStandardItem* > StudyUidItemMapType;

    /// Number of study rows in the tree
    int m_studyRowCount;

    /// Map to registers study Instance UID/study root item. Used to associate the series to theirs study in the tree
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
