/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATATOOLS_OBJECT_GENERATOR_HPP_
#define _FWDATATOOLS_OBJECT_GENERATOR_HPP_

#include <fwCore/base.hpp>

#include <fwData/StructureTraits.hpp>
#include <fwData/ROITraits.hpp>
#include <fwData/ReconstructionTraits.hpp>
#include <fwData/StructureTraitsDictionary.hpp>
#include <fwData/Node.hpp>
#include <fwData/Port.hpp>
#include <fwData/Composite.hpp>
#include <fwData/PatientDB.hpp>
#include <fwData/Array.hpp>
#include <fwData/Material.hpp>
#include <fwData/Color.hpp>
#include <fwData/TransfertFunction.hpp>
#include <fwData/Dictionary.hpp>
#include <fwData/DictionaryOrgan.hpp>

#include "fwDataTools/export.hpp"

namespace fwDataTools
{

class ObjectGenerator
{
public:
    FWDATATOOLS_API static ::fwData::PatientDB::sptr createPatientDB(const unsigned char nbPatient,
                                                     const unsigned char nbStudy,
                                                     const unsigned char nbAcquisition,
                                                     const unsigned char nbReconstruction);
    FWDATATOOLS_API static ::fwData::Array::sptr randomizeArray(const std::string type, fwData::Array::SizeType sizes);
    FWDATATOOLS_API static ::fwData::Color::sptr randomizeColor();
    FWDATATOOLS_API static ::fwData::TransferFunction::sptr createTFColor(unsigned char nbPoints, double window, double level );
    FWDATATOOLS_API static ::fwData::TransferFunction::sptr createTFColor();
    FWDATATOOLS_API static ::fwData::StructureTraitsDictionary::sptr createStructureTraitsDictionary();
    FWDATATOOLS_API static ::fwData::StructureTraits::sptr createStructureTraits();

    /// Return a composite containing the ROITraits and a reference on its StructureTraits and mask node (because of weak ptr)
    FWDATATOOLS_API static ::fwData::Composite::sptr createROITraits();

    /// Return a composite containing the ReconstructionTraits and a reference on its StructureTraits and mask/mesh node (because of weak ptr)
    FWDATATOOLS_API static ::fwData::Composite::sptr createReconstructionTraits();
    FWDATATOOLS_API static ::fwData::Node::sptr createNode();
    FWDATATOOLS_API static ::fwData::Port::sptr createPort();
    FWDATATOOLS_API static ::fwData::Material::sptr createMaterial( );
    FWDATATOOLS_API static ::fwData::ProcessObject::sptr createProcessObject();
    FWDATATOOLS_API static ::fwData::ResectionDB::sptr generateResectionDB();
    FWDATATOOLS_API static ::fwData::Resection::sptr generateResection();
    FWDATATOOLS_API static ::fwData::Point::sptr generatePoint();
    FWDATATOOLS_API static ::fwData::Plane::sptr generatePlane();
    FWDATATOOLS_API static ::fwData::Dictionary::sptr createDictionary();
    FWDATATOOLS_API static ::fwData::DictionaryOrgan::sptr createDictionaryOrgan();

};

} // namespace fwDataTools

#endif // _FWDATATOOLS_OBJECT_GENERATOR_HPP_
