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
#include <fwRenderOgre/picker/IPicker.hpp>
#include <fwRenderOgre/registry/detail.hpp>

#include <fwCore/BaseObject.hpp>

#include <fwData/Object.hpp>

#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreVector3.h>

namespace fwRenderOgre
{

namespace interactor
{

/**
 * @brief Interface implementation for all selection with the mouse
 */
class FWRENDEROGRE_CLASS_API IPickerInteractor : public ::fwRenderOgre::interactor::IInteractor,
                                                 public ::fwCore::BaseObject,
                                                 public ::fwCom::HasSignals
{

public:

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
    FWRENDEROGRE_API static const ::fwCom::Signals::SignalKeyType s_ADD_POINT_SIG;
    FWRENDEROGRE_API static const ::fwCom::Signals::SignalKeyType s_REMOVE_POINT_SIG;
    /** @} */

    fwCoreNonInstanciableClassDefinitionsMacro( (IPickerInteractor)(::fwCore::BaseObject) )

    /// Constructor. Retrieves the Ogre root and the \<sceneID\> scene manager
    FWRENDEROGRE_API IPickerInteractor();

    /// Destructor
    FWRENDEROGRE_API virtual ~IPickerInteractor();

    /// Initialize picker with the corresponding layer's info
    FWRENDEROGRE_API void initPicker();

    /// Return true if the picker is initialized
    FWRENDEROGRE_API bool isPickerInitialized() const;

    /// Set query mask
    FWRENDEROGRE_API void setQueryFlags(std::uint32_t _queryFlags);

protected:

    /// Ogre picker
    ::fwRenderOgre::picker::IPicker* m_picker;

    /// Mask for picking requests
    std::uint32_t m_queryFlags {0};

    /**
     * @name Signals attributes
     * @{
     */
    /// Signal triggered when an action has been triggered
    PointClickedSigType::sptr m_sigAddPoint;

    /// Signal triggered when an action has been triggered
    PointClickedSigType::sptr m_sigRemovePoint;

    /// Signal triggered when a render is requested
    RenderRequestedSigType::sptr m_sigRenderRequested;
    /**
     * @}
     */
};

}
}
