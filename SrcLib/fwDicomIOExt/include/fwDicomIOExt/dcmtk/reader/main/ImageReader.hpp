/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMIOEXT_DCMTK_READER_MAIN_IMAGEREADER_HPP__
#define __FWDICOMIOEXT_DCMTK_READER_MAIN_IMAGEREADER_HPP__

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmnet/diutil.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmimgle/dcmimage.h>

#include <fwDicomData/DicomSeries.hpp>

#include "fwDicomIOExt/config.hpp"

namespace fwDicomIOExt
{
namespace dcmtk
{
namespace reader
{
namespace main
{

/**
 * @class ImageReader
 * @brief This class is used to read the buffer of a DICOM image in DIRECT mode.
 * @date  2013.
 */
class FWDICOMIOEXT_CLASS_API ImageReader
{
public:
    typedef ::fwDicomData::DicomSeries::DicomPathContainerType DicomPathContainerType;

    /**
     * @brief Fill the buffer of an image
     * @param[in] rows Number of rows
     * @param[in] columns Number of columns
     * @param[in] depth Number of frames
     * @param[in] instances Paths to the instances
     * @param[in] destination Destination buffer
     * @param[in] rescaleSlope Slope parameter
     * @param[in] rescaleIntercept Intercept parameter
     * @param[in] pixelRepresentation Pixel representation of the image (signed or unsigned short)
     * @param[in] imageType Type of the image used to create the buffer
     */
    FWDICOMIOEXT_API static void fillImageBuffer(unsigned int rows, unsigned int columns, unsigned int depth,
                                                 DicomPathContainerType& instances, void* destination,
                                                 double rescaleSlope, double rescaleIntercept,
                                                 unsigned short pixelRepresentation, ::fwTools::Type imageType)
    {
        if (pixelRepresentation == 0)
        {
            // Use unsigned char
            if(imageType.sizeOf() == 1)
            {
                ::fwDicomIOExt::dcmtk::reader::main::ImageReader::fillImageBuffer< Uint8 >(
                    rows, columns, depth, instances, destination, rescaleSlope, rescaleIntercept, imageType);
            }
            // Use unsigned short
            else
            {
                ::fwDicomIOExt::dcmtk::reader::main::ImageReader::fillImageBuffer< Uint16 >(
                    rows, columns, depth, instances, destination, rescaleSlope, rescaleIntercept, imageType);
            }
        }
        else
        {
            // Use signed char
            if(imageType.sizeOf() == 1)
            {
                ::fwDicomIOExt::dcmtk::reader::main::ImageReader::fillImageBuffer< Sint8 >(
                    rows, columns, depth, instances, destination, rescaleSlope, rescaleIntercept, imageType);
            }
            // Use signed short
            else
            {
                ::fwDicomIOExt::dcmtk::reader::main::ImageReader::fillImageBuffer< Sint16 >(
                    rows, columns, depth, instances, destination, rescaleSlope, rescaleIntercept, imageType);
            }
        }
    }

    /**
     * @brief Create the signed short or unsigned short temporary buffer used to fill the image buffer
     * The template T is used to determine if we must use signed or unsigned short to create the buffer
     * @param[in] rows Number of rows
     * @param[in] columns Number of columns
     * @param[in] depth Number of frames
     * @param[in] instances Paths to the instances
     */
    template< typename T >
    FWDICOMIOEXT_API static T* createTemporaryBuffer(unsigned int rows, unsigned int columns, unsigned int depth,
                                                     DicomPathContainerType& instances)
    {
        DcmFileFormat fileFormat;
        OFCondition status;
        DcmDataset* dataset;

        // Create temporary buffer
        T* tempoBuffer = new T[rows*columns*depth];

        // Compute slice size
        unsigned int sliceSize = rows*columns*sizeof(T);

        // Slice index
        unsigned short z = 0;

        // Read every instances
        for(DicomPathContainerType::value_type file : instances)
        {
            const std::string filename = file.second.string();
            status = fileFormat.loadFile(filename.c_str());
            FW_RAISE_IF("Unable to read the file: \""+file.second.string()+"\"", status.bad());

            dataset = fileFormat.getDataset();

            // Decompress data set if compressed
            dataset->chooseRepresentation(EXS_LittleEndianExplicit, NULL);

            const Uint16* pixelData;
            dataset->findAndGetUint16Array(DCM_PixelData, pixelData);

            if(pixelData)
            {
                //Add the slice to the temporary buffer
                memcpy(tempoBuffer+z*(rows*columns), pixelData, sliceSize);
                ++z; //Next frame
            }
            else
            {
                SLM_WARN("There is no pixel data in file \"" + file.second.string() + "\".");
            }

        }

        return tempoBuffer;
    }


protected:

