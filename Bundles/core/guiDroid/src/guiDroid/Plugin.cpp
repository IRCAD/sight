/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "guiDroid/Plugin.hpp"

#include <fwRuntime/profile/Profile.hpp>
#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>
#include <fwServices/macros.hpp>

#include <JNIHelper.h>
#include <JUIWindow.h>

// Class name of helper function
#define HELPER_CLASS_NAME "com.ndkgui.helper.NDKHelper"
// Class name of JUIhelper function
#define JUIHELPER_CLASS_NAME "com.ndkgui.helper.JUIHelper"


namespace guiDroid
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::guiDroid::Plugin");

//-----------------------------------------------------------------------------

Plugin::~Plugin() throw()
{
}

//-----------------------------------------------------------------------------

void Plugin::start() throw(::fwRuntime::RuntimeException)
{
    ::fwRuntime::profile::Profile::sptr profile = ::fwRuntime::profile::getCurrentProfile();

    ANativeActivity* activity = profile->getApp()->activity;

    SLM_INFO(" Initialize NDKHelper");
    // The jni initialization
    ndkGui::JNIHelper::Init(activity, HELPER_CLASS_NAME);

    SLM_INFO(" Initialize JUIHelper");
    // The window initialization
    ndkGui::JUIWindow::Init(activity, JUIHELPER_CLASS_NAME);
}

//-----------------------------------------------------------------------------

void Plugin::stop() throw()
{
}

} // namespace guiDroid

