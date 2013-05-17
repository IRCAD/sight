/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/make_shared.hpp>
#include <boost/foreach.hpp>

// fwComEd
#include <fwComEd/Dictionary.hpp>

#include <fwMedData/Series.hpp>
#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/ModelSeries.hpp>
#include <fwMedData/Equipment.hpp>
#include <fwMedData/Patient.hpp>
#include <fwMedData/Study.hpp>

#include <fwData/PointList.hpp>

#include <fwDataIO/writer/registry/macros.hpp>

// gdcmIO
#include "gdcmIO/writer/Series.hpp"
#include "gdcmIO/container/DicomInstance.hpp"
#include "gdcmIO/writer/module/DicomPatient.hpp"
#include "gdcmIO/writer/module/DicomStudy.hpp"
#include "gdcmIO/writer/module/DicomEquipment.hpp"
#include "gdcmIO/writer/module/DicomSeries.hpp"
#include "gdcmIO/writer/module/DicomImageManager.hpp"
#include "gdcmIO/writer/module/DicomSRManager.hpp"
#include "gdcmIO/writer/module/DicomRTManager.hpp"

fwDataIOWriterRegisterMacro( ::gdcmIO::writer::Series );

namespace gdcmIO
{
namespace writer
{

//------------------------------------------------------------------------------

Series::Series(::fwDataIO::writer::IObjectWriter::Key key):
        ::fwData::location::enableFolder< ::fwDataIO::writer::IObjectWriter >(this)
{}

//------------------------------------------------------------------------------

Series::~Series()
{}

//------------------------------------------------------------------------------

void Series::write() throw(::gdcmIO::exception::Failed)
{
    ::fwMedData::Series::sptr series = this->getConcreteObject();
    SLM_ASSERT("fwMedData::Series not instanced", series);

    // Initialization of common variables
    ::boost::shared_ptr< ::gdcmIO::container::DicomInstance > dicomInstance =
            ::boost::make_shared< ::gdcmIO::container::DicomInstance >();

    ::gdcm::DataSet gdcmDataSet;

    ::fwMedData::ImageSeries::sptr imgSeries = ::fwMedData::ImageSeries::dynamicCast(series);
    ::fwMedData::ModelSeries::sptr modelSeries = ::fwMedData::ModelSeries::dynamicCast(series);

    // Write Series
    {
        ::gdcmIO::writer::module::DicomSeries seriesWriter;
        seriesWriter.setDicomInstance(dicomInstance);
        seriesWriter.setObject(series);
        seriesWriter.write(gdcmDataSet);
    }
    // Write Equipment
    {
        ::gdcmIO::writer::module::DicomEquipment equipmentWriter;
        if(modelSeries)
        {
            equipmentWriter.setIsEnhanced(true);
        }
        equipmentWriter.setDicomInstance(dicomInstance);
        equipmentWriter.setObject(series->getEquipment());
        equipmentWriter.write(gdcmDataSet);
    }
    // Write Study
    {
        ::gdcmIO::writer::module::DicomStudy studyWriter;
        studyWriter.setDicomInstance(dicomInstance);
        studyWriter.setObject(series->getStudy());
        studyWriter.write(gdcmDataSet);
    }
    // Write patient
    {
        ::gdcmIO::writer::module::DicomPatient patientWriter;
        patientWriter.setDicomInstance(dicomInstance);
        patientWriter.setObject(series->getPatient());
        patientWriter.write(gdcmDataSet);
    }

    if (imgSeries)
    {
        dicomInstance->setIsMultiFrame( false );    // Define image writing policy

        {
            ::gdcmIO::writer::module::DicomImageManager imageManager;
            imageManager.setObject( imgSeries->getImage() );
            imageManager.setDicomInstance( dicomInstance );

            // Write an image from a data set which contains patient, study and series data.
            // DICOM image file(s) will be written in seriesPath
            imageManager.write(gdcmDataSet, this->getFolder());
        }

        if (this->hasDocumentSR(imgSeries))
        {
            ::gdcmIO::writer::module::DicomSRManager docSRManager;
            docSRManager.setObject( imgSeries->getImage() );
            docSRManager.setDicomInstance( dicomInstance );

            try
            {
                docSRManager.write(gdcmDataSet, this->getFolder());
            }
            catch(::gdcmIO::exception::Failed & e)
            {
                OSLM_ERROR(e.what());
            }
        }
    }
    else if (modelSeries)
    {
        ::gdcmIO::writer::module::DicomRTManager docRTManager;
        docRTManager.setObject( modelSeries );
        docRTManager.setDicomInstance( dicomInstance );

        try
        {
            docRTManager.write(gdcmDataSet, this->getFolder());
        }
        catch(::gdcmIO::exception::Failed & e)
        {
            OSLM_ERROR(e.what());
        }
    }
}

//------------------------------------------------------------------------------

bool Series::hasDocumentSR( ::fwMedData::ImageSeries::csptr imageSeries ) const
{
    ::fwData::Image::csptr image = imageSeries->getImage();
    SLM_ASSERT("Image not instanced", image);

    ::fwData::PointList::sptr pl;
    pl = image->getField< ::fwData::PointList >( ::fwComEd::Dictionary::m_imageLandmarksId );
    // Check if image has landmark and distance
    return ( (pl  && pl->getPoints().size() > 0)
            || image->getField( ::fwComEd::Dictionary::m_imageDistancesId ) );
}

//------------------------------------------------------------------------------

std::string Series::extension()
{
    return std::string("");
};

//------------------------------------------------------------------------------


}   // namespace writer

}   // namespace gdcmIO
