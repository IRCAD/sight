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

#pragma once

#include "uiMedDataQt/config.hpp"

#include <fwMedData/Series.hpp>
#include <fwMedData/Study.hpp>

#include <QPointer>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QString>

#include <map>

namespace uiMedDataQt
{
namespace widget
{

/**
 * @brief This class represents the Selector Model.
 */
class UIMEDDATAQT_CLASS_API SelectorModel : public QStandardItemModel
{

Q_OBJECT

public:

    /// Defines the role of the items data (ITEM_TYPE or UID)
    enum Role
    {
        ITEM_TYPE = Qt::UserRole, ///< Role for the item type (STUDY or SERIES)
        UID                       ///< Role for the fwID of the object
    };

    /// Defines item type (STUDY or SERIES), it is used in items data (ITEM_TYPE role).
    enum ItemType
    {
        STUDY = 1,  ///< Type to represent Study/Patient
        SERIES      ///< Type to represent Series
    };

    /// Defines header columns used in the tree widget of all series.
    enum class ColumnSeriesType : int
    {
        NAME = 0,
        SEX,
        BIRTHDATE,

        MODALITY,

        DESCRIPTION,

        DATE,
        TIME,

        PATIENT_AGE,

        BODY_PART_EXAMINED,
        PATIENT_POSITION,
        CONTRAST_AGENT,
        ACQUISITION_TIME,
        CONTRAST_BOLUS_START_TIME,

        REMOVE
    };

    /// Defines the map associating icons to series (map\<series classname, icon path\>)
    typedef std::map<std::string, std::string> SeriesIconType;

    /// Initializes the model.
    UIMEDDATAQT_API SelectorModel(QWidget* _parent = nullptr);

    /// Destroys the selector.
    UIMEDDATAQT_API ~SelectorModel();

    /**
     * @brief Add the Series in the tree. If the associated study already exist in the tree, the series is added to
     * this study.
     */
    UIMEDDATAQT_API void addSeries(::fwMedData::Series::sptr _series);

    /**
     * @brief Removes the Series from the tree. After deletion, if the study is empty, it will be removed.
     * @param _series series to remove from the tree.
     */
    UIMEDDATAQT_API void removeSeries(::fwMedData::Series::sptr _series);

    /// Clears all items in the model.
    UIMEDDATAQT_API void clear();

    /// Returns item flags with non editable flag
    Qt::ItemFlags flags(const QModelIndex& _index) const
    {
        return (QStandardItemModel::flags(_index) & ~Qt::ItemIsEditable);
    }

    /// Returns the type of the item (SERIES or STUDY) associated to the ITEM_TYPE role.
    UIMEDDATAQT_API ItemType getItemType(const QModelIndex& _index);

    /**
     * @brief Returns the index in the same row as the given index and at the specified column.
     * @param _index index used to get the associated row.
     * @param _column the column of the index to return.
     */
    UIMEDDATAQT_API QModelIndex getIndex(const QModelIndex& _index, int _column);

    /// Removes the rows given by the indexes.
    UIMEDDATAQT_API void removeRows(const QModelIndexList _indexes);

    /// Returns the series item representing the series.
    UIMEDDATAQT_API QStandardItem* findSeriesItem(::fwMedData::Series::sptr _series);

    /// Returns the item representing the study.
    UIMEDDATAQT_API QStandardItem* findStudyItem(::fwMedData::Study::sptr _study);

    /**
     * @brief Sets the specific icons for series in selector.
     * @param _seriesIcons map\<series classname, icon path\>
     */
    UIMEDDATAQT_API void setSeriesIcons(const SeriesIconType& _seriesIcons);

    /// Sets if the selector must be in insert mode.
    void setInsertMode(bool _insert);

    /// Allows removing items or not.
    void setAllowedRemove(bool _allowed);

    /// Sets the remove study button icon.
    void setRemoveStudyIcon(const std::filesystem::path& _path);

    /// Sets the remove serie button icon.
    void setRemoveSerieIcon(const std::filesystem::path& _path);

Q_SIGNALS:

    /**
     * @brief SIGNAL: sent when the button to remove a study is clicked.
     * @param _uid the instance UID of the study to remove.
     */
    void removeStudyInstanceUID(const std::string& _uid);

    /**
     * @brief SIGNAL: sent when the button to remove a serie is clicked.
     * @param _id the ID of the serie to remove.
     */
    void removeSerieID(const std::string& _id);

private:

    typedef std::map< ::fwMedData::DicomValueType, QStandardItem* > StudyUidItemMapType;

    /**
     * @brief Returns the informations contained in the data container as a string, all items are separated by the
     * separator.
     */
    template <typename T>
    QStandardItem* getInfo(T _data, QString _separator);

    /// Removes the study item and all the series associated.
    bool removeStudyItem(QStandardItem* _item);

    /// Removes the series item and the parent study if it is the last series in the study.
    bool removeSeriesItem(QStandardItem* _item);

    /// Adds the icon corresponding to the type of series.
    void addSeriesIcon(::fwMedData::Series::sptr _series, QStandardItem* _item);

    /// Initializes model. Sets headers of the selector.
    void init();

    /// Defines the number of studies rows in the tree.
    int m_studyRowCount { 0 };

    /**
     * @brief Stores a map to register the association of study Instance UID  and study root item.
     * It is used to associate the series to its study in the tree.
     */
    StudyUidItemMapType m_items;

    /// Defines if the selector is in insert mode (adding new series, forbid selection of existing series).
    bool m_insert { false };

    /// Stores a map containing the specified icons for a series (map\<series classname, icon path\>).
    SeriesIconType m_seriesIcons;

    /// Allows to remove items.
    bool m_allowedRemove { true };

    /// Defines the path of the remove study button icon.
    std::filesystem::path m_removeStudyIcon;

    /// Defines the path of the remove serie button icon.
    std::filesystem::path m_removeSerieIcon;

};

//------------------------------------------------------------------------------

template <typename T>
QStandardItem* SelectorModel::getInfo(T _data, QString _separator)
{
    QString dataStr;
    if(!_data.empty())
    {
        typename T::iterator itr = _data.begin();
        std::ostringstream str;
        str << *itr++;
        dataStr = QString::fromStdString(str.str());

        for(; itr != _data.end(); ++itr)
        {
            str.str("");
            str << *itr;
            dataStr.append(_separator + QString::fromStdString(str.str()));
        }
    }
    QStandardItem* item = new QStandardItem(dataStr);
    return item;
}

//-----------------------------------------------------------------------------

inline void SelectorModel::setInsertMode(bool _insert)
{
    m_insert = _insert;
}

//------------------------------------------------------------------------------

inline void SelectorModel::setAllowedRemove(bool _allowed)
{
    m_allowedRemove = _allowed;
}

//-----------------------------------------------------------------------------

inline void SelectorModel::setRemoveStudyIcon(const std::filesystem::path& _path)
{
    m_removeStudyIcon = _path;
}

//-----------------------------------------------------------------------------

inline void SelectorModel::setRemoveSerieIcon(const std::filesystem::path& _path)
{
    m_removeSerieIcon = _path;
}

//-----------------------------------------------------------------------------

} // namespace widget.
} // namespace uiMedDataQt.
