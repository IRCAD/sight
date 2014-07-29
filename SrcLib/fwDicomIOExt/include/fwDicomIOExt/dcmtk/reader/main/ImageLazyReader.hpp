/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMIOEXT_DCMTK_READER_MAIN_IMAGELAZYREADER_HPP__
#define __FWDICOMIOEXT_DCMTK_READER_MAIN_IMAGELAZYREADER_HPP__

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmnet/diutil.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmimgle/dcmimage.h>

#include <fwDicomData/DicomSeries.hpp>
#include <fwTools/Type.hpp>

#include "fwDicomIOExt/dcmtk/helper/Codec.hpp"

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
 * @class ImageLazyReader
 * @brief This class is used to read the buffer of a DICOM image in LAZY mode.
 * @date  2013.
 */
class FWDICOMIOEXT_CLASS_API ImageLazyReader
{
public:
    /**
     * @brief Create an instance buffer according to the image type.
     * @param[in] rows Number of rows
     * @param[in] columns Number of columns
     * @param[in] instance Path to the instance
     * @param[in] rescaleSlope Slope parameter
     * @param[in] rescaleIntercept Intercept parameter
     * @param[in] pixelRepresentation Pixel representation of the image (signed or unsigned short)
     * @param[in] imageType Type of the image used to create the buffer
     */
    FWDICOMIOEXT_API static void* createInstanceBuffer(unsigned int rows, unsigned int columns,
            const ::boost::filesystem::path& instance, double rescaleSlope, double rescaleIntercept,
             unsigned short pixelRepresentation, ::fwTools::Type imageType)
    {
        if (pixelRepresentation == 0)
        {
            // Use unsigned char
            if(imageType.sizeOf() == 1)
            {
                return ::fwDicomIOExt::dcmtk::reader::main::ImageLazyReader::createInstanceBuffer< Uint8 >(
                        rows, columns, instance, rescaleSlope, rescaleIntercept, imageType);
            }
            // Use unsigned short
            else
            {
                return ::fwDicomIOExt::dcmtk::reader::main::ImageLazyReader::createInstanceBuffer< Uint16 >(
                        rows, columns, instance, rescaleSlope, rescaleIntercept, imageType);
            }
        }
        else
        {
            // Use signed chart
            if(imageType.sizeOf() == 1)
            {
                return ::fwDicomIOExt::dcmtk::reader::main::ImageLazyReader::createInstanceBuffer< Sint8 >(
                        rows, columns, instance, rescaleSlope, rescaleIntercept, imageType);
            }
            // Use signed short
            else
            {
                return ::fwDicomIOExt::dcmtk::reader::main::ImageLazyReader::createInstanceBuffer< Sint16 >(
                        rows, columns, instance, rescaleSlope, rescaleIntercept, imageType);
            }
        }
    }


protected:

    /**
     * @brief Create an instance buffer according to the image type.
     * The template T is used to determine if we must use signed or unsigned short when reading pixels.
     * @param[in] rows Number of rows
     * @param[in] columns Number of columns
     * @param[in] instance Path to the instance
     * @param[in] rescaleSlope Slope parameter
     * @param[in] rescaleIntercept Intercept parameter
     * @param[in] imageType Type of the image used to create the buffer
     */
    template< typename T >
    FWDICOMIOEXT_API static void* createInstanceBuffer(unsigned int rows, unsigned int columns,
            const ::boost::filesystem::path& instance, double rescaleSlope, double rescaleIntercept,
            ::fwTools::Type imageType)
    {
        void* tempoBuffer;

        //Copy the temporary buffer to the final buffer
        if (imageType == ::fwTools::Type::s_INT8)
        {
            tempoBuffer = ::fwDicomIOExt::dcmtk::reader::main::ImageLazyReader::createInstanceBuffer< T, ::boost::int8_t >(
                    rows, columns, instance, rescaleSlope, rescaleIntercept);
        }
        else if (imageType == ::fwTools::Type::s_INT16)
        {
            tempoBuffer = ::fwDicomIOExt::dcmtk::reader::main::ImageLazyReader::createInstanceBuffer< T, ::boost::int16_t >(
                    rows, columns, instance, rescaleSlope, rescaleIntercept);
        }
        else if (imageType == ::fwTools::Type::s_INT32)
        {
            tempoBuffer = ::fwDicomIOExt::dcmtk::reader::main::ImageLazyReader::createInstanceBuffer< T, ::boost::int32_t >(
                    rows, columns, instance, rescaleSlope, rescaleIntercept);
        }
        else if (imageType == ::fwTools::Type::s_INT64)
        {
            tempoBuffer = ::fwDicomIOExt::dcmtk::reader::main::ImageLazyReader::createInstanceBuffer< T, ::boost::int64_t >(
                    rows, columns, instance, rescaleSlope, rescaleIntercept);
        }
        else if (imageType == ::fwTools::Type::s_UINT8)
        {
            tempoBuffer = ::fwDicomIOExt::dcmtk::reader::main::ImageLazyReader::createInstanceBuffer< T, ::boost::uint8_t >(
                    rows, columns, instance, rescaleSlope, rescaleIntercept);
        }
        else if (imageType == ::fwTools::Type::s_UINT16)
        {
            tempoBuffer = ::fwDicomIOExt::dcmtk::reader::main::ImageLazyReader::createInstanceBuffer< T, ::boost::uint16_t >(
                    rows, columns, instance, rescaleSlope, rescaleIntercept);
        }
        else if (imageType == ::fwTools::Type::s_UINT32)
        {
            tempoBuffer = ::fwDicomIOExt::dcmtk::reader::main::ImageLazyReader::createInstanceBuffer< T, ::boost::uint32_t >(
                    rows, columns, instance, rescaleSlope, rescaleIntercept);
        }
        else if (imageType == ::fwTools::Type::s_UINT64)
        {
            tempoBuffer = ::fwDicomIOExt::dcmtk::reader::main::ImageLazyReader::createInstanceBuffer< T, ::boost::uint64_t >(
                    rows, columns, instance, rescaleSlope, rescaleIntercept);
        }
        else if (imageType == ::fwTools::Type::s_FLOAT)
        {
            tempoBuffer = ::fwDicomIOExt::dcmtk::reader::main::ImageLazyReader::createInstanceBuffer< T, float >(
                    rows, columns, instance, rescaleSlope, rescaleIntercept);
        }
        else if (imageType == ::fwTools::Type::s_DOUBLE)
        {
            tempoBuffer = ::fwDicomIOExt::dcmtk::reader::main::ImageLazyReader::createInstanceBuffer< T, double >(
                    rows, columns, instance, rescaleSlope, rescaleIntercept);
        }

        return tempoBuffer;

    }

