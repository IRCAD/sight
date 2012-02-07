/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <cppunit/extensions/HelperMacros.h>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>

#include <fwData/Color.hpp>
#include <fwData/StructureTraits.hpp>

#include <fwDataTools/MeshGenerator.hpp>

#include "ObjectComparator.hpp"

using namespace fwData;

//------------------------------------------------------------------------------

void ObjectComparator::compareStructureTraitsDictionary(::fwData::StructureTraitsDictionary::sptr structureDico1, ::fwData::StructureTraitsDictionary::sptr structureDico2)
{
    CPPUNIT_ASSERT(structureDico1);
    CPPUNIT_ASSERT(structureDico2);
    std::vector<std::string> structureTypes1 = structureDico1->getStructureTypeNames();
    std::vector<std::string> structureTypes2 = structureDico2->getStructureTypeNames();
    CPPUNIT_ASSERT_EQUAL(structureTypes1.size(), structureTypes2.size());
    CPPUNIT_ASSERT(std::equal(structureTypes1.begin(), structureTypes1.end(), structureTypes2.begin()));

    BOOST_FOREACH(std::string type, structureTypes1)
    {
        ::fwData::StructureTraits::sptr structure1 = structureDico1->getStructure(type);
        ::fwData::StructureTraits::sptr structure2 = structureDico2->getStructure(type);
        ObjectComparator::compareStructureTraits(structure1, structure2);
    }
}

//------------------------------------------------------------------------------

