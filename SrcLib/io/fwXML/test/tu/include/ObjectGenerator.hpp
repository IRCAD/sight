/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWXML_TEST_TU_OBJECT_GENERATOR_HPP_
#define _FWXML_TEST_TU_OBJECT_GENERATOR_HPP_

#include <vector>

#include <fwTools/Object.hpp>

#include <fwData/StructureTraits.hpp>
#include <fwData/ROITraits.hpp>
#include <fwData/ReconstructionTraits.hpp>
#include <fwData/StructureTraitsDictionary.hpp>
#include <fwData/Node.hpp>
#include <fwData/Port.hpp>
#include <fwData/Composite.hpp>
#include <fwData/PatientDB.hpp>
#include <fwData/Patient.hpp>
#include <fwData/Study.hpp>
#include <fwData/Acquisition.hpp>
#include <fwData/Reconstruction.hpp>
#include <fwData/Image.hpp>
#include <fwData/Array.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/Material.hpp>
#include <fwData/Color.hpp>
#include <fwData/TriangularMesh.hpp>
#include <fwData/TransfertFunction.hpp>

class ObjectGenerator
{
public:
    static ::fwData::PatientDB::sptr createPatientDB();
    static ::fwData::Patient::sptr createPatient();
    static ::fwData::Study::sptr createStudy();
    static ::fwData::Acquisition::sptr createAcquisition();
    static ::fwData::Reconstruction::sptr createReconstruction();
    static ::fwData::Image::sptr createImage();
    static ::fwData::Array::sptr createArray();
    static ::fwData::Mesh::sptr createMesh();
    static ::fwData::Material::sptr createMaterial();
    static ::fwData::Color::sptr createColor();
    static ::fwData::TriangularMesh::sptr createTriangularMesh();
    static ::fwData::TransfertFunction::sptr createTransfertFunction();
    static ::fwData::StructureTraitsDictionary::sptr createStructureTraitsDictionary();
    static ::fwData::StructureTraits::sptr createStructureTraits();

    /// Return a composite containing the ROITraits and a reference on its StructureTraits and mask node (because of weak ptr)
    static ::fwData::Composite::sptr createROITraits();

    /// Return a composite containing the ReconstructionTraits and a reference on its StructureTraits and mask/mesh node (because of weak ptr)
    static ::fwData::Composite::sptr createReconstructionTraits();
    static ::fwData::Node::sptr createNode();
    static ::fwData::Port::sptr createPort();

};

#endif // _FWXML_TEST_TU_OBJECT_GENERATOR_HPP_
