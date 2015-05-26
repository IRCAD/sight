/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "arAndroidTools/Camera.hpp"
#include "arAndroidTools/JNIHelper.hpp"

#include <android_native_app_glue.h>
#include <fwCore/spyLog.hpp>
#include <fwRuntime/profile/Profile.hpp>

namespace arAndroidTools
{

//-----------------------------------------------------------------------------

static Camera* current_camera = NULL;
static int g_width            = 0;
static int g_height           = 0;

//-----------------------------------------------------------------------------

extern "C"
{

static void convertYUVtoRGB(int y, int u, int v, int index, unsigned char* pixels)
{
    float uf = static_cast<float>(u);
    float vf = static_cast<float>(v);

    int r = y + static_cast<int>(1.596f*vf);
    int g = y -  static_cast<int>(0.391f*uf + 0.813f*vf);
    int b = y + static_cast<int>(2.018f*uf);

    r = r>255 ? 255 : r<0 ? 0 : r;
    g = g>255 ? 255 : g<0 ? 0 : g;
    b = b>255 ? 255 : b<0 ? 0 : b;


    pixels[index]   = static_cast<char>(r);
    pixels[index+1] = static_cast<char>(g);
    pixels[index+2] = static_cast<char>(b);
    pixels[index+3] = 255;

}

//-----------------------------------------------------------------------------

static void notifyFrameFetched(JNIEnv *env, jobject thiz, int id, jbyteArray data)
{
    SLM_TRACE_FUNC();

    if (current_camera == NULL)
    {
        SLM_FATAL(" current_camera == NULL");
    }
    else
    {

        jboolean isCopy;
        jbyte* data_ptr = env->GetByteArrayElements(data, &isCopy);

        unsigned char* argb = new unsigned char[g_width * g_height * 4 * sizeof(unsigned char)];

        int size     = g_width * g_height;
        int v_offset = size;
        int u_offset = size +1;

        int y1_1,y1_2,y1_3,y1_4;
        int y2_1,y2_2,y2_3,y2_4;
        int y3_1,y3_2,y3_3,y3_4;
        int y4_1,y4_2,y4_3,y4_4;
        int u_1,u_2,u_3,u_4;
        int v_1,v_2,v_3,v_4;

        for(int i = 0, k = 0; i < size; i += 8, k += 8)
        {
            //-----------------------------------------------------------------------------
            // indexes
            //-----------------------------------------------------------------------------
            int id1 = i;
            int id2 = i+2;
            int id3 = i+4;
            int id4 = i+6;

            int k1 = k;
            int k2 = k+2;
            int k3 = k+4;
            int k4 = k+6;

            //-----------------------------------------------------------------------------
            // yuv components
            //-----------------------------------------------------------------------------

            // index 1
            y1_1 = data_ptr[id1  ]&0xff;
            y2_1 = data_ptr[id1+1]&0xff;
            y3_1 = data_ptr[g_width+id1  ]&0xff;
            y4_1 = data_ptr[g_width+id1+1]&0xff;

            u_1 = data_ptr[u_offset + k1]&0xff;
            v_1 = data_ptr[v_offset + k1]&0xff;

            u_1 -= 128;
            v_1 -= 128;

            // index 2
            y1_2 = data_ptr[id2  ]&0xff;
            y2_2 = data_ptr[id2+1]&0xff;
            y3_2 = data_ptr[g_width+id2  ]&0xff;
            y4_2 = data_ptr[g_width+id2+1]&0xff;

            u_2 = data_ptr[u_offset + k2]&0xff;
            v_2 = data_ptr[v_offset + k2]&0xff;

            u_2 -= 128;
            v_2 -= 128;

            //index 3
            y1_3 = data_ptr[id3  ]&0xff;
            y2_3 = data_ptr[id3+1]&0xff;
            y3_3 = data_ptr[g_width+id3  ]&0xff;
            y4_3 = data_ptr[g_width+id3+1]&0xff;

            u_3 = data_ptr[u_offset + k3]&0xff;
            v_3 = data_ptr[v_offset + k3]&0xff;

            u_3 -= 128;
            v_3 -= 128;

            //index 4
            y1_4 = data_ptr[id4  ]&0xff;
            y2_4 = data_ptr[id4+1]&0xff;
            y3_4 = data_ptr[g_width+id4  ]&0xff;
            y4_4 = data_ptr[g_width+id4+1]&0xff;

            u_4 = data_ptr[u_offset + k4]&0xff;
            v_4 = data_ptr[v_offset + k4]&0xff;

            u_4 = u_4 - 128;
            v_4 = v_4 - 128;

            //-----------------------------------------------------------------------------
            // Convert to rgb
            //-----------------------------------------------------------------------------
            convertYUVtoRGB(y1_1, u_1, v_1, id1*4, argb);
            convertYUVtoRGB(y2_1, u_1, v_1, (id1+1)*4, argb);
            convertYUVtoRGB(y3_1, u_1, v_1, (g_width+id1)*4, argb);
            convertYUVtoRGB(y4_1, u_1, v_1, (g_width+id1+1)*4, argb);

            convertYUVtoRGB(y1_2, u_2, v_2, (id2)*4, argb);
            convertYUVtoRGB(y2_2, u_2, v_2, (id2+1)*4, argb);
            convertYUVtoRGB(y3_2, u_2, v_2, (g_width+id2)*4, argb);
            convertYUVtoRGB(y4_2, u_2, v_2, (g_width+id2+1)*4, argb);

            convertYUVtoRGB(y1_3, u_3, v_3, (id3)*4, argb);
            convertYUVtoRGB(y2_3, u_3, v_3, (id3+1)*4, argb);
            convertYUVtoRGB(y3_3, u_3, v_3, (g_width+id3)*4, argb);
            convertYUVtoRGB(y4_3, u_3, v_3, (g_width+id3+1)*4, argb);

            convertYUVtoRGB(y1_4, u_4, v_4, (id4)*4, argb);
            convertYUVtoRGB(y2_4, u_4, v_4, (id4+1)*4, argb);
            convertYUVtoRGB(y3_4, u_4, v_4, (g_width+id4)*4, argb);
            convertYUVtoRGB(y4_4, u_4, v_4, (g_width+id4+1)*4, argb);

            //-----------------------------------------------------------------------------

            if (i!=0 && (i+8)%g_width==0)
            {
                i += g_width;
            }
        }
        // call the fechFrame method to emit signal
        current_camera->fetchFrame(argb);

        env->ReleaseByteArrayElements(data, data_ptr, JNI_ABORT);

    }
}

} // end extern C

//-----------------------------------------------------------------------------

Camera::Camera()
{
    SLM_TRACE_FUNC();
    current_camera = this;

    std::string androidApkPackage(ANDROID_APK_PACKAGE);
    androidApkPackage += "/FwCamera";

    ::fwRuntime::profile::Profile::sptr profile = ::fwRuntime::profile::getCurrentProfile();
    JNIHelper* helper = JNIHelper::getInstance();
    helper->init(profile->getApp()->activity,androidApkPackage);

    JNINativeMethod methods[] =
    {
        {"notifyFrameFetched", "(I[B)V", reinterpret_cast<void *>(notifyFrameFetched)}
    };
    helper->registerNatives(methods, 1);

}

//-----------------------------------------------------------------------------

Camera::~Camera()
{
    SLM_TRACE_FUNC();
}
//-----------------------------------------------------------------------------

void Camera::open(unsigned int cameraId)
{
    SLM_TRACE_FUNC();
    JNIHelper* helper = JNIHelper::getInstance();

    m_cameraId = cameraId;

    // Setup camera
    helper->callMethodBool("setAutoFocus", m_autoFocus);
    helper->callMethodInt("setPreviewWidth", m_width);
    helper->callMethodInt("setPreviewHeight", m_height);
    helper->callMethodInt("setPreviewFrameRate", m_frameRate);

    // camera new instance !
    helper->callMethodInt("open",m_cameraId);
    helper->callMethodInt("setCameraDisplayOrientation",0);

    // get camera parameters
    m_format    = helper->callIntMethod("getPreviewFormat");
    m_width     = helper->callIntMethod("getPreviewWidth");
    m_height    = helper->callIntMethod("getPreviewHeight");
    m_frameRate = helper->callIntMethod("getPreviewFrameRate");

    g_width  = m_width;
    g_height = m_height;

    helper->callMethodBool("fetchEachFrame", true);
    helper->callMethod("startPreview");

}

//-----------------------------------------------------------------------------

void Camera::fetchFrame(unsigned char* frame)
{
    SLM_TRACE_FUNC();
    m_sigFrameFetched(frame);
}

//-----------------------------------------------------------------------------

} // end namespace arAndroidTools




