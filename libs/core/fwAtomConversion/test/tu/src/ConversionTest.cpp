/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "ConversionTest.hpp"

#include <fwAtomConversion/convert.hpp>
#include <fwAtomConversion/DataVisitor.hpp>
#include <fwAtomConversion/exception/ConversionNotManaged.hpp>
#include <fwAtomConversion/exception/DataFactoryNotFound.hpp>
#include <fwAtomConversion/exception/DuplicatedDataUUID.hpp>

#include <core/log/SpyLogger.hpp>
#include <core/reflection/macros.hpp>
#include <core/reflection/Mapper/ValueMapper.hpp>
#include <core/reflection/UserObject.hpp>
#include <core/tools/UUID.hpp>

#include <data/Array.hpp>
#include <data/Boolean.hpp>
#include <data/Color.hpp>
#include <data/Composite.hpp>
#include <data/Float.hpp>
#include <data/Graph.hpp>
#include <data/Image.hpp>
#include <data/Integer.hpp>
#include <data/Landmarks.hpp>
#include <data/Material.hpp>
#include <data/Mesh.hpp>
#include <data/Point.hpp>
#include <data/PointList.hpp>
#include <data/Reconstruction.hpp>
#include <data/reflection/visitor/CompareObjects.hpp>
#include <data/SeriesDB.hpp>
#include <data/String.hpp>
#include <data/TransferFunction.hpp>
#include <data/TransformationMatrix3D.hpp>
#include <data/Vector.hpp>

#include <fwAtoms/Map.hpp>
#include <fwAtoms/Object.hpp>
#include <fwAtoms/Sequence.hpp>

#include <utestData/generator/Object.hpp>
#include <utestData/generator/SeriesDB.hpp>

#include <functional>

CPPUNIT_TEST_SUITE_REGISTRATION( ::fwAtomConversion::ut::ConversionTest );

