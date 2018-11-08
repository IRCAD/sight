/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwVTKQml/FrameBufferItem.hpp"

#include "fwVTKQml/vtkInternalOpenGLRenderWindow.hpp"

#include <fwCore/spyLog.hpp>

#include <QQuickWindow>
#include <QVTKInteractor.h>

namespace fwVTKQml
{

FrameBufferRenderer::FrameBufferRenderer(vtkInternalOpenGLRenderWindow* rw, FrameBufferItem* item) :
    m_vtkRenderWindow(rw),
    m_item(item),
    m_readyToRender(false)
{
    m_vtkRenderWindow->Register(NULL);
    m_vtkRenderWindow->OpenGLInitContext();
}

//-----------------------------------------------------------------------------

FrameBufferRenderer::~FrameBufferRenderer()
{
    m_vtkRenderWindow->Delete();
}

//-----------------------------------------------------------------------------

QOpenGLFramebufferObject* FrameBufferRenderer::createFramebufferObject(const QSize& size)
{
    this->initializeOpenGLFunctions();

    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setTextureTarget(GL_TEXTURE_2D);
    format.setInternalTextureFormat(GL_RGBA);
    format.setSamples(0);

    // No need to delete this pointer, its managed by Qt
    auto framebufferObject = new QOpenGLFramebufferObject(size, format);
    m_vtkRenderWindow->setFrameBufferObject(framebufferObject);
    m_item->ready();
    return framebufferObject;
}

//------------------------------------------------------------------------------------------

void FrameBufferRenderer::render()
{
    if (!m_readyToRender)
    {
        return;
    }

    m_item->lockRenderer();
    m_vtkRenderWindow->Start();
    m_vtkRenderWindow->internalRender(); // vtkXOpenGLRenderWindow renders the scene to the FBO
    m_item->window()->resetOpenGLState();
    this->glDrawBuffer(GL_BACK);
    m_item->unlockRenderer();
}

//-----------------------------------------------------------------------------

void FrameBufferRenderer::synchronize(QQuickFramebufferObject* item)
{
    FrameBufferItem* vtkItem = static_cast<FrameBufferItem*>(item);
    vtkItem->initialize();
    m_readyToRender = true;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

FrameBufferItem::FrameBufferItem(QQuickItem* parent) :
    QQuickFramebufferObject(parent),
    m_win(nullptr),
    m_interactorAdapter(nullptr)
{
    this->setMirrorVertically(true);
    this->setAcceptedMouseButtons(Qt::AllButtons);
    m_win = vtkSmartPointer<vtkInternalOpenGLRenderWindow>::New();
    m_win->SetSize(static_cast<int>(this->width()), static_cast<int>(this->height()));
    m_win->setFrameBufferItem(this);

    m_interactor = vtkSmartPointer<QVTKInteractor>::New();
    m_win->SetInteractor(m_interactor);
    m_interactorAdapter = new QVTKInteractorAdapter(this);
}

//-----------------------------------------------------------------------------

FrameBufferItem::~FrameBufferItem()
{

}

//-----------------------------------------------------------------------------

vtkInternalOpenGLRenderWindow* FrameBufferItem::getRenderWindow() const
{
    return m_win;
}

//-----------------------------------------------------------------------------

QQuickFramebufferObject::Renderer* FrameBufferItem::createRenderer() const
{
    auto renderer =
        new FrameBufferRenderer(static_cast<vtkInternalOpenGLRenderWindow*>(m_win), const_cast<FrameBufferItem*>(this));
    connect(renderer, SIGNAL(ready()), this, SIGNAL(ready()));
    m_interactor->Initialize();
    return renderer;
}

//-----------------------------------------------------------------------------

void FrameBufferItem::initialize()
{
    m_win->SetSize(static_cast<int>(this->width()), static_cast<int>(this->height()));
    m_interactor->SetSize(m_win->GetSize());
}

//-----------------------------------------------------------------------------

bool FrameBufferItem::event(QEvent* evt)
{
    switch (evt->type())
    {
        case QEvent::MouseMove:
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseButtonDblClick:
            break;
        case QEvent::Resize:
            break;

        default:
            if (this->m_win && this->m_win->GetInteractor())
            {
                lockRenderer();
                this->m_interactorAdapter->ProcessEvent(evt, this->m_win->GetInteractor());
                unlockRenderer();
                if (evt->isAccepted())
                {
                    return true;
                }
            }
    }
    return QQuickFramebufferObject::event(evt);
}

//-----------------------------------------------------------------------------

void FrameBufferItem::mousePressEvent(QMouseEvent* event)
{
    if (m_win && m_win->GetInteractor())
    {
        this->lockRenderer();
        m_interactorAdapter->ProcessEvent(event, m_win->GetInteractor());
        this->unlockRenderer();
    }
}

//-----------------------------------------------------------------------------

void FrameBufferItem::mouseMoveEvent(QMouseEvent* event)
{
    if (m_win && m_win->GetInteractor())
    {
        this->lockRenderer();
        m_interactorAdapter->ProcessEvent(event, m_win->GetInteractor());
        this->unlockRenderer();
    }
}

//-----------------------------------------------------------------------------

void FrameBufferItem::mouseReleaseEvent(QMouseEvent* event)
{
    if (m_win && m_win->GetInteractor())
    {
        this->lockRenderer();
        m_interactorAdapter->ProcessEvent(event, m_win->GetInteractor());
        this->unlockRenderer();
    }
}

//-----------------------------------------------------------------------------

void FrameBufferItem::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (m_win && m_win->GetInteractor())
    {
        this->lockRenderer();
        m_interactorAdapter->ProcessEvent(event, m_win->GetInteractor());
        this->unlockRenderer();
    }
}

//------------------------------------------------------------------------------

void FrameBufferItem::lockRenderer()
{
    m_viewLock.lock();
}

//------------------------------------------------------------------------------

void FrameBufferItem::unlockRenderer()
{
    m_viewLock.unlock();
}

} // fwVTKQml
