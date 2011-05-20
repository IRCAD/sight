/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <assert.h>
#include <iostream>
#include <sstream>
#include <locale.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/tokenizer.hpp>

#include <fwTools/Os.hpp>

#include <fwRuntime/io/XMLSubstitute.hpp>
#include <fwRuntime/profile/Profile.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include "fwGuiQt/App.hpp"

namespace fwGuiQt
{

//-----------------------------------------------------------------------------

App::App(int & argc, char ** argv)
    : QApplication (argc, argv)
{
    SLM_TRACE_FUNC();

    ::fwRuntime::profile::Profile::sptr profile = ::fwRuntime::profile::getCurrentProfile();
    SLM_ASSERT("Profile is not initialized", profile);

    setlocale(LC_ALL,"C"); // needed for mfo save process

    std::string appName = profile->getName();

    this->setApplicationName( QString::fromStdString(appName) );

    QObject::connect(this, SIGNAL(aboutToQuit()), this, SLOT(aboutToQuit()));
}

//-----------------------------------------------------------------------------

void App::aboutToQuit()
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void App::onExit()
{
    SLM_TRACE_FUNC();

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QApplication::restoreOverrideCursor();

    qApp->flush();
    qApp->exit(0);
}

//-----------------------------------------------------------------------------

} // namespace fwGuiQt

//-----------------------------------------------------------------------------