    /**
     * @brief Create an instance buffer according to the image type.
     * The template T is used to determine if we must use signed or unsigned short when reading pixels.
     * The template U is used to determine what kind of buffer must be created.
     * @param[in] rows Number of rows
     * @param[in] columns Number of columns
     * @param[in] instance Path to the instance
     * @param[in] rescaleSlope Slope parameter
     * @param[in] rescaleIntercept Intercept parameter
     */
    template< typename T, typename U >
    FWDICOMIOEXT_API static U* createInstanceBuffer(unsigned int rows, unsigned int columns,
            const ::boost::filesystem::path& instance, double rescaleSlope, double rescaleIntercept)
    {
        DcmFileFormat fileFormat;
        OFCondition status;
        DcmDataset* dataset;

        // Register codecs
        ::fwDicomIOExt::dcmtk::helper::Codec::registerCodecs();

        // Create temporary buffer
        U* tempoBuffer = new U[rows * columns];

        // Read instance
        const std::string filename = instance.string();
        status = fileFormat.loadFile(filename.c_str());
        FW_RAISE_IF("Unable to read the file: \""+filename+"\"", status.bad());

        dataset = fileFormat.getDataset();

        // Decompress data set if compressed
        dataset->chooseRepresentation(EXS_LittleEndianExplicit, NULL);


        if(sizeof(T) == 1)
        {
            const Uint8* pixelData;
            dataset->findAndGetUint8Array(DCM_PixelData, pixelData);
            OSLM_WARN_IF("Unable to read pixel data.", !pixelData);

            for (unsigned int x = 0; x < columns; ++x)
            {
                for (unsigned int y = 0; y < rows; ++y)
                {
                    unsigned int position = y + (x * rows);
                    T value;
                    value = (pixelData)?pixelData[position]:0;
                    tempoBuffer[position] = (U)(rescaleSlope * value + rescaleIntercept);
                }
            }
        }
        else
        {
            const Uint16* pixelData;
            dataset->findAndGetUint16Array(DCM_PixelData, pixelData);
            OSLM_WARN_IF("Unable to read pixel data.", !pixelData);

            for (unsigned int x = 0; x < columns; ++x)
            {
                for (unsigned int y = 0; y < rows; ++y)
                {
                    unsigned int position = y + (x * rows);
                    T value;
                    value = (pixelData)?pixelData[position]:0;
                    tempoBuffer[position] = (U)(rescaleSlope * value + rescaleIntercept);
                }
            }
        }


        // Clean up codecs
        ::fwDicomIOExt::dcmtk::helper::Codec::cleanup();

        return tempoBuffer;
    }


};

} //main
} //reader
} //dcmtk
} //fwDicomIOExt


#endif /* __FWDICOMIOEXT_DCMTK_READER_MAIN_IMAGELAZYREADER_HPP__ */
