/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "gdcmIO/reader/dicom/DicomSR.hpp"
#include "gdcmIO/reader/dicom/DicomLandmark.hpp"
#include "gdcmIO/reader/dicom/DicomDistance.hpp"
#include "gdcmIO/helper/GdcmHelper.hpp"
#include "gdcmIO/container/DicomDictionarySR.hpp"

namespace gdcmIO
{
namespace reader
{
namespace dicom
{

//------------------------------------------------------------------------------

DicomSR::DicomSR()
{
    SLM_TRACE_FUNC();

    this->setReader( ::boost::shared_ptr< ::gdcm::Reader >( new ::gdcm::Reader ) );
}

//------------------------------------------------------------------------------

DicomSR::~DicomSR()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void DicomSR::read() throw (::gdcmIO::exception::Failed)
{
    SLM_TRACE_FUNC();

    ::fwData::Image::sptr image = this->getConcreteObject();
    SLM_ASSERT("fwData::Image not instanced", image);

    ::boost::shared_ptr< ::gdcmIO::container::DicomInstance > dicomInstance = this->getDicomInstance();
    SLM_ASSERT("gdcmIO::DicomInstance not set", dicomInstance);

    //*****     Get all file names      *****//
    const std::vector< std::string > &  docSR   = this->getCRefFileNames();

    const unsigned int nbDocSR = docSR.size();
    OSLM_TRACE("Number of SR files : " << nbDocSR);
    if (nbDocSR > 1)
    {
        OSLM_WARN("More than one document SR was found in series. Just the first one will be read.");
    }

    // Get the basic GDCM reader
    ::boost::shared_ptr< ::gdcm::Reader >   gReader = this->getReader();

    //*****     Init tools      *****//
    ::gdcmIO::reader::dicom::DicomLandmark landmark;
    landmark.setObject( image );
    landmark.setReader( gReader );
    landmark.setDicomInstance( dicomInstance );

    ::gdcmIO::reader::dicom::DicomDistance distance;
    distance.setObject( image );
    distance.setReader( gReader );
    distance.setDicomInstance( dicomInstance );

    //*****     Read first file      *****//
    gReader->SetFileName( docSR[0].c_str() );
    if ( gReader->Read() )
    {
        OSLM_TRACE("SR document reading : "<<docSR[0].c_str());

        // Landmarks
        try
        {
            // Read all landmarks found in the document SR
            landmark.read();
        }
        catch (::gdcmIO::exception::Failed &e)
        {
            OSLM_ERROR("Landmark reading failed: "<<e.what());
        }

        // Distances
        try
        {
            // Read all distances found in the document SR
            distance.read();
        }
        catch (::gdcmIO::exception::Failed &e)
        {
            OSLM_ERROR("Distance reading failed: "<<e.what());
        }
    }
    else
    {
        OSLM_ERROR("SR document reading error in file : "<<docSR[0].c_str());
    }
}

}  // namespace dicom
}  // namespace reader
}  // namespace gdcmIO

