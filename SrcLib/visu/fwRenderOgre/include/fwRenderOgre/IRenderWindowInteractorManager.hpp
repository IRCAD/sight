/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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
#include "fwRenderOgre/factory/new.hpp"
#include "fwRenderOgre/interactor/IInteractor.hpp"
#include "fwRenderOgre/registry/detail.hpp"

#include <fwCore/base.hpp>

#include <fwGui/container/fwContainer.hpp>

#include <fwServices/IService.hpp>

#include <OGRE/OgreRenderWindow.h>
#include <OGRE/Overlay/OgreOverlay.h>

#include <string>
#include <unordered_set>

namespace fwRenderOgre
{

/**
 * @brief   Defines a class to manage ogreRenderWindowInteractor in a window.
 *
 */
class FWRENDEROGRE_CLASS_API IRenderWindowInteractorManager : public ::fwCore::BaseObject
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
            FOCUSIN,
            FOCUSOUT
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
        InteractionEnumType interactionType;
    };

    typedef ::fwRenderOgre::factory::Key Key;

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
            ::fwRenderOgre::registry::get()->addFactory(functorKey, &::fwRenderOgre::factory::New<T>);
        }
    };

    using OffscreenMgrKey = ::fwRenderOgre::offscreenInteractorMgrFactory::Key;

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
                            return ::fwRenderOgre::offscreenInteractorMgrFactory::New<T>( _dims );
                        };
            ::fwRenderOgre::registry::getOffscreenMgr()->addFactory(functorKey, fact);
        }
    };

    fwCoreClassMacro(IRenderWindowInteractorManager, ::fwCore::BaseObject);

    typedef std::string FactoryRegistryKeyType;

    typedef std::unordered_set< ::Ogre::Overlay* > OverlaySetType;

    FWRENDEROGRE_API static const FactoryRegistryKeyType REGISTRY_KEY;

    FWRENDEROGRE_API static const FactoryRegistryKeyType OFFSCREEN_REGISTRY_KEY;

    FWRENDEROGRE_API static IRenderWindowInteractorManager::sptr createManager();

    /**
     * @brief Creates an offscreen window using the factory.
     *
     * @param _width  width of underlying render texture.
     * @param _height height of underlying render texture.
     */
    FWRENDEROGRE_API static IRenderWindowInteractorManager::sptr createOffscreenManager(unsigned int _width,
                                                                                        unsigned int _height);

    /// Constructor. Do nothing.
    FWRENDEROGRE_API IRenderWindowInteractorManager();

    /// Destructor. Do nothing.
    FWRENDEROGRE_API virtual ~IRenderWindowInteractorManager();

    /// Call Ogre Widget render immediately
    FWRENDEROGRE_API virtual void renderNow() = 0;

    /// Call Ogre Widget render as soon as possible
    FWRENDEROGRE_API virtual void requestRender() = 0;

    /// Creates an interactor and installs it in window.
    FWRENDEROGRE_API virtual void createContainer( ::fwGui::container::fwContainer::sptr _parent,
                                                   bool renderOnDemand, bool fullscreen ) = 0;

    /// Connects widget and SRender signals and slots.
    FWRENDEROGRE_API virtual void connectToContainer() = 0;

    /// Deletes interactor and manage correctly the window (removing layout).
    FWRENDEROGRE_API virtual void disconnectInteractor() = 0;

    /// Returns Ogre widget
    FWRENDEROGRE_API virtual int getWidgetId() const = 0;

    /// Returns frame ID
    FWRENDEROGRE_API virtual int getFrameId() const = 0;

    /// Sets the rendering context as being enabled against this window and on this thread.
    FWRENDEROGRE_API virtual void makeCurrent() = 0;

    /// Get Ogre RenderWindow
    FWRENDEROGRE_API virtual ::Ogre::RenderTarget* getRenderTarget() = 0;

    /// Returns the texture in which this window manager is rendering. Only implemented for offscreen windows.
    FWRENDEROGRE_API virtual ::Ogre::TexturePtr getRenderTexture() = 0;

    FWRENDEROGRE_API virtual void setEnabledOverlays(const OverlaySetType& overlaySet);

    /// Set the render service using the IOgreRenderWindowInteractor
    virtual void setRenderService(::fwServices::IService::sptr srv)
    {
        m_renderService = srv;
    }

protected:
    ::fwServices::IService::wptr m_renderService;
};

} // namespace fwRenderOgre
