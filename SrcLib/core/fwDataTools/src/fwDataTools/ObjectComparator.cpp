/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>

#include <fwData/Color.hpp>
#include <fwData/StructureTraits.hpp>

#include <fwDataTools/MeshGenerator.hpp>
#include <fwDataTools/Image.hpp>
#include <fwDataTools/Patient.hpp>

#include "fwDataTools/ObjectComparator.hpp"

namespace fwDataTools
{

bool ObjectComparator::compareStructureTraitsDictionary(::fwData::StructureTraitsDictionary::sptr structureDico1, ::fwData::StructureTraitsDictionary::sptr structureDico2)
{
    SLM_ASSERT( "Null structureDico1 pointers", structureDico1);
    SLM_ASSERT( "Null structureDico2 pointers", structureDico2);

    bool compare = true;
    std::vector<std::string> structureTypes1 = structureDico1->getStructureTypeNames();
    std::vector<std::string> structureTypes2 = structureDico2->getStructureTypeNames();

    if(structureTypes1.size()== structureTypes2.size())
    {
        if(std::equal(structureTypes1.begin(), structureTypes1.end(), structureTypes2.begin()))
        {
            BOOST_FOREACH(std::string type, structureTypes1)
            {
                ::fwData::StructureTraits::sptr structure1 = structureDico1->getStructure(type);
                ::fwData::StructureTraits::sptr structure2 = structureDico2->getStructure(type);
                compare &= ObjectComparator::compareStructureTraits(structure1, structure2);
            }
        }
        else
        {
            compare &=false;
            OSLM_ERROR( "Structure type names are differents" );
        }
    }
    else
    {
        compare &=false;
        OSLM_ERROR( "Structure type size are different " << structureTypes1.size() << " != " << structureTypes2.size() );
    }
    return compare;
}

//------------------------------------------------------------------------------

bool ObjectComparator::compareStructureTraits(::fwData::StructureTraits::sptr structure1, ::fwData::StructureTraits::sptr structure2)
{
    // check structure
    SLM_ASSERT( "Null structure1 pointers", structure1);
    SLM_ASSERT( "Null structure2 pointers", structure2);
    bool compare = true;

    compare &= structure1->getType() == structure2->getType();
    OSLM_ERROR_IF( "The structure type are different. ", structure1->getType() != structure2->getType());

    compare &= structure1->getClass() == structure2->getClass();
    OSLM_ERROR_IF( "The structure type are different. ", structure1->getClass() != structure2->getClass());
    compare &= structure1->getNativeExp() == structure2->getNativeExp();
    OSLM_ERROR_IF( "The native expression are different. ", structure1->getNativeExp() != structure2->getNativeExp());
    compare &= structure1->getNativeGeometricExp() == structure2->getNativeGeometricExp();
    OSLM_ERROR_IF( "The native geometric expression are different. ", structure1->getNativeGeometricExp() != structure2->getNativeGeometricExp());
    compare &= structure1->getAttachmentType() == structure2->getAttachmentType();
    OSLM_ERROR_IF( "The attachment type are different. ", structure1->getAttachmentType() != structure2->getAttachmentType());


    ::fwData::Color::sptr color1 = structure1->getColor();
    ::fwData::Color::sptr color2 = structure2->getColor();
    compare &= Patient::compareColor( color1, color2, "Color : ");

    ::fwData::StructureTraits::CategoryContainer categories1 = structure1->getCategories();
    ::fwData::StructureTraits::CategoryContainer categories2 = structure2->getCategories();
    compare &= (categories1.size() == categories1.size());
    OSLM_ERROR_IF( "The categories size are different. ", categories1.size() != categories1.size());

    if(!std::equal(categories1.begin(), categories1.end(), categories2.begin()))
    {
        compare &= false;
    }
    return compare;
}

//------------------------------------------------------------------------------

bool ObjectComparator::compareROITraits(::fwData::ROITraits::sptr roi1, ::fwData::ROITraits::sptr roi2)
{
    SLM_ASSERT( "Null roi1 pointers", roi1);
    SLM_ASSERT( "Null roi2 pointers", roi2);

    bool compare = true;

    compare &=  (roi1->getIdentifier() == roi2->getIdentifier());
    OSLM_ERROR_IF("Node object are different.", roi1->getIdentifier() != roi2->getIdentifier());

    compare &=  (roi1->getEvaluatedExp() == roi2->getEvaluatedExp());
    OSLM_ERROR_IF("Node object are different.", roi1->getEvaluatedExp() != roi2->getEvaluatedExp());
    ObjectComparator::compareStructureTraits(roi1->getStructureTraits(), roi2->getStructureTraits());

    compare &=  ((roi1->getMaskOpNode() && roi2->getMaskOpNode()) || (!roi1->getMaskOpNode() && !roi2->getMaskOpNode()));
    if (roi1->getMaskOpNode() && roi2->getMaskOpNode())
    {
        compare &= ::fwDataTools::ObjectComparator::compareNode(roi1->getMaskOpNode(), roi2->getMaskOpNode());
    }
    return compare;
}

//------------------------------------------------------------------------------

bool ObjectComparator::compareReconstructionTraits(::fwData::ReconstructionTraits::sptr rec1, ::fwData::ReconstructionTraits::sptr rec2)
{
    SLM_ASSERT( "Null rec1 pointers",rec1);
    SLM_ASSERT( "Null rec2 pointers", rec2);

    bool compare = true;

    compare &= (rec1->getIdentifier() == rec2->getIdentifier());
    OSLM_ERROR_IF("Node object are different.",rec1->getIdentifier() != rec2->getIdentifier());

    compare &= ObjectComparator::compareStructureTraits(rec1->getStructureTraits(), rec2->getStructureTraits());

    bool result = ((rec1->getMaskOpNode() && rec2->getMaskOpNode()) || (!rec1->getMaskOpNode() && !rec2->getMaskOpNode()));
    compare &= result;
    OSLM_ERROR_IF("Node object are different.", !result);

    if (rec1->getMaskOpNode() && rec2->getMaskOpNode())
    {
        compare &= ::fwDataTools::ObjectComparator::compareNode(rec1->getMaskOpNode(), rec2->getMaskOpNode());
    }

    bool result2 = ((rec1->getMeshOpNode() && rec2->getMeshOpNode()) || (!rec1->getMeshOpNode() && !rec2->getMeshOpNode()));
    compare &= result2;
    OSLM_ERROR_IF("Node object are different.", !result2);

    if (rec1->getMeshOpNode() && rec2->getMeshOpNode())
    {
        compare &= ::fwDataTools::ObjectComparator::compareNode(rec1->getMeshOpNode(), rec2->getMeshOpNode());
    }
    return compare;
}

//------------------------------------------------------------------------------

bool ObjectComparator::compareNode(::fwData::Node::sptr node1, ::fwData::Node::sptr node2)
{
    SLM_ASSERT( "Null node1 pointers", node1);
    SLM_ASSERT( "Null node2 pointers", node2);

    bool compare = true;

    compare &= (node1->getObject() == node2->getObject());
    OSLM_ERROR_IF("Node object are different.",node1->getObject() != node2->getObject());

    ::fwData::Node::PortContainer inputs1 = node1->getInputPorts();
    ::fwData::Node::PortContainer inputs2 = node2->getInputPorts();

    compare &=  (inputs1.size() == inputs2.size());
    OSLM_ERROR_IF("Number of inputs are different.",inputs1.size() != inputs2.size());

    ::fwData::Node::PortContainer::iterator inputsIter1 = inputs1.begin();
    ::fwData::Node::PortContainer::iterator inputsIter2 = inputs2.begin();
    for (;inputsIter1 != inputs1.end(); ++inputsIter1, ++inputsIter2)
    {
        compare &=  ::fwDataTools::ObjectComparator::comparePort(*inputsIter1, *inputsIter2);
    }

    ::fwData::Node::PortContainer outputs1 = node1->getOutputPorts();
    ::fwData::Node::PortContainer outputs2 = node2->getOutputPorts();

    compare &= (outputs1.size() == outputs2.size());
    OSLM_ERROR_IF("Number of outputs are different.",outputs1.size() != outputs2.size());

    ::fwData::Node::PortContainer::iterator outputsIter1 = outputs1.begin();
    ::fwData::Node::PortContainer::iterator outputsIter2 = outputs2.begin();
    for (;outputsIter1 != outputs1.end(); ++outputsIter1, ++outputsIter2)
    {
        compare &=  ::fwDataTools::ObjectComparator::comparePort(*outputsIter1, *outputsIter2);
    }
    return compare;
}

//------------------------------------------------------------------------------

bool ObjectComparator::comparePort(::fwData::Port::sptr port1, ::fwData::Port::sptr port2)
{
    SLM_ASSERT( "Null port1 pointers", port1);
    SLM_ASSERT( "Null port2 pointers", port2);

    bool compare = true;

    compare &= (port1->getIdentifier() == port2->getIdentifier());
    OSLM_ERROR_IF("Port identifier are different.",port1->getIdentifier() != port2->getIdentifier());

    compare &= (port1->getType() ==  port2->getType());
    OSLM_ERROR_IF("Port type are different.",port1->getType() !=  port2->getType());

    return compare;
}

//------------------------------------------------------------------------------

bool ObjectComparator::comparePatientDB(::fwData::PatientDB::sptr patientDB1, ::fwData::PatientDB::sptr patientDB2)
{
    // check patientDB
    SLM_ASSERT( "Null patientDB1 pointers", patientDB1);
    SLM_ASSERT( "Null patientDB2 pointers",patientDB2);

    bool compare = true;
    compare &= patientDB1->getNumberOfPatients() == patientDB2->getNumberOfPatients();
    OSLM_ERROR_IF( "The number of patient are different. ", patientDB1->getNumberOfPatients() != patientDB2->getNumberOfPatients());

    ::fwData::PatientDB::PatientContainerType patientDB1List = patientDB1->getPatients();
    ::fwData::PatientDB::PatientContainerType::const_iterator iter1 = patientDB1List.begin();
    ::fwData::PatientDB::PatientContainerType::const_iterator iter2 = patientDB2->getPatients().begin();
    while( iter1 != patientDB1List.end ())
    {
        compare &= ::fwDataTools::Patient::comparePatient(*iter1, *iter2);
        iter1++;
        iter2++;
    }
    return compare;
}

//------------------------------------------------------------------------------

bool ObjectComparator::compareMaterial(::fwData::Material::sptr mat1, ::fwData::Material::sptr mat2)
{
    SLM_ASSERT( "Null mat1 pointers", mat1);
    SLM_ASSERT( "Null mat2 pointers", mat2);

    bool compare = true;

    compare &= (mat1->getShadingMode() ==  mat2->getShadingMode());
    OSLM_ERROR_IF( "Shading mode are different. ", mat1->getShadingMode() !=  mat2->getShadingMode());

    compare &= (mat1->getRepresentationMode() == mat2->getRepresentationMode());
    OSLM_ERROR_IF( "Representation mode are different. ", mat1->getRepresentationMode() != mat2->getRepresentationMode());

    compare &= (mat1->getOptionsMode() ==  mat2->getOptionsMode());
    OSLM_ERROR_IF( "Option mode are different.", mat1->getOptionsMode() !=  mat2->getOptionsMode());

    compare &= ::fwDataTools::Patient::compareColor(mat1->ambient(), mat2->ambient());
    compare &= ::fwDataTools::Patient::compareColor(mat1->diffuse(), mat2->diffuse());

    return compare;
}

//------------------------------------------------------------------------------

bool ObjectComparator::compareTransfertFunction(::fwData::TransfertFunction_VERSION_II::sptr tf1, ::fwData::TransfertFunction_VERSION_II::sptr tf2)
{

    SLM_ASSERT( "Null tf1 pointers", tf1);
    SLM_ASSERT( "Null tf2 pointers", tf2);

    bool compare = true;
    compare &= (tf1->getTFData().size() == tf2->getTFData().size());
    OSLM_ERROR_IF( "Number of points in each TF are different.", tf1->getTFData().size() != tf2->getTFData().size());

    compare &= (tf1->getName() == tf2->getName());
    OSLM_ERROR_IF( "Transfert funtion are different.", tf1->getName() != tf2->getName());

    compare &= (tf1->getWindow() == tf2->getWindow());
    OSLM_ERROR_IF( "Window value are different.", tf1->getWindow() != tf2->getWindow());

    compare &= (tf1->getLevel() == tf2->getLevel());
    OSLM_ERROR_IF( "Level value are different.", tf1->getLevel() != tf2->getLevel());

    if( tf1->getTFData().size() > 0 && tf2->getTFData().size() > 0 )
    {
        compare &= (tf1->getMinMaxTFValues() == tf2->getMinMaxTFValues());
        OSLM_ERROR_IF( "Min max TF values are different.", tf1->getMinMaxTFValues() != tf2->getMinMaxTFValues());
    }

    return compare;
}

//------------------------------------------------------------------------------

bool ObjectComparator::compareMesh(::fwData::Mesh::sptr mesh1, ::fwData::Mesh::sptr mesh2)
{
    SLM_ASSERT( "Null mesh1 pointers", mesh1);
    SLM_ASSERT( "Null mesh2 pointers", mesh2);

    bool compare = true;
    compare &= (mesh1->getNumberOfPoints()== mesh2->getNumberOfPoints());
    OSLM_ERROR_IF( "Number of points are different.", mesh1->getNumberOfPoints()!= mesh2->getNumberOfPoints());

    compare &= (mesh1->getNumberOfCells() == mesh2->getNumberOfCells());
    OSLM_ERROR_IF( "Number of cells are different.", mesh1->getNumberOfCells() != mesh2->getNumberOfCells());

    compare &= (mesh1->getCellDataSize() == mesh2->getCellDataSize());
    OSLM_ERROR_IF( "Number of cell data are different.", mesh1->getCellDataSize() != mesh2->getCellDataSize());

    compare &= ObjectComparator::compareArray(mesh1->getPointsArray(), mesh2->getPointsArray());
    compare &= ObjectComparator::compareArray(mesh1->getCellTypesArray(), mesh2->getCellTypesArray());
    compare &= ObjectComparator::compareArray(mesh1->getCellDataOffsetsArray(), mesh2->getCellDataOffsetsArray());
    compare &= ObjectComparator::compareArray(mesh1->getCellDataArray(), mesh2->getCellDataArray());
    compare &= ObjectComparator::compareArray(mesh1->getPointColorsArray(), mesh2->getPointColorsArray());
    compare &= ObjectComparator::compareArray(mesh1->getCellColorsArray(), mesh2->getCellColorsArray());
    compare &= ObjectComparator::compareArray(mesh1->getPointNormalsArray(), mesh2->getPointNormalsArray());
    compare &= ObjectComparator::compareArray(mesh1->getCellNormalsArray(), mesh2->getCellNormalsArray());

    compare &= ObjectComparator::compareDataArrayMesh(mesh1, mesh2);
    return compare;
}

//------------------------------------------------------------------------------

bool  ObjectComparator::compareDataArrayMesh(::fwData::Mesh::sptr mesh1, ::fwData::Mesh::sptr mesh2)
{
    SLM_ASSERT( "Null mesh1 pointers", mesh1);
    SLM_ASSERT( "Null mesh2 pointers", mesh2);

    bool compare = true;
    compare &= mesh1->getDataArrayNames() == mesh2->getDataArrayNames();
    OSLM_ERROR_IF( "Mesh data array name are different.", mesh1->getDataArrayNames() != mesh2->getDataArrayNames());

    std::vector<std::string> vectNames = mesh1->getDataArrayNames();
    BOOST_FOREACH(std::string name, vectNames)
    {
        ::fwData::Array::sptr array1 = mesh1->getDataArray(name);
        ::fwData::Array::sptr array2 = mesh2->getDataArray(name);
        compare &= ObjectComparator::compareArray(array1, array2);
    }
    return compare;
}

//------------------------------------------------------------------------------

bool ObjectComparator::compareArray(::fwData::Array::sptr array1, ::fwData::Array::sptr array2)
{
    SLM_ASSERT( "Null array1 pointers", array1);
    SLM_ASSERT( "Null array2 pointers", array2);

    bool compare = true;

    compare &= ( (!array1 && !array2) || (array1 && array2));

    compare &= (array1->getSizeInBytes() == array2->getSizeInBytes());
    OSLM_ERROR_IF( "Array size in bytes are different.", array1->getSizeInBytes() != array2->getSizeInBytes());

    compare &= (array1->getElementSizeInBytes() == array2->getElementSizeInBytes());
    OSLM_ERROR_IF( "Element size in bytes are different.", array1->getElementSizeInBytes() != array2->getElementSizeInBytes());

    compare &= (array1->getNumberOfDimensions() == array2->getNumberOfDimensions());
    OSLM_ERROR_IF( "Number of dimensions are different.", array1->getNumberOfDimensions() != array2->getNumberOfDimensions());

    compare &= (array1->getSize() == array2->getSize());
    OSLM_ERROR_IF( "Array size are different.", array1->getSize() != array2->getSize());

    compare &= (array1->getNumberOfComponents() == array2->getNumberOfComponents());
    OSLM_ERROR_IF( "Number of components are different.", array1->getNumberOfComponents() != array2->getNumberOfComponents());

    compare &= (array1->getIsBufferOwner() == array2->getIsBufferOwner());
    OSLM_ERROR_IF( "Buffer owner are different.", array1->getIsBufferOwner() != array2->getIsBufferOwner());

    compare &= (array1->getStrides() == array2->getStrides());
    OSLM_ERROR_IF( "Strides are different.", array1->getStrides() != array2->getStrides());

    compare &= (array1->getType().string() == array2->getType().string());
    OSLM_ERROR_IF( "Array type are different.", array1->getType().string() != array2->getType().string());

    compare &= (array1->getType().sizeOf() == array2->getType().sizeOf());
    OSLM_ERROR_IF( "Array type (sizeOf) are different.", array1->getType().sizeOf() != array2->getType().sizeOf());

    if(array1)
    {
        compare &= (array1->getSize() == array2->getSize());

        char *iter1 = array1->begin<char>();
        char *iter2 = array2->begin<char>();

        for (; iter1 != array1->end<char>() ; ++iter1, ++iter2)
        {
            compare &= (*iter1 == *iter2);
        }
    }
    return compare;
}

//------------------------------------------------------------------------------
} // namespace fwDataTools
