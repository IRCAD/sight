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

#include <vtkSmartPointer.h>

class vtkOrientationMarkerWidget;

namespace visuVTKAdaptor
{

/**
 * @brief Adaptor used to display an orientationMarker in a generic scene
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::visuVTKAdaptor::SOrientationMarker">
       <config renderer="default" hAlign="left">
   </service>
   @endcode
 * @subsection Configuration Configuration
 * - \b config(mandatory) : contains the adaptor configuration
 *    - \b renderer(mandatory) : defines the renderer to show the point list.
 *    - \b hAlign(optional, default: 'left') : horizontal alignment (left or right)
 */
class VISUVTKADAPTOR_CLASS_API SOrientationMarker : public ::fwRenderVTK::IAdaptor
{

public:

    fwCoreServiceMacro(SOrientationMarker, ::fwRenderVTK::IAdaptor);
    VISUVTKADAPTOR_API SOrientationMarker();

protected:

    VISUVTKADAPTOR_API void configuring() override;
    VISUVTKADAPTOR_API void starting() override;

    //------------------------------------------------------------------------------

    void updating() override
    {
    }

    void stopping() override;

    /// Horizontal alignment
    std::string m_hAlign;
    vtkSmartPointer<vtkOrientationMarkerWidget> m_widget;
};
} //namespace visuVTKAdaptor
