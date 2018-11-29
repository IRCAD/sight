/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#ifndef __FWTEST_DICOMREADERTEST_HPP__
#define __FWTEST_DICOMREADERTEST_HPP__

#include <fwMedData/ImageSeries.hpp>

#include "fwTest/config.hpp"

namespace fwTest
{

/**
 * @class   DicomReaderTest
 * @brief   This class defines some tools to check dicom manipulations during tests
 *
 * @date    2007-2012.
 */
class FWTEST_CLASS_API DicomReaderTest
{

public:

    /**
     * @brief Method used to verify if a specific dicom file (stored on the
     * test database) is well read
     *
     * This file is located here on test database : "fw4spl/Patient/Dicom/ACHGenou",
     * its represents a CT image of a knee ( 400 slices ).
     */
    FWTEST_API static bool checkSeriesACHGenou( const SPTR(::fwMedData::ImageSeries) &series );

    /**
     * @brief This method is the same as checkSeriesACHGenou but it checks trimmed strings instead of even sized strings
     */
    FWTEST_API static bool checkSeriesACHGenouTrimmed( const SPTR(::fwMedData::ImageSeries) &series );
};




} // namespace fwTest

#endif // __FWTEST_DICOMREADERTEST_HPP__
