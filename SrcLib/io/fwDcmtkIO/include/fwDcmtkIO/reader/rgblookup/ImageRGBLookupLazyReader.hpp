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
namespace rgblookup
{

/**
 * @brief This class is used to read the buffer of a DICOM image in LAZY mode when a pixel lookup must be performed.
 */
class FWDCMTKIO_CLASS_API ImageRGBLookupLazyReader
{
public:

    /**
     * @brief Create an instance buffer according to the image type.
     * The template T is used to determine color format
     * @param[in] rows Number of rows
     * @param[in] columns Number of columns
     * @param[in] instance Path to the instance
     * @param[in] redLookup Red lookup table
     * @param[in] greenLookup Red lookup table
     * @param[in] blueLookup Red lookup table
     * @param[in] pixelValueBitsAllocated Pixel value bits allocated
     */
    template< typename T>
    static void* createInstanceBuffer(unsigned int rows, unsigned int columns,
                                      const ::fwMemory::BufferObject::sptr& instance,
                                      const T* redLookup,
                                      const T* greenLookup,
                                      const T* blueLookup, unsigned short pixelValueBitsAllocated)
    {
        if(pixelValueBitsAllocated == 16)
        {
            return ::fwDcmtkIO::reader::rgblookup::ImageRGBLookupLazyReader::createInstanceBuffer<T, Uint16>(
                rows, columns, instance, redLookup, greenLookup, blueLookup);
        }
        else
        {
            return ::fwDcmtkIO::reader::rgblookup::ImageRGBLookupLazyReader::createInstanceBuffer<T, Uint8>(
                rows, columns, instance, redLookup, greenLookup, blueLookup);
        }
    }

    /**
     * @brief Create an instance buffer according to the image type.
     * The template T is used to determine color format
     * The template U is used to determine pixel value format
     * @param[in] rows Number of rows
     * @param[in] columns Number of columns
     * @param[in] instance Path to the instance
     * @param[in] redLookup Red lookup table
     * @param[in] greenLookup Red lookup table
     * @param[in] blueLookup Red lookup table
     */
    template< typename T, typename U >
    static T* createInstanceBuffer(unsigned int rows, unsigned int columns,
                                   const ::fwMemory::BufferObject::sptr& instance,
                                   const T* redLookup,
                                   const T* greenLookup,
                                   const T* blueLookup)
    {
        DcmFileFormat fileFormat;
        OFCondition status;
        DcmDataset* dataset;

        // Create temporary buffer
        T* tempoBuffer = new T[rows * columns * 3]; // 3 colors (RGB)

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
        dataset->chooseRepresentation(EXS_LittleEndianExplicit, nullptr);

        // Read pixels
        const U* pixelData;
        if(sizeof(U) == 1)
        {
            const Uint8* pdata;
            dataset->findAndGetUint8Array(DCM_PixelData, pdata);
            pixelData = (U*)pdata;
        }
        else
        {
            const Uint16* pdata;
            dataset->findAndGetUint16Array(DCM_PixelData, pdata);
            pixelData = (U*)pdata;
        }

        OSLM_WARN_IF("Unable to read pixel data.", !pixelData);

        for (unsigned int x = 0; x < columns; ++x)
        {
            for (unsigned int y = 0; y < rows; ++y)
            {
                unsigned int position = y + (x * rows);
                U value               = pixelData[position];
                tempoBuffer[position*3]   = static_cast< T >((redLookup[value]/(double)0xffff)*256);
                tempoBuffer[position*3+1] = static_cast< T >((greenLookup[value]/(double)0xffff)*256);
                tempoBuffer[position*3+2] = static_cast< T >((blueLookup[value]/(double)0xffff)*256);
            }
        }

        return tempoBuffer;
    }
};

} //rgblookup
} //reader
} //fwDcmtkIO
