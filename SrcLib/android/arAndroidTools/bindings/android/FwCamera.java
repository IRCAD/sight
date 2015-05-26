/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

package ${ANDROID_APK_PACKAGE};

import android.hardware.Camera;
import android.graphics.ImageFormat;
import android.graphics.SurfaceTexture;

import android.view.Surface;
import android.util.Log;

import android.opengl.GLES20;
import android.opengl.GLES11Ext;
    
import java.lang.Math;
import java.util.List;
import java.util.concurrent.locks.ReentrantLock;

public class FwCamera implements Camera.ShutterCallback,
                                 Camera.PictureCallback,
                                 Camera.AutoFocusCallback,
                                 Camera.PreviewCallback
{
    private int                 m_cameraId                  = -1;
    private Camera              m_camera                    = null;
    private byte[]              m_cameraPreviewFirstBuffer  = null;
    private byte[]              m_cameraPreviewSecondBuffer = null;
    private int                 m_actualPreviewBuffer       = 0;
    private final ReentrantLock m_buffersLock               = new ReentrantLock();
    private boolean             m_isReleased                = false;
    private boolean             m_fetchEachFrame            = false;

    private static final String TAG                         = "FwCamera";
    private boolean             m_autoFocus                 = false;
    private int                 m_width                     = 320;
    private int                 m_height                    = 240;
    private int                 m_frameRate                 = 30;
    private SurfaceTexture      m_surface                   = null;

    public FwCamera()
    {
        
        int[] textures = new int[1];
        GLES20.glGenTextures(1, textures, 0);

        int textureID = textures[0];

        GLES20.glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, textureID);

        /*
         * Create the SurfaceTexture that will feed this textureID, and pass it
         * to the camera
         */
        m_surface = new SurfaceTexture(textureID);
    }
    
    public void open(int cameraId)
    {
        m_cameraId = cameraId;

        try
        {
            m_camera = Camera.open(cameraId);
            Camera.Parameters parameters = m_camera.getParameters();

            List<String> focusModes = parameters.getSupportedFocusModes();
            if (focusModes.contains(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO) && m_autoFocus)
            {
                parameters.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO);
            }
            else
            {
                parameters.setFocusMode(Camera.Parameters.FOCUS_MODE_FIXED);
            }
            //
            Camera.Size size = getBestPreviewSize(m_width, m_height, parameters);
            Camera.Size pictureSize = getSmallestPictureSize(parameters);
            int frameRateRange[] = getBestFrameRate(m_frameRate, parameters);
            parameters.setPreviewFpsRange(frameRateRange[Camera.Parameters.PREVIEW_FPS_MIN_INDEX],
                    frameRateRange[Camera.Parameters.PREVIEW_FPS_MAX_INDEX]);

            if (size != null && pictureSize != null)
            {
                parameters.setPreviewSize(size.width, size.height);
                parameters.setPictureSize(pictureSize.width, pictureSize.height);
            }

            m_camera.setParameters(parameters);

            try
            {    
                 m_camera.setPreviewTexture(m_surface);
            }
            catch (Exception e)
            {
                Log.d(TAG, e.getMessage());
            }

        }
        catch (Exception e)
        {
            Log.d(TAG, e.getMessage());
        }
    }

    public Camera.Parameters getParameters()
    {
        return m_camera.getParameters();
    }

    public void lock()
    {
        try
        {
            m_camera.lock();
        }
        catch (Exception e)
        {
            Log.d(TAG, e.getMessage());
        }
    }

    public void unlock()
    {
        try
        {
            m_camera.unlock();
        }
        catch (Exception e)
        {
            Log.d(TAG, e.getMessage());
        }
    }

    public void release()
    {
        m_isReleased = true;
        m_camera.release();
    }

    public void reconnect()
    {
        try
        {
            m_camera.reconnect();
        }
        catch (Exception e)
        {
            Log.d(TAG, e.getMessage());
        }
    }

    public void setCameraDisplayOrientation(int rotation)
    {
        android.hardware.Camera.CameraInfo info =
            new android.hardware.Camera.CameraInfo();
        android.hardware.Camera.getCameraInfo(m_cameraId, info);
        int degrees = 0;
        
        switch (rotation) 
        {
            case Surface.ROTATION_0: degrees = 0; break;
            case Surface.ROTATION_90: degrees = 90; break;
            case Surface.ROTATION_180: degrees = 180; break;
            case Surface.ROTATION_270: degrees = 270; break;
        }

        int result;
        if (info.facing == Camera.CameraInfo.CAMERA_FACING_FRONT) 
        {
            result = (info.orientation + degrees) % 360;
            result = (360 - result) % 360;  // compensate the mirror
        } 
        else 
        {  // back-facing
            result = (info.orientation - degrees + 360) % 360;
        }
        
        m_camera.setDisplayOrientation(result);
    }

    public void setParameters(Camera.Parameters params)
    {
        try
        {
            m_camera.setParameters(params);
        }
        catch (Exception e)
        {
            Log.d(TAG, e.getMessage());
        }
    }

    public void setPreviewTexture(SurfaceTexture surfaceTexture)
    {
        try
        {
            m_camera.setPreviewTexture(surfaceTexture);
        }
        catch (Exception e)
        {
            Log.d(TAG, e.getMessage());
        }
    }

    public void fetchEachFrame(boolean fetch)
    {
        m_fetchEachFrame = fetch;
    }

    public void startPreview()
    {
        Camera.Size previewSize = m_camera.getParameters().getPreviewSize();
        double bytesPerPixel = ImageFormat.getBitsPerPixel(m_camera.getParameters().getPreviewFormat()) / 8.0;
        int bufferSizeNeeded = (int) Math.ceil(bytesPerPixel * previewSize.width * previewSize.height);

        // We need to clear preview buffers queue here, but there is no method
        // to do it
        // Though just resetting preview callback do the trick
        m_camera.setPreviewCallback(null);
        m_buffersLock.lock();
        if (m_cameraPreviewFirstBuffer == null || m_cameraPreviewFirstBuffer.length < bufferSizeNeeded)
            m_cameraPreviewFirstBuffer = new byte[bufferSizeNeeded];
        if (m_cameraPreviewSecondBuffer == null || m_cameraPreviewSecondBuffer.length < bufferSizeNeeded)
            m_cameraPreviewSecondBuffer = new byte[bufferSizeNeeded];
        addCallbackBuffer();
        m_buffersLock.unlock();
        m_camera.setPreviewCallbackWithBuffer(this);

        m_camera.startPreview();
    }

    public void stopPreview()
    {
        m_camera.stopPreview();
    }

    public void autoFocus()
    {
        m_camera.autoFocus(this);
    }

    public void cancelAutoFocus()
    {
        m_camera.cancelAutoFocus();
    }

    public void takePicture()
    {
        try
        {
            m_camera.takePicture(this, null, this);
        }
        catch (Exception e)
        {
            Log.d(TAG, e.getMessage());
        }
    }

    public byte[] lockAndFetchPreviewBuffer()
    {
        // This method should always be followed by unlockPreviewBuffer()
        // This method is not just a getter. It also marks last preview as
        // already seen one.
        // We should reset actualBuffer flag here to make sure we will not use
        // old preview with future captures
        byte[] result = null;
        m_buffersLock.lock();
        if (m_actualPreviewBuffer == 1)
            result = m_cameraPreviewFirstBuffer;
        else if (m_actualPreviewBuffer == 2)
            result = m_cameraPreviewSecondBuffer;
        m_actualPreviewBuffer = 0;
        return result;
    }

    public void unlockPreviewBuffer()
    {
        if (m_buffersLock.isHeldByCurrentThread())
            m_buffersLock.unlock();
    }

    private void addCallbackBuffer()
    {
        if (m_isReleased)
            return;

        m_camera.addCallbackBuffer((m_actualPreviewBuffer == 1) ? m_cameraPreviewSecondBuffer
                : m_cameraPreviewFirstBuffer);
    }

    @Override
    public void onShutter()
    {
        notifyPictureExposed(m_cameraId);
    }

    @Override
    public void onPictureTaken(byte[] data, Camera camera)
    {
        notifyPictureCaptured(m_cameraId, data);
    }

    @Override
    public void onPreviewFrame(byte[] data, Camera camera)
    {
        m_buffersLock.lock();

        if (data != null && m_fetchEachFrame)
            notifyFrameFetched(m_cameraId, data);

        if (data == m_cameraPreviewFirstBuffer)
            m_actualPreviewBuffer = 1;
        else if (data == m_cameraPreviewSecondBuffer)
            m_actualPreviewBuffer = 2;
        else
            m_actualPreviewBuffer = 0;
        addCallbackBuffer();
        m_buffersLock.unlock();
    }

    @Override
    public void onAutoFocus(boolean success, Camera camera)
    {
        notifyAutoFocusComplete(m_cameraId, success);
    }

    // --------------------------------------------------------------
    // Notification

    private static native void notifyAutoFocusComplete(int id, boolean success);

    private static native void notifyPictureExposed(int id);

    private static native void notifyPictureCaptured(int id, byte[] data);

    public static native void notifyFrameFetched(int id, byte[] data);

    // --------------------------------------------------------------

    // / ADDED PART FOR FW4SPL ///
    private static SurfaceTexture g_surfaceTexture = null;

    public static void createGlobalSurfaceTexture(int texName)
    {
        Log.d(TAG, "createGlobalSurfaceTexture");
        g_surfaceTexture = new SurfaceTexture(texName);
    }

    public void setPreviewTextureFromGlobal()
    {
        Log.d(TAG, "setPreviewTextureFromGlobal");
        setPreviewTexture(g_surfaceTexture);
    }

    public int getPreviewFormat()
    {
        return m_camera.getParameters().getPreviewFormat();
    }

    public void setAutoFocus(boolean autofocus)
    {
        m_autoFocus = autofocus;
    }

    public void setPreviewWidth(int width)
    {
        m_width = width;
    }

    public void setPreviewHeight(int height)
    {
        m_height = height;
    }

    public void setPreviewFrameRate(int frameRate)
    {
        m_frameRate = frameRate;
    }

    public int getPreviewWidth()
    {
        return m_camera.getParameters().getPreviewSize().width;
    }

    public int getPreviewHeight()
    {
        return m_camera.getParameters().getPreviewSize().height;
    }

    public int getPreviewFrameRate()
    {
        return m_frameRate;
    }

    // --------------------------------------------------------------

    // Example taken from
    // https://github.com/commonsguy/cw-advandroid/tree/master/Camera/Picture
    private static Camera.Size getBestPreviewSize(int width, int height, Camera.Parameters parameters)
    {
        Camera.Size result = null;

        for (Camera.Size size : parameters.getSupportedPreviewSizes())
        {
            Log.v(TAG, "current possible size: " + " (" + size.width + " x "

            + size.height + ")");
            if (size.width <= width && size.height <= height)
            {
                if (result == null)
                {
                    result = size;
                }
                else
                {
                    int resultArea = result.width * result.height;
                    int newArea = size.width * size.height;

                    if (newArea > resultArea)
                    {
                        result = size;
                    }
                }
            }
        }

        Log.v(TAG, "Final size: " + " (" + result.width + " x "

        + result.height + ")");

        return (result);
    }

    private static Camera.Size getSmallestPictureSize(Camera.Parameters parameters)
    {
        Camera.Size result = null;

        for (Camera.Size size : parameters.getSupportedPictureSizes())
        {
            if (result == null)
            {
                result = size;
            }
            else
            {
                int resultArea = result.width * result.height;
                int newArea = size.width * size.height;

                if (newArea < resultArea)
                {
                    result = size;
                }
            }
        }

        return (result);
    }

    private static int[] getBestFrameRate(int frameRate, Camera.Parameters parameters)
    {
        List<int[]> supportedFpsRanges = parameters.getSupportedPreviewFpsRange();
        int[] closestRange = supportedFpsRanges.get(0);
        for (int[] range : supportedFpsRanges)
        {
            Log.v(TAG, "current possible fps: " + "frame rate range: ["
                    + range[Camera.Parameters.PREVIEW_FPS_MIN_INDEX] / 1000. + ","
                    + range[Camera.Parameters.PREVIEW_FPS_MAX_INDEX] / 1000. + "]");
            if (range[Camera.Parameters.PREVIEW_FPS_MIN_INDEX] <= frameRate * 1000.
                    && range[Camera.Parameters.PREVIEW_FPS_MAX_INDEX] >= frameRate * 1000.)
            {
                closestRange = range;
            }
        }
        // if (mLogVerbose)
        Log.v(TAG, "Requested fps: " + frameRate + ".Closest frame rate range: ["
                + closestRange[Camera.Parameters.PREVIEW_FPS_MIN_INDEX] / 1000. + ","
                + closestRange[Camera.Parameters.PREVIEW_FPS_MAX_INDEX] / 1000. + "]");
        return closestRange;
    }

}
