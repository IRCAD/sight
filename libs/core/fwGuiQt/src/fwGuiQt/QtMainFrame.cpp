/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include "fwGuiQt/QtMainFrame.hpp"

#include <fwCore/base.hpp>

#include <fwGui/dialog/IMessageDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>

#include <boost/lambda/lambda.hpp>

#include <QCloseEvent>
#include <QMainWindow>

namespace fwGuiQt
{

//------------------------------------------------------------------------------

QtMainFrame::QtMainFrame() noexcept :
    QMainWindow()
{
}

//------------------------------------------------------------------------------

QtMainFrame::~QtMainFrame() noexcept
{
}

//------------------------------------------------------------------------------

void QtMainFrame::setCloseCallback(CloseCallback fct)
{
    m_fctCloseCallback = fct;
}

//------------------------------------------------------------------------------

void QtMainFrame::closeEvent(QCloseEvent* event)
{
    SLM_TRACE_FUNC();
    m_fctCloseCallback();
    event->ignore();
}

//------------------------------------------------------------------------------

} // fwGuiQt