namespace fwAtomConversion
{
namespace ut
{

//-----------------------------------------------------------------------------

// Set up context before running a test.
void ConversionTest::setUp()
{
}

//-----------------------------------------------------------------------------

void ConversionTest::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

void compare(data::Object::sptr objRef, data::Object::sptr objComp)
{
    data::reflection::visitor::CompareObjects visitor;
    visitor.compare(objRef, objComp);
    SPTR(data::reflection::visitor::CompareObjects::PropsMapType) props = visitor.getDifferences();
    for( data::reflection::visitor::CompareObjects::PropsMapType::value_type prop :  (*props) )
    {
        SLM_ERROR( "new object difference found : " << prop.first << " '" << prop.second << "'" );
    }
    CPPUNIT_ASSERT_MESSAGE("Object Not equal", props->size() == 0 );
}

//-----------------------------------------------------------------------------

void ConversionTest::dataToAtomTest()
{
    const data::Object::sptr VALUES[] = {
        data::Integer::New(1337),
        data::Float::New(),
        data::String::New(),
        data::Boolean::New(),
        data::Vector::New(),
        data::Color::New(1.4f, 0.9f, 1.1f, 1.67f),
        data::Array::New(),
        data::Image::New(),
        data::Mesh::New(),
        data::Material::New(),
        data::Reconstruction::New(),
        data::Composite::New(),
        data::Point::New(),
        data::PointList::New(),
        data::TransformationMatrix3D::New(),
        data::TransferFunction::New(),
        data::Graph::New(),
    };

    ::fwAtoms::Object::sptr atom;

    for ( data::Object::sptr object : VALUES )
    {
        atom = ::fwAtomConversion::convert(object);

        const ::camp::Class& metaClass = ::camp::classByName(object->getClassname());

        //Type test
        CPPUNIT_ASSERT_EQUAL( object->getClassname(),
                              atom->getMetaInfo( ::fwAtomConversion::DataVisitor::CLASSNAME_METAINFO ) );

        //Test attribute type
        for( ::fwAtoms::Object::AttributesType::value_type elem :  atom->getAttributes() )
        {
            std::string classname = atom->getMetaInfo( ::fwAtomConversion::DataVisitor::CLASSNAME_METAINFO );
            // Drop attributes from fwAtomConversion::mappers.
            if( ( classname == "data::Mesh" && elem.first == "attributes" ) ||
                ( classname == "data::Graph" && elem.first == "connections" ) )
            {
                continue;
            }

            const int type              = metaClass.property(elem.first).type();
            const std::string attribute = metaClass.property(elem.first).name();
            switch(type)
            {
                case camp::stringType:
                    CPPUNIT_ASSERT(elem.second->isString());
                    CPPUNIT_ASSERT(elem.second->isValue());
                    break;
                case camp::realType:
                case camp::intType:
                    CPPUNIT_ASSERT(elem.second->isNumeric());
                    CPPUNIT_ASSERT(elem.second->isValue());
                    break;
                case camp::boolType:
                    CPPUNIT_ASSERT(elem.second->isBoolean());
                    CPPUNIT_ASSERT(elem.second->isValue());
                    break;
                case camp::userType:
                    if( ( ( classname == "data::Material" ) && ( attribute == "diffuse_texture" ) ) ||
                        ( ( classname == "data::Reconstruction" ) && ( attribute == "image" ) ) ||
                        ( ( classname == "data::Reconstruction" ) && ( attribute == "mesh" ) ) )
                    {
                        CPPUNIT_ASSERT_MESSAGE("classname: " + classname + ", attribute: " + attribute,
                                               !elem.second);
                    }
                    else
                    {
                        CPPUNIT_ASSERT(elem.second->isObject() || elem.second->isBlob());
                    }
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

//-----------------------------------------------------------------------------

void ConversionTest::materialConversionTest()
{
    data::Color::sptr color       = data::Color::New(0.2f, 1.2f, 1.3f, 0.9f);
    data::Material::sptr material = data::Material::New();
    material->setDiffuse(color);

    // Create Atom
    data::Material::sptr materialTmp;
    materialTmp                  = data::Object::copy( material );
    ::fwAtoms::Object::sptr atom = ::fwAtomConversion::convert( materialTmp );
    materialTmp.reset();

    // Create Data from Atom
    data::Object::sptr materialRes        = ::fwAtomConversion::convert(atom);
    data::Material::sptr materialResultat = data::Material::dynamicCast(materialRes);

    compare(material, materialResultat);
}

//-----------------------------------------------------------------------------

void ConversionTest::meshConversionTest()
{
    data::Mesh::sptr mesh = data::Mesh::New();
    const auto lock       = mesh->lock();

    mesh->reserve(90, 30, data::Mesh::CellType::TRIANGLE, data::Mesh::Attributes::POINT_COLORS
                  | data::Mesh::Attributes::POINT_NORMALS
                  | data::Mesh::Attributes::POINT_TEX_COORDS);

    ::fwAtoms::Object::sptr atom = ::fwAtomConversion::convert( mesh );

    // Create Data from Atom
    data::Object::sptr convertedMesh = ::fwAtomConversion::convert(atom);
    data::Mesh::sptr mesh2           = data::Mesh::dynamicCast(convertedMesh);

    compare(mesh, mesh2);

}

//-----------------------------------------------------------------------------

void ConversionTest::graphConversionTest()
{
    ::fwAtoms::Object::sptr atom;
    core::tools::UUID::UUIDType gID, n1ID, n2ID, n3ID, e12ID, e23ID;
    {
        data::Graph::sptr g = data::Graph::New();
        gID = core::tools::UUID::get(g);
        data::Node::sptr n1 = data::Node::New();
        n1ID = core::tools::UUID::get(n1);
        data::Node::sptr n2 = data::Node::New();
        n2ID = core::tools::UUID::get(n2);
        data::Node::sptr n3 = data::Node::New();
        n3ID = core::tools::UUID::get(n3);
        data::Edge::sptr e12 = data::Edge::New();
        e12ID = core::tools::UUID::get(e12);
        data::Edge::sptr e23 = data::Edge::New();
        e23ID = core::tools::UUID::get(e23);

        // build graph
        g->addNode(n1);
        g->addNode(n2);
        g->addNode(n3);

        n1->addOutputPort( data::Port::New() );
        n2->addInputPort( data::Port::New() );
        n2->addOutputPort( data::Port::New() );
        n3->addInputPort( data::Port::New() );

        e12->setIdentifiers("IDNOTdefined", "IDNOTdefined");
        e23->setIdentifiers("IDNOTdefined", "IDNOTdefined");

        g->addEdge(e12, n1, n2);
        g->addEdge(e23, n2, n3);

        // Test field on edge
        e12->setField("infoTest", data::String::New("valueInfoTest"));

        // Create Atom
        atom = ::fwAtomConversion::convert( g );
    }

    // Create Data from Atom
    data::Graph::sptr newGraph = data::Graph::dynamicCast( ::fwAtomConversion::convert(atom) );

    // nodes
    data::Node::sptr n1, n2, n3;

    // Test nodes
    const data::Graph::NodeContainer& nodes = newGraph->getNodes();
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Graph nodes size", (size_t)3, nodes.size() );
    for( data::Node::sptr node :  nodes )
    {
        core::tools::UUID::UUIDType nodeID = core::tools::UUID::get(node);
        CPPUNIT_ASSERT_MESSAGE("Test node uuid", nodeID == n1ID || nodeID == n2ID || nodeID == n3ID );
        if (  nodeID == n1ID )
        {
            n1 = node;
        }
        else if  (  nodeID == n2ID )
        {
            n2 = node;
        }
        else if  (  nodeID == n3ID )
        {
            n3 = node;
        }

    }
    CPPUNIT_ASSERT_MESSAGE("Test node n1", n1 );
    CPPUNIT_ASSERT_MESSAGE("Test node n2", n2 );
    CPPUNIT_ASSERT_MESSAGE("Test node n3", n3 );

    // Test edges
    const data::Graph::ConnectionContainer& connections = newGraph->getConnections();
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Graph connections size", (size_t)2, connections.size() );
    for( data::Graph::ConnectionContainer::value_type elem  :  connections )
    {
        core::tools::UUID::UUIDType edgeID = core::tools::UUID::get(elem.first);
        CPPUNIT_ASSERT_MESSAGE("Test edge uuid", edgeID == e12ID || edgeID == e23ID );
        if ( edgeID == e12ID )
        {
            CPPUNIT_ASSERT( newGraph->getSourceNode( elem.first ) == n1 );
            CPPUNIT_ASSERT( newGraph->getDestinationNode( elem.first ) == n2 );

            // test field
            CPPUNIT_ASSERT( elem.first->getField("infoTest") );
            CPPUNIT_ASSERT( elem.first->getField< data::String >("infoTest")->value() == "valueInfoTest" );
        }
        else
        {
            CPPUNIT_ASSERT( newGraph->getSourceNode( elem.first ) == n2 );
            CPPUNIT_ASSERT( newGraph->getDestinationNode( elem.first ) == n3 );
        }
    }
}

//-----------------------------------------------------------------------------

void ConversionTest::tfConversionTest()
{
    ::fwAtoms::Object::sptr atom;

    data::TransferFunction::sptr tf = utestData::generator::Object::createTFColor(15, 120, 50);

    // Create Atom
    atom = ::fwAtomConversion::convert( tf );

    data::TransferFunction::sptr newTF =
        data::TransferFunction::dynamicCast( ::fwAtomConversion::convert(atom) );

    compare(tf, newTF);
}

//-----------------------------------------------------------------------------

void ConversionTest::seriesDBConversionTest()
{
    ::fwAtoms::Object::sptr atom;

    data::SeriesDB::sptr sdb = utestData::generator::SeriesDB::createSeriesDB(2, 3, 1);

    // Create Atom
    atom = ::fwAtomConversion::convert( sdb );

    data::SeriesDB::sptr newSdb =
        data::SeriesDB::dynamicCast( ::fwAtomConversion::convert(atom) );

    compare(sdb, newSdb);
}

//-----------------------------------------------------------------------------

void ConversionTest::landmarksConversionTest()
{
    {
        // Basic test with empty structure
        ::fwAtoms::Object::sptr atom;

        data::Landmarks::sptr landmarks = data::Landmarks::New();

        // Create Atom
        atom = ::fwAtomConversion::convert( landmarks );

        data::Landmarks::sptr newLandmarks =
            data::Landmarks::dynamicCast( ::fwAtomConversion::convert(atom) );

        compare(landmarks, newLandmarks);
    }

    {
        const std::string GROUP1                = "group_1";
        const std::string GROUP2                = "group_2";
        const std::string GROUP3                = "group_3";
        const data::Landmarks::ColorType COLOR1 = {{1.f, 0.0f, 0.0f, 1.0f}};
        const data::Landmarks::ColorType COLOR2 = {{0.f, 1.0f, 0.0f, 1.0f}};
        const data::Landmarks::ColorType COLOR3 = {{0.f, 0.0f, 1.0f, 1.0f}};
        const data::Landmarks::SizeType SIZE1   = 3.45f;
        const data::Landmarks::SizeType SIZE2   = 7.5f;
        const data::Landmarks::SizeType SIZE3   = 1.3f;
        const data::Landmarks::Shape SHAPE1     = data::Landmarks::Shape::SPHERE;
        const data::Landmarks::Shape SHAPE2     = data::Landmarks::Shape::CUBE;
        const data::Landmarks::Shape SHAPE3     = data::Landmarks::Shape::SPHERE;
        const bool VISIBILITY1                  = true;
        const bool VISIBILITY2                  = false;
        const bool VISIBILITY3                  = true;

        const data::Landmarks::PointType POINT1 = {{3.5, 5.8, 2.56}};
        const data::Landmarks::PointType POINT2 = {{8.25, 56.0, 45.4}};
        const data::Landmarks::PointType POINT3 = {{0.0, 0.0, 0.0}};
        const data::Landmarks::PointType POINT4 = {{0.5, 0.6, 0.7}};

        data::Landmarks::sptr landmarks = data::Landmarks::New();

        landmarks->addGroup(GROUP1, COLOR1, SIZE1, SHAPE1, VISIBILITY1);
        landmarks->addGroup(GROUP2, COLOR2, SIZE2, SHAPE2, VISIBILITY2);
        landmarks->addGroup(GROUP3, COLOR3, SIZE3, SHAPE3, VISIBILITY3);
        landmarks->addPoint(GROUP1, POINT1);
        landmarks->addPoint(GROUP2, POINT2);
        landmarks->addPoint(GROUP3, POINT3);
        landmarks->addPoint(GROUP1, POINT4);

        // Create Atom
        ::fwAtoms::Object::sptr atom = ::fwAtomConversion::convert( landmarks );

        data::Landmarks::sptr newLandmarks =
            data::Landmarks::dynamicCast( ::fwAtomConversion::convert(atom) );

        CPPUNIT_ASSERT(newLandmarks);

        CPPUNIT_ASSERT_EQUAL(landmarks->getNumberOfGroups(), newLandmarks->getNumberOfGroups());
        CPPUNIT_ASSERT_EQUAL(landmarks->getNumberOfPoints(), newLandmarks->getNumberOfPoints());

        const data::Landmarks::GroupNameContainer names = landmarks->getGroupNames();

        for (const auto& name : names)
        {
            CPPUNIT_ASSERT_NO_THROW(newLandmarks->getGroup(name));

            const data::Landmarks::LandmarksGroup& group    = landmarks->getGroup(name);
            const data::Landmarks::LandmarksGroup& newGroup = newLandmarks->getGroup(name);

            CPPUNIT_ASSERT(group.m_color == newGroup.m_color);
            CPPUNIT_ASSERT_EQUAL(group.m_points.size(), newGroup.m_points.size());

            for (size_t i = 0; i < group.m_points.size(); ++i)
            {
                CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("point[" + std::to_string(i) + "][0]",
                                                     group.m_points[i][0], newGroup.m_points[i][0], 0.00001);
                CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("point[" + std::to_string(i) + "][1]",
                                                     group.m_points[i][1], newGroup.m_points[i][1], 0.00001);
                CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("point[" + std::to_string(i) + "][2]",
                                                     group.m_points[i][2], newGroup.m_points[i][2], 0.00001);
            }
            CPPUNIT_ASSERT_EQUAL(group.m_size, newGroup.m_size);
            CPPUNIT_ASSERT_EQUAL(int(group.m_shape), int(newGroup.m_shape));
            CPPUNIT_ASSERT_EQUAL(group.m_visibility, newGroup.m_visibility);
        }
    }
}

//-----------------------------------------------------------------------------

void ConversionTest::objectMultiReferencedTest()
{
    ::fwAtoms::Object::sptr atom;
    {
        data::Composite::sptr composite = data::Composite::New();
        data::String::sptr data         = data::String::New();
        composite->getContainer()["key1"] = data;
        composite->getContainer()["key2"] = data;

        // Create Atom
        atom = ::fwAtomConversion::convert( composite );
    }

    // Create Data from Atom
    data::Composite::sptr newComposite =
        data::Composite::dynamicCast( ::fwAtomConversion::convert(atom) );
    data::Composite::ContainerType& dataMap = newComposite->getContainer();
    CPPUNIT_ASSERT( dataMap.find("key1") != dataMap.end() );
    CPPUNIT_ASSERT( dataMap.find("key2") != dataMap.end() );
    CPPUNIT_ASSERT( dataMap["key1"] );
    CPPUNIT_ASSERT( dataMap["key2"] );
    CPPUNIT_ASSERT( dataMap["key2"] == dataMap["key1"] );
}

//-----------------------------------------------------------------------------

void ConversionTest::recursiveObjectTest()
{
    ::fwAtoms::Object::sptr atom;
    core::tools::UUID::UUIDType compositeID;
    {
        data::Composite::sptr composite = data::Composite::New();
        compositeID                      = core::tools::UUID::get( composite );
        composite->getContainer()["key"] = composite;
        // Create Atom
        atom = ::fwAtomConversion::convert( composite );

        // Hack, remove composite to destroy composite
        composite->getContainer().erase("key");
    }

    CPPUNIT_ASSERT( !core::tools::UUID::exist( compositeID ) );

    // Create Data from Atom
    data::Composite::sptr newComposite =
        data::Composite::dynamicCast( ::fwAtomConversion::convert(atom) );
    data::Composite::ContainerType& dataMap = newComposite->getContainer();
    CPPUNIT_ASSERT( dataMap.find("key") != dataMap.end() );
    CPPUNIT_ASSERT( newComposite == dataMap["key"] );

    // Same hack as before to deallocate the new composite.
    newComposite->getContainer().erase("key");
    // The atom also references itself, clear it to prevent a memory leak.
    atom->clearAttribute();
}

//-----------------------------------------------------------------------------

void ConversionTest::dataFactoryNotFoundExceptionTest()
{
    ::fwAtoms::Object::sptr atom;
    {
        data::Composite::sptr composite = data::Composite::New();
        data::String::sptr data         = data::String::New();
        composite->getContainer()["key"] = data;

        // Create Atom
        atom = ::fwAtomConversion::convert( composite );
    }

    {
        ::fwAtoms::Map::sptr map    = ::fwAtoms::Map::dynamicCast( atom->getAttribute("values") );
        ::fwAtoms::Object::sptr obj = ::fwAtoms::Object::dynamicCast( (*map)["key"] );
        obj->eraseMetaInfo( ::fwAtomConversion::DataVisitor::CLASSNAME_METAINFO );
        obj->setMetaInfo( ::fwAtomConversion::DataVisitor::CLASSNAME_METAINFO, "CHANGE::CLASNAME" );
    }

    CPPUNIT_ASSERT_THROW( ::fwAtomConversion::convert(atom),
                          ::fwAtomConversion::exception::DataFactoryNotFound );

}

//-----------------------------------------------------------------------------

void ConversionTest::uuidExceptionTest()
{
    // Create data
    data::Composite::sptr composite = data::Composite::New();
    data::String::sptr data         = data::String::New();
    composite->getContainer()["key"] = data;

    // Create Atom
    ::fwAtoms::Object::sptr atom = ::fwAtomConversion::convert( composite );

    CPPUNIT_ASSERT_THROW( ::fwAtomConversion::convert(atom, ::fwAtomConversion::AtomVisitor::StrictPolicy()),
                          ::fwAtomConversion::exception::DuplicatedDataUUID );
}

//-----------------------------------------------------------------------------

void ConversionTest::uuidChangeTest()
{
    // Create data
    data::Composite::sptr composite = data::Composite::New();
    data::String::sptr data         = data::String::New();
    composite->getContainer()["key"] = data;

    // Create Atom
    ::fwAtoms::Object::sptr atom = ::fwAtomConversion::convert( composite );

    data::Composite::sptr compositeReloaded;
    data::String::sptr dataReloaded;

    compositeReloaded = data::Composite::dynamicCast(
        ::fwAtomConversion::convert(atom, ::fwAtomConversion::AtomVisitor::ChangePolicy())
        );
    dataReloaded = data::String::dynamicCast((*compositeReloaded)["key"]);

    CPPUNIT_ASSERT( core::tools::UUID::get(composite) != core::tools::UUID::get(compositeReloaded) );
    CPPUNIT_ASSERT( core::tools::UUID::get(data) != core::tools::UUID::get(dataReloaded) );

}

//-----------------------------------------------------------------------------

void ConversionTest::uuidReuseTest()
{
    // Create data
    data::Composite::sptr composite = data::Composite::New();
    data::String::sptr data         = data::String::New();
    composite->getContainer()["key"] = data;

    // Create Atom
    ::fwAtoms::Object::sptr atom = ::fwAtomConversion::convert( composite );

    data::Composite::sptr compositeReloaded;
    data::String::sptr dataReloaded;

    compositeReloaded = data::Composite::dynamicCast(
        ::fwAtomConversion::convert(atom, ::fwAtomConversion::AtomVisitor::ReusePolicy())
        );
    dataReloaded = data::String::dynamicCast((*compositeReloaded)["key"]);

    CPPUNIT_ASSERT_EQUAL( composite, compositeReloaded );
    CPPUNIT_ASSERT_EQUAL( data, dataReloaded );
}

//-----------------------------------------------------------------------------

class ClassNotCamped : public data::Object
{
public:

    fwCoreClassMacro(ClassNotCamped, data::Object, data::factory::New< ClassNotCamped >)

    ClassNotCamped(data::Object::Key)
    {
    }
    //------------------------------------------------------------------------------

    void cachedDeepCopy(const Object::csptr&, DeepCopyCacheType& ) override
    {
    }

};

//------------------------------------------------------------------------------

void ConversionTest::campFactoryNotFoundExceptionTest()
{
    ClassNotCamped::sptr obj = ClassNotCamped::New();
    CPPUNIT_ASSERT(obj);
    CPPUNIT_ASSERT_THROW( ::fwAtomConversion::convert( obj ), ::camp::ClassNotFound );
}

//-----------------------------------------------------------------------------

void ConversionTest::nullPtrManagmentTest()
{
    // null shared ptr attribute managed
    {
        ::fwAtoms::Object::sptr atom;
        {
            data::Mesh::sptr mesh = data::Mesh::New();
            CPPUNIT_ASSERT( !mesh->hasPointColors() );

            // Create Atom
            atom = ::fwAtomConversion::convert( mesh );
        }

        // null ptr must be also in atom
        CPPUNIT_ASSERT( atom->getAttributes().find("point_colors") != atom->getAttributes().end() );

        data::Mesh::sptr newMesh = data::Mesh::dynamicCast( ::fwAtomConversion::convert(atom) );
        CPPUNIT_ASSERT( newMesh );
        CPPUNIT_ASSERT( !newMesh->hasPointColors() );
    }

    // null shared ptr in map is managed
    {
        ::fwAtoms::Object::sptr atom;
        {
            data::Composite::sptr composite = data::Composite::New();
            composite->getContainer()["key1"] = data::String::New();
            composite->getContainer()["key2"] = data::Object::sptr();

            // Create Atom
            atom = ::fwAtomConversion::convert( composite );
        }

        {
            ::fwAtoms::Map::sptr map = ::fwAtoms::Map::dynamicCast( atom->getAttribute("values") );
            CPPUNIT_ASSERT_EQUAL( (size_t)2, map->size() );
            CPPUNIT_ASSERT( map->find("key2") != map->end() );
            CPPUNIT_ASSERT( !(*map)["key2"] );
        }

        data::Composite::sptr newComposite =
            data::Composite::dynamicCast( ::fwAtomConversion::convert(atom) );
        data::Composite::ContainerType& dataMap = newComposite->getContainer();
        CPPUNIT_ASSERT( newComposite );
        CPPUNIT_ASSERT_EQUAL( (size_t)2, dataMap.size() );
        CPPUNIT_ASSERT( dataMap["key1"] );
        CPPUNIT_ASSERT( dataMap.find("key2") != dataMap.end() );
        CPPUNIT_ASSERT( !dataMap["key2"] );
    }

    // null shared ptr in vector is managed
    {
        ::fwAtoms::Object::sptr atom;
        {
            data::Vector::sptr vector = data::Vector::New();
            vector->getContainer().push_back( data::String::New() );
            vector->getContainer().push_back( data::Object::sptr() );

            // Create Atom
            atom = ::fwAtomConversion::convert( vector );
        }

        {
            ::fwAtoms::Sequence::sptr seq = ::fwAtoms::Sequence::dynamicCast( atom->getAttribute("values") );
            CPPUNIT_ASSERT_EQUAL( (size_t)2, seq->size() );
            CPPUNIT_ASSERT( !(*seq)[1] );
        }

        data::Vector::sptr newVector         = data::Vector::dynamicCast( ::fwAtomConversion::convert(atom) );
        data::Vector::ContainerType& dataVec = newVector->getContainer();
        CPPUNIT_ASSERT( newVector );
        CPPUNIT_ASSERT_EQUAL( (size_t)2, dataVec.size() );
        CPPUNIT_ASSERT( dataVec[0] );
        CPPUNIT_ASSERT( !dataVec[1] );
    }
}

}  // namespace ut
}  // namespace fwAtomConversion

//-----------------------------------------------------------------------------

fwCampAutoDeclareDataMacro((fwAtomConversion)(ut)(ClassNotManaged));

namespace fwAtomConversion
{
namespace ut
{

class ClassNotManaged : public data::Object
{

public:

    fwCoreClassMacro(ClassNotManaged, data::Object, data::factory::New< ClassNotManaged >)

    fwCampMakeFriendDataMacro((fwAtomConversion)(ut)(ClassNotManaged))

    ClassNotManaged(data::Object::Key)
    {
        m_values.insert( std::make_pair( data::String::New(), 0.2 ) );
    }

    //------------------------------------------------------------------------------

    void cachedDeepCopy(const Object::csptr&, DeepCopyCacheType& ) override
    {
    }

    std::map< data::String::sptr, double > m_values;

};

}     // namespace ut
}  // namespace fwAtomConversion

fwCampImplementDataMacro((fwAtomConversion)(ut)(ClassNotManaged))
{
    builder
    .tag("object_version", "1")
    .tag("lib_name", "fwAtomConversion")
    .base< sight::data::Object>()
    .property("values", &::fwAtomConversion::ut::ClassNotManaged::m_values);
}

namespace fwAtomConversion
{
namespace ut
{

//------------------------------------------------------------------------------

void ConversionTest::conversionNotManagedExceptionTest()
{
    // Test ConversionNotManaged throwing during data to atom conversion
    {
        localDeclarefwAtomConversionutClassNotManaged();

        ClassNotManaged::sptr classNotManaged = ClassNotManaged::New();

        CPPUNIT_ASSERT_THROW( ::fwAtomConversion::convert( classNotManaged ),
                              ::fwAtomConversion::exception::ConversionNotManaged );
    }

    // Test ConversionNotManaged throwing during atom to data conversion
    {
        ::fwAtoms::Object::sptr atomObj = ::fwAtoms::Object::New();
        atomObj->setMetaInfo( DataVisitor::CLASSNAME_METAINFO, "data::Vector" );
        atomObj->setMetaInfo( DataVisitor::ID_METAINFO, core::tools::UUID::generateUUID() );

        CPPUNIT_ASSERT_THROW( ::fwAtomConversion::convert( atomObj ),
                              ::fwAtomConversion::exception::ConversionNotManaged );
    }

    // Test ConversionNotManaged throwing during atom to data conversion
    {
        ::fwAtoms::Object::sptr atomObj = ::fwAtoms::Object::New();
        atomObj->setMetaInfo( DataVisitor::CLASSNAME_METAINFO, "data::Vector");
        atomObj->setMetaInfo( DataVisitor::ID_METAINFO, core::tools::UUID::generateUUID());

        ::fwAtoms::Map::sptr atomFields = ::fwAtoms::Map::New();
        atomObj->setAttribute("fields", atomFields);

        ::fwAtoms::Sequence::sptr atomSeq = ::fwAtoms::Sequence::New();
        atomObj->setAttribute("values", atomSeq);

        ::fwAtoms::Map::sptr atomBasicMap = ::fwAtoms::Map::New();
        atomSeq->push_back( atomBasicMap );

        CPPUNIT_ASSERT_THROW( ::fwAtomConversion::convert( atomObj ),
                              ::fwAtomConversion::exception::ConversionNotManaged );
    }
}

}     // namespace ut
}  // namespace fwAtomConversion
