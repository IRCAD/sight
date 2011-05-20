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

#include <itkIO/ImageReader.hpp>

#include "itkIO/InrPatientDBReader.hpp"

namespace itkIO
{

//------------------------------------------------------------------------------

InrPatientDBReader::InrPatientDBReader() :
::fwData::location::enableSingleFile< IObjectReader >(this),
::fwData::location::enableMultiFiles< IObjectReader >(this),
::fwData::location::enableFolder< IObjectReader >(this)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

InrPatientDBReader::~InrPatientDBReader()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void InrPatientDBReader::searchRecursivelyInrFile
(   const ::boost::filesystem::path & path,
    ::std::vector< ::boost::filesystem::path > & inrFiles,
    bool recursiveSearch )
{
    for( ::boost::filesystem::directory_iterator it(path); it != ::boost::filesystem::directory_iterator(); ++it)
    {

        if( ::boost::filesystem::is_directory(*it) && recursiveSearch )
        {
            searchRecursivelyInrFile(*it,inrFiles,recursiveSearch);
        }
        else
        {
            std::string fileStr = (*it).string();
            if ( fileStr.size() > 7 )
            {
                std::string ext1 = fileStr.substr(fileStr.size()-4,4);
                std::string ext2 = fileStr.substr(fileStr.size()-7,7);

                if( ext1 == ".inr" || ext2 == ".inr.gz" )
                {
                    OSLM_DEBUG( "Inr File Found : " << it->string() );
                    inrFiles.push_back(*it);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

std::string InrPatientDBReader::findCommonPath( ::std::vector< ::boost::filesystem::path > & inrFilePathes )
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

    OSLM_DEBUG("InrPatientDBReader::findCommonPath : CommonPath = " << str.substr(0,index));

    return str.substr(0,index);
}



// get percent for ONE file loading then take into account the whole load process then ask its parents to notify the real percent for the whole process
struct LocalFileHandler
{
    ::itkIO::InrPatientDBReader::sptr m_myParent;
    float m_startPercent;
    int m_nbFiles;
    void operator()( float OneFilePercent, std::string msg)
    {
        assert(m_myParent);
        assert(m_nbFiles);
        float realPercent = m_startPercent + OneFilePercent/m_nbFiles;
        m_myParent->notifyProgress(realPercent,msg);
    }
};




//------------------------------------------------------------------------------

::fwData::Patient::sptr InrPatientDBReader::createPatient( const ::boost::filesystem::path inrFilePath,
                                                           int commonPathSize,
                                                           float startPercent,
                                                           int nbFiles)
{
    ::fwData::Image::NewSptr img;
    ::itkIO::ImageReader::NewSptr reader;
    reader->setObject(img);
    reader->setFile(inrFilePath);

    LocalFileHandler oneFileHandler;
    oneFileHandler.m_myParent = this->getSptr();
    oneFileHandler.m_startPercent = startPercent;
    oneFileHandler.m_nbFiles = nbFiles;
    reader->addHandler ( oneFileHandler );

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

void InrPatientDBReader::read()
{
    SLM_TRACE_FUNC();

    ::std::vector< ::boost::filesystem::path > filesToRead;
    if ( ::boost::dynamic_pointer_cast< ::fwData::location::SingleFile >(m_location) )
    {
        filesToRead.push_back( this->getFile() );
    }

    if ( ::boost::dynamic_pointer_cast< ::fwData::location::MultiFiles >(m_location) )
    {
        filesToRead = this->getFiles();
    }

    if ( ::boost::dynamic_pointer_cast< ::fwData::location::Folder >(m_location) )
    {
        searchRecursivelyInrFile( this->getFolder(), filesToRead, this->getRecursive() );
    }

    assert ( filesToRead.size() );

    // do effective reading
    std::string commonPath = findCommonPath(filesToRead);
    int commonPathSize = commonPath.size();
    float startPercent=0.0;

    for (   ::std::vector< ::boost::filesystem::path >::iterator inrFilePath = filesToRead.begin();
            inrFilePath != filesToRead.end();
            inrFilePath++ )
    {
        getConcreteObject()->addPatient( createPatient( *inrFilePath,  commonPathSize, startPercent, filesToRead.size()) );
        startPercent += 1.0/filesToRead.size();
    }

}

//------------------------------------------------------------------------------

} // namespace itkIO
