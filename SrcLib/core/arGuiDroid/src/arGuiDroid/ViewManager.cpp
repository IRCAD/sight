/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "arGuiDroid/ViewManager.hpp"
#include "arGuiDroid/JNIHelper.hpp"

#include <android_native_app_glue.h>
#include <fwCore/spyLog.hpp>
#include <fwRuntime/profile/Profile.hpp>

namespace arGuiDroid
{

//-----------------------------------------------------------------------------

static ViewManager* current_manager = NULL;


//-----------------------------------------------------------------------------

static void notifyFrameFetched(JNIEnv *env, jobject thiz, int id, jbyteArray data)
{

}

//-----------------------------------------------------------------------------

ViewManager::ViewManager()
{
    SLM_TRACE_FUNC();
    current_manager = this;

    std::string androidApkPackage(ANDROID_APK_PACKAGE);
    androidApkPackage += "/ViewManager";

    ::fwRuntime::profile::Profile::sptr profile = ::fwRuntime::profile::getCurrentProfile();
    JNIHelper* helper = JNIHelper::getInstance();
    helper->init(profile->getApp()->activity,androidApkPackage);


//    JNINativeMethod methods[] =
//    {
//        {"notifyFrameFetched", "(I[B)V", reinterpret_cast<void *>(notifyFrameFetched)}
//    };
//    helper->registerNatives(methods, 1);

}

//-----------------------------------------------------------------------------

ViewManager::~ViewManager()
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void ViewManager::createSlider(int max)
{
    SLM_TRACE_FUNC();

    JNIHelper* helper = JNIHelper::getInstance();
    helper->callMethodInt("createSlider",max);

}

//-----------------------------------------------------------------------------

void ViewManager::createButton(std::string label)
{
    SLM_TRACE_FUNC();

    JNIHelper* helper = JNIHelper::getInstance();
    helper->callMethodString("createButton",label);

}

//-----------------------------------------------------------------------------

void ViewManager::update()
{
    SLM_TRACE_FUNC();
    JNIHelper* helper = JNIHelper::getInstance();
    helper->callMethod("update");

    // Setup view
//     helper->callMethodInt("setMax", m_max);
//     helper->callMethodInt("setValue", m_value);


    // helper->callMethodInt("setPreviewFrameRate", m_frameRate);
    //
    // // camera new instance !
    // helper->callMethodInt("open",m_cameraId);
    // helper->callMethodInt("setViewManagerDisplayOrientation",0);
    //
    // // get camera parameters
    // m_format    = helper->callIntMethod("getPreviewFormat");
    // m_width     = helper->callIntMethod("getPreviewWidth");
    // m_height    = helper->callIntMethod("getPreviewHeight");
    // m_frameRate = helper->callIntMethod("getPreviewFrameRate");
    //
    // g_width  = m_width;
    // g_height = m_height;
    //
    // helper->callMethodBool("fetchEachFrame", true);
    // helper->callMethod("startPreview");

}

//-----------------------------------------------------------------------------

//void ViewManager::fetchFrame(unsigned char* frame)
//{
//    SLM_TRACE_FUNC();
//    // m_sigFrameFetched(frame);
//}

//-----------------------------------------------------------------------------

} // end namespace arGuiDroid




