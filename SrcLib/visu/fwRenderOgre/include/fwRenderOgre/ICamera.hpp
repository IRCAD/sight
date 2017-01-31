/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDEROGRE_ICAMERA_HPP__
#define __FWRENDEROGRE_ICAMERA_HPP__

#include "fwRenderOgre/config.hpp"
#include "fwRenderOgre/factory/new.hpp"
#include "fwRenderOgre/IAdaptor.hpp"
#include "fwRenderOgre/registry/detail.hpp"
#include "fwRenderOgre/SRender.hpp"

#include <fwGui/container/fwContainer.hpp>

#include <OGRE/OgreCamera.h>

#include <string>

//namespace Ogre
//{
//class Camera;
//}

namespace fwRenderOgre
{

/**
 * @brief Defines the service interface managing the camera adaptor.
 *
 */
class FWRENDEROGRE_CLASS_API ICamera : public ::fwRenderOgre::IAdaptor
{

public:

    typedef ::fwRenderOgre::cameraFactory::Key Key;

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
            ::fwRenderOgre::registry::getCameraRegistry()->addFactory(functorKey,
                                                                      &::fwRenderOgre::cameraFactory::New<T>);
        }
    };

    fwCoreNonInstanciableClassDefinitionsMacro( (ICamera)(::fwRenderOgre::IAdaptor) )

    typedef std::string FactoryRegistryKeyType;

    FWRENDEROGRE_API static const FactoryRegistryKeyType REGISTRY_KEY;

    /// ICamera factory method.
    FWRENDEROGRE_API static ICamera::sptr createCameraManager();

    /// ICamera stopping and unregistrating method.
    FWRENDEROGRE_API static void destroyCameraManager(ICamera::sptr _cameraManager);

    /// Constructor. Do nothing.
    FWRENDEROGRE_API ICamera();

    /// Destructor. Do nothing.
    FWRENDEROGRE_API virtual ~ICamera();

    /// Associated ogre camera getter.
    FWRENDEROGRE_API virtual ::Ogre::Camera* getCamera() = 0;
    /// Instanciates the associated Ogre camera and the transformation matrix.
    FWRENDEROGRE_API virtual void createCamera(const std::string& _name, ::Ogre::SceneManager* _sceneManager) = 0;
    /// Near clipping plane position setter.
    FWRENDEROGRE_API virtual void setNearClipDistance(::Ogre::Real _nearClipDistance) = 0;
    /// Far clipping plane position setter.
    FWRENDEROGRE_API virtual void setFarClipDistance(::Ogre::Real _farClipDistance) = 0;
    /// Aspect ratio setter.
    FWRENDEROGRE_API virtual void setAspectRatio(::Ogre::Real _ratio) = 0;
    /// Transform Id setter.
    FWRENDEROGRE_API virtual void setTransformName(::fwRenderOgre::SRender::OgreObjectIdType newId) = 0;
    /// Updates Transformation Matrix.
    FWRENDEROGRE_API virtual void updateTF3D() = 0;
};

} // namespace fwRenderOgre

#endif // __FWRENDEROGRE_ICAMERA_HPP__
