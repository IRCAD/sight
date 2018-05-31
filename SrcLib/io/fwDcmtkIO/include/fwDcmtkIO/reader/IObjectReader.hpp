/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwDcmtkIO/config.hpp"

#include <fwMedData/DicomSeries.hpp>

namespace fwDcmtkIO
{
namespace reader
{

/**
 * @brief Base class for Dicom instance reader.
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
    FWDCMTKIO_API virtual ::fwMedData::Series::sptr read(const ::fwMedData::DicomSeries::csptr& series) = 0;

};

} //reader
} //fwDcmtkIO
