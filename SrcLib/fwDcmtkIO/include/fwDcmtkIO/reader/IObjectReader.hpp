/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDCMTKIO_READER_IOBJECTREADER_HPP__
#define __FWDCMTKIO_READER_IOBJECTREADER_HPP__

#include "fwDcmtkIO/config.hpp"

#include <fwDicomData/DicomSeries.hpp>

namespace fwDcmtkIO
{
namespace reader
{

/**
 * @class IObjectReader
 * @brief Base class for Dicom instance reader.
 * @date  2013.
 */
class FWDCMTKIO_CLASS_API IObjectReader
{
public:
    /// Constructor
    FWDCMTKIO_API IObjectReader();

    /// Destructor
    FWDCMTKIO_API virtual ~IObjectReader();

    /**
     * @brief Read the series from the group of instances.
     * @return Returns the created Series.
     */
    FWDCMTKIO_API virtual ::fwMedData::Series::sptr read(::fwDicomData::DicomSeries::sptr series) = 0;

};

} //reader
} //fwDcmtkIO


#endif /* __FWDCMTKIO_READER_IOBJECTREADER_HPP__ */
