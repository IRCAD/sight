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

#include <fwRenderOgre/config.hpp>
#include <fwRenderOgre/factory/new.hpp>
#include <fwRenderOgre/registry/detail.hpp>

#include <fwCom/HasSignals.hpp>
#include <fwCom/Signal.hpp>
#include <fwCom/Signals.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwDataTools/PickingInfo.hpp>

#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreVector3.h>

namespace fwRenderOgre
{

class Layer;

namespace interactor
{

/**
 * @brief Interface implementation for all the interaction with the mouse
 */
class FWRENDEROGRE_CLASS_API IInteractor
{

public:
    typedef std::weak_ptr< IInteractor > wptr;
    typedef std::shared_ptr< IInteractor > sptr;

    /// List of recognized mouse buttons.
    enum MouseButton
    {
        NONE,
        LEFT,
        MIDDLE,
        RIGHT,
        UNKNOWN
    };

    enum class Modifier : std::uint8_t
    {
        NONE     = 0x0,
        SHIFT    = 0x1,
        CONTROL  = SHIFT << 1,
            META = SHIFT << 2,
            ALT  = SHIFT << 3
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
    typedef ::fwCom::Signal< void () > RenderRequestedSigType;
    [[deprecated("Removed in sight 21.0")]]
    FWRENDEROGRE_API static const ::fwCom::Signals::SignalKeyType s_RENDER_REQUESTED_SIG;
    /** @} */

    /// Constructor. Sets the scene manager from the layer if it exists.
    FWRENDEROGRE_API IInteractor(SPTR(Layer)_layer = nullptr);

    /// Destructor
    FWRENDEROGRE_API virtual ~IInteractor();

    /// Change point of interest viewed by the camera
    [[deprecated("Set the scene from the layer instead.")]]
    FWRENDEROGRE_API void setSceneID(const std::string&);

    /// Sets the scene's length, that is the length of the scene's bounding cube.
    FWRENDEROGRE_API virtual void setSceneLength(float _sceneLength);

    /**
     * @brief Listens to mouse move events when one of the mouse buttons is pressed.
     * @param _button currently active mouse button.
     * @param _mods keyboard modifiers.
     * @param _x width coordinate of the mouse.
     * @param _y height coordinate of the mouse.
     * @param _dx width displacement of the mouse since the last event.
     * @param _dx height displacement of the mouse since the last event.
     */
    FWRENDEROGRE_API virtual void mouseMoveEvent(MouseButton _button, Modifier _mods, int _x, int _y, int _dx, int _dy);

    /**
     * @brief Listens to the mouse's wheel being spun.
     * @param _mods keyboard modifiers.
     * @param _angleDelta distance that the wheel is rotated, in eighths of a degree.
     *                    See @ref https://doc.qt.io/qt-5/qwheelevent.html#angleDelta
     * @param _x width coordinate of the mouse.
     * @param _y height coordinate of the mouse.
     */
    FWRENDEROGRE_API virtual void wheelEvent(Modifier _mods, int _angleDelta, int _x, int _y);

    /**
     * @brief Listens to mouse move buttons being released.
     * @param _button released mouse button.
     * @param _mods keyboard modifiers.
     * @param _x width coordinate of the mouse.
     * @param _y height coordinate of the mouse.
     */
    FWRENDEROGRE_API virtual void buttonReleaseEvent(MouseButton _button, Modifier _mods, int _x, int _y);

    /**
     * @brief Listens to mouse move buttons being pressed.
     * @param _button pressed mouse button.
     * @param _mods keyboard modifiers.
     * @param _x width coordinate of the mouse.
     * @param _y height coordinate of the mouse.
     */
    FWRENDEROGRE_API virtual void buttonPressEvent(MouseButton _button, Modifier _mods, int _x, int _y);

    /**
     * @brief Listens to keyboard keys being pressed.
     * @param _key pressed key.
     * @param _mods keyboard modifiers.
     * @param _x width coordinate of the mouse at the time of the event.
     * @param _y height coordinate of the mouse at the time of the event.
     */
    FWRENDEROGRE_API virtual void keyPressEvent(int _key, Modifier _mods, int _mouseX, int _mouseY);

    /**
     * @brief Listens to keyboard keys being released.
     * @param _key pressed key.
     * @param _mods keyboard modifiers.
     * @param _x width coordinate of the mouse at the time of the event.
     * @param _y height coordinate of the mouse at the time of the event.
     */
    FWRENDEROGRE_API virtual void keyReleaseEvent(int _key, Modifier _mods, int _mouseX, int _mouseY);