    /**
     * @brief Fill the buffer of an image
     * The template T is used to determine if we must use signed or unsigned short when reading pixels.
     * @param[in] rows Number of rows
     * @param[in] columns Number of columns
     * @param[in] depth Number of frames
     * @param[in] instances Paths to the instances
     * @param[in] destination Destination buffer
     * @param[in] rescaleSlope Slope parameter
     * @param[in] rescaleIntercept Intercept parameter
     * @param[in] imageType Type of the image used to create the buffer
     */
    template< typename T >
    FWDICOMIOEXT_API static void fillImageBuffer(unsigned int rows, unsigned int columns, unsigned int depth,
                                                 DicomPathContainerType& instances, void* destination,
                                                 double rescaleSlope, double rescaleIntercept,
                                                 ::fwTools::Type imageType)
    {
        T* tempoBuffer = ::fwDicomIOExt::dcmtk::reader::main::ImageReader::createTemporaryBuffer<T>(
            rows, columns, depth, instances);

        //Copy the temporary buffer to the final buffer
        if (imageType == ::fwTools::Type::s_INT8)
        {
            ::fwDicomIOExt::dcmtk::reader::main::ImageReader::copyBuffer< ::boost::int8_t >(
                rows, columns, depth, tempoBuffer, destination, rescaleSlope,
                rescaleIntercept);
        }
        else if (imageType == ::fwTools::Type::s_INT16)
        {
            ::fwDicomIOExt::dcmtk::reader::main::ImageReader::copyBuffer< ::boost::int16_t >(
                rows, columns, depth, tempoBuffer, destination, rescaleSlope,
                rescaleIntercept);
        }
        else if (imageType == ::fwTools::Type::s_INT32)
        {
            ::fwDicomIOExt::dcmtk::reader::main::ImageReader::copyBuffer< ::boost::int32_t >(
                rows, columns, depth, tempoBuffer, destination, rescaleSlope,
                rescaleIntercept);
        }
        else if (imageType == ::fwTools::Type::s_INT64)
        {
            ::fwDicomIOExt::dcmtk::reader::main::ImageReader::copyBuffer< ::boost::int64_t >(
                rows, columns, depth, tempoBuffer, destination, rescaleSlope,
                rescaleIntercept);
        }
        else if (imageType == ::fwTools::Type::s_UINT8)
        {
            ::fwDicomIOExt::dcmtk::reader::main::ImageReader::copyBuffer< ::boost::uint8_t >(
                rows, columns, depth, tempoBuffer, destination, rescaleSlope,
                rescaleIntercept);
        }
        else if (imageType == ::fwTools::Type::s_UINT16)
        {
            ::fwDicomIOExt::dcmtk::reader::main::ImageReader::copyBuffer< ::boost::uint16_t >(
                rows, columns, depth, tempoBuffer, destination, rescaleSlope,
                rescaleIntercept);
        }
        else if (imageType == ::fwTools::Type::s_UINT32)
        {
            ::fwDicomIOExt::dcmtk::reader::main::ImageReader::copyBuffer< ::boost::uint32_t >(
                rows, columns, depth, tempoBuffer, destination, rescaleSlope,
                rescaleIntercept);
        }
        else if (imageType == ::fwTools::Type::s_UINT64)
        {
            ::fwDicomIOExt::dcmtk::reader::main::ImageReader::copyBuffer< ::boost::uint64_t >(
                rows, columns, depth, tempoBuffer, destination, rescaleSlope,
                rescaleIntercept);
        }
        else if (imageType == ::fwTools::Type::s_FLOAT)
        {
            ::fwDicomIOExt::dcmtk::reader::main::ImageReader::copyBuffer< float >(
                rows, columns, depth, tempoBuffer, destination, rescaleSlope,
                rescaleIntercept);
        }
        else if (imageType == ::fwTools::Type::s_DOUBLE)
        {
            ::fwDicomIOExt::dcmtk::reader::main::ImageReader::copyBuffer< double >(
                rows, columns, depth, tempoBuffer, destination, rescaleSlope,
                rescaleIntercept);
        }

        delete tempoBuffer;

    }


    /**
     * @brief Copy the temporary buffer to the image buffer
     * The template T is used to determine the type of the final image buffer
     * The template U is used to determine the type of the temporary buffer
     * @param[in] rows Number of rows
     * @param[in] columns Number of columns
     * @param[in] depth Number of frames
     * @param[in] source Temporary buffer
     * @param[in] destination Final image buffer
     * @param[in] rescaleSlope Slope parameter
     * @param[in] rescaleIntercept Intercept parameter
     */
    template< typename T, typename U >
    FWDICOMIOEXT_API static void copyBuffer(unsigned int rows, unsigned int columns, unsigned int depth,
                                            U* source, void* destination, double rescaleSlope, double rescaleIntercept)
    {
        T* arrayBuffer = static_cast< T* >(destination);
        unsigned short x, y, z;
        unsigned int frameSize = columns*rows;
        x = y = z = 0;
        for(x = 0; x < columns; ++x)
        {
            for(y = 0; y < rows; ++y)
            {
                unsigned int yshift = x*rows;

                for(z = 0; z < depth; ++z)
                {
                    unsigned int position = y + yshift + z*frameSize;
                    T value               = (T)source[position];
                    value                 = rescaleSlope*value+rescaleIntercept;
                    arrayBuffer[position] = value;
                }
            }
        }
    }


};

} //main
} //reader
} //dcmtk
} //fwDicomIOExt


#endif /* __FWDICOMIOEXT_DCMTK_READER_MAIN_IMAGEREADER_HPP__ */
