/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDEROGRE_INTERACTOR_IINTERACTOR_HPP__
#define __FWRENDEROGRE_INTERACTOR_IINTERACTOR_HPP__

#include <fwRenderOgre/factory/new.hpp>
#include <fwRenderOgre/registry/detail.hpp>
#include <fwRenderOgre/config.hpp>

#include <fwCom/Signal.hpp>
#include <fwCom/Signals.hpp>
#include <fwCom/Slots.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/HasSignals.hpp>

#include <fwCore/BaseObject.hpp>

#include <fwData/Object.hpp>

#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreVector3.h>

namespace fwRenderOgre
{

namespace interactor
{

/**
 * @class IInteractor
 * @brief Interface implementation for all the interaction with the mouse
 */
class FWRENDEROGRE_CLASS_API IInteractor : public ::fwCore::BaseObject,
                                           public ::fwCom::HasSignals
{

public:

    /// List of recognized mouse buttons.
    enum MouseButton
    {
        NONE,
        LEFT,
        MIDDLE,
        RIGHT,
        UNKNOWN
    };

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
            ::fwRenderOgre::registry::getInteractorRegistry()->addFactory(functorKey,
                                                                          &::fwRenderOgre::interactorFactory::New<T>);
        }
    };

    /**
     * @name Signals API
     * @{
     */

    typedef ::fwCom::Signal< void ( ::fwData::Object::sptr ) > PointClickedSigType;
    FWRENDEROGRE_API static const ::fwCom::Signals::SignalKeyType s_POINT_CLICKED_SIG;

    /** @} */

    fwCoreNonInstanciableClassDefinitionsMacro( (IInteractor)(::fwCore::BaseObject) )

    /// Constructor.
    /// Retrieves the Ogre root and the \<sceneID\> scene manager
    FWRENDEROGRE_API IInteractor();
    /// Destructor
    FWRENDEROGRE_API virtual ~IInteractor();

    /// Change point of interest viewed by the camera
    FWRENDEROGRE_API void setSceneID(const std::string& sceneID);

protected:

    /// Ogre Root
    ::Ogre::Root* m_ogreRoot;

    /// Current scene manager
    ::Ogre::SceneManager* m_sceneManager;

    /// Mouse's current X position
    int m_mouseX;
    /// Mouse's current Y position
    int m_mouseY;

    /**
     * @name Signals attributes
     * @{
     */

    ///Signal trigerred when action has been trigerred
    PointClickedSigType::sptr m_sigPointClicked;

    /**
     * @}
     */
};
} // interactor
} // fwRenderOgre
#endif // __FWRENDEROGRE_INTERACTOR_IINTERACTOR_HPP__
