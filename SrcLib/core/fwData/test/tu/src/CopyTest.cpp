/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/assign/list_of.hpp>
#include <boost/assign/std/vector.hpp>

#include <fwData/Acquisition.hpp>
#include <fwData/Array.hpp>
#include <fwData/Boolean.hpp>
#include <fwData/Camera.hpp>
#include <fwData/Color.hpp>
#include <fwData/Composite.hpp>
#include <fwData/Dictionary.hpp>
#include <fwData/DictionaryOrgan.hpp>
#include <fwData/Edge.hpp>
#include <fwData/Float.hpp>
#include <fwData/Graph.hpp>
#include <fwData/Histogram.hpp>
#include <fwData/Image.hpp>
#include <fwData/Integer.hpp>
#include <fwData/Line.hpp>
#include <fwData/List.hpp>
#include <fwData/Material.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/Model.hpp>
#include <fwData/Node.hpp>
#include <fwData/None.hpp>
#include <fwData/Object.hpp>
#include <fwData/Patient.hpp>
#include <fwData/PatientDB.hpp>
#include <fwData/Pipeline.hpp>
#include <fwData/Plane.hpp>
#include <fwData/PlaneList.hpp>
#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>
#include <fwData/Port.hpp>
#include <fwData/ProcessObject.hpp>
#include <fwData/Reconstruction.hpp>
#include <fwData/ReconstructionTraits.hpp>
#include <fwData/Resection.hpp>
#include <fwData/ResectionDB.hpp>
#include <fwData/ROITraits.hpp>
#include <fwData/Spline.hpp>
#include <fwData/String.hpp>
#include <fwData/StructureTraits.hpp>
#include <fwData/StructureTraitsDictionary.hpp>
#include <fwData/Study.hpp>
#include <fwData/Tag.hpp>
#include <fwData/TransfertFunction.hpp>
#include <fwData/TransfertFunctionPoint.hpp>
#include <fwData/TransformationMatrix3D.hpp>
#include <fwData/TriangularMesh.hpp>
#include <fwData/Vector.hpp>
#include <fwData/Video.hpp>
#include <fwData/location/Folder.hpp>
#include <fwData/location/MultiFiles.hpp>
#include <fwData/location/SingleFile.hpp>

#include "CopyTest.hpp"


using namespace boost::assign;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwData::ut::CopyTest);

namespace fwData
{
namespace ut
{

//-----------------------------------------------------------------------------

void CopyTest::setUp()
{
    // Set up context before running a test.

}

//-----------------------------------------------------------------------------

void CopyTest::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

void CopyTest::fieldCopyTest()
{
    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::Acquisition);
    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::Array);
    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::Boolean);
    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::Color);
    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::Composite);
    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::Float);
    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::Graph);
    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::Histogram);
    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::Image);
    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::Integer);
    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::Line);
    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::List);
    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::location::Folder);
    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::location::SingleFile);
    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::Material);
    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::Mesh);
    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::Patient);
    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::PatientDB);
    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::Plane);
    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::PlaneList);
    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::Point);
    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::PointList);
    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::Reconstruction);
    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::Resection);
    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::ResectionDB);
    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::String);
    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::Study);
    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::TransfertFunction_VERSION_II);
    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::TransfertFunctionPoint);
    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::TransformationMatrix3D);
    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::TriangularMesh);
    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::Vector);

//Not implemented !?
//    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::Camera);
//    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::Dictionary);
//    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::DictionaryOrgan);
//    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::Edge);
//    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::Model);
//    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::Node);
//    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::None);
//    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::Object);
//    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::Pipeline);
//    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::Port);
//    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::ProcessObject);
//    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::ReconstructionTraits);
//    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::ROITraits);
//    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::Spline);
//    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::StructureTraits);
//    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::StructureTraitsDictionary);
//    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::Tag);
//    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::Video);
//    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::location::MultiFiles);

}

} //namespace ut
} //namespace fwData
