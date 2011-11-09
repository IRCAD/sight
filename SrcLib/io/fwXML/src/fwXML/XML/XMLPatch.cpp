/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwXML/XML/XMLParser.hpp"
#include "fwXML/XML/XMLTranslatorHelper.hpp"
#include "fwXML/XML/XMLPatch.hpp"


namespace fwXML
{

//------------------------------------------------------------------------------

void XMLPatch::PatchNoVersionToNewData( xmlNodePtr node )
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
            PatchNoVersionToNewData(node);
            node = node->next;
        }
    }
}

//------------------------------------------------------------------------------

}