void ObjectComparator::compareStructureTraits(::fwData::StructureTraits::sptr structure1, ::fwData::StructureTraits::sptr structure2)
{
    // check structure
    CPPUNIT_ASSERT(structure1);
    CPPUNIT_ASSERT(structure2);
    CPPUNIT_ASSERT_EQUAL(structure1->getType(), structure2->getType());
    CPPUNIT_ASSERT_EQUAL(structure1->getClass(), structure2->getClass());
    CPPUNIT_ASSERT_EQUAL(structure1->getNativeExp(), structure2->getNativeExp());
    CPPUNIT_ASSERT_EQUAL(structure1->getNativeGeometricExp(), structure2->getNativeGeometricExp());
    CPPUNIT_ASSERT_EQUAL(structure1->getAttachmentType(), structure2->getAttachmentType());

    ::fwData::Color::sptr color1 = structure1->getColor();
    ::fwData::Color::sptr color2 = structure2->getColor();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(color1->red(), color2->red(), 0.000001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(color1->green(), color2->green(), 0.000001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(color1->blue(), color2->blue(), 0.000001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(color1->alpha(), color2->alpha(), 0.000001);

    ::fwData::StructureTraits::CategoryContainer categories1 = structure1->getCategories();
    ::fwData::StructureTraits::CategoryContainer categories2 = structure2->getCategories();
    CPPUNIT_ASSERT_EQUAL(categories1.size(), categories2.size());
    CPPUNIT_ASSERT(std::equal(categories1.begin(), categories1.end(), categories2.begin()));
}

//------------------------------------------------------------------------------

void ObjectComparator::compareROITraits(::fwData::ROITraits::sptr roi1, ::fwData::ROITraits::sptr roi2)
{
    CPPUNIT_ASSERT(roi1);
    CPPUNIT_ASSERT(roi2);
    CPPUNIT_ASSERT_EQUAL(roi1->getIdentifier(), roi2->getIdentifier());
    CPPUNIT_ASSERT_EQUAL(roi1->getEvaluatedExp(), roi2->getEvaluatedExp());
    ObjectComparator::compareStructureTraits(roi1->getStructureTraits(), roi2->getStructureTraits());

    CPPUNIT_ASSERT((roi1->getMaskOpNode() && roi2->getMaskOpNode()) || (!roi1->getMaskOpNode() && !roi2->getMaskOpNode()));
    if (roi1->getMaskOpNode() && roi2->getMaskOpNode())
    {
        ObjectComparator::compareNode(roi1->getMaskOpNode(), roi2->getMaskOpNode());
    }

}

//------------------------------------------------------------------------------

void ObjectComparator::compareReconstructionTraits(::fwData::ReconstructionTraits::sptr rec1, ::fwData::ReconstructionTraits::sptr rec2)
{
    CPPUNIT_ASSERT(rec1);
    CPPUNIT_ASSERT(rec2);
    CPPUNIT_ASSERT_EQUAL(rec1->getIdentifier(), rec2->getIdentifier());
    ObjectComparator::compareStructureTraits(rec1->getStructureTraits(), rec2->getStructureTraits());

    CPPUNIT_ASSERT((rec1->getMaskOpNode() && rec2->getMaskOpNode()) || (!rec1->getMaskOpNode() && !rec2->getMaskOpNode()));
    if (rec1->getMaskOpNode() && rec2->getMaskOpNode())
    {
        ObjectComparator::compareNode(rec1->getMaskOpNode(), rec2->getMaskOpNode());
    }

    CPPUNIT_ASSERT((rec1->getMeshOpNode() && rec2->getMeshOpNode()) || (!rec1->getMeshOpNode() && !rec2->getMeshOpNode()));
    if (rec1->getMeshOpNode() && rec2->getMeshOpNode())
    {
        ObjectComparator::compareNode(rec1->getMeshOpNode(), rec2->getMeshOpNode());
    }
}

//------------------------------------------------------------------------------

void ObjectComparator::compareNode(::fwData::Node::sptr node1, ::fwData::Node::sptr node2)
{
    CPPUNIT_ASSERT(node1);
    CPPUNIT_ASSERT(node2);
    CPPUNIT_ASSERT(node1->getObject() == node2->getObject());

    ::fwData::Node::PortContainer inputs1 = node1->getInputPorts();
    ::fwData::Node::PortContainer inputs2 = node2->getInputPorts();
    CPPUNIT_ASSERT_EQUAL(inputs1.size(), inputs2.size());
    ::fwData::Node::PortContainer::iterator inputsIter1 = inputs1.begin();
    ::fwData::Node::PortContainer::iterator inputsIter2 = inputs2.begin();
    for (;inputsIter1 != inputs1.end(); ++inputsIter1, ++inputsIter2)
    {
        ObjectComparator::comparePort(*inputsIter1, *inputsIter2);
    }

    ::fwData::Node::PortContainer outputs1 = node1->getOutputPorts();
    ::fwData::Node::PortContainer outputs2 = node2->getOutputPorts();
    CPPUNIT_ASSERT_EQUAL(outputs1.size(), outputs2.size());
    ::fwData::Node::PortContainer::iterator outputsIter1 = outputs1.begin();
    ::fwData::Node::PortContainer::iterator outputsIter2 = outputs2.begin();
    for (;outputsIter1 != outputs1.end(); ++outputsIter1, ++outputsIter2)
    {
        ObjectComparator::comparePort(*outputsIter1, *outputsIter2);
    }
}

//------------------------------------------------------------------------------

void ObjectComparator::comparePort(::fwData::Port::sptr port1, ::fwData::Port::sptr port2)
{
    CPPUNIT_ASSERT(port1);
    CPPUNIT_ASSERT(port2);
    CPPUNIT_ASSERT_EQUAL(port1->getIdentifier(), port2->getIdentifier());
    CPPUNIT_ASSERT_EQUAL(port1->getType(), port2->getType());
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void ObjectComparator::comparePatientDB(::fwData::PatientDB::sptr patientDB1, ::fwData::PatientDB::sptr patientDB2)
{
    // check patientDB
    CPPUNIT_ASSERT(patientDB1);
    CPPUNIT_ASSERT(patientDB2);
    CPPUNIT_ASSERT_EQUAL(patientDB1->getPatientSize(), patientDB2->getPatientSize());

    std::pair< PatientDB::PatientIterator, PatientDB::PatientIterator > pairPatientDB1 = patientDB1->getPatients();
    PatientDB::PatientIterator iter1 = pairPatientDB1.first;
    PatientDB::PatientIterator iter2 = patientDB2->getPatients().first;
    while( iter1 != pairPatientDB1.second )
    {
        ObjectComparator::comparePatient(*iter1, *iter2);
        iter1++;
        iter2++;
    }
}

//------------------------------------------------------------------------------

void ObjectComparator::comparePatient(::fwData::Patient::sptr patient1, ::fwData::Patient::sptr patient2)
{
    // check patient
    CPPUNIT_ASSERT(patient1);
    CPPUNIT_ASSERT(patient2);

    CPPUNIT_ASSERT_EQUAL(patient1->getName(), patient2->getName());
    CPPUNIT_ASSERT_EQUAL(patient1->getFirstname(), patient2->getFirstname());
    CPPUNIT_ASSERT_EQUAL(patient1->getBirthdate(), patient2->getBirthdate());
    CPPUNIT_ASSERT_EQUAL(patient1->getIDDicom(), patient2->getIDDicom());
    CPPUNIT_ASSERT_EQUAL(patient1->getIsMale(), patient2->getIsMale());
    CPPUNIT_ASSERT_EQUAL(patient1->getDbID(), patient2->getDbID());

    CPPUNIT_ASSERT_EQUAL(patient1->getStudySize(), patient2->getStudySize());

    std::pair< Patient::StudyIterator, Patient::StudyIterator > pairPatient1 = patient1->getStudies();
    Patient::StudyIterator iter1 = pairPatient1.first;
    Patient::StudyIterator iter2 = patient2->getStudies().first;
    while( iter1 != pairPatient1.second )
    {
        ObjectComparator::compareStudy(*iter1, *iter2);
        iter1++;
        iter2++;
    }
}

//------------------------------------------------------------------------------

void ObjectComparator::compareStudy(::fwData::Study::sptr study1, ::fwData::Study::sptr study2)
{
    // check Study
    CPPUNIT_ASSERT(study1);
    CPPUNIT_ASSERT(study2);
    CPPUNIT_ASSERT_EQUAL(study1->getHospital(), study2->getHospital());
    CPPUNIT_ASSERT_EQUAL(study1->getModality(), study2->getModality());
    CPPUNIT_ASSERT_EQUAL(study1->getAcquisitionZone(), study2->getAcquisitionZone());
    CPPUNIT_ASSERT_EQUAL(study1->getRefRISId(), study2->getRefRISId());
    CPPUNIT_ASSERT_EQUAL(study1->getUID(), study2->getUID());
    CPPUNIT_ASSERT_EQUAL(study1->getDbID(), study2->getDbID());
    CPPUNIT_ASSERT_EQUAL(study1->getAcquisitionSize(), study2->getAcquisitionSize());

    std::pair< Study::AcquisitionIterator, Study::AcquisitionIterator > pairStudy1 = study1->getAcquisitions();
    Study::AcquisitionIterator iter1 = pairStudy1.first;
    Study::AcquisitionIterator iter2 = study2->getAcquisitions().first;
    while( iter1 != pairStudy1.second )
    {
        ObjectComparator::compareAcquisition(*iter1, *iter2);
        iter1++;
        iter2++;
    }
}

//------------------------------------------------------------------------------

void ObjectComparator::compareAcquisition(::fwData::Acquisition::sptr acquisition1, ::fwData::Acquisition::sptr acquisition2)
{
    CPPUNIT_ASSERT(acquisition1);
    CPPUNIT_ASSERT(acquisition2);
    CPPUNIT_ASSERT_EQUAL(acquisition1->getBitsPerPixel(), acquisition2->getBitsPerPixel());
    CPPUNIT_ASSERT_EQUAL(acquisition1->getSliceThickness(), acquisition2->getSliceThickness());
    CPPUNIT_ASSERT_EQUAL(acquisition1->getAxe(), acquisition2->getAxe());
    CPPUNIT_ASSERT_EQUAL(acquisition1->getUnsignedFlag(), acquisition2->getUnsignedFlag());
    CPPUNIT_ASSERT_EQUAL(acquisition1->getAcquisitionIndex(), acquisition2->getAcquisitionIndex());
    CPPUNIT_ASSERT_EQUAL(acquisition1->getImageType(), acquisition2->getImageType());
    CPPUNIT_ASSERT_EQUAL(acquisition1->getImageFormat(), acquisition2->getImageFormat());
    CPPUNIT_ASSERT_EQUAL(acquisition1->getIsMain(), acquisition2->getIsMain());
    CPPUNIT_ASSERT_EQUAL(acquisition1->getIsNormalDir(), acquisition2->getIsNormalDir());
    CPPUNIT_ASSERT_EQUAL(acquisition1->getUID(), acquisition2->getUID());
    CPPUNIT_ASSERT_EQUAL(acquisition1->getDbID(), acquisition2->getDbID());
    CPPUNIT_ASSERT_EQUAL(acquisition1->getLaboID(), acquisition2->getLaboID());
    CPPUNIT_ASSERT_EQUAL(acquisition1->getNetID(), acquisition2->getNetID());
    CPPUNIT_ASSERT_EQUAL(acquisition1->getDateSendToLaboAt(), acquisition2->getDateSendToLaboAt());
    CPPUNIT_ASSERT_EQUAL(acquisition1->getDateReceiveFromLaboAt(), acquisition2->getDateReceiveFromLaboAt());
    CPPUNIT_ASSERT_EQUAL(acquisition1->getDateSendToBDDAt(), acquisition2->getDateSendToBDDAt());
    CPPUNIT_ASSERT_EQUAL(acquisition1->getDateDisponibilityAt(), acquisition2->getDateDisponibilityAt());
    CPPUNIT_ASSERT_EQUAL(acquisition1->getPatientSize(), acquisition2->getPatientSize());
    CPPUNIT_ASSERT_EQUAL(acquisition1->getPatientWeight(), acquisition2->getPatientWeight());
    CPPUNIT_ASSERT_EQUAL(acquisition1->getRadiations(), acquisition2->getRadiations());
    CPPUNIT_ASSERT_EQUAL(acquisition1->getMedicalPrinter(), acquisition2->getMedicalPrinter());
    CPPUNIT_ASSERT_EQUAL(acquisition1->getMedicalPrinterCorp(), acquisition2->getMedicalPrinterCorp());
    CPPUNIT_ASSERT_EQUAL(acquisition1->getPatientPosition(), acquisition2->getPatientPosition());

    CPPUNIT_ASSERT_EQUAL(acquisition1->getReconstructionSize(), acquisition2->getReconstructionSize());

    std::pair< Acquisition::ReconstructionIterator, Acquisition::ReconstructionIterator > pairAcquisition1 = acquisition1->getReconstructions();
    Acquisition::ReconstructionIterator iter1 = pairAcquisition1.first;
    Acquisition::ReconstructionIterator iter2 = acquisition2->getReconstructions().first;
    while( iter1 != pairAcquisition1.second )
    {
        ObjectComparator::compareReconstruction(*iter1, *iter2);
        iter1++;
        iter2++;
    }

    ObjectComparator::compareImage(acquisition1->getImage(), acquisition2->getImage());
}

//------------------------------------------------------------------------------

void ObjectComparator::compareReconstruction(::fwData::Reconstruction::sptr reconstruction1, ::fwData::Reconstruction::sptr reconstruction2)
{
    CPPUNIT_ASSERT(reconstruction1);
    CPPUNIT_ASSERT(reconstruction2);
    CPPUNIT_ASSERT_EQUAL(reconstruction1->getIsVisible(), reconstruction2->getIsVisible());
    CPPUNIT_ASSERT_EQUAL(reconstruction1->getOrganName(), reconstruction2->getOrganName());
    CPPUNIT_ASSERT_EQUAL(reconstruction1->getStructureType(), reconstruction2->getStructureType());
    CPPUNIT_ASSERT_EQUAL(reconstruction1->getIsClosed(), reconstruction2->getIsClosed());
    CPPUNIT_ASSERT_EQUAL(reconstruction1->getIsAutomatic(), reconstruction2->getIsAutomatic());
    CPPUNIT_ASSERT_EQUAL(reconstruction1->getAvgVolume(), reconstruction2->getAvgVolume());
    CPPUNIT_ASSERT_EQUAL(reconstruction1->getVolStdDeviation(), reconstruction2->getVolStdDeviation());
    CPPUNIT_ASSERT_EQUAL(reconstruction1->getVolPctConfidence(), reconstruction2->getVolPctConfidence());
    CPPUNIT_ASSERT_EQUAL(reconstruction1->getReconstructionTime(), reconstruction2->getReconstructionTime());
    CPPUNIT_ASSERT_EQUAL(reconstruction1->getMaskGenerated(), reconstruction2->getMaskGenerated());
    CPPUNIT_ASSERT_EQUAL(reconstruction1->getLevel(), reconstruction2->getLevel());
    CPPUNIT_ASSERT_EQUAL(reconstruction1->getLabel(), reconstruction2->getLabel());
    CPPUNIT_ASSERT_EQUAL(reconstruction1->getGenerated3D(), reconstruction2->getGenerated3D());
    CPPUNIT_ASSERT_EQUAL(reconstruction1->getType3D(), reconstruction2->getType3D());
    CPPUNIT_ASSERT_EQUAL(reconstruction1->getPath(), reconstruction2->getPath());
    CPPUNIT_ASSERT_EQUAL(reconstruction1->getDbID(), reconstruction2->getDbID());

    ObjectComparator::compareImage(reconstruction1->getImage(), reconstruction2->getImage());
    ObjectComparator::compareMaterial(reconstruction1->getMaterial(), reconstruction2->getMaterial());
    ObjectComparator::compareMesh(reconstruction1->getMesh(), reconstruction2->getMesh());
}

//------------------------------------------------------------------------------

void ObjectComparator::compareMaterial(::fwData::Material::sptr mat1, ::fwData::Material::sptr mat2)
{
    CPPUNIT_ASSERT(mat1);
    CPPUNIT_ASSERT(mat2);
    CPPUNIT_ASSERT_EQUAL(mat1->getShadingMode(), mat1->getShadingMode());
    CPPUNIT_ASSERT_EQUAL(mat1->getRepresentationMode(), mat1->getRepresentationMode());
    CPPUNIT_ASSERT_EQUAL(mat1->getOptionsMode(), mat1->getOptionsMode());

    ObjectComparator::compareColor(mat1->ambient(), mat2->ambient());
    ObjectComparator::compareColor(mat1->diffuse(), mat2->diffuse());
}


//------------------------------------------------------------------------------

void ObjectComparator::compareColor(::fwData::Color::sptr col1, ::fwData::Color::sptr col2)
{
    CPPUNIT_ASSERT(col1);
    CPPUNIT_ASSERT(col2);

    CPPUNIT_ASSERT_EQUAL(col1->red(), col2->red());
    CPPUNIT_ASSERT_EQUAL(col1->green(), col2->green());
    CPPUNIT_ASSERT_EQUAL(col1->blue(), col2->blue());
    CPPUNIT_ASSERT_EQUAL(col1->alpha(), col2->alpha());
}

//------------------------------------------------------------------------------

void ObjectComparator::compareTriangularMesh(::fwData::TriangularMesh::sptr trian1, ::fwData::TriangularMesh::sptr trian2)
{
    CPPUNIT_ASSERT(trian1);
    CPPUNIT_ASSERT(trian2);

    CPPUNIT_ASSERT_EQUAL(trian1->cells().size(), trian2->cells().size());
    CPPUNIT_ASSERT_EQUAL(trian1->points().size(), trian2->points().size());
    CPPUNIT_ASSERT(trian1->cells() == trian2->cells());
    CPPUNIT_ASSERT(trian1->points() == trian2->points());
}

//------------------------------------------------------------------------------

void ObjectComparator::compareTransfertFunction(::fwData::TransfertFunction::sptr tf1, ::fwData::TransfertFunction::sptr tf2)
{
    CPPUNIT_ASSERT(tf1);
    CPPUNIT_ASSERT(tf2);

    CPPUNIT_ASSERT_EQUAL(tf1->getTransfertFunctionPointSize(), tf2->getTransfertFunctionPointSize());
    CPPUNIT_ASSERT_EQUAL(tf1->getEncoding(), tf2->getEncoding());
    CPPUNIT_ASSERT_EQUAL(tf1->getName(), tf2->getName());
    CPPUNIT_ASSERT(tf1->getCenterWidth() == tf2->getCenterWidth());
    CPPUNIT_ASSERT(tf1->getMinMax() == tf2->getMinMax());

    std::pair< TransfertFunction::TransfertFunctionPointIterator, TransfertFunction::TransfertFunctionPointIterator > pairTransfertFunction1 = tf1->getTransfertFunctionPoints();
    TransfertFunction::TransfertFunctionPointIterator iter1 = pairTransfertFunction1.first;
    TransfertFunction::TransfertFunctionPointIterator iter2 = tf2->getTransfertFunctionPoints().first;
    while( iter1 != pairTransfertFunction1.second )
    {
        ObjectComparator::compareTransfertFunctionPoint(*iter1, *iter2);
        iter1++;
        iter2++;
    }
}

//------------------------------------------------------------------------------

void ObjectComparator::compareTransfertFunctionPoint(::fwData::TransfertFunctionPoint::sptr tfp1, ::fwData::TransfertFunctionPoint::sptr tfp2)
{
    CPPUNIT_ASSERT(tfp1);
    CPPUNIT_ASSERT(tfp2);
    CPPUNIT_ASSERT_EQUAL(tfp1->getValue(), tfp1->getValue());

    ObjectComparator::compareColor(tfp1->getColor(), tfp2->getColor());
}

//------------------------------------------------------------------------------

void ObjectComparator::compareImage(::fwData::Image::sptr image1, ::fwData::Image::sptr image2)
{
    CPPUNIT_ASSERT(image1);
    CPPUNIT_ASSERT(image2);

    CPPUNIT_ASSERT_EQUAL(image1->getDimension(), image2->getDimension());
    CPPUNIT_ASSERT_EQUAL(image1->getManagesBuff(), image2->getManagesBuff());
    CPPUNIT_ASSERT_EQUAL(image1->getSpacing().back(), image2->getSpacing().back());
    CPPUNIT_ASSERT_EQUAL(image1->getOrigin().back(), image2->getOrigin().back());
    CPPUNIT_ASSERT_EQUAL(image1->getSize().back(), image2->getSize().back());
    CPPUNIT_ASSERT_EQUAL(image1->getFilename(), image2->getFilename());
    CPPUNIT_ASSERT_EQUAL(image1->getWindowCenter(), image2->getWindowCenter());
    CPPUNIT_ASSERT_EQUAL(image1->getWindowWidth(), image2->getWindowWidth());
    CPPUNIT_ASSERT_EQUAL(image1->getRescaleIntercept(), image2->getRescaleIntercept());
}

//------------------------------------------------------------------------------

void ObjectComparator::compareMesh(::fwData::Mesh::sptr mesh1, ::fwData::Mesh::sptr mesh2)
{
    CPPUNIT_ASSERT_EQUAL(mesh1->getNumberOfPoints(), mesh2->getNumberOfPoints());
    CPPUNIT_ASSERT_EQUAL(mesh1->getNumberOfCells() , mesh2->getNumberOfCells());
    CPPUNIT_ASSERT_EQUAL(mesh1->getCellDataSize()  , mesh2->getCellDataSize());

    ObjectComparator::compareArray(mesh1->getPointsArray(), mesh2->getPointsArray());
    ObjectComparator::compareArray(mesh1->getCellTypesArray(), mesh2->getCellTypesArray());
    ObjectComparator::compareArray(mesh1->getCellDataOffsetsArray(), mesh2->getCellDataOffsetsArray());
    ObjectComparator::compareArray(mesh1->getCellDataArray(), mesh2->getCellDataArray());
    ObjectComparator::compareArray(mesh1->getPointColorsArray(), mesh2->getPointColorsArray());
    ObjectComparator::compareArray(mesh1->getCellColorsArray(), mesh2->getCellColorsArray());
    ObjectComparator::compareArray(mesh1->getPointNormalsArray(), mesh2->getPointNormalsArray());
    ObjectComparator::compareArray(mesh1->getCellNormalsArray(), mesh2->getCellNormalsArray());

    ObjectComparator::compareDataArrayMesh(mesh1, mesh2);
}
//------------------------------------------------------------------------------

void ObjectComparator::compareDataArrayMesh(::fwData::Mesh::sptr mesh1, ::fwData::Mesh::sptr mesh2)
{
    CPPUNIT_ASSERT( mesh1->getDataArrayNames() == mesh2->getDataArrayNames());

    std::vector<std::string> vectNames = mesh1->getDataArrayNames();
    BOOST_FOREACH(std::string name, vectNames)
    {
        ::fwData::Array::sptr array1 = mesh1->getDataArray(name);
        ::fwData::Array::sptr array2 = mesh2->getDataArray(name);
        ObjectComparator::compareArray(array1, array2);
    }
}

//------------------------------------------------------------------------------

void ObjectComparator::compareArray(::fwData::Array::sptr array1, ::fwData::Array::sptr array2)
{
    CPPUNIT_ASSERT( (!array1 && !array2) || (array1 && array2));

    CPPUNIT_ASSERT_EQUAL(array1->getSizeInBytes(), array2->getSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(array1->getElementSizeInBytes(), array2->getElementSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(array1->getNumberOfDimensions(), array2->getNumberOfDimensions());
    CPPUNIT_ASSERT(array1->getSize() == array2->getSize());
    CPPUNIT_ASSERT_EQUAL(array1->getNumberOfComponents(), array2->getNumberOfComponents());
    CPPUNIT_ASSERT_EQUAL(array1->getIsBufferOwner(), array2->getIsBufferOwner());
    CPPUNIT_ASSERT(array1->getStrides() == array2->getStrides());
    CPPUNIT_ASSERT(array1->getType().string() == array2->getType().string());
    CPPUNIT_ASSERT(array1->getType().sizeOf() == array2->getType().sizeOf());

    if(array1)
    {
        CPPUNIT_ASSERT(array1->getSize() == array2->getSize());

        char *iter1 = array1->begin<char>();
        char *iter2 = array2->begin<char>();

        for (; iter1 != array1->end<char>() ; ++iter1, ++iter2)
        {
            CPPUNIT_ASSERT_EQUAL(*iter1, *iter2);
        }
    }
}

//------------------------------------------------------------------------------
