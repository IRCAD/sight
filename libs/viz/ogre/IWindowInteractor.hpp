/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "viz/ogre/config.hpp"
#include "viz/ogre/factory/new.hpp"
#include "viz/ogre/IGraphicsWorker.hpp"
#include "viz/ogre/interactor/IInteractor.hpp"
#include "viz/ogre/registry/detail.hpp"

#include <core/base.hpp>

#include <services/IService.hpp>

#include <OGRE/OgreRenderWindow.h>
#include <OGRE/Overlay/OgreOverlay.h>

#include <ui/base/container/fwContainer.hpp>

#include <string>
#include <unordered_set>

namespace sight::viz::ogre
{

/**
 * @brief   Defines a class to manage ogreRenderWindowInteractor in a window.
 *
 */
class VIZ_OGRE_CLASS_API IWindowInteractor : public core::BaseObject
{

public:

    /// Inner class used to send informations about mouse events.
    class InteractionInfo
    {
    public:

        typedef enum InteractionEnum
        {
            MOUSEMOVE,
            WHEELMOVE,
            RESIZE,
            KEYPRESS,
            KEYRELEASE,
            BUTTONRELEASE,
            BUTTONPRESS,
            BUTTONDOUBLEPRESS
        } InteractionEnumType;

        /**
         * @brief
         */

        int x;
        int y;
        int dx;
        int dy;
        int delta;
        int key;
        interactor::IInteractor::MouseButton button;
        interactor::IInteractor::Modifier modifiers;
        InteractionEnumType interactionType;
    };

    typedef viz::ogre::factory::Key Key;

    /**
     * @brief Class used to register a class factory in factory registry.
     * This class defines also the object factory ( 'create' )
     *
     * @tparam T Factory product type
     */
    template <typename T>
    class Registrar
    {
    public:
        Registrar(std::string functorKey)
        {
            viz::ogre::registry::get()->addFactory(functorKey, &viz::ogre::factory::New<T>);
        }
    };

    using OffscreenMgrKey = viz::ogre::offscreenInteractorMgrFactory::Key;

    /**
     * @brief Class used to register an offscreen window interactor factory in the factory registry.
     *
     * @tparam T Factory product type
     */
    template <typename T>
    class OffscreenMgrRegistrar
    {
    public:
        OffscreenMgrRegistrar(std::string functorKey)
        {
            auto fact = [](std::pair<unsigned int, unsigned int> _dims) -> std::shared_ptr< T >
                        {
                            // Capture the factory inside a lambda to distinguish it from overloaded methods.
                            return viz::ogre::offscreenInteractorMgrFactory::New<T>( _dims );
                        };
            viz::ogre::registry::getOffscreenMgr()->addFactory(functorKey, fact);
        }
    };

    fwCoreClassMacro(IWindowInteractor, core::BaseObject)

    typedef std::string FactoryRegistryKeyType;

    VIZ_OGRE_API static const FactoryRegistryKeyType REGISTRY_KEY;

    VIZ_OGRE_API static const FactoryRegistryKeyType OFFSCREEN_REGISTRY_KEY;

    VIZ_OGRE_API static IWindowInteractor::sptr createManager();

    /**
     * @brief Creates an offscreen window using the factory.
     *
     * @param _width  width of underlying render texture.
     * @param _height height of underlying render texture.
     */
    VIZ_OGRE_API static IWindowInteractor::sptr createOffscreenManager(unsigned int _width,
                                                                       unsigned int _height);

    /// Constructor. Do nothing.
    VIZ_OGRE_API IWindowInteractor();

    /// Destructor. Do nothing.
    VIZ_OGRE_API virtual ~IWindowInteractor();

    /// Call Ogre Widget render immediately
    VIZ_OGRE_API virtual void renderNow() = 0;

    /// Call Ogre Widget render as soon as possible
    VIZ_OGRE_API virtual void requestRender() = 0;

    /// Creates an interactor and installs it in window.
    VIZ_OGRE_API virtual void createContainer( ui::base::container::fwContainer::sptr _parent,
                                               bool renderOnDemand, bool fullscreen ) = 0;

    /// Connects widget and SRender signals and slots.
    VIZ_OGRE_API virtual void connectToContainer() = 0;

    /// Deletes interactor and manage correctly the window (removing layout).
    VIZ_OGRE_API virtual void disconnectInteractor() = 0;

    /// Returns Ogre widget
    VIZ_OGRE_API virtual int getWidgetId() const = 0;

    /// Returns frame ID
    VIZ_OGRE_API virtual int getFrameId() const = 0;

    /// Sets the rendering context as being enabled against this window and on this thread.
    VIZ_OGRE_API virtual void makeCurrent() = 0;

    /// Get Ogre RenderWindow
    VIZ_OGRE_API virtual ::Ogre::RenderTarget* getRenderTarget() = 0;

    /// Returns the texture in which this window manager is rendering. Only implemented for offscreen windows.
    VIZ_OGRE_API virtual ::Ogre::TexturePtr getRenderTexture() = 0;

    /// Spawns a worker able to handle graphics resources in parallel.
    VIZ_OGRE_API virtual IGraphicsWorker* createGraphicsWorker() = 0;

    /// Gets the vertical logical DPI of the monitor on which the window is displayed.
    /// The logical DPI takes accessibility features and desktop zoom into account and is used for font rendering.
    VIZ_OGRE_API virtual float getLogicalDotsPerInch() const = 0;

    /**
     * @brief Sets the fullscreen or windowed rendering mode.
     *
     * @param _fullscreen whether to render in fullscreen mode. Use windowed mode otherwise.
     * @param _screenNumber index of the screen on which to render in fullscreen mode.
     */
    VIZ_OGRE_API virtual void setFullscreen(bool _fullscreen, int _screenNumber);

    /// Set the render service using the IOgreRenderWindowInteractor
    virtual void setRenderService(services::IService::sptr srv)
    {
        m_renderService = srv;
    }

protected:
    services::IService::wptr m_renderService;
};

} // namespace sight::viz::ogre
