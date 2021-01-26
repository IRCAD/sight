/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include <core/base.hpp>

#include <data/DicomSeries.hpp>

#include <services/macros.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace fwGdcmIO
{
namespace ut
{

typedef std::set<std::string> ExcludeSetType;
class DicomSeriesWriterTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( DicomSeriesWriterTest );
CPPUNIT_TEST( writeReadTest );
CPPUNIT_TEST( writeReadAnonymiseTest );
CPPUNIT_TEST( writeReadDirArchiveTest );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void writeReadTest();
    void writeReadAnonymiseTest();
    void writeReadDirArchiveTest();

private:

    /**
     * @brief Read Dicom series at given path and compare them to m_srcDicomSeries.
     *
     * @note Do not compare local_dicom_paths attributes
     *
     * @param p path to read Dicom series to be compared
     * @param anonymized if true, further attributes related to anonymisation are not compared.
     */
    void checkDicomSeries(
        const std::filesystem::path& p, bool anonymized = false);

    data::DicomSeries::sptr m_srcDicomSeries;

};

} // namespace ut
} // namespace fwGdcmIO
