/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/Material.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/TriangularMesh.hpp>

#include <fwDataIO/reader/MeshReader.hpp>
#include <fwDataTools/MeshGenerator.hpp>

#include "fwXML/Serializer.hpp"
#include "fwXML/XML/XMLParser.hpp"
#include "fwXML/XML/XMLTranslatorHelper.hpp"
#include "fwXML/XML/XMLPatch.hpp"

namespace fwXML
{

//------------------------------------------------------------------------------

void XMLPatch::PatchVersion1ToVersion2( xmlNodePtr node )
{
    if (node &&  node->type == XML_ELEMENT_NODE)
    {
        std::string nodeName( (const char*)node->name );

        if(nodeName == "Reconstruction")
        {
            xmlNodePtr nodeReconstruction = node;
            float red, green, blue, alpha;

            xmlNodePtr redNode = XMLParser::findChildNamed( nodeReconstruction, "Red");
            xmlNodePtr greenNode = XMLParser::findChildNamed( nodeReconstruction, "Green");
            xmlNodePtr blueNode  = XMLParser::findChildNamed( nodeReconstruction, "Blue");
            xmlNodePtr alphaNode = XMLParser::findChildNamed( nodeReconstruction, "Alpha");

            red = ::boost::lexical_cast< float >( XMLParser::getTextValue(redNode) );
            green = ::boost::lexical_cast< float >(XMLParser::getTextValue(greenNode));
            blue = ::boost::lexical_cast< float >(XMLParser::getTextValue(blueNode));
            alpha = ::boost::lexical_cast< float >(XMLParser::getTextValue(alphaNode));

            xmlUnlinkNode(redNode);
            xmlFreeNode(redNode);
            xmlUnlinkNode(greenNode);
            xmlFreeNode(greenNode);
            xmlUnlinkNode(blueNode);
            xmlFreeNode(blueNode);
            xmlUnlinkNode(alphaNode);
            xmlFreeNode(alphaNode);

            xmlNodePtr subNodeReconstruction = nodeReconstruction->children;
            while ( subNodeReconstruction )
            {
                std::string nodeName( (const char*)subNodeReconstruction->name );
                if(nodeName == "Field")
                {
                    xmlNodePtr fieldNode = subNodeReconstruction;
                    std::string labelName = XMLParser::getAttribute (fieldNode, "label");
                    if(labelName == "ID_MESH")
                    {
                        xmlNodePtr trianNode = XMLParser::findChildNamed(fieldNode, "TriangularMesh");
                        xmlNodePtr bufferTrianNode = XMLParser::findChildNamed(trianNode, "Buffer");
                        std::string filename =  XMLParser::getAttribute(bufferTrianNode, "filename");

                        xmlUnlinkNode(trianNode);
                        xmlFreeNode(trianNode);

                        ::fwData::Mesh::NewSptr meshData;
                        ::fwDataIO::reader::MeshReader::NewSptr reader;
                        reader->setObject(meshData);
                        reader->setFile(Serializer().rootFolder() / filename);
                        reader->read();
                        ::fwDataTools::MeshGenerator::generatePointNormals(meshData);

                        xmlNodePtr mesh = XMLTH::toXMLRecursive(meshData);
                        Serializer().IOforExtraXML(meshData, true);
                        xmlAddChild( fieldNode, mesh);
                    }
                }
                subNodeReconstruction = subNodeReconstruction->next;
            }
            ::fwData::Material::NewSptr matData;
            matData->ambient()->setRGBA(red, green, blue, alpha);
            xmlNodePtr material = XMLTH::toXMLRecursive(matData);
            xmlNodePtr materialNode = xmlNewNode(NULL, BAD_CAST "material");
            xmlAddChild( materialNode, material);

            xmlAddChild( nodeReconstruction, materialNode);
        } //end (nodeName == "Reconstruction")
        else
        {
            node = node->children;
            while ( node )
            {
                PatchVersion1ToVersion2(node);
                node = node->next;
            }
        }
    }
}

//------------------------------------------------------------------------------

void XMLPatch::PatchNoVersionToVersion1( xmlNodePtr node )
{
    if (node &&  node->type == XML_ELEMENT_NODE)
    {
        std::string NodeName( (const char*)node->name );

        //////////////////////
        // renaming node name
        //////////////////////
        if ( NodeName == "StringField")  { xmlNodeSetName(node, BAD_CAST "String");  }
        if ( NodeName == "FloatField")   { xmlNodeSetName(node, BAD_CAST "Float");   }
        if ( NodeName == "IntegerField") { xmlNodeSetName(node, BAD_CAST "Integer"); }
        if ( NodeName == "BooleanField") { xmlNodeSetName(node, BAD_CAST "Boolean"); }


        ////////////////////////////////////////////////////////////////////////
        // manage extraXML : search for  <Buffer> sub node with attribute protocol="::DataIO:: -> ::fwDataIO::
        ///////////////////////////////////////////////////////////////////////
        std::string protocolValue;
        xmlNodePtr bufferNode;
        try
        {
            bufferNode = XMLParser::findChildNamed( node, "Buffer");
            if ( bufferNode )
            {
                protocolValue = XMLParser::getAttribute (bufferNode, "protocol");
            }
        }
        catch ( ::fwTools::Failed ef)
        {
            OSLM_TRACE(" no protocol class for node->name=" << NodeName )
        }
        // if protocol exist and contain ::dataIO:: replace it by fwDataIO
        if ( !protocolValue.empty() &&  ( protocolValue.find("::dataIO::") != std::string::npos ) )
        {
            protocolValue = protocolValue.replace(0,strlen("::dataIO::"),"::fwDataIO::");
            xmlSetProp( bufferNode , BAD_CAST "protocol",  xmlStrdup( BAD_CAST protocolValue.c_str() ) );
        }


        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // manage class attribut : adapt "::data::" to "::fwData::" and manage {String,Float,Interger}Field renaming
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::string className,newClassName;
        try
        {
            className = XMLParser::getAttribute (node, "class");
        }
        catch ( ::fwTools::Failed ef)
        {
            OSLM_TRACE(" no attrib class for node->name=" << NodeName );
        }
        OSLM_INFO( "PatchNoVersionToNewData nodeName=" << NodeName << " className=" << className);

        // renaming class attribute if required
        if ( !className.empty() )
        {
            // special case for Object class
            if ( className.find("::data::Object") != std::string::npos )
            {
                newClassName = "::fwTools::Object";
            }
            else if ( className.find("::data::") != std::string::npos ) // general case (namespace)
            {
                newClassName = className.replace(0,strlen("::data::"),"::fwData::");
            }

            // specific className : StringField, FloatField, IntegerField
            if ( className.find("StringField") != std::string::npos )
            {
                newClassName = className.replace( className.find("StringField"), strlen("StringField"), "String" );
            }
            else if ( className.find("FloatField") != std::string::npos )
            {
                newClassName = className.replace( className.find("FloatField"), strlen("FloatField"), "Float" );
            }
            else if ( className.find("IntegerField") != std::string::npos )
            {
                newClassName = className.replace( className.find("IntegerField"), strlen("IntegerField"), "Integer" );
            }
            else if ( className.find("BooleanField") != std::string::npos )
            {
                newClassName = className.replace( className.find("BooleanField"), strlen("BooleanField"), "Boolean" );
            }
        }
        // change needed ?
        if ( !newClassName.empty() )
        {
            OSLM_DEBUG( "PatchNoVersionToNewData NEWclassName=" << newClassName );
            assert( xmlHasProp( node, BAD_CAST "class" ) );
            xmlSetProp( node , BAD_CAST "class",  xmlStrdup( BAD_CAST newClassName.c_str() ) );
        }

        // continue parsing to child
        node = node->children;
        while ( node )
        {
            PatchNoVersionToVersion1(node);
            node = node->next;
        }
    }
}

//------------------------------------------------------------------------------

}
