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

#include "ui/qt/qt_main_frame.hpp"

#include <core/base.hpp>

#include <ui/__/dialog/message.hpp>

#include <QCloseEvent>
#include <QMainWindow>

namespace sight::ui::qt
{

//------------------------------------------------------------------------------

qt_main_frame::qt_main_frame() noexcept =
    default;

//------------------------------------------------------------------------------

qt_main_frame::~qt_main_frame() noexcept =
    default;

//------------------------------------------------------------------------------

void qt_main_frame::set_close_callback(CloseCallback _fct)
{
    m_fct_close_callback = _fct;
}

//------------------------------------------------------------------------------

void qt_main_frame::closeEvent(QCloseEvent* _event)
{
    m_fct_close_callback();
    _event->ignore();
}

//------------------------------------------------------------------------------

} // namespace sight::ui::qt
