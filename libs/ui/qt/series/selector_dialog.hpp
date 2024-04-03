/************************************************************************
 *
 * Copyright (C) 2022-2024 IRCAD France
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

#include <data/series_set.hpp>

#include <QDialog>

namespace sight::ui::qt::series
{

/// This class allows to display a DICOM series
class SIGHT_UI_QT_CLASS_API_QT selector_dialog : public QDialog
{
Q_OBJECT

public:

    /// Initializes the selector dialog.
    /// @param _series_set the series to select.
    /// @param _parent the parent widget.
    SIGHT_UI_QT_API_QT selector_dialog(
        data::series_set::csptr _series_set,
        const std::string& _displayed_column,
        QWidget* _parent = nullptr
    );

    /// Destroys the selector.
    SIGHT_UI_QT_API_QT ~selector_dialog() noexcept override;

    /// Returns the selected series.
    SIGHT_UI_QT_API_QT data::series_set::sptr get_selection() const;

private:

    class selector_dialog_impl;
    std::unique_ptr<selector_dialog_impl> m_pimpl;
};

} // namespace sight::ui::qt::series
