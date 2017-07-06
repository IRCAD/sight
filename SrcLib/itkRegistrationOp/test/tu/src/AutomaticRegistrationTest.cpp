/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "AutomaticRegistrationTest.hpp"

#include <itkRegistrationOp/AutomaticRegistration.hpp>
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

CPPUNIT_TEST_SUITE_REGISTRATION(::itkRegistrationOp::ut::AutomaticRegistrationTest);

namespace itkRegistrationOp
{
namespace ut
{

typedef ::itk::Image< std::int16_t, 3> ImageType;

//------------------------------------------------------------------------------

::fwData::Image::sptr createSphereImage()
{
    ImageType::Pointer image = ImageType::New();
    typedef itk::EllipseSpatialObject< 3 >   EllipseType;

    typedef itk::SpatialObjectToImageFilter<
            EllipseType, ImageType >   SpatialObjectToImageFilterType;

    SpatialObjectToImageFilterType::Pointer imageFilter =
        SpatialObjectToImageFilterType::New();

    ImageType::SizeType size;
    size[ 0 ] = 100;
    size[ 1 ] = 100;
    size[ 2 ] = 100;

    imageFilter->SetSize( size );

    ImageType::SpacingType spacing;
    spacing.Fill(1);
    imageFilter->SetSpacing(spacing);

    EllipseType::Pointer ellipse = EllipseType::New();
    EllipseType::ArrayType radiusArray;
    radiusArray[0] = 10;
    radiusArray[1] = 15;
    radiusArray[2] = 20;
    ellipse->SetRadius(radiusArray);

    typedef EllipseType::TransformType TransformType;
    TransformType::Pointer transform = TransformType::New();
    transform->SetIdentity();

    TransformType::OutputVectorType translation;
    TransformType::CenterType center;

    translation[ 0 ] = 50;
    translation[ 1 ] = 50;
    translation[ 2 ] = 50;
    transform->Translate( translation, false );

    ellipse->SetObjectToParentTransform( transform );

    imageFilter->SetInput(ellipse);

    ellipse->SetDefaultInsideValue(255);
    ellipse->SetDefaultOutsideValue(0);
    imageFilter->SetUseObjectValue( true );
    imageFilter->SetOutsideValue( 0 );

    imageFilter->Update();

    image->Graft(imageFilter->GetOutput());

    ::fwData::Image::sptr outputImage = ::fwData::Image::New();
    ::fwItkIO::itkImageToFwDataImage(image, outputImage);

    return outputImage;

}

//------------------------------------------------------------------------------

void AutomaticRegistrationTest::setUp()
{

}

//------------------------------------------------------------------------------

void AutomaticRegistrationTest::tearDown()
{

}

//------------------------------------------------------------------------------

void AutomaticRegistrationTest::identityTest()
{
    const ::fwData::Image::SizeType SIZE = {{ 32, 32, 32 }};

    const ::fwData::Image::SpacingType SPACING = {{ 0.5, 0.5, 0.5 }};
    const ::fwData::Image::OriginType ORIGIN   = {{ 0., 0., 0. }};
    const ::fwTools::Type TYPE                 = ::fwTools::Type::s_INT16;

    ::fwData::Image::sptr target    = ::fwData::Image::New();
    ::fwData::Image::sptr reference = ::fwData::Image::New();

    ::fwTest::generator::Image::generateImage(target, SIZE, SPACING, ORIGIN, TYPE);
    ::fwTest::generator::Image::randomizeArray(target->getDataArray());

    //Test if registering same image returns identity
    reference->deepCopy(target);

    ::fwData::TransformationMatrix3D::sptr mat = ::fwData::TransformationMatrix3D::New();

    ::itkRegistrationOp::AutomaticRegistration::registerImage(target, reference,
                                                              mat,
                                                              ::itkRegistrationOp::AutomaticRegistration::NORMALIZED_CORRELATION);

    for(size_t i = 0; i < 4; ++i)
    {
        for(size_t j = 0; j < 4; ++j)
        {

            double coef = mat->getCoefficient(i, j);
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

void AutomaticRegistrationTest::rigidTransformTest()
{
    namespace itkReg = ::itkRegistrationOp;

    ::fwData::Image::sptr target = createSphereImage();

    ::fwData::Image::sptr reference = ::fwData::Image::New();

    ::fwData::TransformationMatrix3D::sptr transform = ::fwData::TransformationMatrix3D::New();
    //set a translation
    transform->setCoefficient(0, 3, 4.);
    transform->setCoefficient(1, 3, 12.);
    transform->setCoefficient(2, 3, 7.);

    ::itkRegistrationOp::Resampler::resample(target, reference, transform);

    ::fwData::TransformationMatrix3D::sptr initTrf = ::fwData::TransformationMatrix3D::New();

    itkReg::AutomaticRegistration::registerImage(target, reference, initTrf,
                                                 itkReg::AutomaticRegistration::NORMALIZED_CORRELATION);

    for(size_t i = 0; i < 3; ++i)
    {
        double expected = transform->getCoefficient(i, 3);
        double actual   = initTrf->getCoefficient(i, 3);

        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Registration matrix does not match initial matrix",
                                             expected, -actual, 1e-4);
    }

}

//------------------------------------------------------------------------------
void AutomaticRegistrationTest::rotationTransformTest()
{
    namespace itkReg = ::itkRegistrationOp;

    ::fwData::Image::sptr target = createSphereImage();

    ::fwData::Image::sptr reference = ::fwData::Image::New();

    ::fwData::Image::sptr registeredImage = ::fwData::Image::New();

    ::fwData::TransformationMatrix3D::sptr transform = ::fwData::TransformationMatrix3D::New();
    //set a rotation on one axis

    ::glm::dmat4 rot = ::glm::rotate(::glm::dmat4(), ::glm::radians(5.), ::glm::dvec3(0., 0., 1.));

    ::fwDataTools::TransformationMatrix3D::setTF3DFromMatrix(transform, rot);

    std::cout<<"transform : "<<*transform<<std::endl;

    ::itkRegistrationOp::Resampler::resample(target, reference, transform);

    ::fwData::TransformationMatrix3D::sptr initTrf = ::fwData::TransformationMatrix3D::New();

    itkReg::AutomaticRegistration::registerImage(target, reference, initTrf,
                                                 itkReg::AutomaticRegistration::MEAN_SQUARES);

    ::glm::dmat4 res         = ::fwDataTools::TransformationMatrix3D::getMatrixFromTF3D(initTrf);
    ::glm::dquat orientation = ::glm::toQuat(res);
    ::glm::dvec3 axis        = ::glm::axis(orientation);
    double angle = ::glm::angle(orientation);

    std::cout<<"registered matrix : "<<*initTrf<<std::endl;

    ::itkRegistrationOp::Resampler::resample(reference, registeredImage, initTrf);
    ::fwVtkIO::ImageWriter::sptr writer = ::fwVtkIO::ImageWriter::New();
    writer->setObject(registeredImage);
    writer->setFile("./registered.vtk");
    writer->write();

    writer->setObject(target);
    writer->setFile("./target.vtk");
    writer->write();

    writer->setObject(reference);
    writer->setFile("./reference.vtk");
    writer->write();

    std::cout<<"axis : ["<<axis.x<<" , "<<axis.y<<" , "<<axis.z<<" ]"<<std::endl;
    std::cout<<"angle : "<< ::glm::degrees(angle)<<std::endl;

}
} // ut
} // itkRegistrationOp
