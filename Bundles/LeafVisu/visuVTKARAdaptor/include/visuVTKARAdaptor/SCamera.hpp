/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKARADAPTOR_SCAMERA_HPP__
#define __VISUVTKARADAPTOR_SCAMERA_HPP__

#include "visuVTKARAdaptor/config.hpp"

#include <fwRenderVTK/IAdaptor.hpp>

class vtkPerspectiveTransform;
class vtkCommand;

namespace visuVTKARAdaptor
{

/**
 * @brief   Places the camera inside a VTK scene and updates its transform matrix.
 *
 * @section Slots Slots
 * - \b calibrate(): recompute the FOV.
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
 *
 * @subsection Input Input
 * - \b camera [::arData::Camera] (optional): camera calibration.
 *
 * @subsection In-Out In-Out
 * - \b transform [::fwData::TransformationMatrix3D]: camera position and orientation.
 *
 * @subsection Configuration Configuration
 * - \b renderer : ID of renderer the adaptor must use.
 */
class VISUVTKARADAPTOR_CLASS_API SCamera : public ::fwRenderVTK::IAdaptor
{

public:
    fwCoreServiceClassDefinitionsMacro( (SCamera)(::fwRenderVTK::IAdaptor) );

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

    /// Calibrate vtk camera.
    void calibrate();

    vtkPerspectiveTransform* m_transOrig; ///<  VTK original perspective transform.
    vtkCommand* m_cameraCommand; ///< VTK camera command.
    vtkCommand* m_resizeCommand; ///< VTK renderer resize command.

};

} //namespace visuVTKARAdaptor

#endif /* __VISUVTKARADAPTOR_SCAMERA_HPP__ */
