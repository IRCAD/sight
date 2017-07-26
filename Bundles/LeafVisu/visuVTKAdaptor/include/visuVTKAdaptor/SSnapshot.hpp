/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_SSNAPSHOT_HPP__
#define __VISUVTKADAPTOR_SSNAPSHOT_HPP__

#include "visuVTKAdaptor/config.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwRenderVTK/IAdaptor.hpp>

namespace visuVTKAdaptor
{
/**
 * @brief This service will snapshot the current generic scene.
 *
 * It has two slots that can either snapshot into an ::fwData::Image or into a chosen image on the filesystem.
 *
 * @section Slots Slots
 * - \b snap(std::string filePath)    : This slot snaps onto the filesystem at a specified filepath.
 * - \b snapToImage()                 : This slot snaps into the configured image.
 *
 * @section XML XML configuration
 * @code{.xml}
   <service type="::visuVTKAdaptor::SSnapshot" autoConnect="yes">
       <out key="image" uid="..." />
       <config renderer="default" />
   </service>
   @endcode
 *
 * @subsection Outpu Output
 * - \b image [::fwData::Image] : Defines the fwData::Image to write into. Only used if snapToImage() is called.
 *
 * @subsection Configuration Configuration:
 * - \b config(mandatory) : contains the adaptor configuration
 *    - \b renderer (mandatory): defines the renderer to show the arrow. It must be different from the 3D objects
 *    renderer.
 */

class VISUVTKADAPTOR_CLASS_API SSnapshot : public ::fwRenderVTK::IAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro( (SSnapshot)(::fwRenderVTK::IAdaptor) );

    VISUVTKADAPTOR_API SSnapshot() noexcept;

    VISUVTKADAPTOR_API virtual ~SSnapshot() noexcept;

protected:

    VISUVTKADAPTOR_API void configuring();
    VISUVTKADAPTOR_API void starting();
    VISUVTKADAPTOR_API void updating();
    VISUVTKADAPTOR_API void stopping();

private:

    /**
     * @name Slots
     * @{
     */
    /// Type of slot to snap shot
    static const ::fwCom::Slots::SlotKeyType s_SNAP_SLOT;

    /// Type of slot to snap shot to image
    static const ::fwCom::Slots::SlotKeyType s_SNAPTOIMAGE_SLOT;

    /// Slot: snap shot the generic scene.
    void snap(std::string filePath);

    /// Slot: snap shot the generic scene to fwData::Image
    void snapToImage();
    /**
     * @}
     */

    /// UID of the image
    std::string m_imageUid;
};

} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_SSNAPSHOT_HPP__
