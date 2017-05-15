/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDEROGRE_INTERACTOR_IPICKERINTERACTOR_HPP__
#define __FWRENDEROGRE_INTERACTOR_IPICKERINTERACTOR_HPP__

#include <fwRenderOgre/config.hpp>
#include <fwRenderOgre/factory/new.hpp>
#include <fwRenderOgre/interactor/IInteractor.hpp>
#include <fwRenderOgre/picker/IPicker.hpp>
#include <fwRenderOgre/registry/detail.hpp>

#include <fwCore/BaseObject.hpp>

#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreVector3.h>

namespace fwRenderOgre
{

namespace interactor
{

/**
 * @brief Interface implementation for all selection with the mouse
 */
class FWRENDEROGRE_CLASS_API IPickerInteractor : public ::fwRenderOgre::interactor::IInteractor
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

    fwCoreNonInstanciableClassDefinitionsMacro( (IPickerInteractor)(::fwCore::BaseObject) )

    /// Constructor.
    /// Retrieves the Ogre root and the \<sceneID\> scene manager
    FWRENDEROGRE_API IPickerInteractor();
    /// Destructor
    FWRENDEROGRE_API virtual ~IPickerInteractor();

    FWRENDEROGRE_API void virtual mouseClickEvent(int x, int y, int width, int height) = 0;

    /**
     * @brief Initilaize picker with the corresponding layer's info
     *        This method should only be called by a layer that contains this interactor
     */
    FWRENDEROGRE_API void  initPicker();

    FWRENDEROGRE_API bool  isPickerInitialized();

protected:

    /// Ogre picker
    ::fwRenderOgre::picker::IPicker* m_picker;

    bool m_isPickerInitialized;

};

}
}
#endif // __FWRENDEROGRE_INTERACTOR_IPICKERINTERACTOR_HPP__
