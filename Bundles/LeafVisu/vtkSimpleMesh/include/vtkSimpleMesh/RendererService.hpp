/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef VTKSIMPLEMESH_RENDERER_SERVICE_HPP_
#define VTKSIMPLEMESH_RENDERER_SERVICE_HPP_

#include <fwRenderVTK/IVtkRenderWindowInteractorManager.hpp>

#include <vtkCommand.h>

#include <fwRender/IRender.hpp>

#include <fwServices/ObjectMsg.hpp>

#include "vtkSimpleMesh/config.hpp"

// VTK
class vtkRenderer;
class vtkPolyDataNormals;

namespace vtkSimpleMesh
{

/**
 * @brief   Renderer service.
 * @class   RendererService.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 *
 * Service rendering a ::fwData::Mesh using VTK.
 *
 * Service registered details : \n
 * fwServicesRegisterMacro( ::fwRender::IRender , ::vtkSimpleMesh::RendererService , ::fwData::Mesh)
 */
class VTKSIMPLEMESH_CLASS_API RendererService : public fwRender::IRender
{
public :

    fwCoreServiceClassDefinitionsMacro ( (RendererService)(::fwRender::IRender) ) ;

    VTKSIMPLEMESH_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_CAM_POSITION_SLOT;
    typedef ::fwCom::Slot<void(const double*, const double*, const double*)> UpdateCamPositionSlotType;

    VTKSIMPLEMESH_API static const ::fwCom::Signals::SignalKeyType s_CAM_UPDATED_SIG;
    typedef ::fwCom::Signal< void (const double*, const double*, const double*) > CamUpdatedSignalType;

    /**
    * @brief    Constructor
    */
    VTKSIMPLEMESH_API RendererService() throw() ;

    /**
    * @brief    Destructor
    */
    VTKSIMPLEMESH_API virtual ~RendererService() throw() ;


    /// Slot to receive message with new camera position. Update camera position with message information.
    void updateCamPosition(const double positionValue[3], const double focalValue[3], const double viewUpValue[3]);

    /**
    * @brief VTK event managing of the VTK Camera position.
    *
    * This method is used to update the VTK camera position.
    */
    void notifyCamPositionUpdated();


protected :

    /**
    * @brief Starting method.
    *
    * This method is used to initialize the service.
    * Initialize VTK renderer and wxWidget containers
    */
    VTKSIMPLEMESH_API virtual void starting() throw(fwTools::Failed);


    /**
    * @brief Configuring method.
    *
    * XML configuration sample:
    * @verbatim
    <service impl="::vtkSimpleMesh::RendererService" type="::fwRender::IRender" autoConnect="yes" >
            <win guiContainerId="900"/>
    </service>
    @endverbatim
    * This method is used to configure the service.
    */
    VTKSIMPLEMESH_API virtual void configuring() throw(::fwTools::Failed);


    /**
    * @brief Stopping method.
    *
    * Destroy VTK renderer and wxWidget containers
    */
    VTKSIMPLEMESH_API virtual void stopping() throw(fwTools::Failed);

    /**
    * @brief Updating method.
    *
    * This method is used to update the service.
    * Make a render if necessary
    */
    VTKSIMPLEMESH_API virtual void updating() throw(fwTools::Failed);

    /**
    * @brief Updating method (react on data modifications).
    * @param[in] _msg ::fwServices::ObjectMsg::csptr.
    *
    * This method is used to update the service.
    */
    VTKSIMPLEMESH_API virtual void receiving( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed);

    /// @brief vtk renderer
    vtkRenderer * m_render ;

private :

    /// @brief VTK Interactor window manager
    ::fwRenderVTK::IVtkRenderWindowInteractorManager::sptr m_interactorManager;

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
    void updateVTKPipeline(bool resetCamera = true);

    /**
    * @brief the m_bPipelineIsInit value is \b true
    * if the pipeline is initialized.
    */
    bool m_bPipelineIsInit;

    vtkCommand* m_loc;

    /// Slot to call updateCamPosition method
    UpdateCamPositionSlotType::sptr m_slotUpdateCamPosition;

    CamUpdatedSignalType::sptr m_sigCamUpdated;

    /// To store the polyData
    vtkSmartPointer<vtkPolyData> m_vtkPolyData;

    /// Vtk mapper
    vtkSmartPointer<vtkPolyDataMapper> m_mapper;

    /// Hight resolution timer to log information about computing function time
    ::fwCore::HiResTimer m_hiResTimer;
};

}

#endif /* VTKSIMPLEMESH_RENDERER_SERVICE_HPP_ */
