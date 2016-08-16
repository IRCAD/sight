/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2015-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDCMTKIO_READER_MPPSREADER_HPP__
#define __FWDCMTKIO_READER_MPPSREADER_HPP__

#include <fwTools/Type.hpp>

#include <fwData/Image.hpp>

#include "fwDcmtkIO/config.hpp"
#include "fwDcmtkIO/reader/IObjectReader.hpp"

namespace fwDcmtkIO
{
namespace reader
{

/**
 * @brief Reader for MPPS
 */
class FWDCMTKIO_CLASS_API MPPSReader : public ::fwDcmtkIO::reader::IObjectReader
{
public:
    typedef ::fwMedData::DicomSeries::DicomPathContainerType DicomPathContainerType;

    /// Constructor
    FWDCMTKIO_API MPPSReader();

    /// Destructor
    FWDCMTKIO_API virtual ~MPPSReader();

    /// Override
    FWDCMTKIO_API virtual ::fwMedData::Series::sptr read(::fwMedData::DicomSeries::sptr series);


};

} //reader
} //fwDcmtkIO


#endif /* __FWDCMTKIO_READER_MPPSREADER_HPP__ */
