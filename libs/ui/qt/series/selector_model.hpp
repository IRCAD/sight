/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <sight/ui/qt/config.hpp>

#include <data/series.hpp>

#include <QPointer>
#include <QStandardItem>
#include <QStandardItemModel>

#include <filesystem>

namespace sight::ui::qt::series
{

/**
 * @brief This class represents the selector Model.
 */
class SIGHT_UI_QT_CLASS_API_QT selector_model : public QStandardItemModel
{
Q_OBJECT

public:

    /// Defines the role of the items data (ITEM_TYPE or UID)
    enum role
    {
        item_type = Qt::UserRole, ///< Role for the item type (STUDY or SERIES)
        uid,                      ///< Role for the fwID of the object
        icon                      ///< Role to determine whether it holds the icon
    };

    /// Defines item type (STUDY or SERIES), it is used in items data (ITEM_TYPE role).
    enum item_t
    {
        study = 1, ///< Type to represent Study/Patient
        series     ///< Type to represent Series
    };

    /// Defines the map associating icons to series (map\<series classname, icon path\>)
    using series_icon_t = std::map<std::string, std::string>;

    using QStandardItemModel::removeRows;

    /// Initializes the model.
    SIGHT_UI_QT_API_QT selector_model(
        const std::string& _display_columns,
        QWidget* _parent   = nullptr,
        bool _allow_remove = false
    );

    /// Destroys the selector.
    SIGHT_UI_QT_API_QT ~selector_model() override = default;

    /**
     * @brief Add the Series in the tree. If the associated study already exist in the tree, the series is added to
     * this study.
     */
    SIGHT_UI_QT_API_QT void add_series(data::series::sptr _series);

    /**
     * @brief Removes the Series from the tree. After deletion, if the study is empty, it will be removed.
     * @param _series series to remove from the tree.
     */
    SIGHT_UI_QT_API_QT void remove_series(data::series::sptr _series);

    /// Clears all items in the model.
    SIGHT_UI_QT_API_QT void clear();

    /// Returns item flags with non editable flag
    Qt::ItemFlags flags(const QModelIndex& _index) const override
    {
        return QStandardItemModel::flags(_index) & ~Qt::ItemIsEditable;
    }

    /// Returns the type of the item (SERIES or STUDY) associated to the ITEM_TYPE role.
    SIGHT_UI_QT_API_QT item_t get_item_type(const QModelIndex& _index);

    /**
     * @brief Returns the index in the same row as the given index and at the specified column.
     * @param _index index used to get the associated row.
     * @param _column the column of the index to return.
     */
    SIGHT_UI_QT_API_QT QModelIndex get_index(const QModelIndex& _index, int _column);

    /// Removes the rows given by the indexes.
    SIGHT_UI_QT_API_QT void removeRows(const QModelIndexList _indexes);

    /// Returns the series item representing the series.
    SIGHT_UI_QT_API_QT QStandardItem* find_series_item(data::series::sptr _series);

    /// Returns the item representing the study.
    SIGHT_UI_QT_API_QT QStandardItem* find_study_item(data::series::sptr _series);

    /**
     * @brief Sets the specific icons for series in selector.
     * @param _series_icons map\<series classname, icon path\>
     */
    SIGHT_UI_QT_API_QT void set_series_icons(const series_icon_t& _series_icons);

    /// Sets if the selector must be in insert mode.
    void set_insert_mode(bool _insert);

    /// Allows removing items or not.
    void allow_remove(bool _allowed);

    /// Sets the remove study button icon.
    void set_remove_study_icon(const std::filesystem::path& _path);

    /// Sets the remove series button icon.
    void set_remove_series_icon(const std::filesystem::path& _path);

Q_SIGNALS:

    /**
     * @brief SIGNAL: sent when the button to remove a study is clicked.
     * @param _uid the instance UID of the study to remove.
     */
    void remove_study_instance_uid(const std::string& _uid);

    /**
     * @brief SIGNAL: sent when the button to remove a series is clicked.
     * @param _id the ID of the series to remove.
     */
    void remove_series_id(const std::string& _id);

private:

    using study_uid_item_map_type = std::map<data::dicom_value_t, QStandardItem*>;

    /**
     * @brief Returns the informations contained in the data container as a string, all items are separated by the
     * separator.
     */
    template<typename T>
    QStandardItem* get_info(T _data, QString _separator);

    /// Removes the study item and all the series associated.
    bool remove_study_item(QStandardItem* _item);

    /// Removes the series item and the parent study if it is the last series in the study.
    bool remove_series_item(QStandardItem* _item);

    /// Adds the icon corresponding to the type of series.
    void add_series_icon(data::series::sptr _series, QStandardItem* _item);

    /// Initializes model. Sets headers of the selector.
    void init();

    /// Defines the number of studies rows in the tree.
    int m_study_row_count {0};

    /**
     * @brief Stores a map to register the association of study Instance UID  and study root item.
     * It is used to associate the series to its study in the tree.
     */
    study_uid_item_map_type m_items;

    /// Defines if the selector is in insert mode (adding new series, forbid selection of existing series).
    bool m_insert {false};

    /// Stores a map containing the specified icons for a series (map\<series classname, icon path\>).
    series_icon_t m_series_icons;

    /// Allows to remove items.
    bool m_remove_allowed;

    /// Defines the path of the remove study button icon.
    std::filesystem::path m_remove_study_icon;

    /// Defines the path of the remove series button icon.
    std::filesystem::path m_remove_series_icon;

    /// A list of columns to be displayed
    std::vector<std::string> m_display_columns;
};

//------------------------------------------------------------------------------

template<typename T>
QStandardItem* selector_model::get_info(T _data, QString _separator)
{
    QString data_str;
    if(!_data.empty())
    {
        typename T::iterator itr = _data.begin();
        std::ostringstream str;
        str << *itr++;
        data_str = QString::fromStdString(str.str());

        for( ; itr != _data.end() ; ++itr)
        {
            str.str("");
            str << *itr;
            data_str.append(_separator + QString::fromStdString(str.str()));
        }
    }

    QStandardItem* item = new QStandardItem(data_str);
    return item;
}

//-----------------------------------------------------------------------------

inline void selector_model::set_insert_mode(bool _insert)
{
    m_insert = _insert;
}

//------------------------------------------------------------------------------

inline void selector_model::allow_remove(bool _allowed)
{
    m_remove_allowed = _allowed;
}

//-----------------------------------------------------------------------------

inline void selector_model::set_remove_study_icon(const std::filesystem::path& _path)
{
    m_remove_study_icon = _path;
}

//-----------------------------------------------------------------------------

inline void selector_model::set_remove_series_icon(const std::filesystem::path& _path)
{
    m_remove_series_icon = _path;
}

} // namespace sight::ui::qt::series.
