/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/lexical_cast.hpp>

#include <fwData/Mesh.hpp>
#include <fwData/Composite.hpp>

#include "fwXML/XML/MeshXMLTranslator.hpp"
#include "fwXML/XML/XMLParser.hpp"
#include "fwXML/XML/XMLTranslatorHelper.hpp"
#include "fwXML/Serializer.hpp"

namespace fwXML
{

//------------------------------------------------------------------------------

MeshXMLTranslator::MeshXMLTranslator()
{}

//------------------------------------------------------------------------------

MeshXMLTranslator::~MeshXMLTranslator()
{}

//------------------------------------------------------------------------------

xmlNodePtr MeshXMLTranslator::getXMLFrom( ::fwData::Object::sptr obj )
{
    ::fwData::Mesh::sptr pMesh = ::fwData::Mesh::dynamicCast(obj);
    SLM_ASSERT("Mesh not instanced", pMesh);

    // create master node with className+id
    xmlNodePtr masterNode = XMLTranslatorHelper::MasterNode( obj );

    xmlNodePtr nbPoints = XMLTH::newElement("nbPoints", pMesh->getNumberOfPoints() );
    xmlAddChild( masterNode , nbPoints );
    xmlNodePtr nbCells = XMLTH::newElement("nbCells", pMesh->getNumberOfCells() );
    xmlAddChild( masterNode , nbCells );
    xmlNodePtr cellDataSize = XMLTH::newElement("cellDataSize", pMesh->getCellDataSize() );
    xmlAddChild( masterNode , cellDataSize );

    ::fwData::Composite::NewSptr meshArrays;
    (*meshArrays)["points"]          = pMesh->getPointsArray();
    (*meshArrays)["cellTypes"]       = pMesh->getCellTypesArray();
    (*meshArrays)["cellData"]        = pMesh->getCellDataArray();
    (*meshArrays)["cellDataOffsets"] = pMesh->getCellDataOffsetsArray();

    if(pMesh->getPointColorsArray())
    {
        (*meshArrays)["pointColors"]   = pMesh->getPointColorsArray();
    }
    if(pMesh->getCellColorsArray())
    {
        (*meshArrays)["cellColors"]    = pMesh->getCellColorsArray();
    }
    if(pMesh->getPointNormalsArray())
    {
        (*meshArrays)["pointNormals"]  = pMesh->getPointNormalsArray();
    }
    if(pMesh->getCellNormalsArray())
    {
        (*meshArrays)["cellNormals"]   = pMesh->getCellNormalsArray();
    }
    std::vector<std::string> vectNames = pMesh->getDataArrayNames();
    if(!vectNames.empty())
    {
        ::fwData::Composite::NewSptr dataArrays;
        BOOST_FOREACH(std::string name, vectNames)
        {
            ::fwData::Array::sptr array = pMesh->getDataArray(name);
            OSLM_ASSERT("Array "<<name<<" not initialized in Mesh.",array);
            (*dataArrays)[name] = array;
        }
        (*meshArrays)["dataArray"] = dataArrays;
    }

    xmlNodePtr meshArraysNode = XMLTH::toXMLRecursive(meshArrays);
    xmlAddChild( masterNode, meshArraysNode);

    return masterNode;
}

//------------------------------------------------------------------------------

void MeshXMLTranslator::updateDataFromXML( ::fwData::Object::sptr toUpdate,  xmlNodePtr source)
{
    SLM_ASSERT("toUpdate not instanced", toUpdate);

    //get its label
    ::fwData::Mesh::sptr pMesh = ::fwData::Mesh::dynamicCast(toUpdate);

    if ( source->children != NULL )
    {
        xmlNodePtr elementNode = XMLParser::nextXMLElement( source->children );
        while ( elementNode )
        {
            std::string nodeName = (const char *) elementNode->name;
            if ( nodeName == "nbPoints" )
            {
                std::string litteralValue = XMLParser::getTextValue(elementNode);
                pMesh->setNumberOfPoints( ::boost::lexical_cast< ::fwData::Mesh::Id >(litteralValue) );
            }
            else if ( nodeName == "nbCells" )
            {
                std::string litteralValue = XMLParser::getTextValue(elementNode);
                pMesh->setNumberOfCells( ::boost::lexical_cast< ::fwData::Mesh::Id >(litteralValue) );
            }
            else if ( nodeName == "cellDataSize" )
            {
                std::string litteralValue = XMLParser::getTextValue(elementNode);
                pMesh->setCellDataSize( ::boost::lexical_cast< ::fwData::Mesh::Id >(litteralValue) );
            }
            else if ( nodeName == "Composite" )
            {
                ::fwData::Object::sptr valueObj;
                valueObj = Serializer().ObjectsFromXml( elementNode, true );
                SLM_ASSERT("valueObj not instanced", valueObj);
                ::fwData::Composite::sptr meshArrays = ::fwData::Composite::dynamicCast(valueObj);
                SLM_ASSERT("composite not instanced", meshArrays);

                ::fwData::Array::sptr pointsSrc = ::fwData::Array::dynamicCast((*meshArrays)["points"]);
                ::fwData::Array::sptr points = pMesh->getPointsArray();
                points->swap(pointsSrc);

                ::fwData::Array::sptr cellTypesSrc = ::fwData::Array::dynamicCast((*meshArrays)["cellTypes"]);
                ::fwData::Array::sptr cellTypes = pMesh->getCellTypesArray();
                cellTypes->swap(cellTypesSrc);

                ::fwData::Array::sptr cellDataSrc = ::fwData::Array::dynamicCast((*meshArrays)["cellData"]);
                ::fwData::Array::sptr cellData = pMesh->getCellDataArray();
                cellData->swap(cellDataSrc);

                ::fwData::Array::sptr cellDataOffsetsSrc = ::fwData::Array::dynamicCast((*meshArrays)["cellDataOffsets"]);
                ::fwData::Array::sptr cellDataOffsets = pMesh->getCellDataOffsetsArray();
                cellDataOffsets->swap(cellDataOffsetsSrc);

                if(meshArrays->find("pointColors")!=meshArrays->end())
                {
                    ::fwData::Array::sptr pointColors = pMesh->getPointColorsArray();
                    ::fwData::Array::sptr pointColorsSrc = ::fwData::Array::dynamicCast((*meshArrays)["pointColors"]);
                    if(pointColors)
                    {
                        pointColors->swap(pointColorsSrc);
                    }
                    else
                    {
                        pMesh->setPointColorsArray(pointColorsSrc);
                    }
                }
                if(meshArrays->find("cellColors")!=meshArrays->end())
                {
                    ::fwData::Array::sptr cellColors = pMesh->getCellColorsArray();
                    ::fwData::Array::sptr cellColorsSrc = ::fwData::Array::dynamicCast((*meshArrays)["cellColors"]);
                    if(cellColors)
                    {
                        cellColors->swap(cellColorsSrc);
                    }
                    else
                    {
                        pMesh->setCellColorsArray(cellColorsSrc);
                    }
                }
                if(meshArrays->find("pointNormals")!=meshArrays->end())
                {
                    ::fwData::Array::sptr pointNormals = pMesh->getPointNormalsArray();
                    ::fwData::Array::sptr pointNormalsSrc = ::fwData::Array::dynamicCast((*meshArrays)["pointNormals"]);
                    if(pointNormals)
                    {
                        pointNormals->swap(pointNormalsSrc);
                    }
                    else
                    {
                        pMesh->setPointNormalsArray(pointNormalsSrc);
                    }
                }
                if(meshArrays->find("cellNormals")!=meshArrays->end())
                {
                    ::fwData::Array::sptr cellNormals = pMesh->getCellNormalsArray();
                    ::fwData::Array::sptr cellNormalsSrc = ::fwData::Array::dynamicCast((*meshArrays)["cellNormals"]);
                    if(cellNormals)
                    {
                        cellNormals->swap(cellNormalsSrc);
                    }
                    else
                    {
                        pMesh->setCellNormalsArray(cellNormalsSrc);
                    }
                }
                if(meshArrays->find("dataArray") != meshArrays->end())
                {
                    ::fwData::Composite::sptr dataArrays = ::fwData::Composite::dynamicCast((*meshArrays)["dataArray"]);
                    BOOST_FOREACH(::fwData::Composite::value_type elt, *dataArrays)
                    {
                        ::fwData::Array::sptr array = pMesh->getDataArray(elt.first);
                        ::fwData::Array::sptr arraySrc = ::fwData::Array::dynamicCast(elt.second);
                        if(array)
                        {
                            array->swap(arraySrc);
                        }
                        else
                        {
                            pMesh->addDataArray(elt.first, array);
                        }
                    }
                }
            }
            // Element ++
            elementNode = ::fwXML::XMLParser::nextXMLElement(elementNode->next);
        }
    }
}

//------------------------------------------------------------------------------

} //namespace fwXML
