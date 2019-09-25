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

#include "vtkSimpleMesh/config.hpp"

#include <fwRender/IRender.hpp>

#include <fwRenderVTK/IVtkRenderWindowInteractorManager.hpp>

#include <boost/shared_array.hpp>

#include <vtkCommand.h>
#include <vtkSmartPointer.h>

// VTK
class vtkRenderer;
class vtkPolyData;
class vtkPolyDataMapper;

namespace vtkSimpleMesh
{

/**
 * @brief   Service rendering a ::fwData::Mesh using VTK.
 *
 * This service displays a mesh in a scene.
 *
 * The scene owns a 3D camera that can be moved by the user on clicking in the scene.
 *   - When the camera moved, a signal 'camUpdated' is emitted with the new camera information (position, focal and
 *     view up).
 *  - To update the camera without clicking, you could called the slot 'updateCamPosition'.
 *
 * @section Signals Signals
 * - \b camUpdated(SharedArray, SharedArray, SharedArray): Emitted when the camera has moved.
 *
 * @section Slots Slots
 * - \b updateCamPosition(SharedArray, SharedArray, SharedArray): update the camera position and orientation.
 * - \b initPipeline(): initialize or reinitialize the vtk pipeline.
 * - \b updatePipeline(): update the vtk pipeline.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::vtkSimpleMesh::SRenderer">
            <in key="mesh" uid="..." />
       </service>
   @endcode
 * @subsection Input Input:
 * - \b mesh [::fwData::Mesh]: mesh to be displayed.
 */
class VTKSIMPLEMESH_CLASS_API SRenderer : public fwRender::IRender
{
public:

    fwCoreServiceMacro(SRenderer, ::fwRender::IRender);

    typedef ::boost::shared_array< double > SharedArray;

    VTKSIMPLEMESH_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_CAM_POSITION_SLOT;
    VTKSIMPLEMESH_API static const ::fwCom::Slots::SlotKeyType s_INIT_PIPELINE_SLOT;
    VTKSIMPLEMESH_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_PIPELINE_SLOT;

    VTKSIMPLEMESH_API static const ::fwCom::Signals::SignalKeyType s_CAM_UPDATED_SIG;
    typedef ::fwCom::Signal< void (SharedArray, SharedArray, SharedArray) > CamUpdatedSignalType;

    /**
     * @brief    Constructor
     */
    VTKSIMPLEMESH_API SRenderer() noexcept;

    /**
     * @brief    Destructor
     */
    VTKSIMPLEMESH_API virtual ~SRenderer() noexcept;

    /// This method is used to notify that the VTK camera position is updated.
    void notifyCamPositionUpdated();

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect mesh::s_MODIFIED_SIG to this::s_INIT_PIPELINE_SLOT
     * Connect mesh::s_VERTEX_MODIFIED_SIG to this::s_UPDATE_PIPELINE_SLOT
     */
    VTKSIMPLEMESH_API virtual KeyConnectionsMap getAutoConnections() const override;

protected:

    /**
     * @brief Starting method.
     *
     * This method is used to initialize the service.
     * Initialize VTK renderer and qt containers
     */
    VTKSIMPLEMESH_API virtual void starting() override;

    /**
     * @brief This method is used to configure the service. Initialize the qt container.
     */
    VTKSIMPLEMESH_API virtual void configuring() override;

    /**
     * @brief Stopping method.
     *
     * Destroy the VTK renderer and the qt container
     */
    VTKSIMPLEMESH_API virtual void stopping() override;

    /**
     * @brief Updating method.
     *
     * This method is used to update the service.
     * Make a render if necessary
     */
    VTKSIMPLEMESH_API virtual void updating() override;

    /// @brief vtk renderer
    vtkRenderer* m_render;

private:

    /// @brief VTK Interactor window manager
    ::fwRenderVTK::IVtkRenderWindowInteractorManager::sptr m_interactorManager;

    /// This method is used to initialize the VTK pipeline.
    void initVTKPipeline();

    /**
     * @brief This method is used to update the VTK pipeline.
     * @param resetCamera if true : reset the camera position
     */
    void updateVTKPipeline(bool resetCamera = true);

    /// Slot: receives new camera information (position, focal, viewUp). Update camera with new information.
    void updateCamPosition(SharedArray positionValue,
                           SharedArray focalValue,
                           SharedArray viewUpValue);

    /// Slot: initialize the pipeline
    void initPipeline();

    /// Slot: update the pipeline
    void updatePipeline();

    /**
     * @brief the m_bPipelineIsInit value is \b true
     * if the pipeline is initialized.
     */
    bool m_bPipelineIsInit;

    vtkCommand* m_loc;

    /// Signal emitted when camera position is updated.
    CamUpdatedSignalType::sptr m_sigCamUpdated;

    /// To store the polyData
    vtkSmartPointer<vtkPolyData> m_vtkPolyData;

    /// Vtk mapper
    vtkSmartPointer<vtkPolyDataMapper> m_mapper;

    /// Hight resolution timer to log information about computing function time
    ::fwCore::HiResTimer m_hiResTimer;
};

}
