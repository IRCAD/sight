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

#include "fwVTKQml/vtkInternalOpenGLRenderWindow.hpp"

#include "fwVTKQml/FrameBufferItem.hpp"

namespace fwVTKQml
{

vtkInternalOpenGLRenderWindow::vtkInternalOpenGLRenderWindow() :
    m_fbItem(nullptr)
{
}

vtkInternalOpenGLRenderWindow::~vtkInternalOpenGLRenderWindow()
{
    this->OffScreenRendering = 0;
}

//------------------------------------------------------------------------------

void vtkInternalOpenGLRenderWindow::OpenGLInitState()
{
    this->MakeCurrent();
    Superclass::OpenGLInitState();
}

//------------------------------------------------------------------------------

void vtkInternalOpenGLRenderWindow::OpenGLEndState()
{
}

//------------------------------------------------------------------------------

void vtkInternalOpenGLRenderWindow::internalRender()
{
    Superclass::Render();
}

//------------------------------------------------------------------------------

void vtkInternalOpenGLRenderWindow::Render()
{
    if (m_fbItem)
    {
        m_fbItem->update();
    }
}

//------------------------------------------------------------------------------

void vtkInternalOpenGLRenderWindow::setFrameBufferItem(FrameBufferItem* fbItem)
{
    m_fbItem = fbItem;
}

//------------------------------------------------------------------------------

void vtkInternalOpenGLRenderWindow::setFrameBufferObject(QOpenGLFramebufferObject* fbo)
{
    this->SetFrontBuffer(GL_COLOR_ATTACHMENT0);
    this->SetFrontRightBuffer(GL_COLOR_ATTACHMENT0);
    this->SetBackLeftBuffer(GL_COLOR_ATTACHMENT0);
    this->SetBackRightBuffer(GL_COLOR_ATTACHMENT0);

    auto size = fbo->size();

    this->Size[0]                 = size.width();
    this->Size[1]                 = size.height();
    this->NumberOfFrameBuffers    = 1;
    this->FrameBufferObject       = static_cast<unsigned int>(fbo->handle());
    this->DepthRenderBufferObject = 0;
    this->TextureObjects[0]       = static_cast<unsigned int>(fbo->texture());
    this->OffScreenRendering      = true;
    this->OffScreenUseFrameBuffer = true;
    this->Modified();
}

//------------------------------------------------------------------------------

vtkInternalOpenGLRenderWindow* vtkInternalOpenGLRenderWindow::New()
{
    return new vtkInternalOpenGLRenderWindow;
}

}
