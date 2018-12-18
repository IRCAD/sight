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

#include <vtkSmartPointer.h>

#include <vector>

class vtkTransform;
class vtkCommand;

namespace visuVTKAdaptor
{

/**
 * @brief Adaptor binding a TransformationMatrix3D to a vtkTransform.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service type="::visuVTKAdaptor::STransform" autoConnect="yes">
        <inout key="tm3d" uid="..." />
        <config transform="transform" autoRender="true" parent="parentTransform" />
    </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b tm3d [::fwData::TransformationMatrix3D]: Sight transform matrix.
 *
 * @subsection Configuration Configuration
 *  - \b transform (mandatory) : the vtkTransform to associate to the adaptor
 *  - \b autoRender (optional, "true" by default): if autoRender=true,  the scene is automatically rendered after
 *    doStart, doUpdate, doSwap and doStop if m_vtkPipelineModified=true.
 *  - \b parent (optional): id of the parent vtkTransform, it will be concatenated with this current vtkTransform.
 */
class VISUVTKADAPTOR_CLASS_API STransform : public ::fwRenderVTK::IAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro( (STransform)(::fwRenderVTK::IAdaptor) )

    static const ::fwServices::IService::KeyType s_TM3D_INOUT;

    /// Constructor
    VISUVTKADAPTOR_API STransform() noexcept;

    /// Destructor
    VISUVTKADAPTOR_API virtual ~STransform() noexcept;

    /// Set a new vtkTransform
    VISUVTKADAPTOR_API void setTransform(vtkTransform* t);

    /// Returns the current vtkTransform
    VISUVTKADAPTOR_API vtkTransform* getTransform();

    /// Updates the TransformationMatrix3D from the vtkTransform
    VISUVTKADAPTOR_API void updateFromVtk();

protected:

    VISUVTKADAPTOR_API void configuring() override;

    /// Registers transforms and then updates
    VISUVTKADAPTOR_API void starting() override;

    /// Updates the vtkTransform from the TransformationMatrix3D
    VISUVTKADAPTOR_API void updating() override;

    /// Cleanup
    VISUVTKADAPTOR_API void stopping() override;

    /// Updates
    VISUVTKADAPTOR_API void swapping() override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect TransformationMatrix3D::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsMap getAutoConnections() const override;

    /// Current vtkTransform
    vtkTransform* m_transform;

    /// Command used to listen m_transform changes
    vtkCommand* m_transformCommand;

    /// Parent vtkTransform id (optional)
    std::string m_parentId;

    /// Parent vtkTransform (optional)
    vtkSmartPointer<vtkTransform> m_parentTransform;
};

} //namespace visuVTKAdaptor
