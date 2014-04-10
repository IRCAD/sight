/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include <gdcmImageHelper.h>
#include <gdcmImageReader.h>
#include <gdcmIPPSorter.h>
#include <gdcmPixelFormat.h>
#include <gdcmScanner.h>
#include <gdcmRescaler.h>
#include <gdcmImageApplyLookupTable.h>

#include <fwComEd/helper/Array.hpp>
#include <fwDicomIOExt/dcmtk/helper/Image.hpp>
#include <fwMedData/ImageSeries.hpp>
#include <fwTools/dateAndTime.hpp>
#include <fwTools/fromIsoExtendedString.hpp>

#include "gdcmIO/reader/iod/CTMRImageIOD.hpp"
#include "gdcmIO/helper/DicomData.hpp"

namespace gdcmIO
{
namespace reader
{
namespace iod
{

//------------------------------------------------------------------------------

CTMRImageIOD::CTMRImageIOD(::fwDicomData::DicomSeries::sptr dicomSeries,
        SPTR(::gdcmIO::container::DicomInstance) instance) :
        ::gdcmIO::reader::iod::InformationObjectDefinition(dicomSeries, instance)
{
    // Instantiates a specific reader
    m_gdcmReader = ::boost::shared_ptr< ::gdcm::ImageReader >( new ::gdcm::ImageReader );
}

//------------------------------------------------------------------------------

CTMRImageIOD::~CTMRImageIOD()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void CTMRImageIOD::read(::fwMedData::Series::sptr series) throw(::gdcmIO::exception::Failed)
{
    SLM_TRACE_FUNC();

    // Get the gdcm reader (instantiate in the constructor)
    ::gdcm::ImageReader & imageReader = *( ::boost::static_pointer_cast< ::gdcm::ImageReader >( m_gdcmReader ) );

    ::fwDicomData::DicomSeries::DicomPathContainerType pathContainer = m_dicomSeries->getLocalDicomPaths();

    // Read the first file and others if necessary
    imageReader.SetFileName( pathContainer.begin()->second.string().c_str() );
    if (imageReader.Read())
    {
        try
        {
            this->readImage(series);
        }
        catch (::gdcmIO::exception::Failed &e)
        {
            std::stringstream ss;
            ss << "Error found in DICOM image : " << e.what();
            throw ::gdcmIO::exception::Failed(ss.str());
        }
    }
    else
    {
        throw ::gdcmIO::exception::Failed("Unable to read the DICOM image using the GDCM Image Reader.");
    }
}

//------------------------------------------------------------------------------

void CTMRImageIOD::readImage(::fwMedData::Series::sptr series) throw(::gdcmIO::exception::Failed)
{
    SLM_TRACE_FUNC();

    // Retrieve images
    ::fwMedData::ImageSeries::sptr imageSeries = ::fwMedData::ImageSeries::dynamicCast(series);
    SLM_ASSERT("ImageSeries should not be null.", imageSeries);
    ::fwData::Image::sptr image = imageSeries->getImage();
    ::boost::shared_ptr< ::gdcm::ImageReader > imageReader =
            ::boost::static_pointer_cast< ::gdcm::ImageReader >( m_gdcmReader );
    ::gdcm::Image &gdcmImage = imageReader->GetImage();
    const ::gdcm::DataSet &dataset = imageReader->GetFile().GetDataSet();

    // Path container
    ::fwDicomData::DicomSeries::DicomPathContainerType pathContainer = m_dicomSeries->getLocalDicomPaths();

    // DicomInstance is necessary for SR reading
    SLM_ASSERT("gdcmIO::DicomInstance not set", m_instance);

    // Set wether there is several files or not
    m_instance->setIsMultiFiles( pathContainer.size() > 1 );

    // Image origin
    const double *gdcmOrigin = gdcmImage.GetOrigin();
    ::fwData::Image::OriginType origin (3,0);
    if ( gdcmOrigin != 0 )
    {
        std::copy( gdcmOrigin, gdcmOrigin+3, origin.begin() );
    }
    image->setOrigin( origin );
    OSLM_TRACE("Image's origin : " << origin[0] << " : " << origin[1] << " : " <<origin[2]);

    // Image dimension
    const unsigned int dimension = gdcmImage.GetNumberOfDimensions();
    OSLM_TRACE("Image's dimension : " << dimension);

    // Image's spacing
    const double * gdcmSpacing = gdcmImage.GetSpacing();
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
    image->setSpacing( spacing );

    //Image's window center (double)
    std::string windowCenter = ::gdcmIO::helper::DicomData::getTrimmedTagValue<0x0028,0x1050>(dataset);
    std::vector<std::string> splitedWindowCenters;
    if ( !windowCenter.empty() )
    {
        // If there is several window center we only take the first one
        ::boost::split( splitedWindowCenters, windowCenter, ::boost::is_any_of( "\\" ) );
        image->setWindowCenter( ::boost::lexical_cast< double >(splitedWindowCenters[0]));
    }
    OSLM_TRACE("Image's window center : " << image->getWindowCenter());

    //Image's window width (double)
    std::string windowWidth = ::gdcmIO::helper::DicomData::getTrimmedTagValue<0x0028,0x1051>(dataset);
    std::vector<std::string> splitedWindowWidth;
    if ( !windowWidth.empty() )
    {
        // If there is several window width we only take the first one
        ::boost::split( splitedWindowWidth, windowWidth, ::boost::is_any_of( "\\" ) );
        image->setWindowWidth( ::boost::lexical_cast< double >(splitedWindowWidth[0]));
    }
    OSLM_TRACE("Image's window width : " << image->getWindowWidth());


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
    image->setType(imageType);
    ::gdcm::PixelFormat targetPixelFormat = ::gdcmIO::helper::DicomData::getPixelType(image);

    // Compute number of components
    std::string photometricInterpretation = ::gdcmIO::helper::DicomData::getTrimmedTagValue<0x0028,0x0004>(dataset);
    std::string pixelPresentation = ::gdcmIO::helper::DicomData::getTrimmedTagValue<0x0008,0x9205>(dataset);

    if(photometricInterpretation == "MONOCHROME2")
    {
        image->setNumberOfComponents(1);
    }
    else if(photometricInterpretation == "RGB" || photometricInterpretation == "YBR")
    {
        image->setNumberOfComponents(3);
    }
    else if(photometricInterpretation == "ARGB" || photometricInterpretation == "CMYK")
    {
        image->setNumberOfComponents(4);
    }
    else if(photometricInterpretation == "PALETTE COLOR" || pixelPresentation == "COLOR")
    {
        image->setNumberOfComponents(3);
    }
    else
    {
        std::stringstream ss;
        ss << "The photometric interpretation \"" + photometricInterpretation + "\" is not supported.";
        throw ::gdcmIO::exception::Failed(ss.str());
    }

    // Update image size
    std::vector<unsigned int> dimensions = ::gdcm::ImageHelper::GetDimensionsValue(imageReader->GetFile());
    unsigned short sizeZ = pathContainer.size();
    if(sizeZ == 1)
    {
        sizeZ = gdcmImage.GetBufferLength() / (dimensions[0] * dimensions[1] * (bitsAllocated/8));
    }
    image->setSize(::boost::assign::list_of(dimensions[0])(dimensions[1])(sizeZ));

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
        rescaler.Rescale(finalBuffer, static_cast<const char*>(temporaryBuffer), imageSize);
    }

    // Set image buffer
    ::fwData::Array::sptr array = image->getDataArray();
    ::fwComEd::helper::Array helper(array);
    helper.setBuffer(finalBuffer, true, image->getType(), image->getSize(), 1);

    // Delete temporary buffer
    delete temporaryBuffer;
}

//------------------------------------------------------------------------------

void* CTMRImageIOD::readImageBuffer() throw(::gdcmIO::exception::Failed)
{
    ::boost::shared_ptr< ::gdcm::ImageReader >  imageReader =
            ::boost::static_pointer_cast< ::gdcm::ImageReader >( m_gdcmReader );
    ::gdcm::Image &gdcmImage = imageReader->GetImage();
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
        std::string filename = v.second.string();

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
        const std::string sopInstanceUID = ::gdcmIO::helper::DicomData::getTagValue<0x0008,0x0018>(gdcmDatasetRoot);
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

}  // namespace iod
}  // namespace reader
}  // namespace gdcmIO
