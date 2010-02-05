/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef VTKSIMPLENEGATO_RENDERER_SERVICE_HPP_
#define VTKSIMPLENEGATO_RENDERER_SERVICE_HPP_

#include <wx/wxprec.h>
#include <wx/aui/aui.h>

#include  <vtkRenderer.h>
#include  <vtkImagePlaneWidget.h>
#include  <vtkOutlineFilter.h>

#include <vtkinria3d/wxVTKRenderWindowInteractor.h>

#include <fwRender/IRender.hpp>

#include "vtkSimpleNegato/config.hpp"

namespace vtkSimpleNegato
{

/**
* @brief    Renderer service.
* @class    RendererService.
* @author   IRCAD (Research and Development Team).
* @date 2009.
*
* Service rendering a ::fwData::Image using VTK.
*
* Service registered details : \n
* REGISTER_SERVICE( ::fwRender::IRender , ::vtkSimpleNegato::RendererService , ::fwData::Image)
*/
class VTKSIMPLENEGATO_CLASS_API RendererService : public fwRender::IRender
{
public :
    fwCoreServiceClassDefinitionsMacro ( (RendererService)(::fwRender::IRender::Baseclass) );

    /**
    * @brief    Constructor
    */
    VTKSIMPLENEGATO_API RendererService() throw() ;

    /**
    * @brief    Destructor
    */
    VTKSIMPLENEGATO_API virtual ~RendererService() throw() ;


protected :

    /**
    * @brief Starting method.
    *
    * This method is used to initialize the service.
    * Initialize VTK renderer and wxWidget containers
    */
    VTKSIMPLENEGATO_API virtual void starting() throw(fwTools::Failed);

    /**
    * @brief Stopping method.
    *
    * Destroy VTK renderer and wxWidget containers
    */
    VTKSIMPLENEGATO_API virtual void stopping() throw(fwTools::Failed);

    /**
    * @brief Updating method.
    *
    * This method is used to update the service.
    * Make a render if necessary
    */
    VTKSIMPLENEGATO_API virtual void updating() throw(fwTools::Failed);

    /**
    * @brief Configuring method.
    *
    * This method is used to configure the service.
    */
    VTKSIMPLENEGATO_API virtual void configuring() throw(::fwTools::Failed);

    /**
    * @brief Updating method (react on data modifications).
    * @param[in] _msg ::fwServices::ObjectMsg::csptr.
    *
    * This method is used to update the service.
    */
    VTKSIMPLENEGATO_API virtual void updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed);

    /// @brief vtk renderer
    vtkRenderer * m_render ;

private :
    /// @brief required to facilitate resize of an empty vtk rendering window : why ?
    wxAuiManager* m_wxmanager;
    /// @brief VTK Interactor window
    ::wxVTKRenderWindowInteractor* m_interactor;

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

    /// @brief image wireframe outline.
    vtkOutlineFilter* m_outline;

    /// @brief vtk widget used in the negatoscope axial plane.
    vtkImagePlaneWidget* m_negatoAxial;

    /// @brief vtk widget used in the negatoscope sagittal plane.
    vtkImagePlaneWidget* m_negatoSagittal;

    /// @brief vtk widget used in the negatoscope frontal plane.
    vtkImagePlaneWidget* m_negatoFrontal;

    /**
    * @brief the m_bPipelineIsInit value is \b true
    * if the pipeline is initialized.
    */
    bool m_bPipelineIsInit;
};

}


#endif /* VTKSIMPLENEGATO_RENDERER_SERVICE_HPP_ */