    /**
     * @brief Checks if the cursor is inside a layer's viewport.
     * @param mouseX width coordinate of the mouse in pixels.
     * @param mouseY height coordinate of the mouse in pixels.
     * @param _layer layer on which to check the cursor's belonging.
     */
    FWRENDEROGRE_API static bool isInLayer(int _mouseX, int _mouseY, SPTR(Layer) _layer);

    /**
     * @brief Listen to render window resize events.
     * @param _width the render window's new width.
     * @param _height the render window's new height.
     */
    FWRENDEROGRE_API virtual void resizeEvent(int _width, int _height);

    /** @brief Legacy API, use the equivalent with keyboard modifiers.
     *
     * @deprecated removed in sight 21.0
     * @{
     */
    [[deprecated("Use the equivalent method with keyboard modifiers.")]]
    FWRENDEROGRE_API virtual void mouseMoveEvent(MouseButton, int, int, int, int);
    [[deprecated("Use the equivalent method with keyboard modifiers.")]]
    FWRENDEROGRE_API virtual void wheelEvent(int, int, int);
    [[deprecated("Use the equivalent method with keyboard modifiers.")]]
    FWRENDEROGRE_API virtual void keyPressEvent(int);
    [[deprecated("Use the equivalent method with keyboard modifiers.")]]
    FWRENDEROGRE_API virtual void keyReleaseEvent(int);
    [[deprecated("Use the equivalent method with keyboard modifiers.")]]
    FWRENDEROGRE_API virtual void buttonReleaseEvent(MouseButton, int, int);
    [[deprecated("Use the equivalent method with keyboard modifiers.")]]
    FWRENDEROGRE_API virtual void buttonPressEvent(MouseButton, int, int);
    [[deprecated("Removed in sight 21.0.")]]
    FWRENDEROGRE_API virtual void focusInEvent();
    [[deprecated("Removed in sight 21.0.")]]
    FWRENDEROGRE_API virtual void focusOutEvent();
    /**
     * @}
     */

protected:

    /// Ogre Root
    [[deprecated("Removed in sight 21.0")]]
    ::Ogre::Root* m_ogreRoot { nullptr };

    /// Current scene manager
    [[deprecated("Retrieve the scene manager from the layer instead. Removed in sight 21.0")]]
    ::Ogre::SceneManager* m_sceneManager { nullptr };

    /// Weak reference to the layer on which the interactor interacts.
    WPTR(Layer) m_layer;

    /**
     * @name Signals attributes
     * @{
     */
    /// Signal triggered when a render is requested
    [[deprecated("Call the render request on the Layer directly.")]]
    RenderRequestedSigType::sptr m_sigRenderRequested;
    /**
     * @}
     */

private:

    /// Checks if the cursor is on top of the given viewport.
    static bool isInViewport(int _mouseX, int _mouseY, const ::Ogre::Viewport* const _vp);

};

//------------------------------------------------------------------------------

template<typename INT_TYPE>
static inline bool operator==(INT_TYPE _i, IInteractor::Modifier _m)
{
    static_assert(std::is_integral<INT_TYPE>::value, "Integral type required.");
    return static_cast<INT_TYPE>(_m) == _i;
}

//------------------------------------------------------------------------------

template<typename INT_TYPE>
static inline bool operator==(IInteractor::Modifier _m, INT_TYPE _i)
{
    static_assert(std::is_integral<INT_TYPE>::value, "Integral type required.");
    return _i == static_cast<INT_TYPE>(_m);
}

//------------------------------------------------------------------------------

static inline IInteractor::Modifier operator&(IInteractor::Modifier _m1, IInteractor::Modifier _m2)
{
    return static_cast<IInteractor::Modifier>(
        static_cast<std::underlying_type<IInteractor::Modifier>::type>(_m1) &
        static_cast<std::underlying_type<IInteractor::Modifier>::type>(_m2)
        );
}

//------------------------------------------------------------------------------

static inline IInteractor::Modifier operator|(IInteractor::Modifier _m1, IInteractor::Modifier _m2)
{
    return static_cast<IInteractor::Modifier>(
        static_cast<std::underlying_type<IInteractor::Modifier>::type>(_m1) |
        static_cast<std::underlying_type<IInteractor::Modifier>::type>(_m2)
        );
}

//------------------------------------------------------------------------------

static inline IInteractor::Modifier operator|=(IInteractor::Modifier& _m1, IInteractor::Modifier _m2)
{
    return _m1 = _m1 | _m2;
}

} // interactor
} // fwRenderOgre
