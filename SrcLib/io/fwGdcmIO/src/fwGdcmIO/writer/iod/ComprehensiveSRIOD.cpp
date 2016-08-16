/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGdcmIO/helper/DicomData.hpp"
#include "fwGdcmIO/helper/FileWriter.hpp"
#include "fwGdcmIO/writer/ie/Document.hpp"
#include "fwGdcmIO/writer/ie/Equipment.hpp"
#include "fwGdcmIO/writer/ie/Image.hpp"
#include "fwGdcmIO/writer/ie/Patient.hpp"
#include "fwGdcmIO/writer/ie/Series.hpp"
#include "fwGdcmIO/writer/ie/Study.hpp"
#include "fwGdcmIO/writer/iod/ComprehensiveSRIOD.hpp"

#include <boost/make_shared.hpp>

#include <fwCore/spyLog.hpp>
#include <fwData/Image.hpp>
#include <fwMedData/Equipment.hpp>
#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/Patient.hpp>
#include <fwMedData/Study.hpp>

#include <gdcmWriter.h>

namespace fwGdcmIO
{
namespace writer
{
namespace iod
{

//------------------------------------------------------------------------------

ComprehensiveSRIOD::ComprehensiveSRIOD(SPTR(::fwGdcmIO::container::DicomInstance)instance,
                                       ::boost::filesystem::path folderPath, bool use3DSR) :
    ::fwGdcmIO::writer::iod::InformationObjectDefinition(instance, folderPath), m_use3DSR(use3DSR)
{
}

//------------------------------------------------------------------------------

ComprehensiveSRIOD::~ComprehensiveSRIOD()
{
}

//------------------------------------------------------------------------------

void ComprehensiveSRIOD::write(::fwMedData::Series::sptr series)
{
    // Retrieve image series
    ::fwMedData::ImageSeries::sptr imageSeries = ::fwMedData::ImageSeries::dynamicCast(series);
    SLM_ASSERT("Image series should not be null.", imageSeries);

    // Retrieve image
    ::fwData::Image::sptr image = imageSeries->getImage();

    // Create writer
    SPTR(::gdcm::Writer) writer = std::make_shared< ::gdcm::Writer >();

    // Create Information Entity helpers
    ::fwGdcmIO::writer::ie::Patient patientIE(writer, m_instance, series->getPatient());
    ::fwGdcmIO::writer::ie::Study studyIE(writer, m_instance, series->getStudy());
    ::fwGdcmIO::writer::ie::Series seriesIE(writer, m_instance, series);
    ::fwGdcmIO::writer::ie::Equipment equipmentIE(writer, m_instance, series->getEquipment());
    ::fwGdcmIO::writer::ie::Document documentIE(writer, m_instance, image, m_use3DSR);

    // Write Patient Module - PS 3.3 C.7.1.1
    patientIE.writePatientModule();

    // Write General Study Module - PS 3.3 C.7.2.1
    studyIE.writeGeneralStudyModule();

    // Write Patient Study Module - PS 3.3 C.7.2.2
    studyIE.writePatientStudyModule();

    // Write SR Document Series Module - PS 3.3 C.17.1
    seriesIE.writeSRDocumentSeriesModule();

    // Write General Equipment Module - PS 3.3 C.7.5.1
    equipmentIE.writeGeneralEquipmentModule();

    // Write SR Document General Module - PS 3.3 C.17.2
    documentIE.writeSRDocumentGeneralModule();

    // Write SR Document Content Module - PS 3.3 C.17.3
    documentIE.writeSRDocumentContentModule();

    // Write SOP Common module - PS 3.3 C.12.1
    documentIE.writeSOPCommonModule();

    // Write document
    ::fwGdcmIO::helper::FileWriter::write(m_folderPath.string() + "/imSR", writer);

}

} // namespace iod
} // namespace writer
} // namespace fwGdcmIO
