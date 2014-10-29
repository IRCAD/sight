/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>

#include <gdcmImageHelper.h>
#include <gdcmImageReader.h>
#include <gdcmUIDGenerator.h>
#include <gdcmIPPSorter.h>
#include <gdcmPixelFormat.h>
#include <gdcmRescaler.h>
#include <gdcmImageApplyLookupTable.h>

#include <fwComEd/helper/Array.hpp>
#include <fwComEd/helper/Image.hpp>
#include <fwDicomData/DicomSeries.hpp>
#include <fwData/Image.hpp>
#include <fwDicomIOExt/dcmtk/helper/Image.hpp>

#include "gdcmIO/helper/DicomData.hpp"
#include "gdcmIO/reader/ie/Image.hpp"

namespace gdcmIO
{
namespace reader
{
namespace ie
{

//------------------------------------------------------------------------------

Image::Image(SPTR(::fwDicomData::DicomSeries) dicomSeries,
        SPTR(::gdcm::Reader) reader,
        SPTR(::gdcmIO::container::DicomInstance) instance,
        ::fwData::Image::sptr image):
        ::gdcmIO::reader::ie::InformationEntity< ::fwData::Image >(dicomSeries, reader, instance, image)
{
}

//------------------------------------------------------------------------------

Image::~Image()
{
}

//------------------------------------------------------------------------------

void Image::readImagePlaneModule()
{
    // Retrieve GDCM image
    SPTR(::gdcm::ImageReader) imageReader = ::boost::static_pointer_cast< ::gdcm::ImageReader >(m_reader);
    ::gdcm::Image &gdcmImage = imageReader->GetImage();

    // Retrieve dataset
    ::gdcm::DataSet &dataset = m_reader->GetFile().GetDataSet();

    // Image Position (Patient) - Type 1
    const double *gdcmOrigin = gdcmImage.GetOrigin();
    ::fwData::Image::OriginType origin(3,0);
    if ( gdcmOrigin != 0 )
    {
        std::copy( gdcmOrigin, gdcmOrigin+3, origin.begin() );
    }
    m_object->setOrigin(origin);

    // Pixel Spacing - Type 1
    // Image dimension
    const unsigned int dimension = gdcmImage.GetNumberOfDimensions();
    OSLM_TRACE("Image's dimension : " << dimension);

    // Image's spacing
    const double* gdcmSpacing = gdcmImage.GetSpacing();
    ::fwData::Image::SpacingType spacing (3,1);
    if ( gdcmSpacing != 0 )
    {
        std::copy( gdcmSpacing, gdcmSpacing+dimension, spacing.begin() );
    }

    // Compute Z image spacing when extra information is required
    std::string sliceThickness = ::gdcmIO::helper::DicomData::getTrimmedTagValue<0x0018,0x0050>(dataset);
    std::string spacingBetweenSlices = ::gdcmIO::helper::DicomData::getTrimmedTagValue<0x0018,0x0088>(dataset);

    if(m_dicomSeries->hasComputedValues("SliceThickness"))
    {
        spacing[2] = ::boost::lexical_cast< double >(m_dicomSeries->getComputedTagValues().at("SliceThickness"));
    }
    else if(!sliceThickness.empty())
    {
        spacing[2] = ::boost::lexical_cast< double >(sliceThickness);
    }
    else if(!spacingBetweenSlices.empty())
    {
        spacing[2] = ::boost::lexical_cast< double >(spacingBetweenSlices);
    }

    OSLM_TRACE("Image's spacing : "<<spacing[0]<<"x"<<spacing[1]<<"x"<<spacing[2]);
    m_object->setSpacing( spacing );

}

//------------------------------------------------------------------------------

void Image::readVOILUTModule()
{
    // Retrieve dataset
    ::gdcm::DataSet &dataset = m_reader->GetFile().GetDataSet();

    //Image's window center (double)
    std::string windowCenter = ::gdcmIO::helper::DicomData::getTrimmedTagValue<0x0028,0x1050>(dataset);
    std::vector<std::string> splitedWindowCenters;
    if ( !windowCenter.empty() )
    {
        // If there is several window center we only take the first one
        ::boost::split( splitedWindowCenters, windowCenter, ::boost::is_any_of( "\\" ) );
        m_object->setWindowCenter( ::boost::lexical_cast< double >(splitedWindowCenters[0]));
    }
    OSLM_TRACE("Image's window center : " << m_object->getWindowCenter());

    //Image's window width (double)
    std::string windowWidth = ::gdcmIO::helper::DicomData::getTrimmedTagValue<0x0028,0x1051>(dataset);
    std::vector<std::string> splitedWindowWidth;
    if ( !windowWidth.empty() )
    {
        // If there is several window width we only take the first one
        ::boost::split( splitedWindowWidth, windowWidth, ::boost::is_any_of( "\\" ) );
        m_object->setWindowWidth( ::boost::lexical_cast< double >(splitedWindowWidth[0]));
    }
    OSLM_TRACE("Image's window width : " << m_object->getWindowWidth());

}

//------------------------------------------------------------------------------

void Image::readImagePixelModule()
{
    // Retrieve GDCM image
    SPTR(::gdcm::ImageReader) imageReader = ::boost::static_pointer_cast< ::gdcm::ImageReader >(m_reader);
    ::gdcm::Image &gdcmImage = imageReader->GetImage();

    // Retrieve dataset
    ::gdcm::DataSet &dataset = m_reader->GetFile().GetDataSet();

    // Retrieve image information before processing the rescaling
    ::gdcm::PixelFormat pixelFormat = ::gdcm::ImageHelper::GetPixelFormatValue(imageReader->GetFile());
    ::gdcm::PixelFormat::ScalarType scalarType = pixelFormat.GetScalarType();

    std::vector< double > rescaleInterceptSlope =
            ::gdcm::ImageHelper::GetRescaleInterceptSlopeValue(imageReader->GetFile());
    double rescaleIntercept = rescaleInterceptSlope[0];
    double rescaleSlope = rescaleInterceptSlope[1];
    unsigned short samplesPerPixel = pixelFormat.GetSamplesPerPixel();
    unsigned short bitsAllocated = pixelFormat.GetBitsAllocated();
    unsigned short bitsStored = pixelFormat.GetBitsStored();
    unsigned short highBit = pixelFormat.GetHighBit();
    unsigned short pixelRepresentation = pixelFormat.GetPixelRepresentation();

    // Compute final image type
    ::fwDicomIOExt::dcmtk::helper::Image imageHelper(
            samplesPerPixel,bitsAllocated,bitsStored, highBit, pixelRepresentation, rescaleSlope, rescaleIntercept);
    ::fwTools::Type imageType = imageHelper.findImageTypeFromMinMaxValues();
    m_object->setType(imageType);
    ::gdcm::PixelFormat targetPixelFormat = ::gdcmIO::helper::DicomData::getPixelType(m_object);

    // Compute number of components
    std::string photometricInterpretation = ::gdcmIO::helper::DicomData::getTrimmedTagValue<0x0028,0x0004>(dataset);
    std::string pixelPresentation = ::gdcmIO::helper::DicomData::getTrimmedTagValue<0x0008,0x9205>(dataset);

    if(photometricInterpretation == "MONOCHROME2")
    {
        m_object->setNumberOfComponents(1);
    }
    else if(photometricInterpretation == "RGB" || photometricInterpretation == "YBR")
    {
        m_object->setNumberOfComponents(3);
    }
    else if(photometricInterpretation == "ARGB" || photometricInterpretation == "CMYK")
    {
        m_object->setNumberOfComponents(4);
    }
    else if(photometricInterpretation == "PALETTE COLOR" || pixelPresentation == "COLOR")
    {
        m_object->setNumberOfComponents(3);
    }
    else
    {
        std::stringstream ss;
        ss << "The photometric interpretation \"" + photometricInterpretation + "\" is not supported.";
        throw ::gdcmIO::exception::Failed(ss.str());
    }

    // Update image size
    std::vector<unsigned int> dimensions = ::gdcm::ImageHelper::GetDimensionsValue(imageReader->GetFile());
    unsigned short sizeZ = m_dicomSeries->getLocalDicomPaths().size();
    if(sizeZ == 1)
    {
        sizeZ = gdcmImage.GetBufferLength() / (dimensions[0] * dimensions[1] * (bitsAllocated/8));
    }
    m_object->setSize(::boost::assign::list_of(dimensions[0])(dimensions[1])(sizeZ));


    // Compute real image size
    const unsigned long imageSize = dimensions[0] * dimensions[1] * sizeZ * (bitsAllocated/8);
    const unsigned long newImageSize = dimensions[0] * dimensions[1] * sizeZ * (targetPixelFormat.GetBitsAllocated()/8);

    // Let's read the image buffer
    void* temporaryBuffer = this->readImageBuffer();
    char* finalBuffer = new char[newImageSize];

    if(photometricInterpretation == "PALETTE COLOR" || pixelPresentation == "COLOR")
    {
        //TODO: Apply lookup
//        ::gdcm::LookupTable lookup = gdcmImage.GetLUT();
//        lookup.Decode(finalBuffer, newImageSize, (const char*)temporaryBuffer, imageSize);
       throw ::gdcmIO::exception::Failed("The photometric interpretation \"PALETTE COLOR\" is not yet supported");
    }
    else
    {
        // Create rescaler
        ::gdcm::Rescaler rescaler;
        rescaler.SetIntercept(rescaleIntercept);
        rescaler.SetSlope(rescaleSlope);
        rescaler.SetPixelFormat(scalarType);
        rescaler.SetTargetPixelType(targetPixelFormat.GetScalarType());
        rescaler.SetUseTargetPixelType(true);

        // Rescale the image
        rescaler.Rescale(finalBuffer, (const char*)temporaryBuffer, imageSize);
    }

    // Set image buffer
    ::fwData::Array::sptr array = m_object->getDataArray();
    ::fwComEd::helper::Array helper(array);
    helper.setBuffer(finalBuffer, true, m_object->getType(), m_object->getSize(), 1);

    // Delete temporary buffer
    delete (char*)temporaryBuffer;
}

//------------------------------------------------------------------------------

void* Image::readImageBuffer() throw(::gdcmIO::exception::Failed)
{
    // Retrieve GDCM image
    SPTR(::gdcm::ImageReader) imageReader = ::boost::static_pointer_cast< ::gdcm::ImageReader >(m_reader);
    ::gdcm::Image &gdcmImage = imageReader->GetImage();

    // Retrieve Datasets
    ::gdcm::DataSet &gdcmDatasetRoot = imageReader->GetFile().GetDataSet();
    ::gdcm::DataSet &gdcmDatasetHeader = imageReader->GetFile().GetHeader();

    // Path container
    ::fwDicomData::DicomSeries::DicomPathContainerType pathContainer = m_dicomSeries->getLocalDicomPaths();

    // Raw buffer for all frames
    char *gdcmGlobalBuffer;
    const unsigned long frameSize = gdcmImage.GetBufferLength();
    const unsigned long imageSize = frameSize * pathContainer.size();
    OSLM_TRACE("Global buffer size : " << imageSize);

    // Allocate raw buffer
    try
    {
        gdcmGlobalBuffer = new char[imageSize];
    }
    catch (...)
    {
        throw ::gdcmIO::exception::Failed("There is not enough memory available to open this image.");
    }

    // Read every frames
    unsigned int frameNumber = 0;
    BOOST_FOREACH(::fwDicomData::DicomSeries::DicomPathContainerType::value_type v, pathContainer)
    {
        // Get filename
        const std::string filename = v.second.string();

        // Clear data because GDCM reader does not read twice the same tag
        gdcmDatasetRoot.Clear();
        gdcmDatasetHeader.Clear();

        // Read a frame
        imageReader->SetFileName( filename.c_str() );
        if ( imageReader->Read() )
        {
            // Get raw buffer and set it in the image buffer
            if ( !gdcmImage.GetBuffer( gdcmGlobalBuffer + frameNumber * frameSize ) )
            {
                throw ::gdcmIO::exception::Failed("Failed to get a frame buffer");
            }
        }
        else
        {
            std::stringstream ss;
            ss << "Reading error on frame : " << frameNumber;
            throw ::gdcmIO::exception::Failed(ss.str());
        }

        // Reference SOP Instance UID in dicomInstance for SR reading
        const std::string sopInstanceUID = ::gdcmIO::helper::DicomData::getTrimmedTagValue<0x0008,0x0018>(gdcmDatasetRoot);
        if(!sopInstanceUID.empty())
        {
            m_instance->getRefSOPInstanceUIDContainer().push_back(sopInstanceUID);
        }
        OSLM_WARN_IF("Empty SOP instance UID found", sopInstanceUID.empty());

        // Next frame
        ++frameNumber;
    }

    return (void*)gdcmGlobalBuffer;
}

//------------------------------------------------------------------------------

} // namespace ie
} // namespace reader
} // namespace gdcmIO
