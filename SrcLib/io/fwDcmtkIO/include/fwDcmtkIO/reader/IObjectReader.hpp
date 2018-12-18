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
