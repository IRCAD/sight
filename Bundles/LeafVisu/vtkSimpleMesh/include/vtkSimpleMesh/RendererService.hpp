/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VTKSIMPLEMESH_RENDERERSERVICE_HPP__
#define __VTKSIMPLEMESH_RENDERERSERVICE_HPP__

#include "vtkSimpleMesh/config.hpp"

#include <fwRenderVTK/IVtkRenderWindowInteractorManager.hpp>

#include <fwRender/IRender.hpp>

#include <fwServices/ObjectMsg.hpp>

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
 * @brief   Renderer service.
 * @class   RendererService
 *
 * Service rendering a ::fwData::Mesh using VTK.
 *
 * Service registered details : \n
 * fwServicesRegisterMacro( ::fwRender::IRender , ::vtkSimpleMesh::RendererService , ::fwData::Mesh)
 */
class VTKSIMPLEMESH_CLASS_API RendererService : public fwRender::IRender
{
public:

    fwCoreServiceClassDefinitionsMacro ( (RendererService)(::fwRender::IRender) );

    typedef ::boost::shared_array< double > SharedArray;

    VTKSIMPLEMESH_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_CAM_POSITION_SLOT;
    typedef ::fwCom::Slot<void (SharedArray, SharedArray, SharedArray)> UpdateCamPositionSlotType;

    VTKSIMPLEMESH_API static const ::fwCom::Signals::SignalKeyType s_CAM_UPDATED_SIG;
    typedef ::fwCom::Signal< void (SharedArray, SharedArray, SharedArray) > CamUpdatedSignalType;

    /**
     * @brief    Constructor
     */
    VTKSIMPLEMESH_API RendererService() throw();

    /**
     * @brief    Destructor
     */
    VTKSIMPLEMESH_API virtual ~RendererService() throw();


    /// Slot to receive new camera information (position, focal, viewUp). Update camera with new information.
    void updateCamPosition(SharedArray positionValue,
                           SharedArray focalValue,
                           SharedArray viewUpValue);

    /// This method is used to notify that the VTK camera position is updated.
    void notifyCamPositionUpdated();


protected:

    /**
     * @brief Starting method.
     *
     * This method is used to initialize the service.
     * Initialize VTK renderer and qt containers
     */
    VTKSIMPLEMESH_API virtual void starting() throw(fwTools::Failed);


    /**
     * @brief Configuring method.
     *
     * XML configuration sample:
     * @verbatim
       <service impl="::vtkSimpleMesh::RendererService" type="::fwRender::IRender" autoConnect="yes" />
       @endverbatim
     * This method is used to configure the service. Initialize qt container.
     */
    VTKSIMPLEMESH_API virtual void configuring() throw(::fwTools::Failed);


    /**
     * @brief Stopping method.
     *
     * Destroy VTK renderer and qt containers
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
     * This method is used to update the vtk pipeline when the mesh is modified.
     */
    VTKSIMPLEMESH_API virtual void receiving( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed);

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

    /**
     * @brief the m_bPipelineIsInit value is \b true
     * if the pipeline is initialized.
     */
    bool m_bPipelineIsInit;

    vtkCommand* m_loc;

    /// Slot to call updateCamPosition method
    UpdateCamPositionSlotType::sptr m_slotUpdateCamPosition;

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

#endif /* __VTKSIMPLEMESH_RENDERERSERVICE_HPP__ */
