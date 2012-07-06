/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <string>

// gdcmIO
#include "gdcmIO/writer/DicomImageWriterManager.hpp"
#include "gdcmIO/writer/DicomImageWriter.hpp"
//#include "gdcmIO/writer/DicomCTImageWriter.hpp"

namespace gdcmIO
{

namespace writer
{

//------------------------------------------------------------------------------

DicomImageWriterManager::DicomImageWriterManager()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

DicomImageWriterManager::~DicomImageWriterManager()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void DicomImageWriterManager::write(::gdcm::DataSet & a_gDs, const ::boost::filesystem::path & a_path)
{
    SLM_TRACE_FUNC();

    ::fwData::Acquisition::sptr series = this->getConcreteObject();
    SLM_ASSERT("fwData::Acquisition not instanced", series);

    SLM_ASSERT("gdcmIO::DicomInstance not set",this->getDicomInstance().get())

    //*****     Handle image writer      *****//

    // Initialization of common variables
    ::boost::shared_ptr< DicomImageWriter > imageWriter;  // Common pointer which can be down-casted.
    ::boost::shared_ptr< DicomInstance >    dicomInstance   = this->getDicomInstance();

    // Instantiate the appropriated writer following the modality
//    const std::string &                     modality        = dicomInstance->getCRefModality();
//    if (modality == "CT")
//    {
//        imageWriter = ::boost::shared_ptr< DicomCTImageWriter >( new DicomCTImageWriter );
//    }
//    else
    {
        SLM_WARN("Image modality does not handle properly.");
        imageWriter = ::boost::shared_ptr< DicomImageWriter >( new DicomImageWriter );
    }

    // Configure the image writer
    imageWriter->setObject( series );
    imageWriter->setFile(a_path);           // The path will be completed by the image writer
    imageWriter->setDicomInstance( dicomInstance );
    imageWriter->setMultiFrame( dicomInstance->getIsMultiFrame() );

    // Write the image
    imageWriter->setDataSet(a_gDs);
    imageWriter->write();
}

}   // namespace writer

}   // namespace gdcmIO
