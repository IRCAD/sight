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

#pragma once

#include "ui/qt/config.hpp"
#include "ui/qt/series/SelectorModel.hpp"

#include <QModelIndex>
#include <QTreeView>
#include <QVector>

namespace sight::ui::qt::series
{

/**
 * @brief This selector represents the Series in a hierarchical view (Study/Patient->Series).
 */
class UI_QT_CLASS_API_QT Selector : public QTreeView
{
Q_OBJECT

public:

    /// Defines a map associating icons to series (map\<series classname, icon path\>)
    using SeriesIconType = SelectorModel::SeriesIconType;

    using SeriesVectorType = QVector<data::Series::sptr>;

    /// Initializes the tree view.
    UI_QT_API_QT Selector(const std::string& displayColumns, QWidget* _parent = nullptr);

    /// Destroys the selector.
    UI_QT_API_QT ~Selector() override = default;

    /// Clears all items in the tree.
    void clear();

    /**
     * @brief Adds the Series in the tree. If the associated study already exists in the tree, the series is added to
     * this study.
     * @param _series series to add in the tree.
     */
    UI_QT_API_QT void addSeries(data::Series::sptr _series);

    /**
     * @brief Removes the Series from the tree. After deletion, if the study is empty, it will be removed.
     * @param _series series to remove from the tree.
     */
    UI_QT_API_QT void removeSeries(data::Series::sptr _series);

    /// Returns the type of the item (SERIES or STUDY)
    UI_QT_API_QT SelectorModel::ItemType getItemType(const QModelIndex& _index) const;

    /// Catches the delete key event and remove the selected items.
    UI_QT_API_QT void keyPressEvent(QKeyEvent* _event) override;

    /**
     * @brief Sets the specific icons for series in selector.
     * @param _seriesIcons map\<series classname, icon path\>
     */
    UI_QT_API_QT void setSeriesIcons(const SeriesIconType& _seriesIcons);

    /// Allows removing items or not.
    UI_QT_API_QT void allowRemove(bool _allowed);

    /// Sets if the selector must be in insert mode.
    void setInsertMode(bool _insert);

    /// Sets the remove study button icon.
    void setRemoveStudyIcon(const std::filesystem::path& _path);

    /// Sets the remove series button icon.
    void setRemoveSeriesIcon(const std::filesystem::path& _path);

    /// Returns the selected series.
    inline SeriesVectorType getSelectedSeries() const
    {
        return getSeries(this->selectedIndexes());
    }

    /// Returns a meaningful preferred size for the selector.
    UI_QT_API_QT QSize sizeHint() const override;

Q_SIGNALS:

    /**
     * @brief Signal emitted when the selection change.
     * @param _selection contains the new selection of series.
     * @param _deselection contains the new deselection of series.
     * @note selection and deselection contain only the change of selection. The series always selected or deselected
     * don't appear in this selection/deselection.
     */
    void seriesSelected(
        QVector<data::Series::sptr> _selection,
        QVector<data::Series::sptr> _deselection
    );

    /**
     * @brief Signal emitted when series are deleted.
     * @param _selection contains the deleted series.
     */
    void removeSeries(QVector<data::Series::sptr> _selection);

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
    void onRemoveStudyInstanceUID(const std::string& _uid);

    /**
     * @brief SLOT: called when the selector model sends a signal to remove the series.
     * @param _id the ID of the series to remove.
     */
    void onRemoveSeriesID(const std::string& _id);

private:

    /**
     * @brief Returns all the Series associated to the selection.
     * @note If a study is selected, return an empty selection.
     */
    static inline SeriesVectorType getSeries(const QItemSelection& _selection)
    {
        return getSeries(_selection.indexes());
    }

    /**
     * @brief Returns all the Series associated to the selection.
     * @note It ignores the studies.
     */
    static SeriesVectorType getSeries(const QModelIndexList& _indexList);

    /// Returns all the study indexes contained in the given indexList
    static QModelIndexList getStudyIndexes(const QModelIndexList& _indexList);

    /// Returns all the series associated with the study index
    SeriesVectorType getSeriesFromStudyIndex(const QModelIndex& _index) const;

    /// Deletes the selected items and notify the deleted series.
    void deleteSelection();

    /// Tree model
    QPointer<SelectorModel> m_model {nullptr};

    /// Allows to remove items.
    bool m_removeAllowed {true};
};

//-----------------------------------------------------------------------------

inline void Selector::clear()
{
    m_model->clear();
}

//-----------------------------------------------------------------------------

inline void Selector::setInsertMode(bool _insert)
{
    m_model->setInsertMode(_insert);
}

//-----------------------------------------------------------------------------

inline void Selector::setRemoveStudyIcon(const std::filesystem::path& _path)
{
    m_model->setRemoveStudyIcon(_path);
}

//-----------------------------------------------------------------------------

inline void Selector::setRemoveSeriesIcon(const std::filesystem::path& _path)
{
    m_model->setRemoveSeriesIcon(_path);
}

} // namespace sight::ui::qt::series
