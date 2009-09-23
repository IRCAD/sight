/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <wx/wx.h>
#include <wx/version.h>

#include <fwServices/macros.hpp>
#include <fwServices/helper.hpp>
#include <fwServices/ObjectServiceRegistry.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwComEd/fieldHelper/BackupHelper.hpp>

#include <io/IWriter.hpp>

#include <fwCore/base.hpp>

#include <fwData/Image.hpp>

#include <fwWX/ProgressTowx.hpp>
#include <vtkIO/ImageWriter.hpp>

#include "ioVTK/ImageWriterService.hpp"


namespace ioVTK
{

REGISTER_SERVICE( ::io::IWriter , ::ioVTK::ImageWriterService , ::fwData::Image ) ;

//------------------------------------------------------------------------------

ImageWriterService::ImageWriterService() throw() :
	m_bServiceIsConfigured(false),
	m_fsImgPath("")
{}

//------------------------------------------------------------------------------

ImageWriterService::~ImageWriterService() throw()
{}

//------------------------------------------------------------------------------

void ImageWriterService::configuring() throw(::fwTools::Failed)
{
	if( m_configuration->findConfigurationElement("filename") )
	{
		std::string filename = m_configuration->findConfigurationElement("filename")->getExistingAttributeValue("id") ;
		m_fsImgPath = ::boost::filesystem::path( filename ) ;
		m_bServiceIsConfigured = ::boost::filesystem::exists(m_fsImgPath);
		OSLM_TRACE("Filename found" << filename ) ;
	}
}

//------------------------------------------------------------------------------

void ImageWriterService::configureWithIHM()
{
	static wxString _sDefaultPath = _("");
	wxString title = _("Choose an vtk file to save image");
	wxString file = wxFileSelector(
			title,
			_sDefaultPath,
			wxT(""),
			wxT(""),
			wxT("Vtk (*.vtk)|*.vtk"),
			wxFD_SAVE,

			wxTheApp->GetTopWindow() );

	if( file.IsEmpty() == false)
	{
		m_fsImgPath = ::boost::filesystem::path( wxConvertWX2MB(file), ::boost::filesystem::native );
		m_bServiceIsConfigured = true;
		_sDefaultPath = wxConvertMB2WX( m_fsImgPath.branch_path().string().c_str() );
	}
}

//------------------------------------------------------------------------------

void ImageWriterService::starting() throw(::fwTools::Failed)
{
	SLM_TRACE("ImageWriterService::starting()");
}

//------------------------------------------------------------------------------

void ImageWriterService::stopping() throw(::fwTools::Failed)
{
	SLM_TRACE("ImageWriterService::stopping()");
}

//------------------------------------------------------------------------------

void ImageWriterService::info(std::ostream &_sstream )
{
	_sstream << "ImageWriterService::info";
}

//------------------------------------------------------------------------------

bool ImageWriterService::saveImage( const ::boost::filesystem::path vtkFile, ::boost::shared_ptr< ::fwData::Image > _pImg )
{
	SLM_TRACE("ImageWriterService::saveImage");
	::vtkIO::ImageWriter myWriter;

	myWriter.setObject(_pImg);
	myWriter.setFile(vtkFile);

	bool bValue = true;

	try
	{
		::fwWX::ProgressTowx progressMeterGUI("Saving Images ");
		myWriter.addHandler( progressMeterGUI );
		myWriter.write();

	}
	catch (const std::exception & e)
	{
		std::stringstream ss;
		ss << "Warning during loading : " << e.what();
		wxString wxStmp( ss.str().c_str(), wxConvLocal );
		wxMessageBox( wxStmp, _("Warning"), wxOK|wxICON_WARNING );
		bValue = false;
	}
	catch( ... )
	{
		std::stringstream ss;
		ss << "Warning during loading : ";
		wxString wxStmp( ss.str().c_str(), wxConvLocal );
		wxMessageBox( wxStmp, _("Warning"), wxOK|wxICON_WARNING );
		bValue = false;
	}
	return bValue;
}

//------------------------------------------------------------------------------

void ImageWriterService::updating() throw(::fwTools::Failed)
{
	SLM_TRACE("ImageWriterService::updating()");

	if( m_bServiceIsConfigured )
	{
		// Retrieve dataStruct associated with this service
		::fwData::Image::sptr pImage = this->getObject< ::fwData::Image >() ;
		assert(pImage);

		wxBeginBusyCursor();
		saveImage(m_fsImgPath,pImage);
		wxEndBusyCursor();
	}
}

//------------------------------------------------------------------------------

} // namespace ioVtk
