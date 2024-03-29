/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include <QMainWindow>
#include <QPointer>

#include <functional>

namespace sight::ui::qt
{

/**
 * @brief defines the Qt main frame.
 *
 */
class UI_QT_CLASS_API_QT qt_main_frame : public QMainWindow
{
Q_OBJECT

public:

    /**
     * @brief Constructor.
     */
    UI_QT_API_QT qt_main_frame() noexcept;

    /// @brief Destructor.
    UI_QT_API_QT ~qt_main_frame() noexcept override;

    using CloseCallback = std::function<void ()>;
    UI_QT_API_QT void set_close_callback(CloseCallback _fct);

protected:

    void closeEvent(QCloseEvent* _event) override;

private:

    CloseCallback m_fct_close_callback;
};

} // namespace sight::ui::qt
