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

#include "ui/qt/series/selector_model.hpp"

#include <QModelIndex>
#include <QTreeView>
#include <QVector>

namespace sight::ui::qt::series
{

/**
 * @brief This selector represents the Series in a hierarchical view (Study/Patient->Series).
 */
class SIGHT_UI_QT_CLASS_API_QT selector : public QTreeView
{
Q_OBJECT

public:

    /// Defines a map associating icons to series (map\<series classname, icon path\>)
    using series_icon_t = selector_model::series_icon_t;

    using series_vector_t = QVector<data::series::sptr>;

    /// Initializes the tree view.
    SIGHT_UI_QT_API_QT selector(const std::string& _display_columns, QWidget* _parent = nullptr);

    /// Destroys the selector.
    SIGHT_UI_QT_API_QT ~selector() override = default;

    /// Clears all items in the tree.
    void clear();

    /**
     * @brief Adds the Series in the tree. If the associated study already exists in the tree, the series is added to
     * this study.
     * @param _series series to add in the tree.
     */
    SIGHT_UI_QT_API_QT void add_series(data::series::sptr _series);

    /**
     * @brief Removes the Series from the tree. After deletion, if the study is empty, it will be removed.
     * @param _series series to remove from the tree.
     */
    SIGHT_UI_QT_API_QT void remove_series(data::series::sptr _series);

    /// Returns the type of the item (SERIES or STUDY)
    SIGHT_UI_QT_API_QT selector_model::item_t get_item_type(const QModelIndex& _index) const;

    /// Catches the delete key event and remove the selected items.
    SIGHT_UI_QT_API_QT void keyPressEvent(QKeyEvent* _event) override;

    /**
     * @brief Sets the specific icons for series in selector.
     * @param _series_icons map\<series classname, icon path\>
     */
    SIGHT_UI_QT_API_QT void set_series_icons(const series_icon_t& _series_icons);

    /// Allows removing items or not.
    SIGHT_UI_QT_API_QT void allow_remove(bool _allowed);

    /// Sets if the selector must be in insert mode.
    void set_insert_mode(bool _insert);

    /// Sets the remove study button icon.
    void set_remove_study_icon(const std::filesystem::path& _path);

    /// Sets the remove series button icon.
    void set_remove_series_icon(const std::filesystem::path& _path);

    /// Returns the selected series.
    inline series_vector_t get_selected_series() const
    {
        return get_series(this->selectedIndexes());
    }

    /// Returns a meaningful preferred size for the selector.
    SIGHT_UI_QT_API_QT QSize sizeHint() const override;

Q_SIGNALS:

    /**
     * @brief Signal emitted when the selection change.
     * @param _selection contains the new selection of series.
     * @param _deselection contains the new deselection of series.
     * @note selection and deselection contain only the change of selection. The series always selected or deselected
     * don't appear in this selection/deselection.
     */
    void series_selected(
        QVector<data::series::sptr> _selection,
        QVector<data::series::sptr> _deselection
    );

    /**
     * @brief Signal emitted when series are deleted.
     * @param _selection contains the deleted series.
     */
    void remove_series(QVector<data::series::sptr> _selection);

protected Q_SLOTS:

    /**
     * @brief Slot called when the selection changed. Emits a signal containing the new selected/deselected series. If a
     * Study is selected, no series are added in signal.
     * @param _selected contains the new selected items.
     * @param _deselected contains the new deselected items.
     * @note selection and deselection contain only the change of selection. The items always selected or deselected
     * don't appear in this selection/deselection.
     */
    void selectionChanged(const QItemSelection& _selected, const QItemSelection& _deselected) override;

private Q_SLOTS:

    /**
     * @brief SLOT: called when the selector model sends a signal to remove the study.
     * @param _uid the instance UID of the study to remove.
     */
    void on_remove_study_instance_uid(const std::string& _uid);

    /**
     * @brief SLOT: called when the selector model sends a signal to remove the series.
     * @param _id the ID of the series to remove.
     */
    void on_remove_series_id(const std::string& _id);

private:

    /**
     * @brief Returns all the Series associated to the selection.
     * @note If a study is selected, return an empty selection.
     */
    static inline series_vector_t get_series(const QItemSelection& _selection)
    {
        return get_series(_selection.indexes());
    }

    /**
     * @brief Returns all the Series associated to the selection.
     * @note It ignores the studies.
     */
    static series_vector_t get_series(const QModelIndexList& _index_list);

    /// Returns all the study indexes contained in the given indexList
    static QModelIndexList get_study_indexes(const QModelIndexList& _index_list);

    /// Returns all the series associated with the study index
    series_vector_t get_series_from_study_index(const QModelIndex& _index) const;

    /// Deletes the selected items and notify the deleted series.
    void delete_selection();

    /// Tree model
    QPointer<selector_model> m_model {nullptr};

    /// Allows to remove items.
    bool m_remove_allowed {true};
};

//-----------------------------------------------------------------------------

inline void selector::clear()
{
    m_model->clear();
}

//-----------------------------------------------------------------------------

inline void selector::set_insert_mode(bool _insert)
{
    m_model->set_insert_mode(_insert);
}

//-----------------------------------------------------------------------------

inline void selector::set_remove_study_icon(const std::filesystem::path& _path)
{
    m_model->set_remove_study_icon(_path);
}

//-----------------------------------------------------------------------------

inline void selector::set_remove_series_icon(const std::filesystem::path& _path)
{
    m_model->set_remove_series_icon(_path);
}

} // namespace sight::ui::qt::series
