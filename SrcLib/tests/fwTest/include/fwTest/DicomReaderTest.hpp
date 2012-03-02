/* ***** BEGIN LICENSE BLOCK *****
* FW4SPL - Copyright (C) IRCAD, 2009-2010.
* Distributed under the terms of the GNU Lesser General Public License (LGPL) as
* published by the Free Software Foundation.
* ****** END LICENSE BLOCK ****** */

#ifndef __FWTEST__DICOMREADERTEST_HPP__
#define __FWTEST__DICOMREADERTEST_HPP__

#include <fwData/Patient.hpp>

#include "fwTest/config.hpp"

namespace fwTest
{

/**
 * @class   DicomReaderTest
 * @brief   This class defines some tools to check dicom manipulations during tests
 * @author  IRCAD (Research and Development Team).
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
    FWTEST_API static bool checkPatientACHGenou( ::fwData::Patient::sptr patient );
};




} // namespace fwTest

#endif // __FWTEST__DICOMREADERTEST_HPP__
