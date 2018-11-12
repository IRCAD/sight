/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwVTKQml/config.hpp"

#include <QOpenGLFramebufferObject>
#include <vtkExternalOpenGLRenderWindow.h>

namespace fwVTKQml
{

class FrameBufferItem;

/**
 * @brief This class is used to render a offscreen VTK window in a FrameBuffer
 */
class FWVTKQML_CLASS_API vtkInternalOpenGLRenderWindow : public vtkExternalOpenGLRenderWindow
{
public:
    friend class FrameBufferRenderer;

    static vtkInternalOpenGLRenderWindow* New();
    vtkTypeMacro(vtkInternalOpenGLRenderWindow, vtkExternalOpenGLRenderWindow);

    /**
     *  @brief: initialize openGL context/functions
     */
    FWVTKQML_API virtual void OpenGLInitState() override;

    FWVTKQML_API virtual void OpenGLEndState();

    /// Call render method of the renderer
    FWVTKQML_API virtual void Render() override;

    /// Render into the framebuffer
    FWVTKQML_API void  internalRender();

    /// Set frame buffer to the window
    FWVTKQML_API void setFrameBufferObject(QOpenGLFramebufferObject*);

    /// Set the renderer
    FWVTKQML_API void setFrameBufferItem(FrameBufferItem*);

protected:
    /// Constructor
    FWVTKQML_API vtkInternalOpenGLRenderWindow();
    /// Destructor
    FWVTKQML_API ~vtkInternalOpenGLRenderWindow();

private:

    /// Frame renderer
    FrameBufferItem* m_fbItem;
};

} //namespace fwVTKQml
