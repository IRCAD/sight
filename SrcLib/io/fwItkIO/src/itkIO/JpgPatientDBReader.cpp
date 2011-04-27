/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>

#include <fwCore/base.hpp>

#include <fwData/PatientDB.hpp>
#include <fwData/Patient.hpp>
#include <fwData/Study.hpp>
#include <fwData/Acquisition.hpp>
#include <fwData/Image.hpp>
#include <fwTools/Factory.hpp>

#include "itkIO/ImageReader.hpp"
#include "itkIO/JpgPatientDBReader.hpp"

namespace itkIO
{

//------------------------------------------------------------------------------

JpgPatientDBReader::JpgPatientDBReader() :
::fwData::location::enableFolder< ::fwDataIO::reader::IObjectReader >(this)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

JpgPatientDBReader::~JpgPatientDBReader()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void JpgPatientDBReader::searchRecursivelyInrFile
( const ::boost::filesystem::path & path, ::std::vector< ::boost::filesystem::path > & inrFiles )
{
    for(boost::filesystem::directory_iterator it(path); it != boost::filesystem::directory_iterator(); ++it)
    {
        if( ::boost::filesystem::is_directory(*it) )
        {
            searchRecursivelyInrFile(*it,inrFiles);
        }
        else
        {
            std::string fileStr = (*it).string();
            if ( fileStr.size() > 7 )
            {
                std::string ext1 = fileStr.substr(fileStr.size()-4,4);
                std::string ext2 = fileStr.substr(fileStr.size()-5,5);

                if( ext1 == ".jpg" || ext1 == ".JPG" || ext1 == ".jpeg" || ext1 == ".JPEG" )
                {
                    OSLM_DEBUG( "JPEG File Found : " << it->string() );
                    inrFiles.push_back(*it);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

std::string JpgPatientDBReader::findCommonPath( ::std::vector< ::boost::filesystem::path > & inrFilePathes )
{
    bool newCommonLetterFound = true;

    unsigned int minimunPathSize = inrFilePathes[0].string().size();
    for (   ::std::vector< ::boost::filesystem::path >::iterator inrFilePath = inrFilePathes.begin();
    inrFilePath != inrFilePathes.end();
    inrFilePath++ )
    {
        if ( minimunPathSize > inrFilePath->string().size() )
        {
            minimunPathSize = inrFilePath->string().size();
        }
    }

    std::string str = "";
    unsigned int cmpt = 0;

    while( newCommonLetterFound && minimunPathSize != cmpt )
    {
        str = inrFilePathes[0].string().substr(0,cmpt);
        for (   ::std::vector< ::boost::filesystem::path >::iterator inrFilePath = inrFilePathes.begin();
        inrFilePath != inrFilePathes.end();
        inrFilePath++ )
        {
            if ( inrFilePath->string().substr(0,cmpt) != str )
            {
                newCommonLetterFound = false;
            }
        }
        cmpt++;
    }


    int indexA = str.find_last_of("\\");
    int indexB = str.find_last_of("/");

    int index = 0;
    if ( -1 !=  indexA )
    {
        index = indexA + 1;
    }
    if ( -1 !=  indexB && indexB > index)
    {
        index = indexB + 1 ;
    }

    OSLM_DEBUG("JpgPatientDBReader::findCommonPath : CommonPath = " << str.substr(0,index));

    return str.substr(0,index);
}

//------------------------------------------------------------------------------

::fwData::Patient::sptr JpgPatientDBReader::createPatient( const ::boost::filesystem::path inrFilePath, int commonPathSize )
{
    ::fwData::Image::NewSptr img;

    ::itkIO::ImageReader::NewSptr reader;
    reader->setObject(img);
    reader->setFile(inrFilePath);
    reader->read();

    ::fwData::Acquisition::NewSptr acq;
    ::fwData::Study::NewSptr       study;
    ::fwData::Patient::NewSptr     patient;

    acq->setImage(img);
    study->addAcquisition(acq);
    patient->addStudy(study);
    patient->setCRefName( inrFilePath.string().substr(commonPathSize,-1) );
    return patient;
}
//------------------------------------------------------------------------------

void JpgPatientDBReader::read()
{
    SLM_TRACE_FUNC();

    assert( ::boost::filesystem::exists( getFolder() ) );

    ::std::vector< ::boost::filesystem::path > inrFilePathes;
    searchRecursivelyInrFile( getFolder(), inrFilePathes );

    if ( inrFilePathes.size() > 0 )
    {
        std::string commonPath = findCommonPath(inrFilePathes);
        int commonPathSize = commonPath.size();
        float numFile = 0;

        for (   ::std::vector< ::boost::filesystem::path >::iterator inrFilePath = inrFilePathes.begin();
        inrFilePath != inrFilePathes.end();
        inrFilePath++ )
        {
            getConcreteObject()->addPatient( createPatient( *inrFilePath,  commonPathSize ) );
            notifyProgress( ++numFile*1.0/inrFilePathes.size(), inrFilePath->string() );
        }
    }
}

//------------------------------------------------------------------------------

} // namespace itkIO
