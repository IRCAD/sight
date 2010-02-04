/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <fstream>
#include <wx/wx.h>

#include <boost/filesystem/operations.hpp>

#include <io/IReader.hpp>
#include <fwServices/helper.hpp>
#include <fwServices/ObjectMsg.hpp>
#include <fwServices/IEditionService.hpp>
#include <fwData/TriangularMesh.hpp>
#include <fwCore/base.hpp>
#include <fwServices/macros.hpp>

#include <fwDataIO/writer/TriangularMeshWriter.hpp>

#include "ioData/TriangularMeshWriterService.hpp"

REGISTER_SERVICE( ::io::IWriter , ::ioData::TriangularMeshWriterService , ::fwData::TriangularMesh ) ;

namespace ioData
{

TriangularMeshWriterService::TriangularMeshWriterService()
{
}

//-----------------------------------------------------------------------------

void TriangularMeshWriterService::info(std::ostream &_sstream )
{
	this->SuperClass::info( _sstream ) ;
	_sstream << std::endl << " Triangular Mesh writer" ;
}

//-----------------------------------------------------------------------------

std::vector< std::string > TriangularMeshWriterService::getSupportedExtensions()
{
	std::vector< std::string > extensions ;
	extensions.push_back(".trian");
	return extensions ;
}

//-----------------------------------------------------------------------------

TriangularMeshWriterService::~TriangularMeshWriterService() throw()
{
}

//------------------------------------------------------------------------------

void TriangularMeshWriterService::configuring( ) throw(::fwTools::Failed)
{
	OSLM_INFO( "TriangularMeshWriterService::configure : " << *m_configuration );
	if( m_configuration->findConfigurationElement("filename") )
	{
		std::string filename = m_configuration->findConfigurationElement("filename")->getValue() ;
		OSLM_INFO( "TriangularMeshWriterService::configure filename: " << filename );
		::boost::filesystem::path location = boost::filesystem::path( filename ) ;
		m_filename = location;
	}
}

//------------------------------------------------------------------------------

void TriangularMeshWriterService::configureWithIHM()
{
	static wxString _sDefaultPath = _("");
	wxString title = _("Choose an TrianMesh file");
	wxString folder = wxFileSelector(
			title,
			_sDefaultPath,
			wxT(""),
			wxT(""),
			wxT("TrianMesh (*.trian)|*.trian"),
			wxFD_SAVE,
			wxTheApp->GetTopWindow() );

	if( folder.IsEmpty() == false)
	{
		m_filename = ::boost::filesystem::path( wxConvertWX2MB(folder), ::boost::filesystem::native );
		_sDefaultPath = wxConvertMB2WX( m_filename.branch_path().string().c_str() );
	}
}

//------------------------------------------------------------------------------

void TriangularMeshWriterService::updating() throw(::fwTools::Failed)
{
	SLM_INFO("[TriangularMeshWriterService::update]");

	if ( !m_filename.empty() )
	{
		// Retrieve object
		::fwData::TriangularMesh::sptr mesh = this->getObject< ::fwData::TriangularMesh >( );
		assert( mesh ) ;

		::fwDataIO::writer::TriangularMeshWriter writer;
		writer.setObject( mesh );
		writer.setFile(m_filename);
		writer.write();
	}
}
}
