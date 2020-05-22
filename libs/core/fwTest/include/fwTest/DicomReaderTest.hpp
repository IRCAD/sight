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

#include "fwTest/config.hpp"

#include <fwMedData/ImageSeries.hpp>

namespace fwTest
{

class FWTEST_CLASS_API DicomReaderTest
{

public:

    /**
     * @brief Method used to verify if a specific dicom file (stored on the
     * test database) is well read
     *
     * This file is located here on test database : "sight/Patient/Dicom/JMSGenou",
     * its represents a CT image of a knee ( 400 slices ).
     */
    FWTEST_API static bool checkSeriesJMSGenou( const SPTR(::fwMedData::ImageSeries)& series );

    /**
     * @brief This method is the same as checkSeriesJMSGenou but it checks trimmed strings instead of even sized strings
     */
    FWTEST_API static bool checkSeriesJMSGenouTrimmed( const SPTR(::fwMedData::ImageSeries)& series );
};

} // namespace fwTest
