/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUOGREADAPTOR_SLIGHT_HPP__
#define __VISUOGREADAPTOR_SLIGHT_HPP__

#include "visuOgreAdaptor/config.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwData/Color.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwRenderOgre/IAdaptor.hpp>

#include <OGRE/OgreMovableObject.h>

namespace Ogre
{
class Light;
}

namespace visuOgreAdaptor
{

/**
 * @brief Adaptor for a light.
 *
 * @section Slots Slots
 * - \b updateXOffset(float): Called when the x offset is changed and moves the light accordingly.
 * - \b updateYOffset(float): Called when the y offset is changed and moves the light accordingly.
 * - \b setDoubleParameter(double, string): Calls a double parameter slot according to the given key.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <adaptor uid="lightAdaptor" class="::visuOgreAdaptor::SLight" objectId="lightTF">
        <config name="sceneLight" attachToCamera="yes" xOffset="30.5" yOffset="45" />
    </adaptor>
 * @endcode
 * With :
 * - \b name (optional): defines a name for the associated Ogre light.
 * - \b attachToCamera (optional, yes/no, default=no): defines if the light is attached to the camera or not.
 * Only if the attachToCamera option is activated :
 * - \b xOffset (optional, float, default=0.0): Angle in degrees defining the rotation of the light around x axis.
 * - \b yOffset (optional, float, default=0.0): Angle in degrees defining the rotation of the light around y axis.
 */
class VISUOGREADAPTOR_CLASS_API SLight : public ::fwRenderOgre::IAdaptor
{

public:
    fwCoreServiceClassDefinitionsMacro((SLight)(::fwRenderOgre::IAdaptor));

    /**
     * @name Slots API
     * @{
     */
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_X_OFFSET_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_Y_OFFSET_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_SET_DOUBLE_PARAMETER_SLOT;
    /** @} */

    /// Constructor.
    VISUOGREADAPTOR_API SLight() throw();

    /// Destructor. Does nothing
    VISUOGREADAPTOR_API virtual ~SLight() throw();

    /// Updates Transformation Matrix

protected:
    /// Starting method. Do nothing
    VISUOGREADAPTOR_API void doStart() throw(fwTools::Failed);

    /// Stopping method
    VISUOGREADAPTOR_API void doStop() throw(fwTools::Failed);

    /// Configures the service.
    VISUOGREADAPTOR_API void doConfigure() throw(fwTools::Failed);

    /// Swaping method, only asks for an updating.
    VISUOGREADAPTOR_API void doSwap() throw(fwTools::Failed);

    /// Updates the light position and orientation
    VISUOGREADAPTOR_API void doUpdate() throw(fwTools::Failed);

private:

    void updateXOffset(float _xOffset);
    void updateYOffset(float _yOffset);
    void setDoubleParameter(double val, std::string key);

    /// Transformation Matrix.
    ::fwData::TransformationMatrix3D::csptr m_lightMat;

    /// Color of the associated Ogre light.
    ::fwData::Color::csptr m_lightColor;

    /// Ogre light managed by this adaptor.
    ::Ogre::Light* m_light;

    /// Name of the associated Ogre light.
    std::string m_lightName;

    /// Defines if the light is attached to the camera or not.
    bool m_attachedToCamera;

    /// Angle in degrees defining the rotation of the light around x axis.
    float m_xOffset;

    /// Angle in degrees defining the rotation of the light around y axis.
    float m_yOffset;
};

} //namespace visuOgreAdaptor

#endif // __VISUOGREADAPTOR_SLIGHT_HPP__
