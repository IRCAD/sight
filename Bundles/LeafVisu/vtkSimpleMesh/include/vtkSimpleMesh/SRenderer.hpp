/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VTKSIMPLEMESH_SRENDERER_HPP__
#define __VTKSIMPLEMESH_SRENDERER_HPP__

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
 * @class   SRenderer
 *
 * This service displays a mesh in a scene.
 *
 * The scene owns a 3D camera that can be moved by the user on clicking in the scene.
 *   - When the camera moved, a signal 'camUpdated' is emitted with the new camera information (position, focal and
 *     view up).
 *  - To update the camera without clicking, you could called the slot 'updateCamPosition'.
 */
class VTKSIMPLEMESH_CLASS_API SRenderer : public fwRender::IRender
{
public:

    fwCoreServiceClassDefinitionsMacro ( (SRenderer)(::fwRender::IRender) );

    typedef ::boost::shared_array< double > SharedArray;

    VTKSIMPLEMESH_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_CAM_POSITION_SLOT;
    typedef ::fwCom::Slot<void (SharedArray, SharedArray, SharedArray)> UpdateCamPositionSlotType;

    /// Slot to initialize pipeline
    VTKSIMPLEMESH_API static const ::fwCom::Slots::SlotKeyType s_INIT_PIPELINE_SLOT;
    typedef ::fwCom::Slot<void ()> InitPipelineSlotType;

    /// Slot to update pipeline
    VTKSIMPLEMESH_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_PIPELINE_SLOT;
    typedef ::fwCom::Slot<void ()> UpdatePipelineSlotType;

    VTKSIMPLEMESH_API static const ::fwCom::Signals::SignalKeyType s_CAM_UPDATED_SIG;
    typedef ::fwCom::Signal< void (SharedArray, SharedArray, SharedArray) > CamUpdatedSignalType;

    /**
     * @brief    Constructor
     */
    VTKSIMPLEMESH_API SRenderer() throw();

    /**
     * @brief    Destructor
     */
    VTKSIMPLEMESH_API virtual ~SRenderer() throw();


    /// This method is used to notify that the VTK camera position is updated.
    void notifyCamPositionUpdated();

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect mesh::s_MODIFIED_SIG to this::s_INIT_PIPELINE_SLOT
     * Connect mesh::s_VERTEX_MODIFIED_SIG to this::s_UPDATE_PIPELINE_SLOT
     */
    VTKSIMPLEMESH_API virtual KeyConnectionsType getObjSrvConnections() const;


protected:

    /**
     * @brief Starting method.
     *
     * This method is used to initialize the service.
     * Initialize VTK renderer and qt containers
     */
    VTKSIMPLEMESH_API virtual void starting() throw(fwTools::Failed);


    /**
     * @brief This method is used to configure the service. Initialize the qt container.
     *
     * XML configuration sample:
     * @verbatim
       <service impl="::vtkSimpleMesh::SRenderer" type="::fwRender::IRender" autoConnect="yes" />
       @endverbatim
     */
    VTKSIMPLEMESH_API virtual void configuring() throw(::fwTools::Failed);


    /**
     * @brief Stopping method.
     *
     * Destroy the VTK renderer and the qt container
     */
    VTKSIMPLEMESH_API virtual void stopping() throw(fwTools::Failed);

    /**
     * @brief Updating method.
     *
     * This method is used to update the service.
     * Make a render if necessary
     */
    VTKSIMPLEMESH_API virtual void updating() throw(fwTools::Failed);

    /// @brief vtk renderer
    vtkRenderer * m_render;

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

    /// Slot to call updateCamPosition method
    UpdateCamPositionSlotType::sptr m_slotUpdateCamPosition;

    /// Slot to call initPipeline method
    InitPipelineSlotType::sptr m_slotInitPipeline;

    /// Slot to call initPipeline method
    UpdatePipelineSlotType::sptr m_slotUpdatePipeline;

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

#endif /* __VTKSIMPLEMESH_SRENDERER_HPP__ */
