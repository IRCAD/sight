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

#include <fwData/TransferFunction.hpp>

#include <fwDataTools/helper/MedicalImage.hpp>

#include <fwRenderVTK/IAdaptor.hpp>

class vtkCommand;

namespace visuVTKAdaptor
{

/**
 * @brief This service allows to interact with the plane (switch normal, deselect, move along the normal)
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::visuVTKAdaptor::SPlaneInteractor">
       <inout key="plane" uid="..." />
       <config renderer="default" />
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b plane [::fwData::Plane]: plane to interact.
 * @subsection Configuration Configuration
 * - \b config(mandatory) : contains the adaptor configuration
 *    - \b renderer(mandatory) : identifier of the renderer to use
 */
class VISUVTKADAPTOR_CLASS_API SPlaneInteractor : public ::fwRenderVTK::IAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro( (SPlaneInteractor)(::fwRenderVTK::IAdaptor) )

    VISUVTKADAPTOR_API SPlaneInteractor() noexcept;

    VISUVTKADAPTOR_API virtual ~SPlaneInteractor() noexcept;

    /// Switch plane normal
    VISUVTKADAPTOR_API void switchPlaneNormal();

    /// Move the plane along the normal
    VISUVTKADAPTOR_API void pushPlane(double factor);

    /// Deselect the plane
    VISUVTKADAPTOR_API void deselectPlane();

protected:

    VISUVTKADAPTOR_API void configuring() override;
    VISUVTKADAPTOR_API void starting() override;
    VISUVTKADAPTOR_API void updating() override;
    VISUVTKADAPTOR_API void stopping() override;

    vtkCommand* m_vtkObserver;
    float m_priority;

};

} //namespace visuVTKAdaptor
