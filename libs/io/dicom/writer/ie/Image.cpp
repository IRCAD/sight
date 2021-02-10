/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "io/dicom/writer/ie/Image.hpp"

#include "io/dicom/helper/DicomDataTools.hpp"
#include "io/dicom/helper/DicomDataWriter.hxx"

#include <data/Image.hpp>

#include <gdcmImageWriter.h>

namespace sight::io::dicom
{
namespace writer
{
namespace ie
{

//------------------------------------------------------------------------------

Image::Image(const SPTR(::gdcm::Writer)& writer,
             const SPTR(io::dicom::container::DicomInstance)& instance,
             const data::Image::csptr& image,
             const core::log::Logger::sptr& logger,
             ProgressCallback progress,
             CancelRequestedCallback cancel) :
    io::dicom::writer::ie::InformationEntity< data::Image >(writer, instance, image, logger, progress, cancel)
{
}

//------------------------------------------------------------------------------

Image::~Image()
{
}

//------------------------------------------------------------------------------

void Image::writeGeneralImageModule()
{
    // Nothing to write
}

//------------------------------------------------------------------------------

void Image::writeGeneralImageModuleSpecificTags(unsigned int instanceNumber)
{
    // Retrieve dataset
    ::gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    // Instance Number
    io::dicom::helper::DicomDataWriter::setTagValue< int, 0x0020, 0x0013 >(instanceNumber, dataset);

}

//------------------------------------------------------------------------------

void Image::writeImagePlaneModule()
{
    // Retrieve dataset
    ::gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    // Retrieve GDCM image
    SPTR(::gdcm::ImageWriter) imageWriter = std::static_pointer_cast< ::gdcm::ImageWriter >(m_writer);
    ::gdcm::Image& gdcmImage              = imageWriter->GetImage();

    // Pixel Spacing - Type 1
    // WARNING : some DICOM image have not any spacing (NOT SUPPORTED BY Sight), but stuff like "Pixel Aspect Ratio"
    const std::size_t dimension = m_object->getNumberOfDimensions();
    const auto& spacing         = m_object->getSpacing2();
    for (unsigned int i = 0; i < dimension; ++i)
    {
        gdcmImage.SetSpacing(i, spacing[i]);
    }

    // Slice Thickness - Type 2
    // NOTE: ::gdcm::ImageWriter may replace SliceThickness values according to Image Origins
    io::dicom::helper::DicomDataWriter::setTagValue< double, 0x0018, 0x0050 >(spacing[2], dataset);

    // Image Orientation (Patient) - Type 1
    // This tag his handled by gdcm writer

}

//------------------------------------------------------------------------------

void Image::writeImagePlaneModuleSpecificTags(unsigned int instanceNumber)
{
    // Retrieve GDCM image
    SPTR(::gdcm::ImageWriter) imageWriter = std::static_pointer_cast< ::gdcm::ImageWriter >(m_writer);
    ::gdcm::Image& gdcmImage              = imageWriter->GetImage();

    // Image Position (Patient) - Type 1
    const auto& origin  = m_object->getOrigin2();
    const auto& spacing = m_object->getSpacing2();
    gdcmImage.SetOrigin(0, origin[0]);
    gdcmImage.SetOrigin(1, origin[1]);
    gdcmImage.SetOrigin(2, origin[2] + spacing[2]*instanceNumber);
}

//------------------------------------------------------------------------------

void Image::writeImagePixelModule()
{
    // Retrieve GDCM image
    ::gdcm::ImageWriter* imageWriter = std::static_pointer_cast< ::gdcm::ImageWriter >(m_writer).get();
    ::gdcm::Image& gdcmImage         = imageWriter->GetImage();

    // Image's photometric interpretation - Type 1
    ::gdcm::PhotometricInterpretation photoInter =
        io::dicom::helper::DicomDataTools::getPhotometricInterpretation(m_object);
    gdcmImage.SetPhotometricInterpretation(photoInter);

    // Image's pixel type
    ::gdcm::PixelFormat pixelFormat = io::dicom::helper::DicomDataTools::getPixelType(m_object);
    gdcmImage.SetPixelFormat(pixelFormat);

    //Image's number of dimension
    unsigned int dimension =
        static_cast<unsigned int>((m_instance->getIsMultiFiles()) ? 2 : m_object->getNumberOfDimensions());
    gdcmImage.SetNumberOfDimensions(dimension);

    // Image's dimension
    const data::Image::Size& size = m_object->getSize2();
    for (unsigned int i = 0; i < dimension; ++i)
    {
        gdcmImage.SetDimension(i, static_cast<unsigned int>(size[i]));
    }

}

//------------------------------------------------------------------------------

void Image::writeImagePixelModuleSpecificTags(unsigned int instanceNumber)
{
    SLM_ASSERT("Wrong instance number.", m_instance->getIsMultiFiles() || instanceNumber == 0);

    // Retrieve GDCM image
    ::gdcm::ImageWriter* imageWriter = std::static_pointer_cast< ::gdcm::ImageWriter >(m_writer).get();
    ::gdcm::Image& gdcmImage         = imageWriter->GetImage();

    // Compute buffer size
    const data::Image::Size& size = m_object->getSize2();
    std::size_t bufferLength      = size[0] * size[1] * gdcmImage.GetPixelFormat().GetPixelSize();
    bufferLength = (!m_instance->getIsMultiFiles()) ? (bufferLength*size[2]) : bufferLength;

    // Retrieve image buffer
    const auto dumpLock     = m_object->lock();
    const char* imageBuffer = static_cast< char* >(m_object->getBuffer());

    // Pixel Data - Type 1C
    ::gdcm::DataElement pixeldata(::gdcm::Tag(0x7fe0, 0x0010));
    pixeldata.SetByteValue(imageBuffer + instanceNumber * bufferLength, static_cast<unsigned int>(bufferLength));
    gdcmImage.SetDataElement(pixeldata);
}

//------------------------------------------------------------------------------

void Image::writeVOILUTModule()
{
    // Retrieve dataset
    ::gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    const double windowCenter = m_object->getWindowCenter();
    const double windowWidth  = m_object->getWindowWidth();
    if(windowCenter || windowWidth)
    {
        // Image's windows center
        io::dicom::helper::DicomDataWriter::setTagValues< double, 0x0028, 0x1050 >(&windowCenter, 1, dataset);

        // Image's windows width
        io::dicom::helper::DicomDataWriter::setTagValues< double, 0x0028, 0x1051 >(&windowWidth, 1, dataset);
    }

}

//------------------------------------------------------------------------------

void Image::writeSOPCommonModule()
{
    // Retrieve dataset
    ::gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    // SOP Class UID
    io::dicom::helper::DicomDataWriter::setTagValue<  0x0008, 0x0016 >(m_instance->getSOPClassUID(), dataset);
}

//------------------------------------------------------------------------------

void Image::writeSOPCommonModuleSpecificTags(unsigned int instanceNumber)
{
    // Retrieve dataset
    ::gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    // SOP Instance UID
    const std::string sopInstanceUID = m_instance->getSOPInstanceUIDContainer()[instanceNumber];
    io::dicom::helper::DicomDataWriter::setTagValue< 0x0008, 0x0018 >(sopInstanceUID, dataset);
}

//------------------------------------------------------------------------------

void Image::writeCTImageModule()
{
    // Retrieve dataset
    ::gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    // Image Type - Type 1 - FIXME: Fake Value
    const ::gdcm::String< 92, 16 > imageType = "ORIGINAL\\PRIMARY\\AXIAL";
    io::dicom::helper::DicomDataWriter::setTagValues< ::gdcm::String< 92, 16 >, 0x0008, 0x0008 >(&imageType, 1,
                                                                                                 dataset);

    // Acquisition Number - Type 2 - FIXME: Fake Value
    unsigned int acquisitionNumber = 1;
    io::dicom::helper::DicomDataWriter::setTagValue< unsigned int, 0x0020, 0x0012 >(acquisitionNumber, dataset);

    // KVP - Type 2 - FIXME: Fake Value
    io::dicom::helper::DicomDataWriter::setTagValue< double, 0x0018, 0x0060 >(1, dataset);

    // Rescale Type - Type 1C - FIXME: Fake Value
    io::dicom::helper::DicomDataWriter::setTagValue< 0x0028, 0x1054 >("HU", dataset);

}

//------------------------------------------------------------------------------

void Image::writeMRImageModule()
{
    // Retrieve dataset
    ::gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    // Image Type - Type 1 - FIXME: Fake Value
    const ::gdcm::String< 92, 16 > imageType = "ORIGINAL\\PRIMARY\\MPR";
    io::dicom::helper::DicomDataWriter::setTagValues< ::gdcm::String< 92, 16 >, 0x0008, 0x0008 >(&imageType, 1,
                                                                                                 dataset);

    // Scanning Sequence - Type 1 - FIXME: Fake Value
    const ::gdcm::String< 92, 16 > scanningSequence = "SE";
    io::dicom::helper::DicomDataWriter::setTagValues< ::gdcm::String< 92, 16 >, 0x0018, 0x0020 >(&scanningSequence, 1,
                                                                                                 dataset);

    // Sequence Variant - Type 1 - FIXME: Fake Value
    const ::gdcm::String< 92, 16 > sequenceVariant = "NONE";
    io::dicom::helper::DicomDataWriter::setTagValues< ::gdcm::String< 92, 16 >, 0x0018, 0x0021 >(&sequenceVariant, 1,
                                                                                                 dataset);

    // Scan Options - Type 2 - FIXME: Fake Value
    const ::gdcm::String< 92, 16 > scanOption = "";
    io::dicom::helper::DicomDataWriter::setTagValues< ::gdcm::String< 92, 16 >, 0x0018, 0x0022 >(&scanOption, 1,
                                                                                                 dataset);

    // MR Acquisition Type - Type 2 - FIXME: Fake Value
    io::dicom::helper::DicomDataWriter::setTagValue< 0x0018, 0x0023 >("3D", dataset);

    // Repetition Time - Type 2C - FIXME: Fake Value
    io::dicom::helper::DicomDataWriter::setTagValue< double, 0x0018, 0x0080 >(0, dataset);

    // Echo Time - Type 2 - FIXME: Fake Value
    io::dicom::helper::DicomDataWriter::setTagValue< double, 0x0018, 0x0081 >(1, dataset);

    // Echo Train Length - Type 2 - FIXME: Fake Value
    io::dicom::helper::DicomDataWriter::setTagValue< unsigned int, 0x0018, 0x0091 >(1, dataset);
}

//------------------------------------------------------------------------------

} // namespace ie
} // namespace writer
} // namespace sight::io::dicom
