/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2015-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDCMTKIO_READER_IMAGETAGREADER_HPP__
#define __FWDCMTKIO_READER_IMAGETAGREADER_HPP__

#include "fwDcmtkIO/config.hpp"
#include "fwDcmtkIO/reader/IObjectReader.hpp"

#include <fwData/Image.hpp>
#include <fwTools/Type.hpp>


namespace fwDcmtkIO
{

namespace reader
{

/**
 * @brief Reader for image tag
 */
class FWDCMTKIO_CLASS_API ImageTagReader : public ::fwDcmtkIO::reader::IObjectReader
{
public:
    typedef ::fwMedData::DicomSeries::DicomPathContainerType DicomPathContainerType;

    /// Constructor
    FWDCMTKIO_API ImageTagReader();

    /// Destructor
    FWDCMTKIO_API virtual ~ImageTagReader();

    /// Override
    FWDCMTKIO_API virtual ::fwMedData::Series::sptr read(::fwMedData::DicomSeries::sptr series);


};

} //reader
} //fwDcmtkIO


#endif /* __FWDCMTKIO_READER_IMAGETAGREADER_HPP__ */
