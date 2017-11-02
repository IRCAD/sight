/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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

