/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __ARANDROIDTOOLS_CAMERA_HPP__
#define __ARANDROIDTOOLS_CAMERA_HPP__

#include <boost/signals2.hpp>

#include <jni.h>

#include "arAndroidTools/config.hpp"

namespace arAndroidTools
{

class ARANDROIDTOOLS_CLASS_API Camera
{

public:

    ARANDROIDTOOLS_API Camera();

    ARANDROIDTOOLS_API virtual ~Camera();

    ARANDROIDTOOLS_API void open(unsigned int cameraId);

    ARANDROIDTOOLS_API void fetchFrame(unsigned char* frame);

    int getFormat()
    {
        return m_format;
    }

    void setWidth(int width)
    {
        m_width = width;
    }
    void setHeight(int height)
    {
        m_height = height;
    }
    void setFrameRate(int frameRate)
    {
        m_frameRate = frameRate;
    }
    void setAutoFocus(bool autofocus)
    {
        m_autoFocus = autofocus;
    }


    int getWidth()
    {
        return m_width;
    }
    int getHeight()
    {
        return m_height;
    }
    int getFrameRate()
    {
        return m_frameRate;
    }
    bool getAutoFocus()
    {
        return m_autoFocus;
    }

    ::boost::signals2::signal<void( unsigned char*)> m_sigFrameFetched;

private:
    unsigned int m_cameraId;

    int m_format;

    int m_width;
    int m_height;
    int m_frameRate;

    bool m_autoFocus;

};

} // end namespace arAndroidTools

#endif // __ARANDROIDTOOLS_CAMERA_HPP__
