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

#include <fwData/Image.hpp>

#include <fwDataTools/helper/MedicalImage.hpp>

#include <fwRenderVTK/IAdaptor.hpp>

#include <vtkSmartPointer.h>

class vtkPolyDataAlgorithm;
class vtkImageData;
class vtkGlyph3D;

namespace visuVTKAdaptor
{

/**
 * @brief This adaptor displays vector field contains in a ::fwData::Image
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::visuVTKAdaptor::SVectorField" autoConnect="yes">
       <in key="image" uid="..." />
       <config renderer="default" />
   </service>
   @endcode
 * @subsection Input Input
 * - \b image [::fwData::Image]: image containing vector field (image with 3 components).
 * @subsection Configuration Configuration
 * - \b config(mandatory) : contains the adaptor configuration
 *    - \b renderer(mandatory) : renderer where the mesh is displayed
 */
class VISUVTKADAPTOR_CLASS_API SVectorField : public ::fwRenderVTK::IAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro( (SVectorField)(::fwRenderVTK::IAdaptor) )

    VISUVTKADAPTOR_API SVectorField() noexcept;

    VISUVTKADAPTOR_API virtual ~SVectorField() noexcept;

protected:

    VISUVTKADAPTOR_API void configuring() override;
    VISUVTKADAPTOR_API void starting() override;
    VISUVTKADAPTOR_API void updating() override;
    VISUVTKADAPTOR_API void stopping() override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect Image::s_BUFFER_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsMap getAutoConnections() const override;

    virtual void buildPipeline();
    virtual void destroyPipeline();

    vtkSmartPointer<vtkPolyDataAlgorithm> m_arrowSource;
    vtkSmartPointer<vtkImageData> m_imageData;
    vtkSmartPointer<vtkGlyph3D> glyphFilter;

};

} //namespace visuVTKAdaptor
