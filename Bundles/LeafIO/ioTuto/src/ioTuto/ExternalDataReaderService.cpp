/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
#include <iostream>
#include <fstream>
#include <boost/filesystem/operations.hpp>
#include <fwServices/helper.hpp>
#include <fwServices/macros.hpp>

#include "ioTuto/ExternalDataReaderService.hpp"
#include "fwData/Composite.hpp"
#include "fwData/String.hpp"
#include "fwData/TransformationMatrix3D.hpp"

REGISTER_SERVICE( ::io::IReader , ::ioTuto::ExternalDataReaderService , ::fwData::Composite ) ;

namespace ioTuto
{

//-----------------------------------------------------------------------------
ExternalDataReaderService::ExternalDataReaderService()
{
}

//-----------------------------------------------------------------------------
void ExternalDataReaderService::info(std::ostream &_sstream )
{
    this->::io::IReader::info( _sstream ) ;
    _sstream << std::endl << " External data file reader" ;
}

//-----------------------------------------------------------------------------
std::vector< std::string > ExternalDataReaderService::getSupportedExtensions()
{
    std::vector< std::string > extensions ;
    extensions.push_back(".us");
    return extensions ;
}

//-----------------------------------------------------------------------------
ExternalDataReaderService::~ExternalDataReaderService() throw()
{
}

//------------------------------------------------------------------------------
void ExternalDataReaderService::configuring( ) throw(::fwTools::Failed)
{
    OSLM_INFO( "ExternalDataReaderService::configure : " << *m_configuration );
    if( m_configuration->findConfigurationElement("filename") )
    {
        std::string filename = m_configuration->findConfigurationElement("filename")->getValue() ;
        OSLM_INFO( "ExternalDataReaderService::configure filename: " << filename );
        m_fsExternalDataPath = boost::filesystem::path( filename );
    }
}

//------------------------------------------------------------------------------
void ExternalDataReaderService::configureWithIHM()
{
    static wxString _sDefaultPath = _("");
    wxString title = _("Choose an external data file");
    wxString folder = wxFileSelector(
            title,
            _sDefaultPath,
            wxT(""),
            wxT(""),
            wxT("External data (*.us)|*.us"),
            wxFD_FILE_MUST_EXIST,
            wxTheApp->GetTopWindow() );

    if( folder.IsEmpty() == false)
    {
        m_fsExternalDataPath = ::boost::filesystem::path( wxConvertWX2MB(folder), ::boost::filesystem::native );
        _sDefaultPath = wxConvertMB2WX( m_fsExternalDataPath.branch_path().string().c_str() );
    }
}

//------------------------------------------------------------------------------
void ExternalDataReaderService::updating() throw(::fwTools::Failed)
{
    SLM_INFO("[ExternalDataReaderService::update]");

    configureWithIHM();

    std::string imageName;
    ::fwData::Composite::sptr dataComposite = this->getObject<::fwData::Composite>();
    assert( dataComposite ) ;
    try {
        if (::boost::filesystem::exists(m_fsExternalDataPath))
        {
            // reading of the file
            std::fstream file;
            file.open(m_fsExternalDataPath.native_file_string().c_str(), std::fstream::in);
            if (!file.is_open())
            {
                OSLM_ERROR( "External data file loading error for " << m_fsExternalDataPath);
                std::string str = "Unable to open ";
                str+= m_fsExternalDataPath.native_file_string();
                throw std::ios_base::failure(str);
            }
            file >> imageName;
            dataComposite->getRefMap()["filename"] = ::fwData::String::NewSptr(imageName);
            // Clean all the field
            dataComposite->removeField("TF1");
            dataComposite->removeField("TF2");
            int readedValue = 0;
            double value;

            while(!file.eof())
            {
                readedValue = 0;
                ::fwData::TransformationMatrix3D::NewSptr transformation1;
                transformation1->getRefCoefficients().clear();
                ::fwData::TransformationMatrix3D::NewSptr transformation2;
                transformation2->getRefCoefficients().clear();
                while ( !file.eof() && readedValue<32 )
                {
                    file >> value;
                    if  (readedValue<16)
                    {
                        transformation1->getRefCoefficients().push_back( value );
                    }
                    else
                    {
                        transformation2->getRefCoefficients().push_back( value );
                    }
                    readedValue++;
                }
                // TF1 contains the first and third tranformations
                dataComposite->addFieldElement("TF1",transformation1);
                // TF2 contains the first and third tranformations
                dataComposite->addFieldElement("TF2",transformation2);
            }
            assert( readedValue==32 );
            file.close();
        }
    }
    catch(std::ios_base::failure exception)
    {
        OSLM_ERROR( "External data file loading error for " << exception.what());
}
}
}

