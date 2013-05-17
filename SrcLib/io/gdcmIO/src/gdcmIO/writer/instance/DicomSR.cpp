/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

// GDCM
#include <gdcmWriter.h>

// fwComEd
#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>
#include <fwComEd/Dictionary.hpp>

// fwData
#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>
#include <fwData/String.hpp>

#include "gdcmIO/writer/instance/DicomSR.hpp"
#include "gdcmIO/writer/instance/DicomInstance.hxx"
#include "gdcmIO/writer/module/DicomLandmark.hpp"
#include "gdcmIO/writer/module/DicomDistance.hpp"
#include "gdcmIO/helper/GdcmHelper.hpp"
#include "gdcmIO/container/DicomDictionarySR.hpp"

namespace gdcmIO
{
namespace writer
{
namespace instance
{

//------------------------------------------------------------------------------

DicomSR::DicomSR()
{
    SLM_TRACE_FUNC();

    // Instantiate genereic GDCM writer
    this->setWriter( ::boost::shared_ptr< ::gdcm::Writer >( new ::gdcm::Writer ) );
}

//------------------------------------------------------------------------------

DicomSR::~DicomSR()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void DicomSR::write() throw (::gdcmIO::exception::Failed)
{
    SLM_TRACE_FUNC();

    ::fwData::Image::sptr image = this->getConcreteObject();
    SLM_ASSERT("fwData::Image not instanced", image);

    ::boost::shared_ptr< ::gdcmIO::container::DicomInstance > dicominstance = this->getDicomInstance();
    SLM_ASSERT("gdcmIO::DicomInstance not instanced", dicominstance);

    //*****     Handled SR content      *****//
    ::gdcm::DataSet & gDsRoot = this->getWriter()->GetFile().GetDataSet();

    //*****     Handled Landmarks      *****//
    ::fwData::PointList::sptr pl;
    pl = image->getField< ::fwData::PointList >( fwComEd::Dictionary::m_imageLandmarksId );
    if (pl && pl->getPoints().size() > 0)
    {
        ::gdcmIO::writer::module::DicomLandmark landmarkWriter;

        landmarkWriter.setObject(image);
        landmarkWriter.setDicomInstance( dicominstance );

        try
        {// Write landmark in the content sequence of root
            landmarkWriter.write(gDsRoot);
        }
        catch(::gdcmIO::exception::Failed & e)
        {
           std::stringstream sStr; sStr.str("");
           sStr << "Landmark writing error : " << e.what();
           throw ::gdcmIO::exception::Failed(sStr.str());
        }
    }

    //*****     Handled distances      *****//
    if (image->getField( fwComEd::Dictionary::m_imageDistancesId ))
    {
        ::gdcmIO::writer::module::DicomDistance distanceWriter;

        distanceWriter.setObject(image);
        distanceWriter.setDicomInstance( dicominstance );

        try
        {// Write distance in the content sequence of root
            distanceWriter.write(gDsRoot);
        }
        catch(::gdcmIO::exception::Failed & e)
        {
           std::stringstream sStr; sStr.str("");
           sStr << "Distance writing error : " << e.what();
           throw ::gdcmIO::exception::Failed(sStr.str());
        }
    }

    //*****     Handled SR document      *****//
    this->writeSRDocumentSeries();

    this->writeSRDocumentGeneral();

    //*****     Complete the file      *****//
    // Set the instance to SR document
    ::boost::shared_ptr< ::gdcmIO::container::DicomInstance > dicomInstance = this->getDicomInstance();
    dicomInstance->setSOPClassUID( ::gdcm::MediaStorage::GetMSString( ::gdcm::MediaStorage::ComprehensiveSR ) );

    ::gdcm::UIDGenerator gUIDgen;
    gUIDgen.SetRoot( dicomInstance->getSOPClassUID().c_str() );
    dicomInstance->setSOPInstanceUID( gUIDgen.Generate() );

    //*****     Write the file      *****//
    DicomInstance< ::fwData::Image >::write();
}

//------------------------------------------------------------------------------

void DicomSR::writeSRDocumentSeries()
{
    ::gdcm::DataSet & gDsRoot = this->getDataSet();

    // Modality (will be written by DicomInstance)
    ::boost::shared_ptr< ::gdcmIO::container::DicomInstance > dicomInstance = this->getDicomInstance();
    dicomInstance->setModality("SR");

    // Referenced Performed Procedure Step Sequence (0008,1111) // Type 2
    {
        ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > gSqProcedure = new ::gdcm::SequenceOfItems();
        gSqProcedure->SetLengthToUndefined();

        helper::GdcmData::insertSQ<0x0008,0x1111>(gSqProcedure, gDsRoot);   // Referenced Performed Procedure Step Sequence
    }
}

//------------------------------------------------------------------------------

void DicomSR::writeSRDocumentGeneral()
{
    SLM_TRACE_FUNC();

    ::gdcm::DataSet & gDsRoot = this->getDataSet();

    // Instance Number (0020,0013)
    this->getDicomInstance()->setInstanceNumber("1");

    const ::boost::posix_time::ptime    now                 = ::boost::posix_time::second_clock::local_time();
    const std::string                   currentDateAndTime  = ::boost::posix_time::to_iso_string(now);
    const size_t                        pos                 = currentDateAndTime.find("T"); // "T" is the separator between date and time
    const std::string                   date                = currentDateAndTime.substr(0, pos);
    const std::string                   time                = currentDateAndTime.substr(pos+1);

    // Content Date
    helper::GdcmData::setTagValue<0x0008,0x0023>(date, gDsRoot);
    OSLM_TRACE("Content date : "<<date);

    // Content Time
    helper::GdcmData::setTagValue<0x0008,0x0033>(time, gDsRoot);
    OSLM_TRACE("Content time : "<<time);

    // Type value
    helper::GdcmData::setTagValue<0x0040,0xa040>(::gdcmIO::container::DicomDictionarySR::getTypeString(
            ::gdcmIO::container::DicomDictionarySR::CONTAINER), gDsRoot);

    // Continuity of content (landmarks and distances are separated)
    // NOTE : Continuity is fixed to SEPARATE because it provides a better compatibility.
    helper::GdcmData::setTagValue<0x0040,0xa050>(::gdcmIO::container::DicomDictionarySR::getContinuityString(
            ::gdcmIO::container::DicomDictionarySR::SEPARATE),    gDsRoot);

    // Performed Procedure Code Sequence (0040,A372)    // Type 2
    ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > gSqProcedure = new ::gdcm::SequenceOfItems();
    gSqProcedure->SetLengthToUndefined();
    {
        // CID 7000 ??
    }
    helper::GdcmData::setSQ<0x0040,0xa372>(gSqProcedure,        gDsRoot);   // Performed Procedure Code Sequence

    // Completion flag
    helper::GdcmData::setTagValue<0x0040,0xa491>("PARTIAL",     gDsRoot);   // Set to COMPLETE?

    // Verification Flag
    const std::string verifFlag = "UNVERIFIED"; // Set to VERIFIED?
    helper::GdcmData::setTagValue<0x0040,0xa493>(verifFlag,     gDsRoot);

    /*
    if (verifFlag == "VERIFIED")
    {
//        Verifying Observer Sequence (0040,A073)
//        >Verifying Observer Name (0040,A075)
//        >Verifying Observer (0040,A088)               // Type 2
//        >>Include 'Code Sequence Macro' Table 8.8-1
//        >Verifying Organization (0040,A027)
//        >Verification DateTime (0040,A030)
    }
    */
}

} // namespace instance
} // namespace writer
} // namespace gdcmIO
