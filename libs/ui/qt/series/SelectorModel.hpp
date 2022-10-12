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

#pragma once

#include "ui/qt/config.hpp"

#include <data/Series.hpp>

#include <QPointer>
#include <QStandardItem>
#include <QStandardItemModel>

#include <filesystem>

namespace sight::ui::qt::series
{

/**
 * @brief This class represents the Selector Model.
 */
class UI_QT_CLASS_API SelectorModel : public QStandardItemModel
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
        STUDY = 1, ///< Type to represent Study/Patient
        SERIES     ///< Type to represent Series
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

    using QStandardItemModel::removeRows;

    /// Initializes the model.
    UI_QT_API SelectorModel(QWidget* _parent = nullptr);

    /// Destroys the selector.
    UI_QT_API ~SelectorModel() override = default;

    /**
     * @brief Add the Series in the tree. If the associated study already exist in the tree, the series is added to
     * this study.
     */
    UI_QT_API void addSeries(data::Series::sptr _series);

    /**
     * @brief Removes the Series from the tree. After deletion, if the study is empty, it will be removed.
     * @param _series series to remove from the tree.
     */
    UI_QT_API void removeSeries(data::Series::sptr _series);

    /// Clears all items in the model.
    UI_QT_API void clear();

    /// Returns item flags with non editable flag
    Qt::ItemFlags flags(const QModelIndex& _index) const override
    {
        return QStandardItemModel::flags(_index) & ~Qt::ItemIsEditable;
    }

    /// Returns the type of the item (SERIES or STUDY) associated to the ITEM_TYPE role.
    UI_QT_API ItemType getItemType(const QModelIndex& _index);

    /**
     * @brief Returns the index in the same row as the given index and at the specified column.
     * @param _index index used to get the associated row.
     * @param _column the column of the index to return.
     */
    UI_QT_API QModelIndex getIndex(const QModelIndex& _index, int _column);

    /// Removes the rows given by the indexes.
    UI_QT_API void removeRows(const QModelIndexList _indexes);

    /// Returns the series item representing the series.
    UI_QT_API QStandardItem* findSeriesItem(data::Series::sptr _series);

    /// Returns the item representing the study.
    UI_QT_API QStandardItem* findStudyItem(data::Series::sptr _series);

    /**
     * @brief Sets the specific icons for series in selector.
     * @param _seriesIcons map\<series classname, icon path\>
     */
    UI_QT_API void setSeriesIcons(const SeriesIconType& _seriesIcons);

    /// Sets if the selector must be in insert mode.
    void setInsertMode(bool _insert);

    /// Allows removing items or not.
    void allowRemove(bool _allowed);

    /// Sets the remove study button icon.
    void setRemoveStudyIcon(const std::filesystem::path& _path);

    /// Sets the remove series button icon.
    void setRemoveSeriesIcon(const std::filesystem::path& _path);

Q_SIGNALS:

    /**
     * @brief SIGNAL: sent when the button to remove a study is clicked.
     * @param _uid the instance UID of the study to remove.
     */
    void removeStudyInstanceUID(const std::string& _uid);

    /**
     * @brief SIGNAL: sent when the button to remove a series is clicked.
     * @param _id the ID of the series to remove.
     */
    void removeSeriesID(const std::string& _id);

private:

    typedef std::map<data::DicomValueType, QStandardItem*> StudyUidItemMapType;

    /**
     * @brief Returns the informations contained in the data container as a string, all items are separated by the
     * separator.
     */
    template<typename T>
    QStandardItem* getInfo(T _data, QString _separator);

    /// Removes the study item and all the series associated.
    bool removeStudyItem(QStandardItem* _item);

    /// Removes the series item and the parent study if it is the last series in the study.
    bool removeSeriesItem(QStandardItem* _item);

    /// Adds the icon corresponding to the type of series.
    void addSeriesIcon(data::Series::sptr _series, QStandardItem* _item);

    /// Initializes model. Sets headers of the selector.
    void init();

    /// Defines the number of studies rows in the tree.
    int m_studyRowCount {0};

    /**
     * @brief Stores a map to register the association of study Instance UID  and study root item.
     * It is used to associate the series to its study in the tree.
     */
    StudyUidItemMapType m_items;

    /// Defines if the selector is in insert mode (adding new series, forbid selection of existing series).
    bool m_insert {false};

    /// Stores a map containing the specified icons for a series (map\<series classname, icon path\>).
    SeriesIconType m_seriesIcons;

    /// Allows to remove items.
    bool m_removeAllowed {true};

    /// Defines the path of the remove study button icon.
    std::filesystem::path m_removeStudyIcon;

    /// Defines the path of the remove series button icon.
    std::filesystem::path m_removeSeriesIcon;
};

//------------------------------------------------------------------------------

template<typename T>
QStandardItem* SelectorModel::getInfo(T _data, QString _separator)
{
    QString dataStr;
    if(!_data.empty())
    {
        typename T::iterator itr = _data.begin();
        std::ostringstream str;
        str << *itr++;
        dataStr = QString::fromStdString(str.str());

        for( ; itr != _data.end() ; ++itr)
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

inline void SelectorModel::allowRemove(bool _allowed)
{
    m_removeAllowed = _allowed;
}

//-----------------------------------------------------------------------------

inline void SelectorModel::setRemoveStudyIcon(const std::filesystem::path& _path)
{
    m_removeStudyIcon = _path;
}

//-----------------------------------------------------------------------------

inline void SelectorModel::setRemoveSeriesIcon(const std::filesystem::path& _path)
{
    m_removeSeriesIcon = _path;
}

} // namespace sight::ui::qt::series.
