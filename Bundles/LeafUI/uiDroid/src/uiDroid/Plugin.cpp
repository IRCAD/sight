/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiDroid/Plugin.hpp"

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>
#include <fwRuntime/profile/Profile.hpp>
#include <fwServices/macros.hpp>
#include <JNIHelper.h>
#include <JUIWindow.h>
#include <JUIBase.h>

// Class name of helper function
#define HELPER_CLASS_NAME "com.sample.helper.NDKHelper"
// Class name of JUIhelper function
#define JUIHELPER_CLASS_NAME "com.sample.helper.JUIHelper"

namespace uiDroid
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::uiDroid::Plugin");

// Overrides
void Plugin::start() throw(::fwRuntime::RuntimeException)
{
    ::fwRuntime::profile::Profile::sptr profile = ::fwRuntime::profile::getCurrentProfile();

    ANativeActivity* activity = profile->getApp()->activity;

    // The jni initialization
    ndkGui::JNIHelper::Init(activity, HELPER_CLASS_NAME);

    // The window initialization
    ndkGui::JUIWindow::Init(activity, JUIHELPER_CLASS_NAME);
}

} // namespace uiDroid
