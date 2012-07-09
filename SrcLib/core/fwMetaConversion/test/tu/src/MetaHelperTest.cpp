#include <fwMetaConversion/MetaHelper.hpp>

#include <fwData/Array.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/Image.hpp>
#include <fwData/Float.hpp>
#include <fwData/Integer.hpp>
#include <fwData/Boolean.hpp>
#include <fwData/String.hpp>
#include <fwData/Color.hpp>
#include <fwData/Vector.hpp>
#include <fwData/Material.hpp>
#include <fwData/Reconstruction.hpp>
#include <fwData/Acquisition.hpp>
#include <fwData/Study.hpp>
#include <fwData/Patient.hpp>
#include <fwData/PatientDB.hpp>
#include <fwData/TransformationMatrix3D.hpp>
#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>
#include <fwData/Composite.hpp>
#include <fwData/TransferFunction.hpp>
#include <fwDataTools/Patient.hpp>
#include <fwDataTools/MeshGenerator.hpp>

#include "MetaHelperTest.hpp"


CPPUNIT_TEST_SUITE_REGISTRATION( ::fwMetaConversion::ut::MetaHelperTest );

namespace fwMetaConversion
{
namespace ut
{

void MetaHelperTest::setUp()
{

};
    // Set up context before running a test.


//-----------------------------------------------------------------------------

void MetaHelperTest::tearDown()
{
    // Clean up after the test run.
}

void MetaHelperTest::dataToMeta()
{
    ::fwData::Patient::sptr patient = ::fwData::Patient::New();
    ::fwDataTools::Patient::generatePatient(patient, 2, 2, 2);

    const ::fwData::Object::sptr VALUES[] = {
            ::fwData::Integer::New(1337),
            ::fwData::Float::New(),
            ::fwData::String::New(),
            ::fwData::Boolean::New(),
            ::fwData::String::New(),
            ::fwData::Vector::New(),
            ::fwData::Color::New(1.4, 0.9, 1.1, 1.67),
            ::fwData::Array::New(),
            ::fwData::Image::New(),
            ::fwData::Mesh::New(),
            ::fwData::Material::New(),
            ::fwData::Reconstruction::New(),
            ::fwData::Acquisition::New(),
            ::fwData::Study::New(),
            ::fwData::Composite::New(),
            ::fwData::Patient::New(),
            ::fwData::Point::New(),
            ::fwData::PointList::New(),
            ::fwData::TransformationMatrix3D::New(),
            ::fwData::TransferFunction::New(),
             patient,
    };

    ::fwMetaData::Object::sptr metaObject;

    BOOST_FOREACH ( fwData::Object::sptr  object, VALUES )
    {
        MetaHelper metaHelper;
        metaObject = metaHelper.dataToMeta( object);

        const ::camp::Class& metaClass = ::camp::classByName(object->getClassname());

        //Type test
        CPPUNIT_ASSERT_EQUAL(object->getClassname(), metaObject->getType());


        ::fwMetaData::Object::Attributes attributes = metaObject->getAttributes();
        ::fwMetaData::Object::Attributes::const_iterator cIt = attributes.begin();

         //Test attribute type
        for(; cIt != attributes.end(); ++cIt)
        {
                int type = metaClass.property(cIt->first).type();
                switch(type)
                {
                case camp::stringType:
                    CPPUNIT_ASSERT(cIt->second->isString());
                    CPPUNIT_ASSERT(cIt->second->isValue());
                    break;
                case camp::realType :
                case camp::intType :
                    CPPUNIT_ASSERT(cIt->second->isNumeric());
                    CPPUNIT_ASSERT(cIt->second->isValue());
                    break;
                case camp::boolType :
                    CPPUNIT_ASSERT(cIt->second->isBoolean());
                    CPPUNIT_ASSERT(cIt->second->isValue());
                    break;
                case camp::userType:
                    CPPUNIT_ASSERT(cIt->second->isObject() || cIt->second->isBlob());
                    break;
                case camp::mappingType:
                    CPPUNIT_ASSERT(cIt->second->isMapping());
                    break;
                case camp::enumType:
                    CPPUNIT_ASSERT(cIt->second->isString());
                    CPPUNIT_ASSERT(cIt->second->isValue());
                    break;
                case camp::arrayType:
                    CPPUNIT_ASSERT(cIt->second->isSequence());
                    break;
                }
        }




    }
}

void MetaHelperTest::metaToData()
{
    MetaHelper metaHelper;

    ::fwData::Color::sptr color = ::fwData::Color::New(0.2, 1.2, 1.3, 0.9);
    ::fwData::Material::sptr material = ::fwData::Material::New();
    material->setAmbient(color);

    ::fwMetaData::Object::sptr metaObject = metaHelper.dataToMeta( material);
    ::fwData::Object::sptr materialRes = metaHelper.metaToData(metaObject);
    ::fwData::Material::sptr materialResultat = ::fwData::Material::dynamicCast(materialRes);
    bool materialComparison = ::fwDataTools::Patient::compareMaterial(material, materialResultat, std::string("material"));

    CPPUNIT_ASSERT_MESSAGE("Material Not equal" , materialComparison);

    metaHelper.clearCache();
    metaHelper.clearMetaCache();

    ::fwData::Patient::sptr patient = ::fwData::Patient::New();
    ::fwDataTools::Patient::generatePatient(patient, 1, 1, 1);

    metaObject = metaHelper.dataToMeta( patient);
    ::fwData::Object::sptr patientObject = metaHelper.metaToData(metaObject);
    ::fwData::Patient::sptr patientResultat = ::fwData::Patient::dynamicCast(patientObject);
    bool patientComparison = ::fwDataTools::Patient::comparePatient(patient, patientResultat);

    CPPUNIT_ASSERT_MESSAGE("Patient Not equal" , patientComparison);
}

}  // namespace ut
}  // namespace fwMetaConversion
