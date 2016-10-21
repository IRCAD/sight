/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGdcmIO/reader/ie/Image.hpp"

#include "fwGdcmIO/helper/DicomData.hpp"

#include <fwData/Image.hpp>

#include <fwDataTools/helper/Array.hpp>
#include <fwDataTools/helper/Image.hpp>

#include <fwDicomTools/Image.hpp>

#include <fwMath/VectorFunctions.hpp>

#include <fwMedData/DicomSeries.hpp>

#include <gdcmIPPSorter.h>
#include <gdcmImageApplyLookupTable.h>
#include <gdcmImageHelper.h>
#include <gdcmImageReader.h>
#include <gdcmPixelFormat.h>
#include <gdcmRescaler.h>
#include <gdcmUIDGenerator.h>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/math/special_functions/round.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/lu.hpp>

namespace fwGdcmIO
{
namespace reader
{
namespace ie
{

//------------------------------------------------------------------------------

Image::Image(SPTR(::fwMedData::DicomSeries)dicomSeries,
             SPTR(::gdcm::Reader)reader,
             SPTR(::fwGdcmIO::container::DicomInstance)instance,
             ::fwData::Image::sptr image,
             ::fwLog::Logger::sptr logger,
             const ProgressCallback& callback,
             const bool& cancelled) :
    ::fwGdcmIO::reader::ie::InformationEntity< ::fwData::Image >(dicomSeries, reader, instance, image, logger,
                                                                 callback, cancelled),
    m_enableBufferRotation(true)
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
    SPTR(::gdcm::ImageReader) imageReader = std::static_pointer_cast< ::gdcm::ImageReader >(m_reader);
    const ::gdcm::Image& gdcmImage = imageReader->GetImage();

    // Retrieve dataset
    const ::gdcm::DataSet& dataset = m_reader->GetFile().GetDataSet();

    // Image Position (Patient) - Type 1
    const double* gdcmOrigin = gdcmImage.GetOrigin();
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
    std::string sliceThickness       = ::fwGdcmIO::helper::DicomData::getTrimmedTagValue<0x0018,0x0050>(dataset);
    std::string spacingBetweenSlices = ::fwGdcmIO::helper::DicomData::getTrimmedTagValue<0x0018,0x0088>(dataset);

    if(m_dicomSeries->hasComputedValues("SliceThickness"))
    {
        spacing[2] = ::boost::lexical_cast< double >(m_dicomSeries->getComputedTagValues().at("SliceThickness"));
        std::stringstream ss;
        ss << "Voxel depth has been computed using the image slice positions. The computed value is " << spacing[2] <<
            " instead of " << sliceThickness << " for the SliceThickness tag.";
        m_logger->warning(ss.str());
    }
    else if(!sliceThickness.empty())
    {
        spacing[2] = ::boost::lexical_cast< double >(sliceThickness);
        std::stringstream ss;
        ss << "Voxel depth has been retrieved using the SliceThickness tag value. The retrieved value is " <<
            spacing[2] << ".";
        m_logger->information(ss.str());
    }
    else if(!spacingBetweenSlices.empty())
    {
        spacing[2] = ::boost::lexical_cast< double >(spacingBetweenSlices);
        std::stringstream ss;
        ss << "Voxel depth has been retrieved using the SpacingBetweenSlices tag value. The retrieved value is " <<
            spacing[2] << ".";
        m_logger->information(ss.str());
    }

