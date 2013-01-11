/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef VTKCOMPOSITEMESH_RENDERER_SERVICE_HPP_
#define VTKCOMPOSITEMESH_RENDERER_SERVICE_HPP_

#include <boost/shared_array.hpp>

#include <fwRenderVTK/IVtkRenderWindowInteractorManager.hpp>

#include <vtkCommand.h>

#include <fwRender/IRender.hpp>

#include <fwServices/ObjectMsg.hpp>

#include <fwData/Image.hpp>

#include "vtkCompositeMesh/config.hpp"


// VTK
class vtkRenderer;
class vtkPolyDataNormals;

namespace vtkCompositeMesh
{

/**
 * @brief   Renderer service.
 * @class   RendererService.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 *
 * Service rendering ::fwData::Mesh contained in a ::fwData::Composite using VTK.
 *
 * Service registered details : \n
 * fwServicesRegisterMacro( ::fwRender::IRender , ::vtkCompositeMesh::RendererService , ::fwData::Composite)
 */
class VTKCOMPOSITEMESH_CLASS_API RendererService : public ::fwRender::IRender
{
public :

    fwCoreServiceClassDefinitionsMacro ( (RendererService)(::fwRender::IRender) ) ;

    typedef ::boost::shared_array< double > SharedArray;

    VTKCOMPOSITEMESH_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_CAM_POSITION_SLOT;
    typedef ::fwCom::Slot<void(SharedArray, SharedArray, SharedArray)> UpdateCamPositionSlotType;

    VTKCOMPOSITEMESH_API static const ::fwCom::Signals::SignalKeyType s_CAM_UPDATED_SIG;
    typedef ::fwCom::Signal< void (SharedArray, SharedArray, SharedArray) > CamUpdatedSignalType;


    /**
    * @brief    Constructor
    */
    VTKCOMPOSITEMESH_API RendererService() throw() ;

    /**
    * @brief    Destructor
    */
    VTKCOMPOSITEMESH_API virtual ~RendererService() throw() ;

    /// This method is used to notify that the VTK camera position is updated.
    void notifyCamPositionUpdated();

    /// Slot to receive new camera information (position, focal, viewUp). Update camera with new information.
    VTKCOMPOSITEMESH_API virtual void updateCamPosition(SharedArray positionValue,
                                                        SharedArray focalValue,
                                                        SharedArray viewUpValue);

protected :

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
    * @verbatim
    <service impl="::vtkCompositeMesh::RendererService" type="::fwRender::IRender" autoConnect="yes" />
    @endverbatim
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

    /**
    * @brief Receiving method (react on data modifications).
    * @param[in] _msg ::fwServices::ObjectMsg::csptr.
    *
    * This method is used to update the vtk pipeline when the mesh is modified.
    */
    VTKCOMPOSITEMESH_API virtual void receiving( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed);

    /// @brief vtk renderer
    vtkRenderer * m_render ;

private :

    /// @brief VTK Interactor window manager
    ::fwRenderVTK::IVtkRenderWindowInteractorManager::sptr m_interactorManager;

    /// This method is used to create an actor and put it through the VTK pipeline.
    void createAndAddActorToRender();

    /// This method is used to initialize the VTK pipeline.
    void initVTKPipeline();

    /// This method is used to update the VTK pipeline.
    void updateVTKPipeline();



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

    /// Signal emitted when camera position is updated.
    CamUpdatedSignalType::sptr m_sigCamUpdated;


};

}

#endif /* VTKCOMPOSITEMESH_RENDERER_SERVICE_HPP_ */
