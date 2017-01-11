/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VTKCOMPOSITEMESH_RENDERERSERVICE_HPP__
#define __VTKCOMPOSITEMESH_RENDERERSERVICE_HPP__

#include "vtkCompositeMesh/config.hpp"

#include <fwData/Image.hpp>

#include <fwRender/IRender.hpp>
#include <fwRenderVTK/IVtkRenderWindowInteractorManager.hpp>



#include <vtkCommand.h>

#include <boost/shared_array.hpp>


// VTK
class vtkRenderer;
class vtkPolyDataNormals;

namespace vtkCompositeMesh
{

/**
 * @brief   Service rendering ::fwData::Mesh contained in a ::fwData::Composite using VTK.
 * @class   RendererService

 * Service registered details : \n
 * fwServicesRegisterMacro( ::fwRender::IRender , ::vtkCompositeMesh::RendererService , ::fwData::Composite)
 */
class VTKCOMPOSITEMESH_CLASS_API RendererService : public ::fwRender::IRender
{
public:

    fwCoreServiceClassDefinitionsMacro ( (RendererService)(::fwRender::IRender) );

    typedef ::boost::shared_array< double > SharedArray;

    VTKCOMPOSITEMESH_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_CAM_POSITION_SLOT;
    typedef ::fwCom::Slot<void (SharedArray, SharedArray, SharedArray)> UpdateCamPositionSlotType;

    /// Slot to update pipeline
    VTKCOMPOSITEMESH_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_PIPELINE_SLOT;
    typedef ::fwCom::Slot<void ()> UpdatePipelineSlotType;

    VTKCOMPOSITEMESH_API static const ::fwCom::Signals::SignalKeyType s_CAM_UPDATED_SIG;
    typedef ::fwCom::Signal< void (SharedArray, SharedArray, SharedArray) > CamUpdatedSignalType;


    /**
     * @brief    Constructor
     */
    VTKCOMPOSITEMESH_API RendererService() throw();

    /**
     * @brief    Destructor
     */
    VTKCOMPOSITEMESH_API virtual ~RendererService() throw();

    /// This method is used to notify that the VTK camera position is updated.
    void notifyCamPositionUpdated();

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect mesh::s_MODIFIED_SIG to this::s_UPDATE_PIPELINE_SLOT
     */
    VTKCOMPOSITEMESH_API virtual KeyConnectionsType getObjSrvConnections() const;

protected:

    /**
     * @brief Starting method.
     *
     * This method is used to initialize the service.
     * Initialize VTK renderer and create qt containers
     */
    VTKCOMPOSITEMESH_API virtual void starting() throw(fwTools::Failed);


    /**
     * @brief Configuring method.
     *
     * XML configuration sample:
     * @code{.xml}
       <service impl="::vtkCompositeMesh::RendererService" type="::fwRender::IRender" autoConnect="yes" />
       @endcode
     * This method is used to configure the service. Initialize qt container.
     */
    VTKCOMPOSITEMESH_API virtual void configuring() throw(::fwTools::Failed);


    /**
     * @brief Stopping method.
     *
     * Destroy VTK renderer and containers
     */
    VTKCOMPOSITEMESH_API virtual void stopping() throw(fwTools::Failed);

    /**
     * @brief Updating method.
     *
     * This method is used to update the service.
     * Make a render if necessary
     */
    VTKCOMPOSITEMESH_API virtual void updating() throw(fwTools::Failed);

    /// @brief vtk renderer
    vtkRenderer * m_render;

private:

    /// @brief VTK Interactor window manager
    ::fwRenderVTK::IVtkRenderWindowInteractorManager::sptr m_interactorManager;

    /// This method is used to create an actor and put it through the VTK pipeline.
    void createAndAddActorToRender();

    /// This method is used to initialize the VTK pipeline.
    void initVTKPipeline();

    /// This method is used to update the VTK pipeline.
    void updateVTKPipeline();


    /// Slot: receives new camera information (position, focal, viewUp). Update camera with new information.
    void updateCamPosition(SharedArray positionValue,
                           SharedArray focalValue,
                           SharedArray viewUpValue);

    /// Slot: update the pipeline
    void updatePipeline();


    /// Contains the mesh, and allows to compute normals.
    vtkPolyDataNormals* m_normals;

    /**
     * @brief the m_bPipelineIsInit value is \b true
     * if the pipeline is initialized.
     */
    bool m_bPipelineIsInit;

    vtkCommand* m_loc;

    /// Slot to call updateCamPosition method.
    UpdateCamPositionSlotType::sptr m_slotUpdateCamPosition;

    /// Slot to call updatePipeline method
    UpdatePipelineSlotType::sptr m_slotUpdatePipeline;

    /// Signal emitted when camera position is updated.
    CamUpdatedSignalType::sptr m_sigCamUpdated;


};

}

#endif /* __VTKCOMPOSITEMESH_RENDERERSERVICE_HPP__ */
