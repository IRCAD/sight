/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <gdcmReader.h>
#include <gdcmItem.h>

#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/case_conv.hpp>

// fwComEd
#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>
#include <fwComEd/Dictionary.hpp>

// fwData
#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>
#include <fwData/String.hpp>
#include <fwData/Boolean.hpp>

#include "gdcmIO/Dicomdir.hpp"

namespace gdcmIO
{

//------------------------------------------------------------------------------

Dicomdir::Dicomdir()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

Dicomdir::Dicomdir(const ::boost::filesystem::path dicomDir)
{
    this->read(dicomDir);
}

//------------------------------------------------------------------------------

Dicomdir::~Dicomdir()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------
bool Dicomdir::read(const ::boost::filesystem::path dicomDir)
{
    typedef std::set < gdcm::DataElement > DataElementSet;
    typedef DataElementSet::const_iterator DES_ConstIterator;

    const gdcm::Tag tagMediaStorageSOPClassUID(0x0002, 0x0002);
    const gdcm::Tag tagDirectoryRecordSequence(0x0004, 0x1220);
    const gdcm::Tag tagDirectoryRecordType(0x0004, 0x1430);

    const std::string MediaStorageSOPClassUID_Value("1.2.840.10008.1.3.10");

    std::string fileName = dicomDir.string();

    std::stringstream strm;
    std::string strmStr;

    bool readOk = false;
    gdcm::Reader reader;
    reader.SetFileName(fileName.c_str());
    if(reader.Read())
    {
        gdcm::File &file = reader.GetFile();
        gdcm::DataSet &dataSet = file.GetDataSet();
        gdcm::FileMetaInformation &fileMetaInformation = file.GetHeader();

        gdcm::MediaStorage mediaStorage;
        mediaStorage.SetFromFile(file);
        if(mediaStorage == gdcm::MediaStorage::MediaStorageDirectoryStorage)
        {
            if(fileMetaInformation.FindDataElement(tagMediaStorageSOPClassUID))
            {
                strm.str("");
                fileMetaInformation.GetDataElement(tagMediaStorageSOPClassUID).GetValue().Print(strm);
                strmStr = strm.str();
                ::boost::algorithm::trim(strmStr);
                ::boost::algorithm::to_upper(strmStr);
            }
            else
            {
                SLM_INFO("Media storage SOP Class UID not present.");
            }
            if(MediaStorageSOPClassUID_Value == strmStr)
            {
                DES_ConstIterator itr = dataSet.GetDES().begin();
                for(;itr != dataSet.GetDES().end(); ++itr)
                {
                    if(itr->GetTag() == tagDirectoryRecordSequence)
                    {
                        const gdcm::DataElement& dataElt = (*itr);
                        // Don't use getSequenceOfItems to extract items.s
                        gdcm::SmartPointer < gdcm::SequenceOfItems > seqOfItems = dataElt.GetValueAsSQ();
                        int itemUsed = 0;
                        OSLM_INFO("Number Of Items : " << seqOfItems->GetNumberOfItems() );
                        //                        seqOfItems->End()
                        Patient patient;
                        Study study;
                        Serie serie;
                        Image image;

                        while(itemUsed < seqOfItems->GetNumberOfItems())
                        {
                            itemUsed++;
                            OSLM_INFO("current items : " << itemUsed );
                            strm.str("");
                            if(seqOfItems->GetItem(itemUsed).FindDataElement(tagDirectoryRecordType))
                            {

                                strmStr = this->getValueOfTag(seqOfItems->GetItem(itemUsed),tagDirectoryRecordType);
                                if (strmStr == "PATIENT")
                                {
                                    patient = this->getPatientInformation(seqOfItems->GetItem(itemUsed));
                                    patientList.push_back(patient);
                                    OSLM_INFO("PATIENT current items : " << itemUsed );
                                }
                                else if(strmStr == "STUDY")
                                {
                                    study = this->getStudyInformation(seqOfItems->GetItem(itemUsed));
                                    patients[patient.id].push_back(study);
                                    OSLM_INFO("STUDY current items : " << itemUsed );
                                }
                                else if(strmStr == "SERIES")
                                {
                                    serie = this->getSerieInformation(seqOfItems->GetItem(itemUsed));
                                    studies[study.instanceUID].push_back(serie);
                                    OSLM_INFO("SERIES current items : " << itemUsed );
                                }
                                else if(strmStr == "IMAGE")
                                {
                                    image = this->getImageInformation(seqOfItems->GetItem(itemUsed));
                                    series[serie.instanceUID].push_back(image);
                                    OSLM_INFO("IMAGE current items : " << itemUsed );
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                // The file is not a DICOMDIR.
                readOk = false;
            }
        }
    }
    return readOk;
}

//------------------------------------------------------------------------------

std::string Dicomdir::getValueOfTag(const gdcm::Item & item, const gdcm::Tag& tag)
{

    std::stringstream strm;
    std::string strmStr("");

    strm.str("");
    if(item.FindDataElement(tag))
    {
        item.GetDataElement(tag).GetValue().Print(strm);
    }
    // To see gdcm has a function gdcm::Trim()
    strmStr = strm.str();
    ::boost::algorithm::trim(strmStr);
    ::boost::algorithm::to_upper(strmStr);
    return strmStr;
}

//------------------------------------------------------------------------------

Dicomdir::Patient Dicomdir::getPatientInformation(const gdcm::Item & item)
{
    std::stringstream strm;

    const gdcm::Tag tagPatientName(0x0010, 0x0010);
    const gdcm::Tag tagPatientID(0x0010, 0x0020);

    Patient patient;
    strm.str("");
    if(item.FindDataElement(tagPatientName))
    {
        item.GetDataElement(tagPatientName).GetValue().Print(strm);
        patient.name = strm.str();
        OSLM_INFO("Patient Name : " << strm.str() );
    }
    strm.str("");
    if(item.FindDataElement(tagPatientID))
    {
        item.GetDataElement(tagPatientID).GetValue().Print(strm);
        patient.id =strm.str();
        OSLM_INFO("Patient ID : " << strm.str() );
    }
    return patient;
}

//------------------------------------------------------------------------------

Dicomdir::Study Dicomdir::getStudyInformation(const gdcm::Item & item)
{
    std::stringstream strm;

    const gdcm::Tag tagStudyInstanceUID(0x0020, 0x000d);
    const gdcm::Tag tagStudyDescription(0x0008, 0x1030);
    const gdcm::Tag tagStudyDate(0x0008, 0x0020);

    Study study;
    strm.str("");
    if(item.FindDataElement(tagStudyInstanceUID))
    {
        item.GetDataElement(tagStudyInstanceUID).GetValue().Print(strm);
        study.instanceUID = strm.str();
        OSLM_INFO("Study instance UID : " << strm.str() );
    }
    strm.str("");
    if(item.FindDataElement(tagStudyDate))
    {
        item.GetDataElement(tagStudyDate).GetValue().Print(strm);
        study.studyDate = strm.str();
        OSLM_INFO("Study date : " << strm.str() );
    }
    strm.str("");
    if(item.FindDataElement(tagStudyDescription))
    {
        if(!item.GetDataElement(tagStudyDescription).IsEmpty())
        {
            item.GetDataElement(tagStudyDescription).GetValue().Print(strm);
        }
        study.description =strm.str();
        OSLM_INFO("Study description : " << strm.str() );
    }
    return study;
}

//------------------------------------------------------------------------------

Dicomdir::Serie Dicomdir::getSerieInformation(const gdcm::Item& item)
{
    std::stringstream strm;

    const gdcm::Tag tagSeriesInstanceUID(0x0020, 0x000e);
    const gdcm::Tag tagSeriesModality(0x0008, 0x0060);
    const gdcm::Tag tagSeriesDescription(0x0008, 0x103e);

    Serie serie;
    strm.str("");
    if(item.FindDataElement(tagSeriesInstanceUID))
    {
        item.GetDataElement(tagSeriesInstanceUID).GetValue().Print(strm);
        serie.instanceUID = strm.str();
        OSLM_INFO("Series instance UID : " << strm.str() );
    }
    strm.str("");
    if(item.FindDataElement(tagSeriesModality))
    {
        item.GetDataElement(tagSeriesModality).GetValue().Print(strm);
        serie.modality = strm.str();
        OSLM_INFO("Serie Modality : " << strm.str() );
    }
    strm.str("");
    if(item.FindDataElement(tagSeriesDescription))
    {
        if(!item.GetDataElement(tagSeriesDescription).IsEmpty())
        {
            item.GetDataElement(tagSeriesDescription).GetValue().Print(strm);
        }
        serie.description = strm.str();
        OSLM_INFO("Series description : " << strm.str() );
    }
    return serie;
}

//------------------------------------------------------------------------------

Dicomdir::Image Dicomdir::getImageInformation(const gdcm::Item & item)
{
    const gdcm::Tag tagReferencedFileID(0x0004, 0x1500);
    const gdcm::Tag tagReferencedSOPInstanceUIDInFile(0x0004, 0x1511);

    std::stringstream strm;
    Image img;
    strm.str("");
    if(item.FindDataElement(tagReferencedSOPInstanceUIDInFile))
    {
        item.GetDataElement(tagReferencedSOPInstanceUIDInFile).GetValue().Print(strm);
        img.instanceUID = strm.str();
        OSLM_INFO("Referenced SOP Instance UID In File : " << strm.str() );
    }
    strm.str("");
    if(item.FindDataElement(tagReferencedFileID))
    {
        item.GetDataElement(tagReferencedFileID).GetValue().Print(strm);
        img.fileID = strm.str();
        OSLM_INFO("Referenced File ID : " << strm.str() );
    }
    return img;
}

//------------------------------------------------------------------------------

}
