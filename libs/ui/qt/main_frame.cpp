/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

#include "main_frame.hpp"

#include <core/base.hpp>

#include <ui/__/dialog/message.hpp>

#include <QCloseEvent>
#include <QGuiApplication>
#include <QLayout>
#include <QOpenGLWidget>
#include <QWindow>

namespace sight::ui::qt
{

//------------------------------------------------------------------------------

void main_frame::set_close_callback(CloseCallback _fct)
{
    m_fct_close_callback = _fct;
}

//------------------------------------------------------------------------------

void main_frame::closeEvent(QCloseEvent* _event)
{
    m_fct_close_callback();
    _event->ignore();
}

//------------------------------------------------------------------------------

void main_frame::showEvent(QShowEvent* _event)
{
    // This is a ugly hack to force the re-creation of the window type from RasterSurface to OpenGLSurface, *before*
    // being shown for the first time.
    // Otherwise, QWidget::setParent(..), which is called when adding a child to the layout, will do that while this
    // window is visible making a flickering effect.
    /// @see https://doc.qt.io/qt-6/qopenglwidget.html#limitations-and-other-considerations
    /// @note With Qt 6.8.2, we can call window->setSurfaceType(QSurface::OpenGLSurface)
    /// @note With Qt 6.4.x (Ubuntu 22.04), we must use an uglier hack, add/remove a dummy QOpenGLWidget to the layout
    if(static bool once = false; !once)
    {
        if(auto* const window = this->windowHandle();
           window != nullptr
           && window->surfaceType() != QSurface::OpenGLSurface
           && qApp->platformName() != "offscreen")
        {
                #if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
            window->setSurfaceType(QSurface::OpenGLSurface);
                #else
            QOpenGLWidget dummy;
            this->setCentralWidget(&dummy);
            this->takeCentralWidget();
                #endif
        }

        once = true;
    }

    QMainWindow::showEvent(_event);
}

//------------------------------------------------------------------------------

} // namespace sight::ui::qt
