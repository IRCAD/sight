/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "MatrixTest.hpp"

#include <arlcore/vnl_rigid_vector.h>
#include <arlcore/MatrixR.h>
#include <arlcore/Misc.h>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::ARLcore::ut::MatrixTest );

namespace ARLcore
{
namespace ut
{

void MatrixTest::setUp()
{
    // Set up context before running a test.

}
void MatrixTest::tearDown()
{
    // Clean up after the test run.
}

void MatrixTest::conversionVectorMatrixTest()
{
    vnl_vector< double > input(6);
    input.put(0, 0.15);
    input.put(1, -0.485);
    input.put(2, 0.9);
    input.put(3, 12.48);
    input.put(4, -894.15);
    input.put(5, 540.14);

    arlCore::vnl_rigid_vector vec(input);
    arlCore::vnl_rigid_matrix matrix(vec);

    vnl_matrix_fixed<double, 4, 4> vnlMatrix;
    ::arlCore::convertRotationTranslationToMatrix(input, vnlMatrix);

    for(int j = 0; j<4; ++j)
    {
        for(int i = 0; i<4; ++i)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL(vnlMatrix[i][j], matrix[i][j], 1e-5);
        }
    }

}

} //namespace ut
} //namespace ARLcore
