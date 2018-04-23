/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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

    fwCoreNonInstanciableClassDefinitionsMacro( (IRenderWindowInteractorManager)(::fwCore::BaseObject) )

    typedef std::string FactoryRegistryKeyType;

    typedef std::unordered_set< ::Ogre::Overlay* > OverlaySetType;

    FWRENDEROGRE_API static const FactoryRegistryKeyType REGISTRY_KEY;

    FWRENDEROGRE_API static IRenderWindowInteractorManager::sptr createManager();

    /// Constructor. Do nothing.
    FWRENDEROGRE_API IRenderWindowInteractorManager();

    /// Destructor. Do nothing.
    FWRENDEROGRE_API virtual ~IRenderWindowInteractorManager();

    /// Call Ogre Widget render
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

    /// Set this render service as the current OpenGL context
    FWRENDEROGRE_API virtual void makeCurrent() = 0;

    /// Get Ogre RenderWindow
    FWRENDEROGRE_API virtual ::Ogre::RenderTarget* getRenderTarget() = 0;

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
