/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#pragma once

#include "fwDcmtkIO/config.hpp"
#include "fwDcmtkIO/helper/Codec.hpp"

#include <fwMedData/DicomSeries.hpp>

#include <fwTools/Type.hpp>

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcistrmb.h>
#include <dcmtk/dcmimgle/dcmimage.h>
#include <dcmtk/dcmnet/diutil.h>

namespace fwDcmtkIO
{
namespace reader
{
namespace main
{

/**
 * @brief This class is used to read the buffer of a DICOM image in LAZY mode.
 */
class FWDCMTKIO_CLASS_API ImageLazyReader
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
    FWDCMTKIO_API static void* createInstanceBuffer(unsigned int rows, unsigned int columns,
                                                    const ::fwMemory::BufferObject::sptr& instance,
                                                    double rescaleSlope,
                                                    double rescaleIntercept,
                                                    unsigned short pixelRepresentation,
                                                    ::fwTools::Type imageType)
    {
        if (pixelRepresentation == 0)
        {
            // Use unsigned char
            if(imageType.sizeOf() == 1)
            {
                return ::fwDcmtkIO::reader::main::ImageLazyReader::createInstanceBuffer< Uint8 >(
                    rows, columns, instance, rescaleSlope, rescaleIntercept, imageType);
            }
            // Use unsigned short
            else
            {
                return ::fwDcmtkIO::reader::main::ImageLazyReader::createInstanceBuffer< Uint16 >(
                    rows, columns, instance, rescaleSlope, rescaleIntercept, imageType);
            }
        }
        else
        {
            // Use signed chart
            if(imageType.sizeOf() == 1)
            {
                return ::fwDcmtkIO::reader::main::ImageLazyReader::createInstanceBuffer< Sint8 >(
                    rows, columns, instance, rescaleSlope, rescaleIntercept, imageType);
            }
            // Use signed short
            else
            {
                return ::fwDcmtkIO::reader::main::ImageLazyReader::createInstanceBuffer< Sint16 >(
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
    FWDCMTKIO_API static void* createInstanceBuffer(unsigned int rows, unsigned int columns,
                                                    const ::fwMemory::BufferObject::sptr& instance,
                                                    double rescaleSlope,
                                                    double rescaleIntercept,
                                                    ::fwTools::Type imageType)
    {
        void* tempoBuffer = 0;

        //Copy the temporary buffer to the final buffer
        if (imageType == ::fwTools::Type::s_INT8)
        {
            tempoBuffer = ::fwDcmtkIO::reader::main::ImageLazyReader::createInstanceBuffer< T, std::int8_t >(
                rows, columns, instance, rescaleSlope, rescaleIntercept);
        }
        else if (imageType == ::fwTools::Type::s_INT16)
        {
            tempoBuffer = ::fwDcmtkIO::reader::main::ImageLazyReader::createInstanceBuffer< T, std::int16_t >(
                rows, columns, instance, rescaleSlope, rescaleIntercept);
        }
        else if (imageType == ::fwTools::Type::s_INT32)
        {
            tempoBuffer = ::fwDcmtkIO::reader::main::ImageLazyReader::createInstanceBuffer< T, std::int32_t >(
                rows, columns, instance, rescaleSlope, rescaleIntercept);
        }
        else if (imageType == ::fwTools::Type::s_INT64)
        {
            tempoBuffer = ::fwDcmtkIO::reader::main::ImageLazyReader::createInstanceBuffer< T, std::int64_t >(
                rows, columns, instance, rescaleSlope, rescaleIntercept);
        }
        else if (imageType == ::fwTools::Type::s_UINT8)
        {
            tempoBuffer = ::fwDcmtkIO::reader::main::ImageLazyReader::createInstanceBuffer< T, std::uint8_t >(
                rows, columns, instance, rescaleSlope, rescaleIntercept);
        }
        else if (imageType == ::fwTools::Type::s_UINT16)
        {
            tempoBuffer = ::fwDcmtkIO::reader::main::ImageLazyReader::createInstanceBuffer< T, std::uint16_t >(
                rows, columns, instance, rescaleSlope, rescaleIntercept);
        }
        else if (imageType == ::fwTools::Type::s_UINT32)
        {
            tempoBuffer = ::fwDcmtkIO::reader::main::ImageLazyReader::createInstanceBuffer< T, std::uint32_t >(
                rows, columns, instance, rescaleSlope, rescaleIntercept);
        }
        else if (imageType == ::fwTools::Type::s_UINT64)
        {
            tempoBuffer = ::fwDcmtkIO::reader::main::ImageLazyReader::createInstanceBuffer< T, std::uint64_t >(
                rows, columns, instance, rescaleSlope, rescaleIntercept);
        }
        else if (imageType == ::fwTools::Type::s_FLOAT)
        {
            tempoBuffer = ::fwDcmtkIO::reader::main::ImageLazyReader::createInstanceBuffer< T, float >(
                rows, columns, instance, rescaleSlope, rescaleIntercept);
        }
        else if (imageType == ::fwTools::Type::s_DOUBLE)
        {
            tempoBuffer = ::fwDcmtkIO::reader::main::ImageLazyReader::createInstanceBuffer< T, double >(
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
    FWDCMTKIO_API static U* createInstanceBuffer(unsigned int rows,
                                                 unsigned int columns,
                                                 const ::fwMemory::BufferObject::sptr& instance,
                                                 double rescaleSlope,
                                                 double rescaleIntercept)
    {
        DcmFileFormat fileFormat;
        OFCondition status;
        DcmDataset* dataset;

        // Create temporary buffer
        U* tempoBuffer = new U[rows * columns];

        // Read instance
        const size_t buffSize       = instance->getSize();
        const std::string dicomPath = instance->getStreamInfo().fsFile.string();
        ::fwMemory::BufferObject::Lock lock(instance);
        char* buffer = static_cast< char* >( lock.getBuffer() );

        DcmInputBufferStream is;
        is.setBuffer(buffer, offile_off_t(buffSize));
        is.setEos();

        fileFormat.transferInit();
        if (!fileFormat.read(is).good())
        {
            FW_RAISE("Unable to read Dicom file '"<< dicomPath <<"'");
        }

        fileFormat.loadAllDataIntoMemory();
        fileFormat.transferEnd();

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
                    value                 = static_cast<T>((pixelData) ? pixelData[position] : 0);
                    tempoBuffer[position] = static_cast<U>(rescaleSlope * value + rescaleIntercept);
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
                    value                 = static_cast<T>((pixelData) ? pixelData[position] : 0);
                    tempoBuffer[position] = static_cast<U>(rescaleSlope * value + rescaleIntercept);
                }
            }
        }

        return tempoBuffer;
    }
};

} //main
} //reader
} //fwDcmtkIO
