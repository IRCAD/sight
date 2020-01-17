/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "fwDcmtkIO/reader/ImageStorageReader.hpp"

#include "fwDcmtkIO/reader/main/ImageLazyStream.hpp"
#include "fwDcmtkIO/reader/main/ImageReader.hpp"
#include "fwDcmtkIO/reader/rgblookup/ImageRGBLookupLazyStream.hpp"
#include "fwDcmtkIO/reader/rgblookup/ImageRGBLookupReader.hpp"

#include <fwDataTools/helper/Array.hpp>

#include <fwDicomTools/Image.hpp>
#include <fwDicomTools/Series.hpp>

#include <fwMedData/ImageSeries.hpp>

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcistrmb.h>
#include <dcmtk/dcmimgle/dcmimage.h>
#include <dcmtk/dcmnet/diutil.h>

#include <algorithm>

namespace fwDcmtkIO
{
namespace reader
{

ImageStorageReader::ImageStorageReader()
{
}

//-----------------------------------------------------------------------------

ImageStorageReader::~ImageStorageReader()
{
}

//-----------------------------------------------------------------------------

::fwMedData::Series::sptr ImageStorageReader::read(const ::fwMedData::DicomSeries::csptr& series)
{
    ::fwMedData::DicomSeries::SOPClassUIDContainerType sopClassUIDContainer = series->getSOPClassUIDs();
    std::string sopClassUID = dcmFindNameOfUID(sopClassUIDContainer.begin()->c_str());

    ::fwMedData::ImageSeries::sptr imageSeries = ::fwDicomTools::Series::convertToImageSeries(series);
    DicomContainerType instances = series->getDicomContainer();

    ::fwData::Image::sptr image = ::fwData::Image::New();
    DcmFileFormat fileFormat;
    OFCondition status;
    DcmDataset* dataset;

    //Get informations from the first instance
    const auto firstItem                           = series->getDicomContainer().begin();
    const ::fwMemory::BufferObject::sptr bufferObj = firstItem->second;
    const size_t buffSize                          = bufferObj->getSize();
    const std::string dicomPath                    = bufferObj->getStreamInfo().fsFile.string();
    ::fwMemory::BufferObject::Lock lock(bufferObj);
    char* buffer = static_cast< char* >( lock.getBuffer() );

    DcmInputBufferStream is;
    is.setBuffer(buffer, offile_off_t(buffSize));
    is.setEos();

    fileFormat.transferInit();
    if (!fileFormat.read(is).good())
    {
        FW_RAISE("Unable to read Dicom file '"<< dicomPath <<"' "<<
                 "(slice: '" << firstItem->first << "')");
    }

    fileFormat.loadAllDataIntoMemory();
    fileFormat.transferEnd();

    dataset = fileFormat.getDataset();

    DicomImage dicomImage(dataset, dataset->getOriginalXfer());

    FW_RAISE_IF("Unable to read the file: \""+dicomPath+"\"", status.bad() || (
                    dicomImage.getStatus() != EIS_Normal
                    && dicomImage.getStatus() != EIS_MissingAttribute
                    && dicomImage.getStatus() != EIS_NotSupportedValue
                    ));

    // Decompress data set if compressed
    dataset->chooseRepresentation(EXS_LittleEndianExplicit, nullptr);

    if(dicomImage.getStatus() != EIS_MissingAttribute)
    {
        SLM_WARN("Some informations are missing. The file may have not been read properly.");
    }

    //Spacing
    double spacing[3];
    dataset->findAndGetFloat64(DCM_PixelSpacing, spacing[0], 0);
    dataset->findAndGetFloat64(DCM_PixelSpacing, spacing[1], 1);
    dataset->findAndGetFloat64(DCM_SliceThickness, spacing[2]);

    if(series->hasComputedValues("SliceThickness"))
    {
        spacing[2] = ::boost::lexical_cast< double >(series->getComputedTagValues().at("SliceThickness"));
    }

    if(spacing[0] == 0 || spacing[1] == 0 || spacing[2] == 0)
    {
        spacing[0] = spacing[1] = spacing[2] = 1;
        OSLM_WARN("Invalid value for pixel spacing. Assuming pixel value is 1.");
    }

    image->setSpacing(std::vector< double >(spacing, spacing+3));

    //Origin
    //TODO: Compute the correct origin
    double imagePosition[3];
    dataset->findAndGetFloat64(DCM_ImagePositionPatient, imagePosition[0], 0);
    dataset->findAndGetFloat64(DCM_ImagePositionPatient, imagePosition[1], 1);
    dataset->findAndGetFloat64(DCM_ImagePositionPatient, imagePosition[2], 2);
    image->setOrigin( { imagePosition[0], imagePosition[1], imagePosition[2] } );

    //Size
    unsigned short rows, columns;
    dataset->findAndGetUint16(DCM_Rows, rows);
    dataset->findAndGetUint16(DCM_Columns, columns);

    uint32_t depth;
    if(instances.size() == 1)
    {
        OFString sframesNumber = "";

        if(dataset->findAndGetOFString(DCM_NumberOfFrames, sframesNumber).good())
        {
            depth = static_cast<uint32_t>(std::stoi(sframesNumber.c_str()));
        }
        else
        {
            depth = 1;
        }
    }
    else
    {
        depth = static_cast<unsigned short>(instances.size());
    }

    //FIXME: Remove depth for 2D images ?
    image->setSize( {columns, rows, depth } );

    //Window Center
    double windowCenter = 0;
    dataset->findAndGetFloat64(DCM_WindowCenter, windowCenter);
    image->setWindowCenter(windowCenter);

    //Window Width
    double windowWidth = 0;
    dataset->findAndGetFloat64(DCM_WindowWidth, windowWidth);
    image->setWindowWidth(windowWidth);

    //Number of components
    OFString data;
    dataset->findAndGetOFStringArray(DCM_PhotometricInterpretation, data);
    std::string photometricInterpretation = data.c_str();
    dataset->findAndGetOFStringArray(DCM_PixelPresentation, data);
    std::string pixelPresentation = data.c_str();

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
        FW_RAISE( "The photometric interpretation \"" << photometricInterpretation << "\" is not supported.");
    }

