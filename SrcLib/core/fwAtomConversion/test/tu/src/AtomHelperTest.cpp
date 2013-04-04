/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/log/SpyLogger.hpp>

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
#include <fwData/Graph.hpp>

#include <fwDataTools/Patient.hpp>
#include <fwDataTools/MeshGenerator.hpp>
#include <fwDataTools/Image.hpp>

#include <fwDataCamp/visitor/CompareObjects.hpp>

#include <fwAtoms/Object.hpp>
#include <fwAtoms/Map.hpp>

#include <fwTest/Exception.hpp>

#include <fwAtomConversion/convert.hpp>
#include <fwAtomConversion/DataVisitor.hpp>
#include <fwAtomConversion/exception/DataFactoryNotFound.hpp>
#include <fwAtomConversion/exception/DuplicatedDataUUID.hpp>

#include "AtomHelperTest.hpp"


CPPUNIT_TEST_SUITE_REGISTRATION( ::fwAtomConversion::ut::AtomHelperTest );

namespace fwAtomConversion
{
namespace ut
{

static ::fwTest::Exception fwTestException(""); // force link with fwTest

//-----------------------------------------------------------------------------

// Set up context before running a test.
void AtomHelperTest::setUp()
{};

//-----------------------------------------------------------------------------

void AtomHelperTest::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

void AtomHelperTest::dataToMetaTest()
{
    ::fwData::Patient::sptr patient = ::fwData::Patient::New();
    ::fwDataTools::Patient::generatePatient(patient, 2, 2, 2);

    const ::fwData::Object::sptr VALUES[] = {
            ::fwData::Integer::New(1337),
            ::fwData::Float::New(),
            ::fwData::String::New(),
            ::fwData::Boolean::New(),
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
            ::fwData::Graph::New(),
    };

    ::fwAtoms::Object::sptr atom;

    BOOST_FOREACH ( fwData::Object::sptr  object, VALUES )
    {
        atom = ::fwAtomConversion::convert(object);

        const ::camp::Class& metaClass = ::camp::classByName(object->getClassname());

        //Type test
        CPPUNIT_ASSERT_EQUAL( object->getClassname(),
                              atom->getMetaInfo( ::fwAtomConversion::DataVisitor::CLASSNAME_METAINFO ) );

         //Test attribute type
        BOOST_FOREACH( ::fwAtoms::Object::AttributesType::value_type elem, atom->getAttributes() )
        {
            if ( ! ( atom->getMetaInfo( ::fwAtomConversion::DataVisitor::CLASSNAME_METAINFO ) == "::fwData::Graph" &&
                    elem.first == "connections" ) )
            {
                int type = metaClass.property(elem.first).type();
                switch(type)
                {
                case camp::stringType:
                    CPPUNIT_ASSERT(elem.second->isString());
                    CPPUNIT_ASSERT(elem.second->isValue());
                    break;
                case camp::realType :
                case camp::intType :
                    CPPUNIT_ASSERT(elem.second->isNumeric());
                    CPPUNIT_ASSERT(elem.second->isValue());
                    break;
                case camp::boolType :
                    CPPUNIT_ASSERT(elem.second->isBoolean());
                    CPPUNIT_ASSERT(elem.second->isValue());
                    break;
                case camp::userType:
                    CPPUNIT_ASSERT(elem.second->isObject() || elem.second->isBlob());
                    break;
                case camp::mappingType:
                    CPPUNIT_ASSERT(elem.second->isMapping());
                    break;
                case camp::enumType:
                    CPPUNIT_ASSERT(elem.second->isString());
                    CPPUNIT_ASSERT(elem.second->isValue());
                    break;
                case camp::arrayType:
                    CPPUNIT_ASSERT(elem.second->isSequence());
                    break;
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------

void AtomHelperTest::materialConversionTest()
{
    ::fwData::Color::sptr color = ::fwData::Color::New(0.2, 1.2, 1.3, 0.9);
    ::fwData::Material::sptr material = ::fwData::Material::New();
    material->setAmbient(color);

    // Create Atom
    ::fwData::Material::sptr materialTmp = ::fwData::Material::New();
    materialTmp->deepCopy( material );
    ::fwAtoms::Object::sptr atom = ::fwAtomConversion::convert( materialTmp );
    materialTmp.reset();

    // Create Data from Atom
    ::fwData::Object::sptr materialRes = ::fwAtomConversion::convert(atom);
    ::fwData::Material::sptr materialResultat = ::fwData::Material::dynamicCast(materialRes);

    bool materialComparison = ::fwDataTools::Patient::compareMaterial(material, materialResultat, std::string("material"));
    CPPUNIT_ASSERT_MESSAGE("Material Not equal" , materialComparison);
}

//-----------------------------------------------------------------------------

void AtomHelperTest::patientConversionTest()
{
    ::fwData::Patient::sptr patient = ::fwData::Patient::New();
    ::fwDataTools::Patient::generatePatient(patient, 1, 1, 0);

    // Create Atom
    ::fwData::Patient::sptr patientTmp = ::fwData::Patient::New();
    patientTmp->deepCopy( patient );

    // HACK : Acquisition::netID not managed in deep copy
    ::fwData::Acquisition::sptr acq = patientTmp->getStudies()[0]->getAcquisitions()[0];
    acq->setNetID(patient->getStudies()[0]->getAcquisitions()[0]->getNetID());

    ::fwAtoms::Object::sptr atom = ::fwAtomConversion::convert( patientTmp );

    ::fwData::Image::sptr image =  acq->getImage();
    image->getDataArray()->setIsBufferOwner(false);
    image.reset();
    acq.reset();
    patientTmp.reset();


    // Create Data from Atom
    ::fwData::Object::sptr patientObject = ::fwAtomConversion::convert(atom);
    ::fwData::Patient::sptr patientResultat = ::fwData::Patient::dynamicCast(patientObject);

    using namespace ::fwDataCamp::visitor;
    CompareObjects visitor;
    visitor.compare(patient, patientResultat);
    SPTR(CompareObjects::PropsMapType) props = visitor.getDifferences();
    BOOST_FOREACH(CompareObjects::PropsMapType::value_type prop, (*props) )
    {
        OSLM_ERROR( "new object difference found : " << prop.first << " " << prop.second );
    }
    CPPUNIT_ASSERT_MESSAGE("Patient Not equal" , props->size() == 0 );

    bool patientComparison = ::fwDataTools::Patient::comparePatient(patient, patientResultat);
    CPPUNIT_ASSERT_MESSAGE("Patient Not equal" , patientComparison);
}

//-----------------------------------------------------------------------------

void AtomHelperTest::graphConversionTest()
{
    ::fwAtoms::Object::sptr atom;
    ::fwTools::UUID::UUIDType gID,n1ID,n2ID,n3ID,e12ID,e23ID;
    {
        ::fwData::Graph::sptr g = ::fwData::Graph::New();
        gID = ::fwTools::UUID::get(g);
        ::fwData::Node::sptr  n1 = ::fwData::Node::New();
        n1ID = ::fwTools::UUID::get(n1);
        ::fwData::Node::sptr  n2 = ::fwData::Node::New();
        n2ID = ::fwTools::UUID::get(n2);
        ::fwData::Node::sptr  n3 = ::fwData::Node::New();
        n3ID = ::fwTools::UUID::get(n3);
        ::fwData::Edge::sptr  e12 = ::fwData::Edge::New();
        e12ID = ::fwTools::UUID::get(e12);
        ::fwData::Edge::sptr  e23 = ::fwData::Edge::New();
        e23ID = ::fwTools::UUID::get(e23);

        // build graph
        g->addNode(n1);
        g->addNode(n2);
        g->addNode(n3);

        n1->addOutputPort( ::fwData::Port::New() );
        n2->addInputPort( ::fwData::Port::New() );
        n2->addOutputPort( ::fwData::Port::New() );
        n3->addInputPort( ::fwData::Port::New() );

        e12->setIdentifiers("IDNOTdefined","IDNOTdefined");
        e23->setIdentifiers("IDNOTdefined","IDNOTdefined");

        g->addEdge(e12,n1,n2);
        g->addEdge(e23,n2,n3);

        // Test field on edge
        e12->setField("infoTest",::fwData::String::New("valueInfoTest"));

        // Create Atom
        atom = ::fwAtomConversion::convert( g );
    }

   // Create Data from Atom
   ::fwData::Graph::sptr newGraph = ::fwData::Graph::dynamicCast( ::fwAtomConversion::convert(atom) );

   // nodes
   ::fwData::Node::sptr n1, n2, n3;

   // Test nodes
   const ::fwData::Graph::NodeContainer & nodes = newGraph->getCRefNodes();
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Graph nodes size" ,  (size_t)3, nodes.size() );
   BOOST_FOREACH( ::fwData::Node::sptr node, nodes )
   {
       ::fwTools::UUID::UUIDType nodeID = ::fwTools::UUID::get(node);
       CPPUNIT_ASSERT_MESSAGE("Test node uuid" ,  nodeID == n1ID || nodeID == n2ID || nodeID == n3ID );
       if (  nodeID == n1ID ) { n1 = node; }
       else if  (  nodeID == n2ID ) { n2 = node; }
       else if  (  nodeID == n3ID ) { n3 = node; }

   }
   CPPUNIT_ASSERT_MESSAGE("Test node n1" , n1 );
   CPPUNIT_ASSERT_MESSAGE("Test node n2" , n2 );
   CPPUNIT_ASSERT_MESSAGE("Test node n3" , n3 );

   // Test edges
   const ::fwData::Graph::ConnectionContainer & connections = newGraph->getCRefConnections();
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Graph connections size" ,  (size_t)2, connections.size() );
   BOOST_FOREACH( ::fwData::Graph::ConnectionContainer::value_type elem , connections )
   {
       ::fwTools::UUID::UUIDType edgeID = ::fwTools::UUID::get(elem.first);
       CPPUNIT_ASSERT_MESSAGE("Test edge uuid" ,  edgeID == e12ID || edgeID == e23ID );
       if ( edgeID == e12ID )
       {
           CPPUNIT_ASSERT( newGraph->getSourceNode( elem.first ) == n1 );
           CPPUNIT_ASSERT( newGraph->getDestinationNode( elem.first ) == n2 );

           // test field
           CPPUNIT_ASSERT( elem.first->getField("infoTest") );
           CPPUNIT_ASSERT( elem.first->getField< ::fwData::String >("infoTest")->value() == "valueInfoTest" );
       }
       else
       {
           CPPUNIT_ASSERT( newGraph->getSourceNode( elem.first ) == n2 );
           CPPUNIT_ASSERT( newGraph->getDestinationNode( elem.first ) == n3 );
       }
   }
}

//-----------------------------------------------------------------------------

void AtomHelperTest::objectMultiReferencedTest()
{
    ::fwAtoms::Object::sptr atom;
    {
        ::fwData::Composite::sptr composite = ::fwData::Composite::New();
        ::fwData::String::sptr data = ::fwData::String::New();
        composite->getContainer()["key1"] = data;
        composite->getContainer()["key2"] = data;

        // Create Atom
        atom = ::fwAtomConversion::convert( composite );
    }

    // Create Data from Atom
    ::fwData::Composite::sptr newComposite = ::fwData::Composite::dynamicCast( ::fwAtomConversion::convert(atom) );
    ::fwData::Composite::ContainerType & dataMap = newComposite->getContainer();
    CPPUNIT_ASSERT( dataMap.find("key1") != dataMap.end() );
    CPPUNIT_ASSERT( dataMap.find("key2") != dataMap.end() );
    CPPUNIT_ASSERT( dataMap["key1"] );
    CPPUNIT_ASSERT( dataMap["key2"] );
    CPPUNIT_ASSERT( dataMap["key2"] == dataMap["key1"] );
}

//-----------------------------------------------------------------------------

void AtomHelperTest::recursiveObjectTest()
{
    ::fwAtoms::Object::sptr atom;
    ::fwTools::UUID::UUIDType compositeID;
    {
        ::fwData::Composite::sptr composite = ::fwData::Composite::New();
        compositeID = ::fwTools::UUID::get( composite );
        composite->getContainer()["key"] = composite;
        // Create Atom
        atom = ::fwAtomConversion::convert( composite );

        // Hack, remove composite to destroy composite
        composite->getContainer().erase("key");
    }

    CPPUNIT_ASSERT( ! ::fwTools::UUID::exist( compositeID ) );

    // Create Data from Atom
    ::fwData::Composite::sptr newComposite = ::fwData::Composite::dynamicCast( ::fwAtomConversion::convert(atom) );
    ::fwData::Composite::ContainerType & dataMap = newComposite->getContainer();
    CPPUNIT_ASSERT( dataMap.find("key") != dataMap.end() );
    CPPUNIT_ASSERT( newComposite == dataMap["key"] );
}

//-----------------------------------------------------------------------------

void AtomHelperTest::dataFactoryNotFoundExceptionTest()
{
    ::fwAtoms::Object::sptr atom;
    {
        ::fwData::Composite::sptr composite = ::fwData::Composite::New();
        ::fwData::String::sptr data = ::fwData::String::New();
        composite->getContainer()["key"] = data;

        // Create Atom
        atom = ::fwAtomConversion::convert( composite );
    }

    {
        ::fwAtoms::Map::sptr map = ::fwAtoms::Map::dynamicCast( atom->getAttribute("values") );
        ::fwAtoms::Object::sptr obj = ::fwAtoms::Object::dynamicCast( (*map)["key"] );
        obj->eraseMetaInfo( ::fwAtomConversion::DataVisitor::CLASSNAME_METAINFO );
        obj->setMetaInfo( ::fwAtomConversion::DataVisitor::CLASSNAME_METAINFO, "CHANGE::CLASNAME" );

    }

    CPPUNIT_ASSERT_THROW( ::fwAtomConversion::convert(atom),
                          ::fwAtomConversion::exception::DataFactoryNotFound );

}

//-----------------------------------------------------------------------------

void AtomHelperTest::uuidExceptionTest()
{
    // Create data
    ::fwData::Composite::sptr composite = ::fwData::Composite::New();
    ::fwData::String::sptr data = ::fwData::String::New();
    composite->getContainer()["key"] = data;

    // Create Atom
    ::fwAtoms::Object::sptr atom = ::fwAtomConversion::convert( composite );

    CPPUNIT_ASSERT_THROW( ::fwAtomConversion::convert(atom),
            ::fwAtomConversion::exception::DuplicatedDataUUID );
}

//-----------------------------------------------------------------------------

class ClassNotCamped : public ::fwData::Object
{

public :

    fwCoreClassDefinitionsWithNFactoriesMacro( (ClassNotCamped)(::fwData::Object),
       ((::fwData::factory::New< ClassNotCamped > ,() )) );

    fwDataObjectMacro();

    ClassNotCamped(::fwData::Object::Key key){};

};


void AtomHelperTest::campFactoryNotFoundExceptionTest()
{
    ClassNotCamped::sptr obj = ClassNotCamped::New();
    CPPUNIT_ASSERT(obj);
    CPPUNIT_ASSERT_THROW( ::fwAtomConversion::convert( obj ), ::camp::ClassNotFound );
}

//-----------------------------------------------------------------------------

}  // namespace ut
}  // namespace fwAtomConversion
