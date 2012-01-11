/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWXML_TEST_TU_OBJECT_COMPARATOR_HPP_
#define _FWXML_TEST_TU_OBJECT_COMPARATOR_HPP_

#include <fwData/PatientDB.hpp>
#include <fwData/Patient.hpp>
#include <fwData/Study.hpp>
#include <fwData/Acquisition.hpp>
#include <fwData/Reconstruction.hpp>
#include <fwData/Image.hpp>
#include <fwData/Array.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/Material.hpp>
#include <fwData/TriangularMesh.hpp>
#include <fwData/Color.hpp>
#include <fwData/TransfertFunction.hpp>
#include <fwData/TransfertFunctionPoint.hpp>
#include <fwData/StructureTraitsDictionary.hpp>
#include <fwData/StructureTraits.hpp>
#include <fwData/ROITraits.hpp>
#include <fwData/ReconstructionTraits.hpp>
#include <fwData/Node.hpp>
#include <fwData/Port.hpp>

class ObjectComparator
{
public:
    static void comparePatientDB(::fwData::PatientDB::sptr patientDB1, ::fwData::PatientDB::sptr patientDB2);
    static void comparePatient(::fwData::Patient::sptr patient1, ::fwData::Patient::sptr patient2);
    static void compareStudy(::fwData::Study::sptr study1, ::fwData::Study::sptr study2);
    static void compareAcquisition(::fwData::Acquisition::sptr acquisition1, ::fwData::Acquisition::sptr acquisition2);
    static void compareReconstruction(::fwData::Reconstruction::sptr reconstruction1, ::fwData::Reconstruction::sptr reconstruction2);
    static void compareImage(::fwData::Image::sptr image1, ::fwData::Image::sptr image2);
    static void compareMesh(::fwData::Mesh::sptr mesh1, ::fwData::Mesh::sptr mesh2);
    static void compareArray(::fwData::Array::sptr buff1, ::fwData::Array::sptr buff2);
    static void compareDataArrayMesh(::fwData::Mesh::sptr mesh1, ::fwData::Mesh::sptr mesh2);
    static void compareMaterial(::fwData::Material::sptr mat1, ::fwData::Material::sptr mat2);
    static void compareTriangularMesh(::fwData::TriangularMesh::sptr trian1, ::fwData::TriangularMesh::sptr trian2);
    static void compareColor(::fwData::Color::sptr col1, ::fwData::Color::sptr col2);
    static void compareTransfertFunction(::fwData::TransfertFunction::sptr tf1, ::fwData::TransfertFunction::sptr tf2);
    static void compareTransfertFunctionPoint(::fwData::TransfertFunctionPoint::sptr tfp1, ::fwData::TransfertFunctionPoint::sptr tfp2);
    static void compareStructureTraitsDictionary(::fwData::StructureTraitsDictionary::sptr structureDico1, ::fwData::StructureTraitsDictionary::sptr structureDico2);
    static void compareStructureTraits(::fwData::StructureTraits::sptr structure1, ::fwData::StructureTraits::sptr structure2);
    static void compareROITraits(::fwData::ROITraits::sptr roi1, ::fwData::ROITraits::sptr roi2);
    static void compareReconstructionTraits(::fwData::ReconstructionTraits::sptr rec1, ::fwData::ReconstructionTraits::sptr rec2);
    static void compareNode(::fwData::Node::sptr node1, ::fwData::Node::sptr node2);
    static void comparePort(::fwData::Port::sptr port1, ::fwData::Port::sptr port2);

};

#endif // _FWXML_TEST_TU_OBJECT_COMPARATOR_HPP_
