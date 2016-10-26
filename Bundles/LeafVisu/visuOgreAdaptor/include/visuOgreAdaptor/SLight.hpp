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
#include <fwCom/helper/SigSlotConnection.hpp>

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
 * @section XML XML Configuration
 * @code{.xml}
    <adaptor uid="lightAdaptor" class="::visuOgreAdaptor::SLight" objectId="lightTF">
        <config name="sceneLight" />
    </adaptor>
 * @endcode
 * With :
 * - \b name (optional): defines a name for the associated Ogre light.
 */
class VISUOGREADAPTOR_CLASS_API SLight : public ::fwRenderOgre::IAdaptor
{

public:
    fwCoreServiceClassDefinitionsMacro((SLight)(::fwRenderOgre::IAdaptor));

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

    /// Transformation Matrix.
    ::fwData::TransformationMatrix3D::csptr m_lightMat;

    /// Color of the associated Ogre light.
    ::fwData::Color::csptr m_lightColor;

    /// Ogre light managed by this adaptor.
    ::Ogre::Light* m_light;

    /// Name of the associated Ogre light.
    std::string m_lightName;
};

} //namespace visuOgreAdaptor

#endif // __VISUOGREADAPTOR_SLIGHT_HPP__
