/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "visuVTKAdaptor/config.hpp"

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
 *    - \b renderer (mandatory): defines the renderer to snapshot
 */

class VISUVTKADAPTOR_CLASS_API SSnapshot : public ::fwRenderVTK::IAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro( (SSnapshot)(::fwRenderVTK::IAdaptor) );

    VISUVTKADAPTOR_API SSnapshot() noexcept;

    VISUVTKADAPTOR_API virtual ~SSnapshot() noexcept;

protected:

    VISUVTKADAPTOR_API void configuring() override;
    VISUVTKADAPTOR_API void starting() override;
    VISUVTKADAPTOR_API void updating() override;
    VISUVTKADAPTOR_API void stopping() override;

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