    //Rescale Slope
    double rescaleSlope;
    double rescaleIntercept;
    status           = dataset->findAndGetFloat64(DCM_RescaleSlope, rescaleSlope);
    rescaleSlope     = (status.bad()) ? 1 : rescaleSlope;
    status           = dataset->findAndGetFloat64(DCM_RescaleIntercept, rescaleIntercept);
    rescaleIntercept = (status.bad()) ? 0 : rescaleIntercept;

    //Type
    unsigned short samplesPerPixel     = 1;
    unsigned short bitsAllocated       = 8;
    unsigned short bitsStored          = 8;
    unsigned short highBit             = 7;
    unsigned short pixelRepresentation = 0;

    dataset->findAndGetUint16(DCM_SamplesPerPixel, samplesPerPixel);
    dataset->findAndGetUint16(DCM_BitsAllocated, bitsAllocated);
    dataset->findAndGetUint16(DCM_BitsStored, bitsStored);
    dataset->findAndGetUint16(DCM_HighBit, highBit);
    dataset->findAndGetUint16(DCM_PixelRepresentation, pixelRepresentation);

    //Using lookup tables
    if(photometricInterpretation == "COLOR" || photometricInterpretation == "PALETTE COLOR")
    {
        unsigned short colorBitsAllocated = 0;
        dataset->findAndGetUint16(DCM_RedPaletteColorLookupTableDescriptor, colorBitsAllocated, 2);
        bitsStored = bitsAllocated = colorBitsAllocated;
        highBit    = static_cast<unsigned short>(colorBitsAllocated-1);
    }

    //Find image type
    ::fwDicomTools::Image imageHelper(
        samplesPerPixel, bitsAllocated, bitsStored, highBit, pixelRepresentation, rescaleSlope, rescaleIntercept);
    ::fwTools::Type imageType = imageHelper.findImageTypeFromMinMaxValues();

    //Set image type
    image->setType(imageType);

