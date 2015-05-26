/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __ARANDROIDTOOLS_JNIHELPER_HPP__
#define __ARANDROIDTOOLS_JNIHELPER_HPP__

#include "arAndroidTools/config.hpp"

#include <jni.h>

#include <android_native_app_glue.h>

#include <string>

namespace arAndroidTools
{

class ARANDROIDTOOLS_CLASS_API JNIHelper
{
public:
    /// This methods need to be called before any call to the helper class.
    ARANDROIDTOOLS_API static void init(ANativeActivity* activity, const std::string& helper_class_name);

    /// Retrieve the singleton object of the helper.
    ARANDROIDTOOLS_API static JNIHelper* getInstance();


    ARANDROIDTOOLS_API int callIntMethod(const std::string& method);
    ARANDROIDTOOLS_API void callMethodInt(const std::string& method, int arg1);
    ARANDROIDTOOLS_API void callStaticMethodInt(const std::string& method, int arg1);

    ARANDROIDTOOLS_API bool callBoolMethodString(const std::string& method, const std::string& arg1);
    ARANDROIDTOOLS_API void callMethodBool(const std::string& method, bool arg1);
    ARANDROIDTOOLS_API void callStaticMethodBool(const std::string& method, bool arg1);

    ARANDROIDTOOLS_API void callMethod(const std::string& method);

    ARANDROIDTOOLS_API void registerNatives(JNINativeMethod* methods, int nbMethods);

private:
    JNIHelper();
    virtual ~JNIHelper();
    jclass retrieveClass( JNIEnv *jni, const std::string& class_name );

    ANativeActivity* m_activity;
    jobject m_javaRef;
    jclass m_javaClass;

    /// mutex for synchronization
    mutable pthread_mutex_t m_mutex;
};

} /* namespace arAndroidTools */

#endif /* __ARANDROIDTOOLS_JNIHELPER_HPP__ */
