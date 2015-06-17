/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "arGuiDroid/JNIHelper.hpp"

#include <jni.h>
#include <fwCore/spyLog.hpp>

namespace arGuiDroid
{

#define CLASS_NAME "android/app/NativeActivity"

JNIHelper* JNIHelper::getInstance()
{
    SLM_TRACE_FUNC();
    static JNIHelper helper;
    return &helper;
}

//---------------------------------------------------------------------------

JNIHelper::JNIHelper()
{
    pthread_mutex_init( &m_mutex, NULL );

}

//---------------------------------------------------------------------------

JNIHelper::~JNIHelper()
{
    SLM_TRACE_FUNC();
    pthread_mutex_lock( &m_mutex );

    JNIEnv *env;
    m_activity->vm->AttachCurrentThread( &env, NULL );

    env->DeleteGlobalRef( m_javaRef );
    env->DeleteGlobalRef( m_javaClass );

    m_activity->vm->DetachCurrentThread();

    pthread_mutex_destroy( &m_mutex );
}

//---------------------------------------------------------------------------

void JNIHelper::init( ANativeActivity* activity, const std::string& helper_class_name )
{
    SLM_TRACE_FUNC();
    JNIHelper& helper = *getInstance();
    pthread_mutex_lock( &helper.m_mutex );

    helper.m_activity = activity;

    JNIEnv *env;
    helper.m_activity->vm->AttachCurrentThread( &env, NULL );

// copied from: https://github.com/googlesamples/android-nearby-cpp/blob/master/jni/ndk_helper/JNIHelper.cpp
//    // Retrieve app bundle id
//    jclass android_content_Context = env->GetObjectClass(helper.activity_->clazz);
//    jmethodID midGetPackageName = env->GetMethodID(
//        android_content_Context, "getPackageName", "()Ljava/lang/String;");

//    jstring packageName = (jstring)env->CallObjectMethod(helper.activity_->clazz,
//                                                         midGetPackageName);
//    const char *appname = env->GetStringUTFChars(packageName, NULL);
//    helper.app_bunlde_name_ = std::string(appname);

    jclass cls = helper.retrieveClass( env, helper_class_name );
    helper.m_javaClass = (jclass) env->NewGlobalRef( cls );

    jmethodID constructor = env->GetMethodID(helper.m_javaClass, "<init>", "(Landroid/app/NativeActivity;)V");
    SLM_ASSERT("Method doesn't exist or not found",constructor);
    helper.m_javaRef = env->NewObject( helper.m_javaClass, constructor, helper.m_activity->clazz);
    helper.m_javaRef = env->NewGlobalRef( helper.m_javaRef );
    helper.m_activity->vm->DetachCurrentThread();

    pthread_mutex_unlock( &helper.m_mutex );
}

//---------------------------------------------------------------------------

jclass JNIHelper::retrieveClass( JNIEnv *jni, const std::string& class_name )
{
    SLM_TRACE_FUNC();
    jclass activity_class      = jni->FindClass( CLASS_NAME );
    jmethodID get_class_loader = jni->GetMethodID( activity_class, "getClassLoader", "()Ljava/lang/ClassLoader;" );
    jobject cls                = jni->CallObjectMethod( m_activity->clazz, get_class_loader );
    jclass class_loader        = jni->FindClass( "java/lang/ClassLoader" );
    jmethodID find_class       = jni->GetMethodID( class_loader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;" );

    jstring str_class_name = jni->NewStringUTF( class_name.c_str() );
    jclass class_retrieved = (jclass) jni->CallObjectMethod( cls, find_class, str_class_name );
    jni->DeleteLocalRef( str_class_name );
    return class_retrieved;
}

//---------------------------------------------------------------------------

int JNIHelper::callIntMethod(const std::string& method)
{
    SLM_TRACE_FUNC();
    if( m_activity == NULL )
    {
        SLM_FATAL( "JNIHelper has not been initialized. Call init() to initialize the helper" );
        return 0;
    }

    JNIEnv *env;
    pthread_mutex_lock( &m_mutex );
    m_activity->vm->AttachCurrentThread( &env, NULL );

    jmethodID mid = env->GetMethodID( m_javaClass, method.c_str(), "()I" );
    jint ret      = env->CallIntMethod( m_javaRef, mid );

    m_activity->vm->DetachCurrentThread();
    pthread_mutex_unlock( &m_mutex );

    return ret;
}

//---------------------------------------------------------------------------

void JNIHelper::callMethodInt(const std::string& method, int arg1)
{
    SLM_TRACE_FUNC();
    if( m_activity == NULL )
    {
        SLM_FATAL( "JNIHelper has not been initialized. Call init() to initialize the helper" );
    }

    JNIEnv *env;
    pthread_mutex_lock( &m_mutex );
    m_activity->vm->AttachCurrentThread( &env, NULL );

    jmethodID mid = env->GetMethodID( m_javaClass, method.c_str(), "(I)V" );
    env->CallVoidMethod( m_javaRef, mid, jint(arg1) );

    m_activity->vm->DetachCurrentThread();
    pthread_mutex_unlock( &m_mutex );

}


//---------------------------------------------------------------------------

void JNIHelper::callStaticMethodInt(const std::string& method, int arg1)
{
    SLM_TRACE_FUNC();
    if( m_activity == NULL )
    {
        SLM_FATAL( "JNIHelper has not been initialized. Call init() to initialize the helper" );
    }

    JNIEnv *env;
    pthread_mutex_lock( &m_mutex );
    m_activity->vm->AttachCurrentThread( &env, NULL );

    jmethodID mid = env->GetMethodID( m_javaClass, method.c_str(), "(I)V" );
    env->CallStaticVoidMethod( m_javaClass, mid, jint(arg1) );

    m_activity->vm->DetachCurrentThread();
    pthread_mutex_unlock( &m_mutex );

}

//---------------------------------------------------------------------------

void JNIHelper::callMethodString(const std::string& method, const std::string& arg1)
{
    if( m_activity == NULL )
    {
        SLM_FATAL( "JNIHelper has not been initialized. Call init() to initialize the helper" );
    }

    JNIEnv *env;
    pthread_mutex_lock( &m_mutex );
    m_activity->vm->AttachCurrentThread( &env, NULL );

    jstring jArg1 = env->NewStringUTF( arg1.c_str() );

    jmethodID mid = env->GetMethodID( m_javaClass, method.c_str(), "(Ljava/lang/String;)V" );
    env->CallVoidMethod( m_javaRef, mid, jArg1 );

    env->DeleteLocalRef( jArg1 );
    m_activity->vm->DetachCurrentThread();
    pthread_mutex_unlock( &m_mutex );
}

//---------------------------------------------------------------------------

bool JNIHelper::callBoolMethodString(const std::string& method, const std::string& arg1)
{
    SLM_TRACE_FUNC();
    if( m_activity == NULL )
    {
        SLM_INFO( "JNIHelper has not been initialized. Call init() to initialize the helper" );
        return 0;
    }

    JNIEnv *env;
    pthread_mutex_lock( &m_mutex );
    m_activity->vm->AttachCurrentThread( &env, NULL );

    jstring jArg1 = env->NewStringUTF( arg1.c_str() );

    jmethodID mid = env->GetMethodID( m_javaClass, method.c_str(), "(Ljava/lang/String;)Z" );
    jboolean ret  = env->CallBooleanMethod( m_javaRef, mid, jArg1 );

    env->DeleteLocalRef( jArg1 );
    m_activity->vm->DetachCurrentThread();
    pthread_mutex_unlock( &m_mutex );

    return (ret != JNI_FALSE);
}

//---------------------------------------------------------------------------

void JNIHelper::callMethodBool(const std::string& method, bool arg1)
{
    SLM_TRACE_FUNC();
    if( m_activity == NULL )
    {
        SLM_FATAL( "JNIHelper has not been initialized. Call init() to initialize the helper" );
    }

    JNIEnv *env;
    pthread_mutex_lock( &m_mutex );
    m_activity->vm->AttachCurrentThread( &env, NULL );

    jmethodID mid = env->GetMethodID( m_javaClass, method.c_str(), "(Z)V" );
    env->CallVoidMethod( m_javaRef, mid, jboolean(arg1) );

    m_activity->vm->DetachCurrentThread();
    pthread_mutex_unlock( &m_mutex );

}

//---------------------------------------------------------------------------

void JNIHelper::callStaticMethodBool(const std::string& method, bool arg1)
{
    SLM_TRACE_FUNC();
    if( m_activity == NULL )
    {
        SLM_FATAL( "JNIHelper has not been initialized. Call init() to initialize the helper" );
    }

    JNIEnv *env;
    pthread_mutex_lock( &m_mutex );
    m_activity->vm->AttachCurrentThread( &env, NULL );

    jmethodID mid = env->GetMethodID( m_javaClass, method.c_str(), "(Z)V" );
    env->CallStaticVoidMethod( m_javaClass, mid, jboolean(arg1) );

    m_activity->vm->DetachCurrentThread();
    pthread_mutex_unlock( &m_mutex );

}


//---------------------------------------------------------------------------

void JNIHelper::callMethod(const std::string& method)
{
    SLM_TRACE_FUNC();
    if( m_activity == NULL )
    {
        SLM_FATAL( "JNIHelper has not been initialized. Call init() to initialize the helper" );
    }

    JNIEnv *env;
    pthread_mutex_lock( &m_mutex );
    m_activity->vm->AttachCurrentThread( &env, NULL );

    jmethodID mid = env->GetMethodID( m_javaClass, method.c_str(), "()V" );
    env->CallVoidMethod( m_javaRef, mid );

    m_activity->vm->DetachCurrentThread();
    pthread_mutex_unlock( &m_mutex );

}
//---------------------------------------------------------------------------

void JNIHelper::registerNatives(JNINativeMethod* methods, int nbMethods)
{
    SLM_TRACE_FUNC();
    if( m_activity == NULL )
    {
        SLM_FATAL( "JNIHelper has not been initialized. Call init() to initialize the helper" );
    }

    JNIEnv *env;
    pthread_mutex_lock( &m_mutex );
    m_activity->vm->AttachCurrentThread( &env, NULL );

    env->RegisterNatives(m_javaClass, methods, nbMethods);

    m_activity->vm->DetachCurrentThread();
    pthread_mutex_unlock( &m_mutex );
}

} /* namespace arAndroidTools */
