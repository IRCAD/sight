/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <cstring>

#include <boost/lexical_cast.hpp>
#include <fwTools/ClassFactoryRegistry.hpp>
#include <fwTools/UUID.hpp>

#include <fwData/DictionaryOrgan.hpp>
#include <fwData/String.hpp>
#include <fwData/Node.hpp>

#include "fwXML/XML/DictionaryOrganXMLTranslator.hpp"
#include "fwXML/XML/NodeXMLTranslator.hpp"
#include "fwXML/XML/XMLTranslatorHelper.hpp"
#include "fwXML/XML/XMLParser.hpp"

namespace fwXML
{

DictionaryOrganXMLTranslator::DictionaryOrganXMLTranslator() {};

//------------------------------------------------------------------------------

DictionaryOrganXMLTranslator::~DictionaryOrganXMLTranslator() {};

//------------------------------------------------------------------------------

xmlNodePtr DictionaryOrganXMLTranslator::getXMLFrom( ::fwTools::Object::sptr obj )
{
    ::fwData::DictionaryOrgan::sptr dicoOrgan = ::fwData::DictionaryOrgan::dynamicCast(obj);
    SLM_ASSERT("DictionaryOrgan not instanced", dicoOrgan);

    // create master node with className+id
    xmlNodePtr node = XMLTH::MasterNode( obj );

    xmlAddChild(node,XMLTH::newElement("StructureType", dicoOrgan->getCRefStructureType()));
    xmlAddChild(node,XMLTH::newElement("SALabel", dicoOrgan->getCRefSALabel()));
    xmlAddChild(node,XMLTH::newElement("IsMedicalStructure", dicoOrgan->getCRefIsMedicalStructure()));
    xmlAddChild(node,XMLTH::newElement("AOColor", dicoOrgan->getCRefAOColor()));
    xmlAddChild(node,XMLTH::newElement("AvgVolume", dicoOrgan->getCRefAvgVolume()));
    xmlAddChild(node,XMLTH::newElement("VolStdDeviation", dicoOrgan->getCRefVolStdDeviation()));
    xmlAddChild(node,XMLTH::newElement("NbExams", dicoOrgan->getCRefNbExams()));
    xmlAddChild(node,XMLTH::newElement("Position", dicoOrgan->getCRefPosition()));

    xmlAddChild(node,XMLTH::newElement("IconPath", dicoOrgan->getCRefIconPath().string()));

    xmlAddChild(node,XMLTH::newElement("Red", dicoOrgan->getCRefRed()));
    xmlAddChild(node,XMLTH::newElement("Green", dicoOrgan->getCRefGreen()));
    xmlAddChild(node,XMLTH::newElement("Blue", dicoOrgan->getCRefBlue()));
    xmlAddChild(node,XMLTH::newElement("Alpha", dicoOrgan->getCRefAlpha()));

    xmlAddChild(node,XMLTH::newElement("TexturePath", dicoOrgan->getCRefTexturePath().string()));

    xmlAddChild(node,XMLTH::newElement("AvgTriangleNb", dicoOrgan->getCRefAvgTriangleNb()));
    xmlAddChild(node,XMLTH::newElement("SegMin", dicoOrgan->getCRefSegMin()));
    xmlAddChild(node,XMLTH::newElement("SegMax", dicoOrgan->getCRefSegMax()));
    xmlAddChild(node,XMLTH::newElement("Modality", dicoOrgan->getCRefModality()));
    xmlAddChild(node,XMLTH::newElement("SurfaceElasticity", dicoOrgan->getCRefSurfaceElasticity()));
    xmlAddChild(node,XMLTH::newElement("SurfaceRupture", dicoOrgan->getCRefSurfaceRupture()));
    xmlAddChild(node,XMLTH::newElement("InteriorRupture", dicoOrgan->getCRefInteriorRupture()));
    xmlAddChild(node,XMLTH::newElement("Comment", dicoOrgan->getCRefComment()));

    xmlAddChild(node, XMLTH::PODContainerToXml("BelongsTo", dicoOrgan->getCRefBelongsTo().begin(), dicoOrgan->getCRefBelongsTo().end()));

    xmlNodePtr organNameNode = xmlNewNode(NULL, BAD_CAST "OrganName");
    xmlAddChild(node, organNameNode);
    xmlNodePtr strOrganNameNode = XMLTH::toXMLRecursive( dicoOrgan->getCRefOrganName() );
    xmlAddChild(organNameNode,strOrganNameNode);

    xmlNodePtr roi4OrganNativeExpNode = xmlNewNode(NULL, BAD_CAST "Roi4OrganNativeExp");
    xmlAddChild(node, roi4OrganNativeExpNode);
    xmlNodePtr strRoi4OrganNativeExpNode = XMLTH::toXMLRecursive( dicoOrgan->getCRefRoi4OrganNativeExp() );
    xmlAddChild(roi4OrganNativeExpNode,strRoi4OrganNativeExpNode);

    xmlNodePtr roi4OrganExpNode = xmlNewNode(NULL, BAD_CAST "Roi4OrganExp");
    xmlAddChild(node, roi4OrganExpNode);
    xmlNodePtr strRoi4OrganExpNode = XMLTH::toXMLRecursive( dicoOrgan->getCRefRoi4OrganExp() );
    xmlAddChild(roi4OrganExpNode,strRoi4OrganExpNode);

    xmlNodePtr roi4OrganDataNode = xmlNewNode(NULL, BAD_CAST "Roi4OrganDataNode");
    xmlAddChild(node, roi4OrganDataNode);
    if (!dicoOrgan->getCRefRoi4OrganDataNode().expired())
    {
        xmlNodePtr subRoi4OrganDataNode = XMLTH::toXMLRecursive( dicoOrgan->getCRefRoi4OrganDataNode().lock() );
        xmlAddChild(roi4OrganDataNode,subRoi4OrganDataNode);
    }

    xmlNodePtr maskDataNode = xmlNewNode(NULL, BAD_CAST "MaskDataNode");
    xmlAddChild(node, maskDataNode);
    if (!dicoOrgan->getCRefMaskDataNode().expired())
    {
        xmlNodePtr subMaskDataNode= XMLTH::toXMLRecursive( dicoOrgan->getCRefMaskDataNode().lock() );
        xmlAddChild(maskDataNode,subMaskDataNode);
    }

    xmlNodePtr meshDataNode = xmlNewNode(NULL, BAD_CAST "MeshDataNode");
    xmlAddChild(node, meshDataNode);
    if (!dicoOrgan->getCRefMeshDataNode().expired())
    {
        xmlNodePtr subMeshDataNode = XMLTH::toXMLRecursive( dicoOrgan->getCRefMeshDataNode().lock() );
        xmlAddChild(meshDataNode,subMeshDataNode);
    }

    return node;
}

//------------------------------------------------------------------------------

void DictionaryOrganXMLTranslator::updateDataFromXML( ::fwTools::Object::sptr toUpdate,  xmlNodePtr source)
{
    SLM_ASSERT( "XML node not valid", XMLTH::check< ::fwData::DictionaryOrgan >(toUpdate, source) );
    ::fwData::DictionaryOrgan::sptr dicoOrgan = ::fwData::DictionaryOrgan::dynamicCast(toUpdate);
    SLM_ASSERT("DictionaryOrgan not instanced", dicoOrgan);

    dicoOrgan->getRefStructureType() = XMLTH::getElement< std::string >(XMLParser::findChildNamed( source, "StructureType"));
    dicoOrgan->getRefSALabel() = XMLTH::getElement< std::string >(XMLParser::findChildNamed( source, "SALabel"));
    dicoOrgan->getRefIsMedicalStructure() = XMLTH::getElement< bool >(XMLParser::findChildNamed( source, "IsMedicalStructure"));
    dicoOrgan->getRefAOColor() = XMLTH::getElement< ::boost::uint32_t >(XMLParser::findChildNamed( source, "AOColor"));
    dicoOrgan->getRefAvgVolume() = XMLTH::getElement< double >(XMLParser::findChildNamed( source, "AvgVolume"));
    dicoOrgan->getRefVolStdDeviation() = XMLTH::getElement< double >(XMLParser::findChildNamed( source, "VolStdDeviation"));
    dicoOrgan->getRefNbExams() = XMLTH::getElement< ::boost::uint32_t >(XMLParser::findChildNamed( source, "NbExams"));
    dicoOrgan->getRefPosition() = XMLTH::getElement< std::string >(XMLParser::findChildNamed( source, "Position"));

    std::string iconPath = XMLTH::getElement< std::string >(XMLParser::findChildNamed( source, "IconPath"));;
    dicoOrgan->setCRefIconPath( boost::filesystem::path(iconPath) );

    dicoOrgan->getRefRed() = XMLTH::getElement< double >(XMLParser::findChildNamed( source, "Red"));
    dicoOrgan->getRefGreen() = XMLTH::getElement< double >(XMLParser::findChildNamed( source, "Green"));
    dicoOrgan->getRefBlue() = XMLTH::getElement< double >(XMLParser::findChildNamed( source, "Blue"));
    dicoOrgan->getRefAlpha() = XMLTH::getElement< double >(XMLParser::findChildNamed( source, "Alpha"));

    std::string texturePath = XMLTH::getElement< std::string >(XMLParser::findChildNamed( source, "TexturePath"));;
    dicoOrgan->setCRefTexturePath( boost::filesystem::path(texturePath) );

    dicoOrgan->getRefAvgTriangleNb() = XMLTH::getElement< ::boost::uint32_t >(XMLParser::findChildNamed( source, "AvgTriangleNb"));
    dicoOrgan->getRefSegMin() = XMLTH::getElement< double >(XMLParser::findChildNamed( source, "SegMin"));
    dicoOrgan->getRefSegMax() = XMLTH::getElement< double >(XMLParser::findChildNamed( source, "SegMax"));
    dicoOrgan->getRefModality() = XMLTH::getElement< std::string >(XMLParser::findChildNamed( source, "Modality"));
    dicoOrgan->getRefSurfaceElasticity() = XMLTH::getElement< double >(XMLParser::findChildNamed( source, "SurfaceElasticity"));
    dicoOrgan->getRefSurfaceRupture() = XMLTH::getElement< double >(XMLParser::findChildNamed( source, "SurfaceRupture"));
    dicoOrgan->getRefInteriorRupture() = XMLTH::getElement< double >(XMLParser::findChildNamed( source, "InteriorRupture"));
    dicoOrgan->getRefComment() = XMLTH::getElement< std::string >(XMLParser::findChildNamed( source, "Comment"));


    XMLTH::PODcontainerFromXml(XMLParser::findChildNamed( source, "BelongsTo"), std::back_inserter(dicoOrgan->getRefBelongsTo()));

    xmlNodePtr organNameNode = XMLParser::findChildNamed( source, "OrganName");
    XMLTH::fromXML(dicoOrgan->getRefOrganName(), XMLParser::nextXMLElement(organNameNode->children));

    xmlNodePtr roi4OrganNativeExpNode = XMLParser::findChildNamed( source, "Roi4OrganNativeExp");
    XMLTH::fromXML(dicoOrgan->getRefRoi4OrganNativeExp(), XMLParser::nextXMLElement(roi4OrganNativeExpNode->children));

    xmlNodePtr roi4OrganExpNode = XMLParser::findChildNamed( source, "Roi4OrganExp");
    XMLTH::fromXML(dicoOrgan->getRefRoi4OrganExp(), XMLParser::nextXMLElement(roi4OrganExpNode->children));

    xmlNodePtr roi4OrganDataNode = XMLParser::findChildNamed( source, "Roi4OrganDataNode");
    if (roi4OrganDataNode->children)
    {
        dicoOrgan->getRefRoi4OrganDataNode() = ::fwData::Node::dynamicCast(XMLTH::fromXML(XMLParser::nextXMLElement(roi4OrganDataNode->children)));
    }

    xmlNodePtr maskDataNode = XMLParser::findChildNamed( source, "MaskDataNode");
    if (maskDataNode->children)
    {
        dicoOrgan->getRefMaskDataNode() = ::fwData::Node::dynamicCast(XMLTH::fromXML(XMLParser::nextXMLElement(maskDataNode->children)));
    }

    xmlNodePtr meshDataNode = XMLParser::findChildNamed( source, "MeshDataNode");
    if (meshDataNode->children)
    {
        dicoOrgan->getRefMeshDataNode() = ::fwData::Node::dynamicCast(XMLTH::fromXML(XMLParser::nextXMLElement(meshDataNode->children)));
    }
}

//------------------------------------------------------------------------------

}
