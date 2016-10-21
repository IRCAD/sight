/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDEROGRE_INTERACTOR_IMOVEMENTINTERACTOR_HPP__
#define __FWRENDEROGRE_INTERACTOR_IMOVEMENTINTERACTOR_HPP__

#include <fwCore/BaseObject.hpp>

#include <fwRenderOgre/config.hpp>
#include <fwRenderOgre/factory/new.hpp>
#include <fwRenderOgre/interactor/IInteractor.hpp>
#include <fwRenderOgre/registry/detail.hpp>

#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreVector3.h>

namespace fwRenderOgre
{

namespace interactor
{

/**
 * @class IMovementInteractor
 * @brief Interface implementation for all movement interaction with the mouse
 */
class FWRENDEROGRE_CLASS_API IMovementInteractor : public ::fwRenderOgre::interactor::IInteractor
{

public:

    /**
     * @name Signals API
     * @{
     */
    typedef ::fwCom::Signal< void () > ResetCameraSignalType;
    FWRENDEROGRE_API static const ::fwCom::Signals::SignalKeyType s_RESET_CAMERA_SIG;
    /** @} */

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

    fwCoreNonInstanciableClassDefinitionsMacro( (IMovementInteractor)(::fwCore::BaseObject) )

    /// Constructor.
    /// Retrieves the Ogre root and the \<sceneID\> scene manager
    FWRENDEROGRE_API IMovementInteractor();
    /// Destructor
    FWRENDEROGRE_API virtual ~IMovementInteractor();

    /**
     * @brief Behaviour on a MouseMoveEvent
     * @param x  The mouse X position
     * @param y  The mouse Y position
     * @param dx The mouse X screen displacement
     * @param dy The mouse Y screen displacement
     * @param click Whether or not the left button has been clicked
     */
    FWRENDEROGRE_API virtual void mouseMoveEvent(MouseButton button, int x, int y, int dx, int dy) = 0;

    /**
     * @brief Behaviour on a WheelEvent
     * @param delta The mouse's wheel displacement
     * @param x The mouse X screen coordinate
     * @param y The mouse Y screen coordinate
     */
    FWRENDEROGRE_API virtual void wheelEvent(int delta, int x, int y) = 0;

    /**
     * @brief Called when the window is resized
     * @param x The width of the window
     * @param y The height of the window
     * */
    FWRENDEROGRE_API virtual void resizeEvent(int x, int y) = 0;

    /**
     * @brief Called when a key is pressed
     * @param key key pressed on the keyboard
     * */
    FWRENDEROGRE_API virtual void keyPressEvent(int key) = 0;

    /// Called when a mouse button is released.
    FWRENDEROGRE_API virtual void buttonReleaseEvent(MouseButton button, int x, int y) = 0;

    /// Called when a mouse button is pressed.
    FWRENDEROGRE_API virtual void buttonPressEvent(MouseButton button, int x, int y) = 0;

    /// Update camera's move scale.
    FWRENDEROGRE_API virtual void setMouseScale(float mouseScale);

protected:

    /// Current point of interest, needed especially when rotating
    float m_lookAtZ;

    /// Move scale
    float m_mouseScale;

    /// Current zoom coefficient
    float m_fZoom;

    /// Default mouse scale coefficient (used to move the camera)
    const int MOUSE_SCALE_COEFFICIENT = 200;
};

}
}
#endif // __FWRENDEROGRE_INTERACTOR_IMOVEMENTINTERACTOR_HPP__
