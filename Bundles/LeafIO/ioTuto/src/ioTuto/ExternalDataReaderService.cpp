/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioTuto/ExternalDataReaderService.hpp"

#include <fwData/Composite.hpp>
#include <fwData/String.hpp>
#include <fwData/TransformationMatrix3D.hpp>
#include <fwData/location/Folder.hpp>
#include <fwData/location/SingleFile.hpp>

#include <fwDataTools/helper/Composite.hpp>

#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>

#include <fwServices/macros.hpp>

#include <boost/filesystem/operations.hpp>

#include <fstream>
#include <iostream>

fwServicesRegisterMacro( ::io::IReader, ::ioTuto::ExternalDataReaderService, ::fwData::Composite );

namespace ioTuto
{

//-----------------------------------------------------------------------------

ExternalDataReaderService::ExternalDataReaderService()
{
}

//-----------------------------------------------------------------------------

void ExternalDataReaderService::info(std::ostream& _sstream )
{
    this->::io::IReader::info( _sstream );
    _sstream << std::endl << " External data file reader";
}

//-----------------------------------------------------------------------------

std::vector< std::string > ExternalDataReaderService::getSupportedExtensions()
{
    std::vector< std::string > extensions;
    extensions.push_back(".us");
    return extensions;
}

//-----------------------------------------------------------------------------

ExternalDataReaderService::~ExternalDataReaderService() throw()
{
}

//------------------------------------------------------------------------------

void ExternalDataReaderService::configuring( ) throw(::fwTools::Failed)
{
    if( m_configuration->findConfigurationElement("filename") )
    {
        std::string filename = m_configuration->findConfigurationElement("filename")->getValue();
        OSLM_INFO( "ExternalDataReaderService::configure filename: " << filename );
        this->setFile(filename);
    }
}

//------------------------------------------------------------------------------

void ExternalDataReaderService::configureWithIHM()
{
    static ::boost::filesystem::path _sDefaultPath;

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle("Choose an external data file");
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("us", "*.us");
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::READ);
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::FILE_MUST_EXIST);

    ::fwData::location::SingleFile::sptr result;
    result = ::fwData::location::SingleFile::dynamicCast( dialogFile.show() );
    if (result)
    {
        _sDefaultPath = result->getPath();
        this->setFile(result->getPath());
    }
    else
    {
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

void ExternalDataReaderService::updating() throw(::fwTools::Failed)
{
    this->configureWithIHM();

    std::string imageName;
    ::fwData::Composite::sptr dataComposite = this->getObject< ::fwData::Composite >();
    ::fwDataTools::helper::Composite compositeHelper(dataComposite);
    SLM_ASSERT("dataComposite not instanced", dataComposite);
    try
    {
        if (this->hasLocationDefined())
        {
            // reading of the file
            std::fstream file;
            file.open(this->getFile().string().c_str(), std::fstream::in);
            if (!file.is_open())
            {
                OSLM_ERROR( "External data file loading error for " << this->getFile());
                std::string str = "Unable to open ";
                str += this->getFile().string();
                throw std::ios_base::failure(str);
            }
            file >> imageName;
            int readedValue = 0;
            double value;
            ::fwData::TransformationMatrix3D::sptr transformation1 = ::fwData::TransformationMatrix3D::New();
            ::fwData::TransformationMatrix3D::sptr transformation2 = ::fwData::TransformationMatrix3D::New();
            while(!file.eof())
            {
                readedValue = 0;
                while ( !file.eof() && readedValue<32 )
                {
                    file >> value;
                    if  (readedValue<16)
                    {
                        transformation1->getRefCoefficients()[readedValue] = value;
                    }
                    else
                    {
                        transformation2->getRefCoefficients()[readedValue] = value;
                    }
                    readedValue++;
                }
            }
            file.close();
            // TF1 contains the first and third transformations
            if(dataComposite->find("TF1") == dataComposite->end() )
            {
                compositeHelper.add("TF1", transformation1);
            }
            else
            {
                compositeHelper.swap("TF1", transformation1);
            }
            // TF2 contains the first and third transformations
            if(dataComposite->find("TF2") == dataComposite->end() )
            {
                compositeHelper.add("TF2", transformation2);
            }
            else
            {
                compositeHelper.swap("TF2", transformation2);
            }
            ::fwData::String::sptr imageNameStr = ::fwData::String::New(imageName);
            if(dataComposite->find("filename") == dataComposite->end() )
            {
                compositeHelper.add("filename", imageNameStr);
            }
            else
            {
                compositeHelper.swap("filename", imageNameStr);
            }
            SLM_ASSERT("Unable to open '"+this->getFile().string()+"'.", readedValue == 32 );
        }
    }
    catch(std::ios_base::failure& exception)
    {
        OSLM_ERROR( "External data file loading error for " << exception.what());
    }
}

//-----------------------------------------------------------------------------

::io::IOPathType ExternalDataReaderService::getIOPathType() const
{
    return ::io::FILE;
}

//------------------------------------------------------------------------------

}

