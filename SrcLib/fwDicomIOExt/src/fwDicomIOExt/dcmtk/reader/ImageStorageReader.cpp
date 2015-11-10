/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <algorithm>

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmnet/diutil.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmimgle/dcmimage.h>

#include <boost/assign/list_of.hpp>
#include <boost/assign/std/vector.hpp>

#include <fwComEd/helper/Array.hpp>
#include <fwMedData/ImageSeries.hpp>

#include "fwDicomIOExt/dcmtk/helper/Image.hpp"
#include "fwDicomIOExt/dcmtk/reader/main/ImageLazyStream.hpp"
#include "fwDicomIOExt/dcmtk/reader/main/ImageReader.hpp"
#include "fwDicomIOExt/dcmtk/reader/rgblookup/ImageRGBLookupLazyStream.hpp"
#include "fwDicomIOExt/dcmtk/reader/rgblookup/ImageRGBLookupReader.hpp"
#include "fwDicomIOExt/dcmtk/helper/Series.hpp"
#include "fwDicomIOExt/dcmtk/reader/ImageStorageReader.hpp"

namespace fwDicomIOExt
{
namespace dcmtk
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

::fwMedData::Series::sptr ImageStorageReader::read(::fwDicomData::DicomSeries::sptr series)
{
    ::fwMedData::ImageSeries::sptr imageSeries = ::fwDicomIOExt::dcmtk::helper::Series::convertToImageSeries(series);
    DicomPathContainerType instances = series->getLocalDicomPaths();

    ::fwData::Image::sptr image = ::fwData::Image::New();
    DcmFileFormat fileFormat;
    OFCondition status;
    DcmDataset* dataset;

    //Get informations from the first instance
    std::string firstInstance = instances.begin()->second.string();
    status = fileFormat.loadFile(firstInstance.c_str());
    DicomImage dicomImage(firstInstance.c_str());

    FW_RAISE_IF("Unable to read the file: \""+firstInstance+"\"", status.bad() || (
                    dicomImage.getStatus() != EIS_Normal
                    && dicomImage.getStatus() != EIS_MissingAttribute
                    && dicomImage.getStatus() != EIS_NotSupportedValue
                    ));
    dataset = fileFormat.getDataset();

    // Decompress data set if compressed
    dataset->chooseRepresentation(EXS_LittleEndianExplicit, NULL);

    if(dicomImage.getStatus() != EIS_MissingAttribute)
    {
        SLM_WARN("Some informations are missing. The file may have not been read properly.");
    }

    //Spacing
    double spacing[3];
    dataset->findAndGetFloat64(DCM_PixelSpacing,spacing[0],0);
    dataset->findAndGetFloat64(DCM_PixelSpacing,spacing[1],1);
    dataset->findAndGetFloat64(DCM_SliceThickness,spacing[2]);

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
    image->setOrigin(::boost::assign::list_of(imagePosition[0])(imagePosition[1])(imagePosition[2]));

    //Size
    unsigned short rows, columns;
    int depth;
    dataset->findAndGetUint16(DCM_Rows,rows);
    dataset->findAndGetUint16(DCM_Columns,columns);
    depth = instances.size();
    if(depth == 1)
    {
        //If there is only one file, we check how many frames it contains.
        depth = (depth==0) ? 1 : depth;
        if(depth > 1)
        {
            FW_RAISE ( "Reading a file containing multiple frames is not yet supported by this reader.");
        }
    }


    image->setSize(::boost::assign::list_of(columns)(rows)(depth));

    //Window Center
    double windowCenter = 0;
    dataset->findAndGetFloat64(DCM_WindowCenter,windowCenter);
    image->setWindowCenter(windowCenter);

    //Window Width
    double windowWidth = 0;
    dataset->findAndGetFloat64(DCM_WindowWidth,windowWidth);
    image->setWindowWidth(windowWidth);

    //Number of components
    OFString data;
    dataset->findAndGetOFString(DCM_PhotometricInterpretation, data);
    std::string photometricInterpretation = data.c_str();
    dataset->findAndGetOFString(DCM_PixelPresentation, data);
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
        FW_RAISE ( "The photometric interpretation \"" << photometricInterpretation << "\" is not supported.");
    }

    //Rescale Slope
    double rescaleSlope;
    double rescaleIntercept;
    status           = dataset->findAndGetFloat64(DCM_RescaleSlope,rescaleSlope);
    rescaleSlope     = (status.bad()) ? 1 : rescaleSlope;
    status           = dataset->findAndGetFloat64(DCM_RescaleIntercept,rescaleIntercept);
    rescaleIntercept = (status.bad()) ? 0 : rescaleIntercept;

    //Type
    unsigned short samplesPerPixel     = 1;
    unsigned short bitsAllocated       = 8;
    unsigned short bitsStored          = 8;
    unsigned short highBit             = 7;
    unsigned short pixelRepresentation = 0;

    dataset->findAndGetUint16(DCM_SamplesPerPixel,samplesPerPixel);
    dataset->findAndGetUint16(DCM_BitsAllocated,bitsAllocated);
    dataset->findAndGetUint16(DCM_BitsStored,bitsStored);
    dataset->findAndGetUint16(DCM_HighBit,highBit);
    dataset->findAndGetUint16(DCM_PixelRepresentation,pixelRepresentation);

