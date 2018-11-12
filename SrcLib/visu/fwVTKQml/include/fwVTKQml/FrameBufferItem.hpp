/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwVTKQml/config.hpp"
#include "fwVTKQml/vtkInternalOpenGLRenderWindow.hpp"

#include <QMutex>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFunctions_3_1>
#include <QPointer>
#include <QQuickFramebufferObject>
#include <QVTKInteractorAdapter.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>

namespace fwVTKQml
{

class FrameBufferItem;

/**
 * @brief FrameBuffer renderer, it is used to render the VTK scene into a qml interface.
 *
 * Use the OpenGL API abstraction from Qt instead of from VTK because vtkgl.h
 * and other Qt OpenGL-related headers do not play nice when included in the
 * same compilation unit
 */
class FWVTKQML_CLASS_API FrameBufferRenderer : public QObject,
                                               public QQuickFramebufferObject::Renderer,
                                               protected QOpenGLFunctions_3_1
{
Q_OBJECT
public:

    FWVTKQML_API FrameBufferRenderer(vtkInternalOpenGLRenderWindow*, FrameBufferItem*);
    FWVTKQML_API virtual ~FrameBufferRenderer();
    /**
     * @brief createFramebufferObject: initialize a framebuffer
     * @param size: size of the framebuffer
     * @return QOpenGLFrameBufferObject: View where will be perform openGL command (VTK)
     */
    FWVTKQML_API QOpenGLFramebufferObject* createFramebufferObject(const QSize& size);

    /**
     * @brief render: perform open GL command
     */
    FWVTKQML_API void render();

    /**
     * @brief: synchronize m_item when frame is ready
     */
    FWVTKQML_API virtual void synchronize(QQuickFramebufferObject*);

Q_SIGNALS:
    /**
     * @brief ready
     * Emit when frame is ready
     */
    void ready();

private:
    vtkInternalOpenGLRenderWindow* m_vtkRenderWindow;
    FrameBufferItem* m_item;
    bool m_readyToRender;

    friend class vtkInternalOpenGLRenderWindow;
};

/**
 * @brief qml FrameBuffer iter, it is used to render the VTK scene into a qml interface
 */
class FWVTKQML_CLASS_QT_API FrameBufferItem : public QQuickFramebufferObject
{
Q_OBJECT

public:
    FWVTKQML_QT_API FrameBufferItem(QQuickItem* parent = nullptr);
    FWVTKQML_QT_API virtual ~FrameBufferItem();

    /**
     *  @brief: create a FrameBufferRenderer instance
     */
    FWVTKQML_QT_API QQuickFramebufferObject::Renderer* createRenderer() const override;
    /**
     *  @brief: return vtk render window
     */
    FWVTKQML_QT_API vtkInternalOpenGLRenderWindow* getRenderWindow() const;

    /// Lock the renderer
    FWVTKQML_QT_API void    lockRenderer();

    /// Unlock the renderer
    FWVTKQML_QT_API void    unlockRenderer();

public Q_SLOTS:
    /**
     *  @brief: initialize variables
     */
    FWVTKQML_QT_API void initialize();

protected:
    /**
     * Event management
     * Theses events are transmit to VTK by QVTKAdapter
     * @{
     */
    FWVTKQML_QT_API bool event(QEvent* evt) Q_DECL_OVERRIDE;

    FWVTKQML_QT_API void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    FWVTKQML_QT_API void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    FWVTKQML_QT_API void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    FWVTKQML_QT_API void mouseDoubleClickEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    /**
     * }@
     */

Q_SIGNALS:
    /**
     * @brief ready: emit when FrameBufferItem is ready
     */
    void ready();

private:
    vtkSmartPointer<vtkInternalOpenGLRenderWindow> m_win;
    QPointer<QVTKInteractorAdapter> m_interactorAdapter;
    vtkSmartPointer<vtkRenderWindowInteractor> m_interactor;
    QMutex m_viewLock;
};

} // fwVTKQml
