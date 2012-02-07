/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

// fwComEd
#include <fwComEd/Dictionary.hpp>

// fwTools
#include <fwTools/ClassRegistrar.hpp>

#include <fwData/PointList.hpp>

// gdcmIO
#include "gdcmIO/writer/DicomGlobalWriterManager.hpp"
#include "gdcmIO/DicomInstance.hpp"
#include "gdcmIO/writer/DicomPatientWriter.hpp"
#include "gdcmIO/writer/DicomStudyWriter.hpp"
#include "gdcmIO/writer/DicomSeriesWriter.hpp"
#include "gdcmIO/writer/DicomImageWriterManager.hpp"
#include "gdcmIO/writer/DicomSRWriterManager.hpp"
#include "gdcmIO/writer/DicomRTWriterManager.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwDataIO::writer::IObjectWriter , ::gdcmIO::writer::DicomGlobalWriterManager, ::gdcmIO::writer::DicomGlobalWriterManager );

namespace gdcmIO
{

namespace writer
{

//------------------------------------------------------------------------------

DicomGlobalWriterManager::DicomGlobalWriterManager():
        ::fwData::location::enableFolder< ::fwDataIO::writer::IObjectWriter >(this),
        m_patientID(1)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

DicomGlobalWriterManager::~DicomGlobalWriterManager()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void DicomGlobalWriterManager::write() throw(::fwTools::Failed)
{
    ::fwData::Patient::sptr patient = this->getConcreteObject();
    SLM_ASSERT("fwData::Patient not instanced", patient);

    // Initialization of common variables
    ::boost::shared_ptr< DicomInstance > dicomInstance = ::boost::shared_ptr< DicomInstance >( new DicomInstance );
    DicomPatientWriter  patientWriter;                  // Insert general patient module in a data set
    DicomStudyWriter    studyWriter;                    // Insert general study module in a data set
    DicomSeriesWriter   seriesWriter;                   // Insert general series module in a data set
    ::gdcm::DataSet     gDsPatient, gDsStudy, gDsSeries;// Create data set in the view to copy it
    std::string         patientPath;
    std::string         studyPath;                      // Can be different with patientPath if there are more than one study
    std::string         seriesPath;                     // Can be different with studyPath if there are more than one series
    unsigned int        studyID             = 1;        // Use to make new study directory
    unsigned int        seriesID            = 1;        // Use to make new series directory and to set parameter of DicomSeriesWriter
    bool                makeNewStudyDir     = false;    // Flag to make new directory
    bool                makeNewSeriesDir    = false;

    //*****     Handle patient      *****//
    patientWriter.setObject(patient);

    // Make a new directory
    patientPath = this->getFolder().string() + "/patient_" + ::fwTools::getString< unsigned int >(m_patientID);
    ::boost::filesystem::create_directory( patientPath );
    this->setFolder( patientPath );

    // Write patient data in a data set
    patientWriter.write(gDsPatient);

    //*****     Handle studies      *****//
    studyPath = patientPath;  // Copy of the parent folder path

    if (patient->getStudySize() > 1)
    {// If there are more than one study, one folder will be create for each study
        makeNewStudyDir = true;
    }

    std::pair< ::fwData::Patient::StudyIterator, ::fwData::Patient::StudyIterator > studiesIts = patient->getStudies();
    ::fwData::Patient::StudyIterator studyIt    = studiesIts.first;
    ::fwData::Patient::StudyIterator studyItEnd = studiesIts.second;
    for(; studyIt != studyItEnd ; studyIt++)
    {
        ::fwData::Study::sptr study = *studyIt;

        // In order to avoid to remove all study information in data set (with patient + study data),
        // we copy patient data set in the view to have a data set without study information.
        gDsStudy = gDsPatient;  // Copy of the parent data set

        studyWriter.setObject( study );

        // Make a new directory for each study
        if (makeNewStudyDir)
        {
            studyPath = patientPath + "/study_" + ::fwTools::getString< unsigned int >(studyID++);
            ::boost::filesystem::create_directory( studyPath );
        }

        studyWriter.setDicomInstance( dicomInstance );
        studyWriter.setStudyID(studyID++);

        // Write study data in a data set
        studyWriter.write(gDsStudy);

        //*****     Handle acquisitions      *****//
        seriesPath = studyPath;  // Copy of the parent folder path

        if (study->getAcquisitionSize() > 1)
        {
            makeNewSeriesDir = true;
        }

        std::pair< ::fwData::Study::AcquisitionIterator, ::fwData::Study::AcquisitionIterator > seriesIts = study->getAcquisitions();
        ::fwData::Study::AcquisitionIterator seriesIt    = seriesIts.first;
        ::fwData::Study::AcquisitionIterator seriesItEnd = seriesIts.second;
        // FIXME : If the user duplicate an acquisition, both will be written with the same series UID in two different folders.
        //         This is an issue for document SR which must infers from one series. In this case, if each copy has comment
        //         (landmark, distance), two documents SR will be generated for the same series.
        //         In addition, SR reader will just read the first document SR.
        for(; seriesIt != seriesItEnd ; seriesIt++)
        {
            ::boost::shared_ptr< DicomInstance > dicomSerieInstance = ::boost::shared_ptr< DicomInstance >( new DicomInstance(*dicomInstance) );
            ::fwData::Acquisition::sptr series = *seriesIt;
            gDsSeries = gDsStudy;  // Copy of the parent data set

            seriesWriter.setObject( series );

            // Make a new directory for each series
            if (makeNewSeriesDir)
            {
                seriesPath = studyPath + "/series_" + ::fwTools::getString< unsigned int >(seriesID);
                ::boost::filesystem::create_directory( seriesPath );
            }

            seriesWriter.setDicomInstance( dicomSerieInstance );
            seriesWriter.setSeriesID(seriesID++);

            // Write series data in a data set
            seriesWriter.write(gDsSeries);

            //*****     Handle content of acquisition      *****//
            if (hasImage(series))
            {
                dicomSerieInstance->setIsMultiFrame( false );    // Define image writing policy

                //*****     Handle image content      *****//
                {
                    DicomImageWriterManager    imageManager;
                    imageManager.setObject( series );
                    imageManager.setDicomInstance( dicomSerieInstance );

                    // Write an image from a data set which contains patient, study and series data.
                    // DICOM image file(s) will be written in seriesPath
                    imageManager.write(gDsSeries, seriesPath);
                }

                //*****     Handle SR and RT content      *****//
                //NOTE : those contents depend on an image
                if (hasDocumentSR(series))
                {
                    DicomSRWriterManager    docSRManager;
                    docSRManager.setObject( series->getImage() );
                    docSRManager.setDicomInstance( dicomSerieInstance );

                    try
                    {
                        docSRManager.write(gDsSeries, seriesPath);
                    }
                    catch(::fwTools::Failed & e)
                    {
                        OSLM_ERROR(e.what());
                    }
                }

                if (hasDocumentRT(series))
                {
                    DicomRTWriterManager    docRTManager;
                    docRTManager.setObject( series );
                    docRTManager.setDicomInstance( dicomSerieInstance );

                    try
                    {
                        docRTManager.write(gDsSeries, seriesPath);
                    }
                    catch(::fwTools::Failed & e)
                    {
                        OSLM_ERROR(e.what());
                    }
                }
            }
            else
            {
                throw ::fwTools::Failed("Nothing to save");
            }
        }
    }
}

//------------------------------------------------------------------------------

bool DicomGlobalWriterManager::hasImage( ::fwData::Acquisition::csptr a_series ) const
{
    return ( (a_series->getImage() != 0) ? true : false );
}

//------------------------------------------------------------------------------

bool DicomGlobalWriterManager::hasDocumentSR( ::fwData::Acquisition::csptr a_series ) const
{
    fwData::Image::csptr image = a_series->getImage();
    assert(image != 0);

    // Check if image has landmark and distance
    if ( (image->getFieldSize( fwComEd::Dictionary::m_imageLandmarksId ) > 0
          && image->getFieldSingleElement< fwData::PointList >( fwComEd::Dictionary::m_imageLandmarksId )->getPoints().size() > 0)
      || (image->getFieldSize( fwComEd::Dictionary::m_imageDistancesId ) > 0) )
    {
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------

bool DicomGlobalWriterManager::hasDocumentRT( ::fwData::Acquisition::csptr a_series ) const
{
    // Check number of reconstruction
    return ( (a_series->getReconstructionSize() > 0) ? true : false );
}

//------------------------------------------------------------------------------

std::string DicomGlobalWriterManager::extension()
{
    return std::string("");
};

//------------------------------------------------------------------------------

void DicomGlobalWriterManager::setPatientID(const unsigned int a_id)
{
    m_patientID = a_id;
}

}   // namespace writer

}   // namespace gdcmIO