    OSLM_TRACE("Image's spacing : "<<spacing[0]<<"x"<<spacing[1]<<"x"<<spacing[2]);
    m_object->setSpacing( spacing );

}

//------------------------------------------------------------------------------

void Image::readVOILUTModule()
{
    // Retrieve dataset
    const ::gdcm::DataSet& dataset = m_reader->GetFile().GetDataSet();

    //Image's window center (double)
    std::string windowCenter = ::fwGdcmIO::helper::DicomData::getTrimmedTagValue<0x0028,0x1050>(dataset);
    std::vector<std::string> splitedWindowCenters;
    if ( !windowCenter.empty() )
    {
        // If there is several window center we only take the first one
        ::boost::split( splitedWindowCenters, windowCenter, ::boost::is_any_of( "\\" ) );
        m_object->setWindowCenter( ::boost::lexical_cast< double >(splitedWindowCenters[0]));
    }
    OSLM_TRACE("Image's window center : " << m_object->getWindowCenter());

    //Image's window width (double)
    std::string windowWidth = ::fwGdcmIO::helper::DicomData::getTrimmedTagValue<0x0028,0x1051>(dataset);
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

std::vector<double> getRescaleInterceptSlopeValue(::gdcm::ImageReader* imageReader)
{
    // Retrieve dataset
    const ::gdcm::DataSet& dataset = imageReader->GetFile().GetDataSet();

    // Retrieve rescale values
    std::vector< double > rescale = ::gdcm::ImageHelper::GetRescaleInterceptSlopeValue(imageReader->GetFile());

    // Correct Rescale Intercept and Rescale Slope as GDCM may fail to retrieve them.
    if(dataset.FindDataElement(::gdcm::Tag(0x0028,0x1052)) && dataset.FindDataElement(::gdcm::Tag(0x0028,0x1053)))
    {
        rescale[0] = ::fwGdcmIO::helper::DicomData::getTagValue<0x0028,0x1052, double>(dataset);
        rescale[1] = ::fwGdcmIO::helper::DicomData::getTagValue<0x0028,0x1053, double>(dataset);
    }

    return rescale;
}

//------------------------------------------------------------------------------

void Image::readImagePixelModule()
{
    // Retrieve GDCM image
    SPTR(::gdcm::ImageReader) imageReader = std::static_pointer_cast< ::gdcm::ImageReader >(m_reader);
    const ::gdcm::Image& gdcmImage = imageReader->GetImage();

    // Retrieve dataset
    const ::gdcm::DataSet& dataset = m_reader->GetFile().GetDataSet();

    // Retrieve image information before processing the rescaling
    ::gdcm::PixelFormat pixelFormat = ::gdcm::ImageHelper::GetPixelFormatValue(imageReader->GetFile());

    // Retrieve rescale intercept/slope values
    std::vector<double> rescale = getRescaleInterceptSlopeValue(imageReader.get());
    double rescaleIntercept     = rescale[0];
    double rescaleSlope         = rescale[1];

    unsigned short samplesPerPixel     = pixelFormat.GetSamplesPerPixel();
    unsigned short bitsAllocated       = pixelFormat.GetBitsAllocated();
    unsigned short bitsStored          = pixelFormat.GetBitsStored();
    unsigned short highBit             = pixelFormat.GetHighBit();
    unsigned short pixelRepresentation = pixelFormat.GetPixelRepresentation();

    // Compute final image type
    ::fwDicomTools::Image imageHelper(
        samplesPerPixel,bitsAllocated,bitsStored, highBit, pixelRepresentation, rescaleSlope, rescaleIntercept);
    ::fwTools::Type imageType = imageHelper.findImageTypeFromMinMaxValues();
    m_object->setType(imageType);
    ::gdcm::PixelFormat targetPixelFormat = ::fwGdcmIO::helper::DicomData::getPixelType(m_object);

    // Compute number of components
    std::string photometricInterpretation = ::fwGdcmIO::helper::DicomData::getTrimmedTagValue<0x0028,0x0004>(dataset);
    std::string pixelPresentation         = ::fwGdcmIO::helper::DicomData::getTrimmedTagValue<0x0008,0x9205>(dataset);

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
        throw ::fwGdcmIO::exception::Failed(ss.str());
    }

    // Retrieve image dimensions
    std::vector<unsigned int> dimensions = ::gdcm::ImageHelper::GetDimensionsValue(imageReader->GetFile());

    // Compute real image size (we assume every instance has the same number of
    // slices (1 for CT and MR, may be more for enhanced CT and MR)
    const unsigned long frameBufferSize = gdcmImage.GetBufferLength();
    const unsigned long depth           = frameBufferSize / (dimensions[0] * dimensions[1] * (bitsAllocated/8));
    dimensions[2] = static_cast<unsigned int>(m_dicomSeries->getLocalDicomPaths().size() * depth);
    m_object->setSize(::boost::assign::list_of(dimensions[0])(dimensions[1])(dimensions[2]));

    OSLM_TRACE("Image dimensions : [" << dimensions[0] << "," <<dimensions[1] << "," << dimensions[2] << "]");

    const unsigned long imageBufferSize    = dimensions[0] * dimensions[1] * dimensions[2] * (bitsAllocated/8);
    const unsigned long newImageBufferSize = dimensions[0] * dimensions[1] * dimensions[2] *
                                             (targetPixelFormat.GetBitsAllocated()/8);

    OSLM_TRACE("Image buffer size : " << imageBufferSize);
    OSLM_TRACE("New image buffer size : " << newImageBufferSize);

    // Let's read the image buffer
    bool performRescale = (photometricInterpretation != "PALETTE COLOR" && pixelPresentation != "COLOR");
    char* imageBuffer   = this->readImageBuffer(dimensions, bitsAllocated, targetPixelFormat.GetBitsAllocated(),
                                                performRescale);


    // Correct image buffer according to the image orientation
    if(!m_cancelled && m_enableBufferRotation)
    {
        imageBuffer = this->correctImageOrientation(imageBuffer, dimensions,
                                                    (performRescale) ? targetPixelFormat.GetBitsAllocated() : bitsAllocated);
    }

    // Apply lookup table if required
    if(!m_cancelled && (photometricInterpretation == "PALETTE COLOR" || pixelPresentation == "COLOR"))
    {
        try
        {
            // Create new buffer
            char* coloredBuffer = 0;
            coloredBuffer = new char[newImageBufferSize*3];

            // Apply lookup
            gdcmImage.GetLUT().Decode(coloredBuffer, newImageBufferSize*3, imageBuffer, imageBufferSize);

            // Swap buffers
            delete[] imageBuffer;
            imageBuffer = coloredBuffer;
        }
        catch (...)
        {
            throw ::fwGdcmIO::exception::Failed("There is not enough memory available to open this image.");
        }
    }

    // Set image buffer
    ::fwData::Array::sptr array = m_object->getDataArray();
    ::fwDataTools::helper::Array helper(array);
    helper.setBuffer(imageBuffer, true, m_object->getType(), m_object->getSize(), m_object->getNumberOfComponents());

}

//------------------------------------------------------------------------------

char* Image::readImageBuffer(const std::vector<unsigned int>& dimensions, unsigned short bitsAllocated,
                             unsigned short newBitsAllocated, bool performRescale) throw(::fwGdcmIO::exception::Failed)
{
    // Retrieve GDCM image
    SPTR(::gdcm::ImageReader) imageReader = std::static_pointer_cast< ::gdcm::ImageReader >(m_reader);
    const ::gdcm::Image& gdcmFirstImage = imageReader->GetImage();

    // Path container
    ::fwMedData::DicomSeries::DicomPathContainerType pathContainer = m_dicomSeries->getLocalDicomPaths();

    // Raw buffer for all frames
    char* frameBuffer;
    char* imageBuffer;
    const unsigned long frameBufferSize    = gdcmFirstImage.GetBufferLength();
    const unsigned long newFrameBufferSize = frameBufferSize * (newBitsAllocated/bitsAllocated);
    const unsigned long imageBufferSize    = dimensions.at(0) * dimensions.at(1) * dimensions.at(2) *
                                             ((performRescale ? newBitsAllocated : bitsAllocated)/8);

    // Allocate raw buffer
    try
    {
        frameBuffer = new char[frameBufferSize];
        imageBuffer = new char[imageBufferSize];
    }
    catch (...)
    {
        throw ::fwGdcmIO::exception::Failed("There is not enough memory available to open this image.");
    }

    // Read every frames
    unsigned int frameNumber = 0;
    for(::fwMedData::DicomSeries::DicomPathContainerType::value_type v: pathContainer)
    {
        // Get filename
        const std::string& filename = v.second.string();

        // Read a frame
        ::gdcm::ImageReader frameReader;
        frameReader.SetFileName( filename.c_str() );
        if ( frameReader.Read() )
        {
            const ::gdcm::Image& gdcmImage = frameReader.GetImage();

            // Check frame buffer size
            if(frameBufferSize != gdcmImage.GetBufferLength())
            {
                throw ::fwGdcmIO::exception::Failed("The frame buffer does not have the expected size : " + filename);
            }

            // Get raw buffer and set it in the image buffer
            if ( !gdcmImage.GetBuffer( frameBuffer ) )
            {
                throw ::fwGdcmIO::exception::Failed("Failed to get a frame buffer");
            }
        }
        else
        {
            std::stringstream ss;
            ss << "Reading error on frame : " << frameNumber;
            throw ::fwGdcmIO::exception::Failed(ss.str());
        }


        // Rescale
        if(performRescale)
        {
            // Retrieve rescale intercept/slope values
            std::vector<double> rescale = getRescaleInterceptSlopeValue(&frameReader);
            double rescaleIntercept     = rescale[0];
            double rescaleSlope         = rescale[1];

            // Retrieve image information before processing the rescaling
            ::gdcm::PixelFormat pixelFormat =
                ::gdcm::ImageHelper::GetPixelFormatValue(frameReader.GetFile());
            ::gdcm::PixelFormat::ScalarType scalarType = pixelFormat.GetScalarType();
            ::gdcm::PixelFormat targetPixelFormat      = ::fwGdcmIO::helper::DicomData::getPixelType(m_object);

            // Create rescaler
            ::gdcm::Rescaler rescaler;
            rescaler.SetIntercept(rescaleIntercept);
            rescaler.SetSlope(rescaleSlope);
            rescaler.SetPixelFormat(scalarType);
            rescaler.SetTargetPixelType(targetPixelFormat.GetScalarType());
            rescaler.SetUseTargetPixelType(true);

            // Rescale the image
            rescaler.Rescale(imageBuffer + (frameNumber * newFrameBufferSize), frameBuffer, frameBufferSize);
        }
        else
        {
            // Copy bytes
            memcpy(imageBuffer + frameNumber * frameBufferSize, frameBuffer, frameBufferSize);
        }

        // Reference SOP Instance UID in dicomInstance for SR reading
        const ::gdcm::DataSet& gdcmDatasetRoot = frameReader.GetFile().GetDataSet();
        const std::string sopInstanceUID       = ::fwGdcmIO::helper::DicomData::getTrimmedTagValue<0x0008,0x0018>(
            gdcmDatasetRoot);
        if(!sopInstanceUID.empty())
        {
            m_instance->getRefSOPInstanceUIDContainer().push_back(sopInstanceUID);
        }
        else
        {
            m_logger->warning("A frame with an undefined SOP instance UID has been detected.");
        }

        // Next frame
        ++frameNumber;

        unsigned int progress =
            static_cast<unsigned int>(18 + (frameNumber*100/static_cast<double>(pathContainer.size())) * 0.6);
        m_progressCallback(progress);

        if(m_cancelled)
        {
            break;
        }
    }

    // Delete frame buffer
    delete[] frameBuffer;

    return imageBuffer;
}

//------------------------------------------------------------------------------

char* Image::correctImageOrientation(char* buffer, const std::vector<unsigned int>& dimensions,
                                     unsigned short bitsAllocated)
{
    char* result = buffer;

    // Retrieve GDCM image
    SPTR(::gdcm::ImageReader) imageReader = std::static_pointer_cast< ::gdcm::ImageReader >(m_reader);
    const ::gdcm::Image& gdcmImage = imageReader->GetImage();

    // Retrieve image orientation
    const double* directionCosines = gdcmImage.GetDirectionCosines();

    // Compute U vector
    fwVec3d imageOrientationU =
    {::boost::math::round(directionCosines[0]), ::boost::math::round(directionCosines[1]),
     ::boost::math::round(directionCosines[2])};
    // Try to find the closest axe
    if((fabs(imageOrientationU[0]) + fabs(imageOrientationU[1]) + fabs(imageOrientationU[2])) > 1)
    {
        if(fabs(directionCosines[0]) < fabs(directionCosines[1]) ||
           fabs(directionCosines[0]) < fabs(directionCosines[2]))
        {
            imageOrientationU[0] = 0;
        }
        if(fabs(directionCosines[1]) < fabs(directionCosines[0]) ||
           fabs(directionCosines[1]) < fabs(directionCosines[2]))
        {
            imageOrientationU[1] = 0;
        }
        if(fabs(directionCosines[2]) < fabs(directionCosines[0]) ||
           fabs(directionCosines[2]) < fabs(directionCosines[1]))
        {
            imageOrientationU[2] = 0;
        }
        m_logger->warning(
            "Unable to determine clearly the orientation of the image. The software may display the image in the wrong direction.");
    }

    // Compute V vector
    fwVec3d imageOrientationV =
    {::boost::math::round(directionCosines[3]), ::boost::math::round(directionCosines[4]),
     ::boost::math::round(directionCosines[5])};
    // Try to find the closest axe
    if((fabs(imageOrientationV[0]) + fabs(imageOrientationV[1]) + fabs(imageOrientationV[2])) > 1)
    {
        if(fabs(directionCosines[3]) < fabs(directionCosines[4]) ||
           fabs(directionCosines[3]) < fabs(directionCosines[5]))
        {
            imageOrientationV[0] = 0;
        }
        if(fabs(directionCosines[4]) < fabs(directionCosines[3]) ||
           fabs(directionCosines[4]) < fabs(directionCosines[5]))
        {
            imageOrientationV[1] = 0;
        }
        if(fabs(directionCosines[5]) < fabs(directionCosines[3]) ||
           fabs(directionCosines[5]) < fabs(directionCosines[4]))
        {
            imageOrientationV[2] = 0;
        }
        m_logger->warning(
            "Unable to determine clearly the orientation of the image. The software may display the image in the wrong direction.");
    }

    // Compute W vector
    const fwVec3d imageOrientationW = ::fwMath::cross(imageOrientationU, imageOrientationV);

    // Create orientation matrix
    Image::MatrixType matrix(4,4);
    matrix(0,0) = imageOrientationU[0];
    matrix(1,0) = imageOrientationU[1];
    matrix(2,0) = imageOrientationU[2];
    matrix(3,0) = 0;
    matrix(0,1) = imageOrientationV[0];
    matrix(1,1) = imageOrientationV[1];
    matrix(2,1) = imageOrientationV[2];
    matrix(3,1) = 0;
    matrix(0,2) = imageOrientationW[0];
    matrix(1,2) = imageOrientationW[1];
    matrix(2,2) = imageOrientationW[2];
    matrix(3,2) = 0;
    matrix(0,3) = 0;
    matrix(1,3) = 0;
    matrix(2,3) = 0;
    matrix(3,3) = 1;

    // Compute inverse matrix in order to rotate the buffer
    Image::MatrixType inverseMatrix  = this->computeInverseMatrix(matrix);
    Image::MatrixType identityMatrix = ::boost::numeric::ublas::identity_matrix< double >(inverseMatrix.size1());

    // Check whether the image must be rotated or not
    if(!::boost::numeric::ublas::detail::expression_type_check(inverseMatrix,identityMatrix))
    {
        // Compute new image size
        VectorType sizeVector(4);
        sizeVector(0) = dimensions.at(0);
        sizeVector(1) = dimensions.at(1);
        sizeVector(2) = dimensions.at(2);
        VectorType newSizeVector = ::boost::numeric::ublas::prod(sizeVector, inverseMatrix);
        unsigned short newSizeX  = static_cast<unsigned short>(fabs(newSizeVector[0]));
        unsigned short newSizeY  = static_cast<unsigned short>(fabs(newSizeVector[1]));
        unsigned short newSizeZ  = static_cast<unsigned short>(fabs(newSizeVector[2]));
        newSizeVector(0) = newSizeX;
        newSizeVector(1) = newSizeY;
        newSizeVector(2) = newSizeZ;

        // Compute old size from the new absolute size in order to retrieve pixel flips
        VectorType oldSizeVector = ::boost::numeric::ublas::prod(newSizeVector, matrix);

        // Create new buffer to store rotated image
        const unsigned long size = dimensions.at(0) * dimensions.at(1) * dimensions.at(2) * (bitsAllocated/8);
        char* newBuffer          = new char[size];

        // Rotate image
        unsigned short x, y, z, oldx, oldy, oldz;
        for(z = 0; z < newSizeZ && !m_cancelled; ++z)
        {
            for(y = 0; y < newSizeY; ++y)
            {
                for(x = 0; x < newSizeX; ++x)
                {
                    // Create new position
                    VectorType newPosition(4);
                    newPosition(0) = x;
                    newPosition(1) = y;
                    newPosition(2) = z;

                    // Compute old position
                    VectorType oldPosition = ::boost::numeric::ublas::prod(newPosition, matrix);
                    oldx =
                        (oldSizeVector[0]>
                         0) ? static_cast<unsigned short>(oldPosition[0]) : static_cast<unsigned short>((
                                                                                                            dimensions.
                                                                                                            at(0)-1)+
                                                                                                        oldPosition[0]);
                    oldy =
                        (oldSizeVector[1]>
                         0) ? static_cast<unsigned short>(oldPosition[1]) : static_cast<unsigned short>((
                                                                                                            dimensions.
                                                                                                            at(1)-1)+
                                                                                                        oldPosition[1]);
                    oldz =
                        (oldSizeVector[2]>
                         0) ? static_cast<unsigned short>(oldPosition[2]) : static_cast<unsigned short>((
                                                                                                            dimensions.
                                                                                                            at(2)-1)+
                                                                                                        oldPosition[2]);

                    // Compute indices
                    unsigned int positionIndex    = (x + (y*newSizeX) + z*(newSizeX*newSizeY)) * (bitsAllocated/8);
                    unsigned int oldPositionIndex =
                        (oldx + (oldy*dimensions.at(0)) + oldz*(dimensions.at(0)*dimensions.at(1))) *
                        (bitsAllocated/8);

                    // Copy bytes
                    memcpy(&newBuffer[positionIndex], &buffer[oldPositionIndex], (bitsAllocated/8));
                }
            }
            unsigned int progress = static_cast<unsigned int>(78 + (z*100./newSizeZ) * 0.2);
            m_progressCallback(progress);
        }

        result = newBuffer;
        delete[] buffer;

        // Update image size
        m_object->setSize(::boost::assign::list_of(newSizeX)(newSizeY)(newSizeZ));

        // Update image spacing
        ::fwData::Image::SpacingType spacing = m_object->getSpacing();
        VectorType spacingVector(4);
        spacingVector(0) = spacing[0];
        spacingVector(1) = spacing[1];
        spacingVector(2) = spacing[2];
        VectorType newSpacingVector = ::boost::numeric::ublas::prod(spacingVector,inverseMatrix);
        ::fwData::Image::SpacingType newSpacing(3,1);
        newSpacing[0] = fabs(newSpacingVector[0]);
        newSpacing[1] = fabs(newSpacingVector[1]);
        newSpacing[2] = fabs(newSpacingVector[2]);
        m_object->setSpacing( newSpacing );

        // Update image origin
        ::fwData::Image::OriginType origin = m_object->getOrigin();
        VectorType originVector(4);
        originVector(0) = origin[0];
        originVector(1) = origin[1];
        originVector(2) = origin[2];
        VectorType newOriginVector = ::boost::numeric::ublas::prod(originVector,inverseMatrix);
        ::fwData::Image::OriginType newOrigin(3,0);
        newOrigin[0] = newOriginVector[0];
        newOrigin[1] = newOriginVector[1];
        newOrigin[2] = newOriginVector[2];
        m_object->setOrigin( newOrigin );

        m_logger->warning(
            "Image buffer has been rotated in order to match patient orientation : image orgin could be wrong.");
    }

    return result;
}

//------------------------------------------------------------------------------

Image::MatrixType Image::computeInverseMatrix(MatrixType matrix)
{
    // Create output matrix (identity)
    Image::MatrixType output(matrix.size1(),matrix.size2());
    output.assign(::boost::numeric::ublas::identity_matrix< double >(output.size1()));

    // Create a permutation matrix for the LU-factorization
    boost::numeric::ublas::permutation_matrix< std::size_t > perm(matrix.size1());

    // Perform LU-factorization
    long unsigned int res = boost::numeric::ublas::lu_factorize(matrix, perm);
    if (res != 0)
    {
        SLM_WARN("Cannot compute matrix.");
    }
    else
    {
        // Backsubstitute to get the inverse
        boost::numeric::ublas::lu_substitute(matrix, perm, output);
    }

    // Return inverse matrix or identity
    return output;
}

//------------------------------------------------------------------------------

} // namespace ie
} // namespace reader
} // namespace fwGdcmIO
