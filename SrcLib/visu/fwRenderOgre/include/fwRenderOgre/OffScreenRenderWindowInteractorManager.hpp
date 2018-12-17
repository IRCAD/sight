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

#include "fwRenderOgre/config.hpp"
#include "fwRenderOgre/IRenderWindowInteractorManager.hpp"

namespace fwRenderOgre
{

/**
 * @brief   Defines a class to manage ogreRenderWindowInteractor in a window.
 */
class FWRENDEROGRE_CLASS_API OffScreenRenderWindowInteractorManager :
    public ::fwRenderOgre::IRenderWindowInteractorManager
{
public:
    fwCoreClassDefinitionsWithFactoryMacro((OffScreenRenderWindowInteractorManager)
                                           (::fwRenderOgre::IRenderWindowInteractorManager),
                                           (((unsigned int))(
                                                (unsigned int))), new OffScreenRenderWindowInteractorManager)

    /// Constructor
    FWRENDEROGRE_API OffScreenRenderWindowInteractorManager(unsigned int, unsigned int);
    /// Destructor
    FWRENDEROGRE_API virtual ~OffScreenRenderWindowInteractorManager();

    /// Call Widget render immediately
    FWRENDEROGRE_API virtual void renderNow() override;

    /// Call Widget render
    FWRENDEROGRE_API virtual void requestRender() override;

    /// Create the container that holds the QtWidget.
    FWRENDEROGRE_API virtual void createContainer(::fwGui::container::fwContainer::sptr _parent,
                                                  bool renderOnDemand, bool fullscreen) override;

    /// Connects widget and SRender signals and slots.
    FWRENDEROGRE_API virtual void connectToContainer() override;

    /// Not implemented yet
    /// Deletes interactor and manages correctly the window (removing layout).
    FWRENDEROGRE_API virtual void disconnectInteractor() override;

    /// Returns Ogre widget
    FWRENDEROGRE_API virtual int getWidgetId() const override;

    /// Returns frame ID
    FWRENDEROGRE_API virtual int getFrameId() const override;

    /// Set this render service as the current OpenGL context
    FWRENDEROGRE_API virtual void makeCurrent() override;

    /// Get Ogre RenderWindow
    FWRENDEROGRE_API virtual ::Ogre::RenderTarget* getRenderTarget() override;

    /// Get the Ogre texture attached to the target
    FWRENDEROGRE_API ::Ogre::TexturePtr getRenderTexture() const;

private:
    void render();

    /// Needed to get the widget Id
    static int m_counter;

    /// Used to instantiate the managers related to this instance with a proper name.
    int m_id;

    /// Frame identifier
    int m_frameId { 0 };

    /// Ogre root
    ::Ogre::Root* m_ogreRoot;

    /// Ogre window
    ::Ogre::RenderWindow* m_ogreRenderWindow;

    /// Ogre window
    ::Ogre::RenderTarget* m_ogreRenderTarget;

    /// Ogre window
    ::Ogre::TexturePtr m_ogreTexture;

    /// Window width
    unsigned int m_width { 0 };

    /// Window height
    unsigned int m_height { 0 };

};

//-----------------------------------------------------------------------------

inline int OffScreenRenderWindowInteractorManager::getWidgetId() const
{
    return m_id;
}

//-----------------------------------------------------------------------------

inline int OffScreenRenderWindowInteractorManager::getFrameId() const
{
    return m_frameId;
}

//-----------------------------------------------------------------------------

inline ::Ogre::RenderTarget* OffScreenRenderWindowInteractorManager::getRenderTarget()
{
    return m_ogreRenderTarget;
}

//-----------------------------------------------------------------------------

inline ::Ogre::TexturePtr OffScreenRenderWindowInteractorManager::getRenderTexture() const
{
    return m_ogreTexture;
}

} // namespace fwRenderOgre
