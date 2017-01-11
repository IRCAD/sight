/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_SNAPSHOT_HPP__
#define __VISUVTKADAPTOR_SNAPSHOT_HPP__


#include "visuVTKAdaptor/MeshFactory.hpp"
#include "visuVTKAdaptor/config.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwRenderVTK/IVtkAdaptorService.hpp>

namespace visuVTKAdaptor
{
/**
 * @brief This service will snapshot the current generic scene.
 * It has two slots that can either snapshot into an ::fwData::Image or into a chosen image on the filesystem.
 * @section XML XML configuration
 * @code{.xml}
    <adaptor id="snapshot" uid="snapshotUID" class="::visuVTKAdaptor::Snapshot" objectId="self">
        <config renderer="default" image="imageUID" />
    </adaptor>
   @endcode
 * - \b image(optional)     : Defines the UID of the fwData::Image to write into.
 *
 * @section Slots Slots
 * - \b snap(std::string filePath)    : This slot snaps onto the filesystem at a specified filepath.
 * - \b snapToImage()                 : This slot snaps into the configured image.
 *
 */

class VISUVTKADAPTOR_CLASS_API Snapshot : public ::fwRenderVTK::IVtkAdaptorService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (Snapshot)(::fwRenderVTK::IVtkAdaptorService) );

    VISUVTKADAPTOR_API Snapshot() throw();

    VISUVTKADAPTOR_API virtual ~Snapshot() throw();

protected:

    /// Does nothing.
    VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);

    /// Does nothing.
    VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);

    /// Gets the image object if any and stores its uid.
    VISUVTKADAPTOR_API void doConfigure() throw(fwTools::Failed);

    /// Does nothing.
    VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);

    /// Does nothing.
    VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);

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

#endif // __VISUVTKADAPTOR_SNAPSHOT_HPP__
