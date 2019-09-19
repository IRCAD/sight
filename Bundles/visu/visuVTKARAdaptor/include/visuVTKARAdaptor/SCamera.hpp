/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "visuVTKARAdaptor/config.hpp"

#include <fwRenderVTK/IAdaptor.hpp>

class vtkPerspectiveTransform;
class vtkCommand;

namespace visuVTKARAdaptor
{

/**
 * @brief   Places the camera inside a VTK scene according to a transform matrix. If the transform matrix is specified
 * as an inout key, it will be updated if the corresponding VTK transform is modified.
 *
 * @section Slots Slots
 * - \b calibrate(): recomputes the FOV. Also, it sets the size of the renderer if it is offscreen.
 *
 * @section Signals Signals
 * - \b positionModified(): notifies camera displacements.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::visuVTKARAdaptor::SCamera" autoConnect="yes">
            <inout key="transform" uid="..." />
            <in key="camera" uid="..." />
            <config renderer="default" />
        </service>
   @endcode
 * Or:
 * @code{.xml}
        <service type="::visuVTKARAdaptor::SCamera" autoConnect="yes">
            <in key="transform" uid="..." />
            <in key="camera" uid="..." />
            <config renderer="default" />
        </service>
   @endcode
 *
 * @subsection Input Input
 * - \b camera [::arData::Camera] (optional): camera calibration.
 * - \b transform [::fwData::TransformationMatrix3D] (optional): camera position and orientation.
 *
 * @subsection In-Out In-Out
 * - \b transform [::fwData::TransformationMatrix3D] (optional): camera position and orientation. Will be updated if the
 * underlying VTK transform is modified
 *
 * @subsection Configuration Configuration
 * - \b renderer : ID of renderer the adaptor must use.
 */
class VISUVTKARADAPTOR_CLASS_API SCamera : public ::fwRenderVTK::IAdaptor
{

public:
    fwCoreServiceMacro(SCamera, ::fwRenderVTK::IAdaptor);

    /// Constructor.
    SCamera() noexcept;

    /// Destructor.
    virtual ~SCamera() noexcept;

    /// Update Camera position from VTK.
    VISUVTKARADAPTOR_API void updateFromVtk();

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Camera::s_MODIFIED_SIG to this::s_CALIBRATE_SLOT
     * Connect Camera::s_INTRINSIC_CALIBRATED_SIG to this::s_CALIBRATE_SLOT
     *
     * Connect Transform::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    VISUVTKARADAPTOR_API KeyConnectionsMap getAutoConnections() const override;

protected:

    VISUVTKARADAPTOR_API void configuring() override;

    /// Initializes the camera position and connects it to the observer.
    VISUVTKARADAPTOR_API void starting() override;

    /// Restarts (stop-start).
    VISUVTKARADAPTOR_API void swapping() override;

    /// Updates the camera with the input transform.
    VISUVTKARADAPTOR_API void updating() override;

    /// Removes the observer.
    VISUVTKARADAPTOR_API void stopping() override;

private:

    /// Update vtk camera from current TransformationMatrix3D.
    void updateFromTMatrix3D();

    /// Calibrate vtk camera and set the size of the renderer if it is offscreen.
    void calibrate();

    vtkPerspectiveTransform* m_transOrig; ///<  VTK original perspective transform.
    vtkCommand* m_cameraCommand; ///< VTK camera command.
    vtkCommand* m_resizeCommand; ///< VTK renderer resize command.

    friend class WindowResizeCallBack;
};

} //namespace visuVTKARAdaptor