    //Direct reading mode
    if(::fwMemory::BufferManager::getDefault()->getLoadingMode() == ::fwMemory::BufferManager::DIRECT)
    {
        SLM_INFO("Reading using DIRECT mode.");

        //Default read
        if(photometricInterpretation != "PALETTE COLOR" && pixelPresentation != "COLOR")
        {
            this->directRead(image, instances, rows, columns, depth, rescaleSlope, rescaleIntercept,
                             pixelRepresentation, imageType);
        }
        //RGB lookup read
        else
        {
            this->directRGBLookupRead(image, *dataset, instances, rows, columns, depth, bitsAllocated);
        }

    }
    //Lazy reading mode
    else
    {
        SLM_INFO("Reading using LAZY mode.");

        //Default read
        if(photometricInterpretation != "PALETTE COLOR" && pixelPresentation != "COLOR")
        {
            this->lazyRead(image, series, rows, columns, depth, rescaleSlope, rescaleIntercept, pixelRepresentation,
                           imageType);
        }
        //RGB lookup read
        else
        {
            this->lazyRGBLookupRead(image, series, *dataset, instances, rows, columns, depth, bitsAllocated, imageType);
        }
    }

    // Add the image to the series
    imageSeries->setImage(image);

    return imageSeries;
}

//-----------------------------------------------------------------------------

void ImageStorageReader::directRead(const ::fwData::Image::sptr& image,
                                    DicomContainerType instances,
                                    unsigned short rows, unsigned short columns,
                                    int depth, double rescaleSlope,
                                    double rescaleIntercept,
                                    unsigned short pixelRepresentation,
                                    ::fwTools::Type imageType)
{
    //Allocate image
    image->allocate();
    ::fwData::Array::sptr array = image->getDataArray();
    ::fwDataTools::helper::Array arrayHelper(array);

    //Fill image
    ::fwDcmtkIO::reader::main::ImageReader::fillImageBuffer(rows, columns, depth, instances,
                                                            arrayHelper.getBuffer(), rescaleSlope, rescaleIntercept, pixelRepresentation,
                                                            imageType);
}

//-----------------------------------------------------------------------------

void ImageStorageReader::directRGBLookupRead(const ::fwData::Image::sptr& image,
                                             DcmDataset& dataset,
                                             DicomContainerType instances,
                                             unsigned short rows, unsigned short columns,
                                             int depth, unsigned short bitsAllocated)
{
    //Allocate image
    image->allocate();
    ::fwData::Array::sptr array = image->getDataArray();
    ::fwDataTools::helper::Array arrayHelper(array);

    unsigned short pixelValueBitsAllocated = 8;
    dataset.findAndGetUint16(DCM_BitsAllocated, pixelValueBitsAllocated);

    // 16 bits allocated
    if(bitsAllocated == 16)
    {
        const Uint16* redLookup;
        const Uint16* greenLookup;
        const Uint16* blueLookup;
        // Those values are freed by the dataset destructor
        dataset.findAndGetUint16Array(DCM_RedPaletteColorLookupTableData, redLookup);
        dataset.findAndGetUint16Array(DCM_GreenPaletteColorLookupTableData, greenLookup);
        dataset.findAndGetUint16Array(DCM_BluePaletteColorLookupTableData, blueLookup);

        if(pixelValueBitsAllocated == 16)
        {
            ::fwDcmtkIO::reader::rgblookup::ImageRGBLookupReader::fillImageBuffer<Uint16, Uint16>(rows,
                                                                                                  columns, depth,
                                                                                                  instances,
                                                                                                  arrayHelper.getBuffer(), redLookup, greenLookup,
                                                                                                  blueLookup);
        }
        else
        {
            ::fwDcmtkIO::reader::rgblookup::ImageRGBLookupReader::fillImageBuffer<Uint16, Uint8>(rows,
                                                                                                 columns, depth,
                                                                                                 instances,
                                                                                                 arrayHelper.getBuffer(), redLookup, greenLookup,
                                                                                                 blueLookup);
        }
    }
    // 8 bits allocated
    else
    {
        const Uint8* redLookup;
        const Uint8* greenLookup;
        const Uint8* blueLookup;
        // Those values are freed by the dataset destructor
        dataset.findAndGetUint8Array(DCM_RedPaletteColorLookupTableData, redLookup);
        dataset.findAndGetUint8Array(DCM_GreenPaletteColorLookupTableData, greenLookup);
        dataset.findAndGetUint8Array(DCM_BluePaletteColorLookupTableData, blueLookup);

        if(pixelValueBitsAllocated == 16)
        {
            ::fwDcmtkIO::reader::rgblookup::ImageRGBLookupReader::fillImageBuffer<Uint8, Uint16>(rows,
                                                                                                 columns, depth,
                                                                                                 instances,
                                                                                                 arrayHelper.getBuffer(), redLookup, greenLookup,
                                                                                                 blueLookup);
        }
        else
        {
            ::fwDcmtkIO::reader::rgblookup::ImageRGBLookupReader::fillImageBuffer<Uint8, Uint8>(rows,
                                                                                                columns, depth,
                                                                                                instances,
                                                                                                arrayHelper.getBuffer(), redLookup, greenLookup,
                                                                                                blueLookup);
        }
    }
}

//-----------------------------------------------------------------------------

void ImageStorageReader::lazyRead(const ::fwData::Image::sptr& image,
                                  const ::fwMedData::DicomSeries::csptr& series,
                                  unsigned short rows, unsigned short columns,
                                  int depth, double rescaleSlope,
                                  double rescaleIntercept,
                                  unsigned short pixelRepresentation,
                                  ::fwTools::Type imageType)
{
    // Create information object
    ::fwDcmtkIO::reader::main::ImageLazyInformation::sptr dcmInfo =
        std::make_shared< ::fwDcmtkIO::reader::main::ImageLazyInformation >();
    dcmInfo->m_dicomSeries         = series;
    dcmInfo->m_rows                = rows;
    dcmInfo->m_columns             = columns;
    dcmInfo->m_depth               = depth;
    dcmInfo->m_rescaleSlope        = rescaleSlope;
    dcmInfo->m_rescaleIntercept    = rescaleIntercept;
    dcmInfo->m_pixelRepresentation = pixelRepresentation;
    dcmInfo->m_imageType           = imageType;

    // Create streamer
    ::fwMemory::BufferObject::sptr buffObj = image->getDataArray()->getBufferObject();
    buffObj->setIStreamFactory(
        std::make_shared< ::fwDcmtkIO::reader::main::ImageLazyStream >( dcmInfo ),
        image->getSizeInBytes() );
}

//-----------------------------------------------------------------------------

void ImageStorageReader::lazyRGBLookupRead(const ::fwData::Image::sptr& image,
                                           const ::fwMedData::DicomSeries::csptr& series,
                                           DcmDataset& dataset,
                                           DicomContainerType instances,
                                           unsigned short rows, unsigned short columns,
                                           int depth, unsigned short bitsAllocated,
                                           ::fwTools::Type imageType)
{
    unsigned short pixelValueBitsAllocated = 8;
    dataset.findAndGetUint16(DCM_BitsAllocated, pixelValueBitsAllocated);

    // Create information object
    ::fwDcmtkIO::reader::rgblookup::ImageRGBLookupLazyInformation::sptr dcmInfo =
        std::make_shared< ::fwDcmtkIO::reader::rgblookup::ImageRGBLookupLazyInformation >();
    dcmInfo->m_dicomSeries             = series;
    dcmInfo->m_rows                    = rows;
    dcmInfo->m_columns                 = columns;
    dcmInfo->m_depth                   = depth;
    dcmInfo->m_bitsAllocated           = bitsAllocated;
    dcmInfo->m_pixelValueBitsAllocated = pixelValueBitsAllocated;
    dcmInfo->m_imageType               = imageType;

    // Create streamer
    ::fwMemory::BufferObject::sptr buffObj = image->getDataArray()->getBufferObject();
    buffObj->setIStreamFactory(
        std::make_shared< ::fwDcmtkIO::reader::rgblookup::ImageRGBLookupLazyStream >( dcmInfo ),
        image->getSizeInBytes() );

}

} //reader
} //fwDcmtkIO
