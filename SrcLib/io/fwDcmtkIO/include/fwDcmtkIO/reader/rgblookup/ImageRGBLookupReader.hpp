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

#include <fwMedData/DicomSeries.hpp>

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmimgle/dcmimage.h>
#include <dcmtk/dcmnet/diutil.h>

namespace fwDcmtkIO
{
namespace reader
{
namespace rgblookup
{

/**
 * @brief This class is used to read the buffer of a DICOM image in DIRECT mode when a pixel lookup must be performed.
 * @note This class has only been tested on a single frame DICOM file
 */
class FWDCMTKIO_CLASS_API ImageRGBLookupReader
{
public:
    typedef ::fwMedData::DicomSeries::DicomContainerType DicomContainerType;

    /**
     * @brief Fill the buffer of an image
     * The template T is used to determine color format
     * The template U is used to determine pixel value format
     * @param[in] rows Number of rows
     * @param[in] columns Number of columns
     * @param[in] depth Number of frames
     * @param[in] instances Paths to the instances
     * @param[in] destination Destination buffer
     * @param[in] redLookup Red color lookup table
     * @param[in] greenLookup Green color lookup table
     * @param[in] blueLookup Blue color lookup table
     *
     */
    template< typename T, typename U >
    static void fillImageBuffer(unsigned int rows, unsigned int columns,
                                unsigned int depth,
                                DicomContainerType& instances,
                                void* destination, const T* redLookup,
                                const T* greenLookup,
                                const T* blueLookup)
    {
        U* tempoBuffer = ::fwDcmtkIO::reader::main::ImageReader::createTemporaryBuffer<U>(
            rows, columns, depth, instances);

        T* arrayBuffer = static_cast< T* >(destination);
        unsigned short x, y, z;
        unsigned int frameSize = columns * rows;
        x = y = z = 0;
        for (x = 0; x < columns; ++x)
        {
            for (y = 0; y < rows; ++y)
            {
                unsigned int yshift = x * rows;

                for (z = 0; z < depth; ++z)
                {
                    unsigned int position = y + yshift + z * frameSize;
                    U value               = tempoBuffer[position];

                    arrayBuffer[position*3]   = static_cast< T >((redLookup[value]/(double)0xffff)*256);
                    arrayBuffer[position*3+1] = static_cast< T >((greenLookup[value]/(double)0xffff)*256);
                    arrayBuffer[position*3+2] = static_cast< T >((blueLookup[value]/(double)0xffff)*256);
                }
            }
        }
        delete tempoBuffer;
    }
};

} //rgblookup
} //reader
} //fwDcmtkIO
