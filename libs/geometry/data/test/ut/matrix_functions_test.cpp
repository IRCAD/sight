/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "matrix_functions_test.hpp"

#include <geometry/data/line_functions.hpp>
#include <geometry/data/matrix_functions.hpp>
#include <geometry/data/types.hpp>
#include <geometry/data/vector_functions.hpp>

#include <glm/glm.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::geometry::data::ut::matrix_functions_test);

namespace sight::geometry::data::ut
{

//------------------------------------------------------------------------------

void matrix_functions_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void matrix_functions_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void matrix_functions_test::check_mult_vec_matrix()
{
    const double pi    = 3.14159265358979323846;
    const double theta = pi * .5;

    {
        const fw_matrix4x4 matrix = {{{{1., 0., 0, 0}},
            {{0., std::cos(theta), -std::sin(theta), 0}},
            {{0., std::sin(theta), std::cos(theta), 0}},
            {{0.0, 0.0, 0.0, 1.0
            }
            }
        }
        };
        const fw_vec3d source = {{0, 1, 0}};

        fw_vec3d result;
        geometry::data::mult_vec_matrix(matrix, source, result);

        CPPUNIT_ASSERT_DOUBLES_EQUAL(0, result[0], 0.00001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0, result[1], 0.00001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(1, result[2], 0.00001);
    }
    {
        const fw_matrix4x4 matrix = {{{{1., 0., 0, 52.3}},
            {{0., std::cos(theta), -std::sin(theta), -12.}},
            {{0., std::sin(theta), std::cos(theta), 1.2}},
            {{0.0, 0.0, 0.0, 1.0
            }
            }
        }
        };
        const fw_vec3d source = {{0, 0, 0}};

        fw_vec3d result;
        geometry::data::mult_vec_matrix(matrix, source, result);

        CPPUNIT_ASSERT_DOUBLES_EQUAL(52.3, result[0], 0.00001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(-12., result[1], 0.00001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(1.2, result[2], 0.00001);

        const fw_vec3d source2 = {{0, 10, 0}};
        geometry::data::mult_vec_matrix(matrix, source2, result);

        CPPUNIT_ASSERT_DOUBLES_EQUAL(52.3, result[0], 0.00001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(-12, result[1], 0.00001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(11.2, result[2], 0.00001);
    }
}

//------------------------------------------------------------------------------

void matrix_functions_test::check_rotation_matrix()
{
    const double x = 7.8;
    const double y = 0.6;
    const double z = 2.6;

    const fw_vec3d vec = {{x, y, z}};

    fw_matrix4x4 result = geometry::data::get_rotation_matrix(vec);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.076696, result[0][0], 0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.997054, result[0][1], 0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, result[0][2], 0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, result[0][3], 0.0001);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.3144601, result[1][0], 0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.024189, result[1][1], 0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.948962, result[1][2], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, result[1][3], 0.00001);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.946167, result[2][0], 0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.072782, result[2][1], 0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.315389, result[2][2], 0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, result[2][3], 0.0001);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, result[3][0], 0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, result[3][1], 0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, result[3][2], 0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, result[3][3], 0.0001);
}

//------------------------------------------------------------------------------

void matrix_functions_test::check_inverse()
{
    const fw_matrix4x4 mat = {{{{3.1, 1., -7.9689, 4.9}},
        {{5., -21., -1.3646, 14.4}},
        {{9., -7.2, -23.36, 79.04}},
        {{0.1, -3., -1.234, -49.94
        }
        }
    }
    };

    fw_matrix4x4 result = geometry::data::get_inverse(mat);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.885131908604589, result[0][0], 0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.190212289294002, result[0][1], 0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.29581348830958, result[0][2], 0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.326489683580158, result[0][3], 0.0001);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.185258037237573, result[1][0], 0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.00768780306376934, result[1][1], 0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.0586854220133396, result[1][2], 0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.0769210199757416, result[1][3], 0.0001);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.232796743379289, result[2][0], 0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0724476129133489, result[2][1], 0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.118830425964518, result[2][2], 0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.144341353618837, result[2][3], 0.0001);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.015108772570731, result[3][0], 0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.000947451265806253, result[3][1], 0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.00586927638865189, result[3][2], 0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.0124903619956978, result[3][3], 0.0001);
}

//------------------------------------------------------------------------------

void matrix_functions_test::multiplication_test()
{
    const fw_matrix4x4 mat1 {{
        {0, 1, 2, 3},
        {4, 5, 6, 7},
        {8, 9, 10, 11},
        {12, 13, 14, 15}
    }
    };
    const fw_matrix4x4 mat2 {{
        {16, 17, 18, 19},
        {20, 21, 22, 23},
        {24, 25, 26, 27},
        {28, 29, 30, 31}
    }
    };
    const fw_matrix4x4 expected_result {{
        {152, 158, 164, 170},
        {504, 526, 548, 570},
        {856, 894, 932, 970},
        {1208, 1262, 1316, 1370}
    }
    };
    const fw_matrix4x4 actual_result = mat1 * mat2;

    for(std::uint8_t i = 0 ; i < 4 ; i++)
    {
        for(std::uint8_t j = 0 ; j < 4 ; j++)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(
                "i=" + std::to_string(i) + ", j=" + std::to_string(j),
                expected_result[i][j],
                actual_result[i][j],
                0.0001
            );
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::geometry::data::ut
