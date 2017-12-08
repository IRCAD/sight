/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "FastRegistrationTest.hpp"

#include "helper.hpp"

#include <itkRegistrationOp/FastRegistration.hpp>
#include <itkRegistrationOp/Metric.hpp>
#include <itkRegistrationOp/Resampler.hpp>

#include <fwData/TransformationMatrix3D.hpp>

#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>
#include <fwDataTools/helper/Image.hpp>
#include <fwDataTools/helper/MedicalImageAdaptor.hpp>
#include <fwDataTools/TransformationMatrix3D.hpp>

#include <fwItkIO/itk.hpp>

#include <fwTest/generator/Image.hpp>

#include <fwVtkIO/ImageWriter.hpp>

#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/mat4x4.hpp>

#include <itkEllipseSpatialObject.h>
#include <itkImage.h>
#include <itkSpatialObjectToImageFilter.h>

CPPUNIT_TEST_SUITE_REGISTRATION(::itkRegistrationOp::ut::FastRegistrationTest);

namespace itkRegistrationOp
{
namespace ut
{

using ImageType  = ::itk::Image< std::int16_t, 3>;
namespace itkReg = ::itkRegistrationOp;

//------------------------------------------------------------------------------

void FastRegistrationTest::setUp()
{
}

//------------------------------------------------------------------------------

void FastRegistrationTest::tearDown()
{
}

//------------------------------------------------------------------------------

void FastRegistrationTest::identityTest()
{
    ::fwData::Image::csptr target   = createSphereImage< ::std::uint16_t, 3>();
    ::fwData::Image::sptr reference = ::fwData::Image::New();
    reference->deepCopy(target);
    ::fwData::TransformationMatrix3D::sptr mat = ::fwData::TransformationMatrix3D::New();
    itkReg::FastRegistration::registerImage(target, reference, mat, itkReg::NORMALIZED_CORRELATION);

    for(size_t i = 0; i < 4; ++i)
    {
        for(size_t j = 0; j < 4; ++j)
        {
            const double coef = mat->getCoefficient(i, j);
            if(i == j) //diagonal
            {
                CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Diagonal value is not equal to '1' ", 1., coef, 1e-8);
            }
            else
            {
                CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Non-diagonal value is not equal to '0' ", 0., coef, 1e-8);
            }
        }
    }
}

//------------------------------------------------------------------------------

void FastRegistrationTest::translateTransformTest()
{
    ::fwData::Image::csptr target = createSphereImage< ::std::uint16_t, 3>();

    ::fwData::Image::sptr reference = ::fwData::Image::New();

    ::fwData::TransformationMatrix3D::sptr transform = ::fwData::TransformationMatrix3D::New();
    //set a translation
    transform->setCoefficient(0, 3, 4.);
    transform->setCoefficient(1, 3, 12.);
    transform->setCoefficient(2, 3, 7.);

    itkReg::Resampler::resample(target, reference, transform);

    ::fwData::TransformationMatrix3D::sptr initTrf = ::fwData::TransformationMatrix3D::New();

    itkReg::FastRegistration::registerImage(target, reference, initTrf, itkReg::NORMALIZED_CORRELATION, 0.000001, 500);

    for(size_t i = 0; i < 3; ++i)
    {
        const double expected = transform->getCoefficient(i, 3);
        const double actual   = initTrf->getCoefficient(i, 3);

        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Registration matrix does not match initial matrix",
                                             expected, -actual, 1e-2);
    }
}

} // ut
} // itkRegistrationOp
