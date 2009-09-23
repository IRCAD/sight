/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef VTKSIMPLEMESH_RENDERER_SERVICE_HPP_
#define VTKSIMPLEMESH_RENDERER_SERVICE_HPP_

#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/aui/aui.h>


#include <vtkinria3d/wxVTKRenderWindowInteractor.h>

#include <vtkCommand.h>

#include <fwRender/IRender.hpp>

#include <fwServices/ObjectMsg.hpp>

#include <fwData/Image.hpp>

#include "vtkSimpleMesh/config.hpp"

// VTK
class vtkRenderer;
class vtkPolyDataNormals;

namespace vtkSimpleMesh
{

/**
 * @brief	Renderer service.
 * @class	RendererService.
 * @author	IRCAD (Research and Development Team).
 * @date	2009.
 *
 * Service rendering a ::fwData::TriangularMesh using VTK.
 *
 * Service registered details : \n
 * REGISTER_SERVICE( ::fwRender::IRender , ::vtkSimpleMesh::RendererService , ::fwData::TriangularMesh)
 */
class VTKSIMPLEMESH_CLASS_API RendererService : public fwRender::IRender
{
public :

    fwCoreServiceClassDefinitionsMacro ( (RendererService)(::fwRender::IRender::Baseclass) ) ;

    /**
	* @brief	Constructor
	*/
	VTKSIMPLEMESH_API RendererService() throw() ;

	/**
	* @brief	Destructor
	*/
	VTKSIMPLEMESH_API virtual ~RendererService() throw() ;

    /**
	* @brief VTK event managing of the VTK Camera position.
	*
	* This method is used to update the VTK camera position.
	*/
	void updateCamPosition();


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
	<service implementation="::vtkSimpleMesh::RendererService" type="::fwRender::IRender" autoComChannel="yes" >
            <masterSlaveRelation>master</masterSlaveRelation>
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
	VTKSIMPLEMESH_API virtual void updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed);

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

    bool m_isCamMaster;

};

}

#endif /* VTKSIMPLEMESH_RENDERER_SERVICE_HPP_ */
