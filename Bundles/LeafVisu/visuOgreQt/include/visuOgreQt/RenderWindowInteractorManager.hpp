/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUOGREQT_RENDERWINDOWINTERACTORMANAGER_HPP__
#define __VISUOGREQT_RENDERWINDOWINTERACTORMANAGER_HPP__

#include "visuOgreQt/config.hpp"
#include <visuOgreQt/Window.hpp>

#include <fwCore/base.hpp>

#include <fwGui/container/fwContainer.hpp>

#include <fwRenderOgre/IRenderWindowInteractorManager.hpp>

#include <QObject>
#include <QPointer>

namespace fwGuiQt
{
namespace container
{
class QtContainer;
}
}

namespace visuOgreQt
{

/**
 * @brief   Defines a class to manage ogreRenderWindowInteractor in a window.
 */
class VISUOGREQT_CLASS_API RenderWindowInteractorManager : public QObject,
                                                           public ::fwRenderOgre::IRenderWindowInteractorManager
{
Q_OBJECT

public:

    fwCoreNonInstanciableClassDefinitionsMacro( (RenderWindowInteractorManager)
                                                (::fwRenderOgre::IRenderWindowInteractorManager) )

    /// Constructor
    VISUOGREQT_API RenderWindowInteractorManager(::fwRenderOgre::IRenderWindowInteractorManager::Key key);
    /// Destructor
    VISUOGREQT_API virtual ~RenderWindowInteractorManager();

    /// Call Widget render
    VISUOGREQT_API virtual void requestRender();

    /// Create the container that holds the QtWidget.
    VISUOGREQT_API virtual void createContainer(::fwGui::container::fwContainer::sptr _parent, bool showOverlay,
                                                bool renderOnDemand, bool fullscreen);

    /// Connects widget and SRender signals and slots.
    VISUOGREQT_API virtual void connectToContainer();

    /// Not implemented yet
    /// Deletes interactor and manages correctly the window (removing layout).
    VISUOGREQT_API virtual void disconnectInteractor();

    /// Returns Ogre widget
    VISUOGREQT_API virtual int getWidgetId() const;

    /// Returns frame ID
    VISUOGREQT_API virtual int getFrameId() const;

    /// Set this render service as the current OpenGL context
    VISUOGREQT_API virtual void makeCurrent();

    /// Get Ogre RenderWindow
    VISUOGREQT_API virtual ::Ogre::RenderWindow* getRenderWindow();

private Q_SLOTS:

    /// When the render window is created
    void onRenderWindowCreated();

    /// When the render window is created
    void onInteracted(::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo _info);

    /// When the clipping range has to match the last updating of the scene bounding box
    void onCameraClippingComputation();

    /// When a ray cast request is emitted
    void onRayCastRequested(int, int, int, int);

private:

    /// Pointers to the Qt element of the Widget
    QPointer< ::visuOgreQt::Window > m_qOgreWidget;

    SPTR(::fwGuiQt::container::QtContainer) m_parentContainer;
};

} // namespace visuOgreQt

#endif // __VISUOGREQT_RENDERWINDOWINTERACTORMANAGER_HPP__
