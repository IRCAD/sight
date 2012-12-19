/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/bind.hpp>

#include <fwCore/base.hpp>

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>
#include <fwRuntime/profile/Profile.hpp>

#include <fwServices/macros.hpp>

#include <fwGuiWx/WorkerWx.hpp>

#include "guiWx/Plugin.hpp"

namespace guiWx
{

//-----------------------------------------------------------------------------

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::guiWx::Plugin");

//-----------------------------------------------------------------------------

Plugin::~Plugin() throw()
{}

//-----------------------------------------------------------------------------

void Plugin::start() throw(::fwRuntime::RuntimeException)
{
    ::fwRuntime::profile::Profile::sptr profile = ::fwRuntime::profile::getCurrentProfile();
    SLM_ASSERT("Profile is not initialized", profile);
    int &argc = profile->getRawArgCount();
    char** argv = profile->getRawParams();

    m_workerWx = ::fwGuiWx::getWxWorker(argc, argv);

    ::fwRuntime::profile::getCurrentProfile()->setRunCallback(::boost::bind(&Plugin::run, this));
}

//-----------------------------------------------------------------------------

void Plugin::stop() throw()
{
}

//-----------------------------------------------------------------------------

void setup()
{
    ::fwRuntime::profile::getCurrentProfile()->setup();
}

//-----------------------------------------------------------------------------

int Plugin::run() throw()
{
    m_workerWx->post( ::boost::bind( &setup ) );
    m_workerWx->getFuture().wait(); // This is required to start WorkerWx loop

    ::fwRuntime::profile::getCurrentProfile()->cleanup();
    return ::boost::any_cast<int>(m_workerWx->getFuture().get());
}

} // namespace guiWx
