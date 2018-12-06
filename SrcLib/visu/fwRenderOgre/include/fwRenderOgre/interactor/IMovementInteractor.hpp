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

#include <fwRenderOgre/config.hpp>
#include <fwRenderOgre/factory/new.hpp>
#include <fwRenderOgre/interactor/IInteractor.hpp>
#include <fwRenderOgre/registry/detail.hpp>

#include <fwCore/BaseObject.hpp>

#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreVector3.h>

namespace fwRenderOgre
{

namespace interactor
{

/**
 * @brief Interface implementation for all movement interaction with the mouse
 */
class FWRENDEROGRE_CLASS_API IMovementInteractor : public ::fwRenderOgre::interactor::IInteractor,
                                                   public ::fwCore::BaseObject,
                                                   public ::fwCom::HasSignals
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

    /// Constructor. Retrieves the Ogre root and the \<sceneID\> scene manager
    FWRENDEROGRE_API IMovementInteractor();

    /// Destructor
    FWRENDEROGRE_API virtual ~IMovementInteractor();

    /// Update camera's move scale.
    FWRENDEROGRE_API virtual void setMouseScale(float);

    /// Get the distance to the point of interest.
    FWRENDEROGRE_API float getLookAtZ() const;

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