    //Using lookup tables
    if(photometricInterpretation == "COLOR" || photometricInterpretation == "PALETTE COLOR")
    {
        unsigned short colorBitsAllocated = 0;
        dataset->findAndGetUint16(DCM_RedPaletteColorLookupTableDescriptor, colorBitsAllocated, 2);
        bitsStored = bitsAllocated = colorBitsAllocated;
        highBit    = colorBitsAllocated-1;
    }

    //Find image type
    ::fwDicomIOExt::dcmtk::helper::Image imageHelper(
        samplesPerPixel,bitsAllocated,bitsStored, highBit, pixelRepresentation, rescaleSlope, rescaleIntercept);
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

void ImageStorageReader::directRead(::fwData::Image::sptr image, DicomPathContainerType instances,
                                    unsigned short rows, unsigned short columns, int depth, double rescaleSlope,
                                    double rescaleIntercept,
                                    unsigned short pixelRepresentation, ::fwTools::Type imageType)
{
    //Allocate image
    image->allocate();
    ::fwData::Array::sptr array = image->getDataArray();
    ::fwComEd::helper::Array arrayHelper(array);

    //Fill image
    ::fwDicomIOExt::dcmtk::reader::main::ImageReader::fillImageBuffer(rows, columns, depth, instances,
                                                                      arrayHelper.getBuffer(), rescaleSlope, rescaleIntercept, pixelRepresentation,
                                                                      imageType);
}

//-----------------------------------------------------------------------------

void ImageStorageReader::directRGBLookupRead(::fwData::Image::sptr image, DcmDataset& dataset,
                                             DicomPathContainerType instances, unsigned short rows,
                                             unsigned short columns, int depth,
                                             unsigned short bitsAllocated)
{
    //Allocate image
    image->allocate();
    ::fwData::Array::sptr array = image->getDataArray();
    ::fwComEd::helper::Array arrayHelper(array);

    unsigned short pixelValueBitsAllocated = 8;
    dataset.findAndGetUint16(DCM_BitsAllocated,pixelValueBitsAllocated);

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
            ::fwDicomIOExt::dcmtk::reader::rgblookup::ImageRGBLookupReader::fillImageBuffer<Uint16, Uint16>(rows,
                                                                                                            columns,
                                                                                                            depth,
                                                                                                            instances,
                                                                                                            arrayHelper.getBuffer(), redLookup, greenLookup,
                                                                                                            blueLookup);
        }
        else
        {
            ::fwDicomIOExt::dcmtk::reader::rgblookup::ImageRGBLookupReader::fillImageBuffer<Uint16, Uint8>(rows,
                                                                                                           columns,
                                                                                                           depth,
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
            ::fwDicomIOExt::dcmtk::reader::rgblookup::ImageRGBLookupReader::fillImageBuffer<Uint8, Uint16>(rows,
                                                                                                           columns,
                                                                                                           depth,
                                                                                                           instances,
                                                                                                           arrayHelper.getBuffer(), redLookup, greenLookup,
                                                                                                           blueLookup);
        }
        else
        {
            ::fwDicomIOExt::dcmtk::reader::rgblookup::ImageRGBLookupReader::fillImageBuffer<Uint8, Uint8>(rows,
                                                                                                          columns,
                                                                                                          depth,
                                                                                                          instances,
                                                                                                          arrayHelper.getBuffer(), redLookup, greenLookup,
                                                                                                          blueLookup);
        }
    }

}

//-----------------------------------------------------------------------------

void ImageStorageReader::lazyRead(::fwData::Image::sptr image, ::fwDicomData::DicomSeries::sptr series,
                                  unsigned short rows, unsigned short columns, int depth, double rescaleSlope,
                                  double rescaleIntercept,
                                  unsigned short pixelRepresentation, ::fwTools::Type imageType)
{
    // Create information object
    ::fwDicomIOExt::dcmtk::reader::main::ImageLazyInformation::sptr dcmInfo =
        std::make_shared< ::fwDicomIOExt::dcmtk::reader::main::ImageLazyInformation >();
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
        std::make_shared< ::fwDicomIOExt::dcmtk::reader::main::ImageLazyStream >( dcmInfo ),
        image->getSizeInBytes() );
}

//-----------------------------------------------------------------------------

void ImageStorageReader::lazyRGBLookupRead(::fwData::Image::sptr image, ::fwDicomData::DicomSeries::sptr series,
                                           DcmDataset& dataset, DicomPathContainerType instances, unsigned short rows,
                                           unsigned short columns, int depth,
                                           unsigned short bitsAllocated, ::fwTools::Type imageType)
{
    unsigned short pixelValueBitsAllocated = 8;
    dataset.findAndGetUint16(DCM_BitsAllocated,pixelValueBitsAllocated);

    // Create information object
    ::fwDicomIOExt::dcmtk::reader::rgblookup::ImageRGBLookupLazyInformation::sptr dcmInfo =
        std::make_shared< ::fwDicomIOExt::dcmtk::reader::rgblookup::ImageRGBLookupLazyInformation >();
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
        std::make_shared< ::fwDicomIOExt::dcmtk::reader::rgblookup::ImageRGBLookupLazyStream >( dcmInfo ),
        image->getSizeInBytes() );

}

} //dcmtk
} //reader
} //fwDicomIOExt
