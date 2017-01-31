/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUOGREADAPTOR_SCAMERA_HPP__
#define __VISUOGREADAPTOR_SCAMERA_HPP__

#include "visuOgreAdaptor/config.hpp"
#include "visuOgreAdaptor/STransform.hpp"

#include <fwCom/helper/SigSlotConnection.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwRenderOgre/ICamera.hpp>
#include <fwRenderOgre/ITransformable.hpp>

#include <OGRE/OgreMovableObject.h>

#include <string>
#include <vector>

namespace arData
{
class Camera;
}

namespace visuOgreAdaptor
{

/**
 * @brief   Adaptor from fw4 Camera to Ogre Camera
 *
 * @section XML XML Configuration
 * @code{.xml}
 *  <service uid="cameraAdaptor" type="::visuOgreAdaptor::SCamera">
 *      <in key="transform" uid="cameraTFUid" />
 *      <in key="calibration" uid="cameraCalibUid" />
 *      <config name="sceneCamera" />
 *  </service>
 * @endcode
 * With :
 * - \b name (optional): defines a name for the associated Ogre camera.
 */
class VISUOGREADAPTOR_CLASS_API SCamera : public ::fwRenderOgre::ICamera,
                                          public ::fwRenderOgre::ITransformable
{

public:
    fwCoreServiceClassDefinitionsMacro((SCamera)(::fwRenderOgre::ICamera));

    /// Constructor.
    VISUOGREADAPTOR_API SCamera() throw();

    /// Factory Constructor.
    VISUOGREADAPTOR_API SCamera(::fwRenderOgre::ICamera::Key key);

    /// Destructor. Does nothing
    VISUOGREADAPTOR_API virtual ~SCamera() throw();

    /**
     * @name Slots API
     * @{
     */
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_CALIBRATE_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_TF_SLOT;
    /** @} */

    /// Updates Transformation Matrix
    VISUOGREADAPTOR_API virtual void updateTF3D();

    /// Returns proposals to connect service slots to associated object signals
    VISUOGREADAPTOR_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const;

    /// Associated ogre camera getter.
    VISUOGREADAPTOR_API virtual ::Ogre::Camera* getCamera();

    /// Instanciates the associated Ogre camera and the transformation matrix
    VISUOGREADAPTOR_API virtual void createCamera(const std::string& _name, ::Ogre::SceneManager* _sceneManager);

    /// Near clipping plane position setter.
    VISUOGREADAPTOR_API virtual void setNearClipDistance(::Ogre::Real _nearClipDistance);

    /// Far clipping plane position setter.
    VISUOGREADAPTOR_API virtual void setFarClipDistance(::Ogre::Real _farClipDistance);

    /// Aspect ratio setter.
    VISUOGREADAPTOR_API virtual void setAspectRatio(::Ogre::Real _ratio);

    VISUOGREADAPTOR_API virtual void setTransformName(::fwRenderOgre::SRender::OgreObjectIdType newId);

protected:
    /// Starting method. Does nothing.
    VISUOGREADAPTOR_API void doStart() throw(fwTools::Failed);

    /// Stopping method
    VISUOGREADAPTOR_API void doStop() throw(fwTools::Failed);

    /// Configures the service.
    VISUOGREADAPTOR_API void doConfigure() throw(fwTools::Failed);

    /// Swaping method, only asks for a updating
    VISUOGREADAPTOR_API void doSwap() throw(fwTools::Failed);

    /// Updating method. Does nothing.
    VISUOGREADAPTOR_API void doUpdate() throw(fwTools::Failed);

private:

    /// Returns a shared pointer of the attached transform service.
    ::visuOgreAdaptor::STransform::sptr getTransformService();

    /// Creates a transform Service, and attaches it to a corresponding scene node in the scene.
    void createTransformService();

    /// Attaches a movable object to the associated scene node
    void attachNode(Ogre::MovableObject* _node);

    /// Calibrate the camera parameters according to an arData::Camera
    void calibrate();

    /// Ogre camera managed by this adaptor.
    ::Ogre::Camera* m_camera;

    /// Name of the associated Ogre camera.
    std::string m_cameraName;

    /// Position of the near clipping plane.
    ::Ogre::Real m_nearClipDistance;

    /// Position of the far clipping plane.
    ::Ogre::Real m_farClipDistance;

    /// Aspect ratio for the frustum viewport.
    ::Ogre::Real m_aspectRatio;

    /// camera used to calibrate ogre camera
    CSPTR(::arData::Camera) m_calibration;
};

//------------------------------------------------------------------------------
// Inline method(s)

inline ::Ogre::Camera* SCamera::getCamera()
{
    return m_camera;
}

//------------------------------------------------------------------------------

inline void SCamera::setTransformName(::fwRenderOgre::SRender::OgreObjectIdType newId)
{
    this->setTransformId(newId);
}

//------------------------------------------------------------------------------

inline ::visuOgreAdaptor::STransform::sptr SCamera::getTransformService()
{
    return ::visuOgreAdaptor::STransform::dynamicCast(m_transformService.lock());
}

//------------------------------------------------------------------------------

} //namespace visuOgreAdaptor

#endif // __VISUOGREADAPTOR_SCAMERA_HPP__
