/************************************************************************
 *
 * Copyright (C) 2018 IRCAD France
 * Copyright (C) 2018 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

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
