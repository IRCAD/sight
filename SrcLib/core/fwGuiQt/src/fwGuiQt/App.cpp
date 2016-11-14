/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGuiQt/App.hpp"

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwRuntime/profile/Profile.hpp>

#include <fwTools/Os.hpp>

#include <locale.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/tokenizer.hpp>

#include <iostream>
#include <sstream>

namespace fwGuiQt
{

//-----------------------------------------------------------------------------

App::App(int& argc, char** argv, bool guiEnabled) :
    QApplication(argc, argv, guiEnabled)
{
    SLM_TRACE_FUNC();

    setlocale(LC_ALL, "C"); // needed for mfo save process
    QLocale::setDefault(QLocale::C); // on Linux we need that as well...

    std::string appName = "No name";

    ::fwRuntime::profile::Profile::sptr profile = ::fwRuntime::profile::getCurrentProfile();

    if (profile)
    {
        appName = profile->getName();
    }

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

