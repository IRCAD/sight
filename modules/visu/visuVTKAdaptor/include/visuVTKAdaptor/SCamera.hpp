/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include <vector>

class vtkCommand;
class vtkPerspectiveTransform;

namespace visuVTKAdaptor
{

/**
 * @brief This adaptor updates the camera position according to a matrix (and vice versa)
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::visuVTKAdaptor::SCamera" autoConnect="yes">
       <inout key="transform" uid="..." />
       <config renderer="default" viewAngle="30"/>
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b transform [::fwData::TransformationMatrix3D]: matrix updated by the widget.
 * @subsection Configuration Configuration
 * - \b config(mandatory) : contains the adaptor configuration
 *    - \b renderer (mandatory): defines the renderer to use.
 *    - \b viewAngle (optional): defines the angular height of the camera view measured in degrees (default is 30)
 */
class VISUVTKADAPTOR_CLASS_API SCamera : public ::fwRenderVTK::IAdaptor
{

public:

    fwCoreServiceMacro(SCamera, ::fwRenderVTK::IAdaptor);

    VISUVTKADAPTOR_API SCamera() noexcept;

    VISUVTKADAPTOR_API virtual ~SCamera() noexcept;

    VISUVTKADAPTOR_API void updateFromVtk();

protected:

    VISUVTKADAPTOR_API void configuring() override;
    VISUVTKADAPTOR_API void starting() override;
    VISUVTKADAPTOR_API void updating() override;
    VISUVTKADAPTOR_API void stopping() override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect TransformationMatrix3D::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsMap getAutoConnections() const override;

private:
    vtkCommand* m_cameraCommand;
    vtkPerspectiveTransform* m_transOrig;

    // the angular height of the camera view measured in degrees
    double m_viewAngle;

};

} //namespace visuVTKAdaptor
