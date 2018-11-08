/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
