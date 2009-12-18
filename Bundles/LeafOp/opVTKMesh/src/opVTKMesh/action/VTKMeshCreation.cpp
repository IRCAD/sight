/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/UUID.hpp>

#include <fwData/Image.hpp>
#include <fwData/TriangularMesh.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwComEd/TriangularMeshMsg.hpp>

#include <vtkIO/vtk.hpp>

#include <vtkDiscreteMarchingCubes.h>
#include <vtkWindowedSincPolyDataFilter.h>
#include <vtkThreshold.h>
#include <vtkPolyDataMapper.h>
#include <vtkDecimatePro.h>
#include <vtkGeometryFilter.h>

#include "opVTKMesh/action/VTKMeshCreation.hpp"

namespace opVTKMesh
{

namespace action
{

//-----------------------------------------------------------------------------

REGISTER_SERVICE( ::gui::action::IAction , ::opVTKMesh::action::VTKMeshCreation , ::fwTools::Object ) ;

//-----------------------------------------------------------------------------

VTKMeshCreation::VTKMeshCreation() throw() :
	m_imageUID(""),
	m_meshUID(""),
	m_reduction(0)
{}

//-----------------------------------------------------------------------------

VTKMeshCreation::~VTKMeshCreation() throw()
{}

//-----------------------------------------------------------------------------

void VTKMeshCreation::starting() throw ( ::fwTools::Failed )
{
	::gui::action::IAction::starting();
}

//-----------------------------------------------------------------------------

void VTKMeshCreation::stopping() throw ( ::fwTools::Failed )
{
	::gui::action::IAction::stopping();
}

//-----------------------------------------------------------------------------

void VTKMeshCreation::updating( fwServices::ObjectMsg::csptr _pMsg ) throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void VTKMeshCreation::configuring() throw ( ::fwTools::Failed )
{
	::gui::action::IAction::configuring();

	SLM_ASSERT( "Mesh UID andImage UID must be defined in the service configuration",  m_configuration->findConfigurationElement("image") && m_configuration->findConfigurationElement("mesh") );

	
	m_imageUID = m_configuration->findConfigurationElement("image")->getExistingAttributeValue("uid");

	m_meshUID = m_configuration->findConfigurationElement("mesh")->getExistingAttributeValue("uid");

	if (m_configuration->findConfigurationElement("percentReduction") && m_configuration->findConfigurationElement("percentReduction")->hasAttribute("value"))
	{
		std::string reduce = m_configuration->findConfigurationElement("percentReduction")->getExistingAttributeValue("value");
		m_reduction = boost::lexical_cast<unsigned int>(reduce);
	}

	OSLM_INFO( "Image UID = " << m_imageUID);
	OSLM_INFO( "Mesh UID = " << m_meshUID);
	OSLM_INFO( "Reduction value = " << m_reduction);
}

//-----------------------------------------------------------------------------

void VTKMeshCreation::updating() throw ( ::fwTools::Failed )
{
	SLM_TRACE_FUNC();

	/// Retreive object
	OSLM_ASSERT("Not found the image defined by uid : " << m_imageUID, ::fwTools::UUID::exist(m_imageUID, ::fwTools::UUID::SIMPLE )) ;
	::fwData::Image::sptr pImage = ::fwTools::UUID::get< ::fwData::Image >( m_imageUID ) ;
	OSLM_ASSERT("Not found the mesh defined by uid : " << m_meshUID, ::fwTools::UUID::exist(m_meshUID, ::fwTools::UUID::SIMPLE )) ;
	::fwData::TriangularMesh::sptr pMesh = ::fwTools::UUID::get< ::fwData::TriangularMesh >( m_meshUID ) ;

	///VTK Mesher

	// vtk img
	vtkImageData * vtkImage = ::vtkIO::toVTKImage( pImage );

	// contour filter
	vtkDiscreteMarchingCubes * contourFilter = vtkDiscreteMarchingCubes ::New();
	contourFilter->SetInput((vtkDataObject *)vtkImage);
	contourFilter->SetValue(0, 255);
	contourFilter->ComputeScalarsOn();
	contourFilter->ComputeNormalsOn();
	contourFilter->Update();

	// smooth filter
	vtkWindowedSincPolyDataFilter * smoothFilter = vtkWindowedSincPolyDataFilter::New();
	smoothFilter->SetInput(contourFilter->GetOutput());
	smoothFilter->SetNumberOfIterations( 50 );
	smoothFilter->BoundarySmoothingOn();
	smoothFilter->SetPassBand ( 0.1 );
	smoothFilter->SetFeatureAngle(120.0);
	smoothFilter->SetEdgeAngle(90);
	smoothFilter->FeatureEdgeSmoothingOn();
	smoothFilter->Update();


	// Get polyData
	  vtkPolyData * polyData;

	  // decimate filter
	  unsigned int reduction = m_reduction;
	  if( reduction > 0 )
	  {
	      vtkDecimatePro * decimate = vtkDecimatePro::New();
	      decimate->SetInput( smoothFilter->GetOutput() );
	      decimate->SetTargetReduction( reduction/100.0 );
	      decimate->PreserveTopologyOff();
	      decimate->SplittingOn();
	      decimate->BoundaryVertexDeletionOn();
	      decimate->SetSplitAngle( 120 );
	      decimate->Update();
	      polyData = decimate->GetOutput();
	  }
	  else
	  {
	      polyData = smoothFilter->GetOutput();
	  }


	OSLM_TRACE("final GetNumberOfCells = " << polyData->GetNumberOfCells());
	bool res = ::vtkIO::fromVTKMesh( polyData, pMesh);

	/// Notification
	::fwComEd::TriangularMeshMsg::NewSptr msg;;
	msg->addEvent( ::fwComEd::TriangularMeshMsg::NEW_MESH ) ;
	::fwServices::IEditionService::notify( this->getSptr(), pMesh, msg );
}

//-----------------------------------------------------------------------------

void VTKMeshCreation::info ( std::ostream &_sstream )
{}

//-----------------------------------------------------------------------------

} }
