/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef VTKCOMPOSITEMESH_RENDERER_SERVICE_HPP_
#define VTKCOMPOSITEMESH_RENDERER_SERVICE_HPP_

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

    VTKCOMPOSITEMESH_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_CAM_POSITION_SLOT;
    typedef ::fwCom::Slot<void(const double*, const double*, const double*)> UpdateCamPositionSlotType;

    VTKCOMPOSITEMESH_API static const ::fwCom::Signals::SignalKeyType s_CAM_UPDATED_SIG;
    typedef ::fwCom::Signal< void (const double*, const double*, const double*) > CamUpdatedSignalType;


    /**
    * @brief    Constructor
    */
    VTKCOMPOSITEMESH_API RendererService() throw() ;

    /**
    * @brief    Destructor
    */
    VTKCOMPOSITEMESH_API virtual ~RendererService() throw() ;

    /**
    * @brief VTK event managing of the VTK Camera position.
    *
    * This method is used to update the VTK camera position.
    */
    void notifyCamPositionUpdated();

    /**
    * @brief Slot to receive message with new camera position. Update camera position.
    * @param[in] _msg ::fwServices::ObjectMsg::csptr.
    */
    VTKCOMPOSITEMESH_API virtual void updateCamPosition(const double positionValue[3], const double focalValue[3], const double viewUpValue[3] );

protected :

    /**
    * @brief Starting method.
    *
    * This method is used to initialize the service.
    * Initialize VTK renderer and wxWidget containers
    */
    VTKCOMPOSITEMESH_API virtual void starting() throw(fwTools::Failed);


    /**
    * @brief Configuring method.
    *
    * XML configuration sample:
    * @verbatim
    <service impl="::vtkCompositeMesh::RendererService" type="::fwRender::IRender" autoConnect="yes" >
            <masterSlaveRelation>master</masterSlaveRelation>
            <win guiContainerId="900"/>
    </service>
    @endverbatim
    * This method is used to configure the service.
    */
    VTKCOMPOSITEMESH_API virtual void configuring() throw(::fwTools::Failed);


    /**
    * @brief Stopping method.
    *
    * Destroy VTK renderer and wxWidget containers
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
    * This method is used to update the service.
    */
    VTKCOMPOSITEMESH_API virtual void receiving( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed);

    /// @brief vtk renderer
    vtkRenderer * m_render ;

private :

    /// @brief VTK Interactor window manager
    ::fwRenderVTK::IVtkRenderWindowInteractorManager::sptr m_interactorManager;

    /**
    * @brief Creating  and rendering of an actor method.
    *
    * This method is used to create an actor and put it through the VTK pipeline.
    */
    void createAndAddActorToRender();

    /**
    * @brief VTK pipeline initialization method.
    *
    * This method is used to initialize the VTK pipeline.
    */
    void initVTKPipeline();

    /**
    * @brief VTK pipeline updating method.
    *
    * This method is used to update the VTK pipeline.
    */
    void updateVTKPipeline();



    /**
    * @brief Contains the mesh, and allows to compute normals.
    */
    vtkPolyDataNormals* m_normals;

    /**
    * @brief the m_bPipelineIsInit value is \b true
    * if the pipeline is initialized.
    */
    bool m_bPipelineIsInit;

    vtkCommand* m_loc;

    /// Slot to call updateCamPosition method
    UpdateCamPositionSlotType::sptr m_slotUpdateCamPosition;

    CamUpdatedSignalType::sptr m_sigCamUpdated;


};

}

#endif /* VTKCOMPOSITEMESH_RENDERER_SERVICE_HPP_ */
