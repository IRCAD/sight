/************************************************************************
 *
 * Copyright (C) 2014-2018 IRCAD France
 * Copyright (C) 2014-2018 IHU Strasbourg
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

    /// Call Widget render immediately
    VISUOGREQT_API virtual void renderNow() override;

    /// Call Widget render as soon as possible
    VISUOGREQT_API virtual void requestRender() override;

    /// Create the container that holds the QtWidget.
    VISUOGREQT_API virtual void createContainer(::fwGui::container::fwContainer::sptr _parent,
                                                bool renderOnDemand, bool fullscreen) override;

    /// Connects widget and SRender signals and slots.
    VISUOGREQT_API virtual void connectToContainer() override;

    /// Not implemented yet
    /// Deletes interactor and manages correctly the window (removing layout).
    VISUOGREQT_API virtual void disconnectInteractor() override;

    /// Returns Ogre widget
    VISUOGREQT_API virtual int getWidgetId() const override;

    /// Returns frame ID
    VISUOGREQT_API virtual int getFrameId() const override;

    /// Set this render service as the current OpenGL context
    VISUOGREQT_API virtual void makeCurrent() override;

    /// Get Ogre RenderWindow
    VISUOGREQT_API virtual ::Ogre::RenderTarget* getRenderTarget() override;

    /// Sets the list of overlays to be rendered in this window.
    VISUOGREQT_API virtual void setEnabledOverlays(
        const ::fwRenderOgre::IRenderWindowInteractorManager::OverlaySetType& enabledOverlays) override;

    VISUOGREQT_API virtual void spawnGfxWorker(std::function<void (void)>& _f) const final;

private Q_SLOTS:

    /// When the render window is created
    void onInteracted(::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo _info);

    /// When the clipping range has to match the last updating of the scene bounding box
    void onCameraClippingComputation();

private:

    /// Pointers to the Qt element of the Widget
    QPointer< ::visuOgreQt::Window > m_qOgreWidget;

    SPTR(::fwGuiQt::container::QtContainer) m_parentContainer;
};

} // namespace visuOgreQt
