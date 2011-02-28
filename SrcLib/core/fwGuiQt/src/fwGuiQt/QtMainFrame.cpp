/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QApplication>
#include <QMainWindow>
#include <QCloseEvent>

#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>

#include <fwCore/base.hpp>

#include "fwGuiQt/QtMainFrame.hpp"

namespace fwGuiQt
{


//------------------------------------------------------------------------------

QtMainFrame::QtMainFrame() throw(): QMainWindow()
{}

//------------------------------------------------------------------------------

QtMainFrame::~QtMainFrame() throw()
{
}

//------------------------------------------------------------------------------

void QtMainFrame::setCloseCallback(CloseCallback fct)
{
    m_fctCloseCallback = fct;
}

//------------------------------------------------------------------------------

void QtMainFrame::closeEvent(QCloseEvent *event)
{
    SLM_TRACE_FUNC();
    event->ignore();
    //m_fctCloseCallback();
    qApp->flush();
    qApp->quit();
    //QMainWindow::closeEvent(event);
}


//------------------------------------------------------------------------------

} // fwGuiQt

