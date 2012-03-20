/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATATOOLS_OBJECT_COMPARATOR_HPP_
#define _FWDATATOOLS_OBJECT_COMPARATOR_HPP_

#include <fwCore/base.hpp>

#include <fwData/PatientDB.hpp>
#include <fwData/Array.hpp>
#include <fwData/Material.hpp>
#include <fwData/Color.hpp>
#include <fwData/TransfertFunction.hpp>
#include <fwData/StructureTraitsDictionary.hpp>
#include <fwData/StructureTraits.hpp>
#include <fwData/ROITraits.hpp>
#include <fwData/ReconstructionTraits.hpp>
#include <fwData/Node.hpp>
#include <fwData/Port.hpp>


#include "fwDataTools/export.hpp"

namespace fwDataTools
{

class ObjectComparator
{
public:
    FWDATATOOLS_API static bool comparePatientDB(::fwData::PatientDB::sptr patientDB1, ::fwData::PatientDB::sptr patientDB2);
    FWDATATOOLS_API static bool compareMesh(::fwData::Mesh::sptr mesh1, ::fwData::Mesh::sptr mesh2);
    FWDATATOOLS_API static bool compareArray(::fwData::Array::sptr buff1, ::fwData::Array::sptr buff2);
    FWDATATOOLS_API static bool compareDataArrayMesh(::fwData::Mesh::sptr mesh1, ::fwData::Mesh::sptr mesh2);
    FWDATATOOLS_API static bool compareMaterial(::fwData::Material::sptr mat1, ::fwData::Material::sptr mat2);
    FWDATATOOLS_API static bool compareTransfertFunction(::fwData::TransfertFunction_VERSION_II::sptr tf1, ::fwData::TransfertFunction_VERSION_II::sptr tf2);
    FWDATATOOLS_API static bool compareStructureTraitsDictionary(::fwData::StructureTraitsDictionary::sptr structureDico1, ::fwData::StructureTraitsDictionary::sptr structureDico2);
    FWDATATOOLS_API static bool compareStructureTraits(::fwData::StructureTraits::sptr structure1, ::fwData::StructureTraits::sptr structure2);
    FWDATATOOLS_API static bool compareROITraits(::fwData::ROITraits::sptr roi1, ::fwData::ROITraits::sptr roi2);
    FWDATATOOLS_API static bool compareReconstructionTraits(::fwData::ReconstructionTraits::sptr rec1, ::fwData::ReconstructionTraits::sptr rec2);
    FWDATATOOLS_API static bool compareNode(::fwData::Node::sptr node1, ::fwData::Node::sptr node2);
    FWDATATOOLS_API static bool comparePort(::fwData::Port::sptr port1, ::fwData::Port::sptr port2);
    FWDATATOOLS_API static bool compareDictionary(::fwData::Dictionary::sptr dico1, ::fwData::Dictionary::sptr dico2);
    FWDATATOOLS_API static bool compareProcessObject(::fwData::ProcessObject::sptr po1, ::fwData::ProcessObject::sptr po2);
    FWDATATOOLS_API static bool comparePoint(::fwData::Point::sptr point1, ::fwData::Point::sptr point2);
    FWDATATOOLS_API static bool comparePlane(::fwData::Plane::sptr plane1, ::fwData::Plane::sptr plane2);
    FWDATATOOLS_API static bool comparePlaneList(::fwData::PlaneList::sptr planeList1, ::fwData::PlaneList::sptr planeList2);
    FWDATATOOLS_API static bool compareResection(::fwData::Resection::sptr resec1, ::fwData::Resection::sptr resec2);
    FWDATATOOLS_API static bool compareResectionDB(::fwData::ResectionDB::sptr resec1, ::fwData::ResectionDB::sptr resec2);

};

} // namespace fwdataTools

#endif // _FWDATATOOLS_OBJECT_COMPARATOR_HPP_
