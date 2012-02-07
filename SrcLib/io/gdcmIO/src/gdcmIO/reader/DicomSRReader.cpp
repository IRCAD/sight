/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "gdcmIO/reader/DicomSRReader.hpp"
#include "gdcmIO/reader/DicomLandmarkReader.hpp"
#include "gdcmIO/reader/DicomDistanceReader.hpp"
#include "gdcmIO/helper/GdcmHelper.hpp"
#include "gdcmIO/DicomDictionarySR.hpp"

namespace gdcmIO
{

namespace reader
{

//------------------------------------------------------------------------------

DicomSRReader::DicomSRReader()
{
    SLM_TRACE_FUNC();

    this->setReader( ::boost::shared_ptr< ::gdcm::Reader >( new ::gdcm::Reader ) );
}

//------------------------------------------------------------------------------

DicomSRReader::~DicomSRReader()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void DicomSRReader::read() throw (::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    ::fwData::Image::sptr image = this->getConcreteObject();
    SLM_ASSERT("fwData::Image not instanced", image);

    ::boost::shared_ptr< DicomInstance > dicomInstance = this->getDicomInstance();
    SLM_ASSERT("gdcmIO::DicomInstance not set", dicomInstance);

    //*****     Get all file names      *****//
    const std::vector< std::string > &  docSR   = this->getCRefFileNames();

    const unsigned int                  nbDocSR = docSR.size();
    OSLM_TRACE("Number of SR files : " << nbDocSR);
    if (nbDocSR > 1)
    {
        OSLM_WARN("More than one document SR was found in series. Just the first one will be read.");
    }

    // Get the basic GDCM reader
    ::boost::shared_ptr< ::gdcm::Reader >   gReader = this->getReader();

    //*****     Init tools      *****//
    DicomLandmarkReader landmarkReader;
    landmarkReader.setObject( image );
    landmarkReader.setReader( gReader );
    landmarkReader.setDicomInstance( dicomInstance );

    DicomDistanceReader distanceReader;
    distanceReader.setObject( image );
    distanceReader.setReader( gReader );
    distanceReader.setDicomInstance( dicomInstance );

    //*****     Read first file      *****//
    gReader->SetFileName( docSR[0].c_str() );
    if ( gReader->Read() )
    {
        OSLM_TRACE("SR document reading : "<<docSR[0].c_str());

        // Landmarks
        try
        {// Read all landmarks found in the document SR
            landmarkReader.read();
        }
        catch (::fwTools::Failed &e)
        {
            OSLM_ERROR("Landmark reading failed: "<<e.what());
        }

        // Distances
        try
        {// Read all distances found in the document SR
            distanceReader.read();
        }
        catch (::fwTools::Failed &e)
        {
            OSLM_ERROR("Distance reading failed: "<<e.what());
        }
    }
    else
    {
        OSLM_ERROR("SR document reading error in file : "<<docSR[0].c_str());
    }
}

}  // namespace reader

}  // namespace gdcmIO

