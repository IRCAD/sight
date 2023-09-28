/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#include <data/SeriesSet.hpp>

#include <QDialog>

namespace sight::ui::qt::series
{

/// This class allows to display a DICOM series
class UI_QT_CLASS_API_QT selector : public QDialog
{
Q_OBJECT

public:

    /// Initializes the selector dialog.
    /// @param series the series to select.
    /// @param parent the parent widget.
    UI_QT_API_QT selector(
        data::SeriesSet::csptr series_set,
        const std::string& displayedColumn,
        QWidget* parent = nullptr
    );

    /// Destroys the selector.
    UI_QT_API_QT ~selector() noexcept override;

    /// Returns the selected series.
    UI_QT_API_QT data::SeriesSet::sptr get_selection() const;

private:

    class SelectorDialogImpl;
    std::unique_ptr<SelectorDialogImpl> m_pimpl;
};

} // namespace sight::ui::qt::series
