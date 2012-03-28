/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>
#include <fwTools/UUID.hpp>

#include <fwXML/visitor/accept.hpp>
#include <fwData/Composite.hpp>
#include <fwData/Resection.hpp>
#include <fwData/Vector.hpp>
#include <fwData/List.hpp>
#include <fwData/Node.hpp>
#include <fwData/Graph.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/Acquisition.hpp>
#include <fwData/PatientDB.hpp>

#include <fwServices/Base.hpp>

#include "fwXML/visitor/CollectFileFormatService.hpp"

namespace visitor
{

//------------------------------------------------------------------------------

CollectFileFormatService::CollectFileFormatService()
{}

//------------------------------------------------------------------------------

CollectFileFormatService::~CollectFileFormatService()
{}

//------------------------------------------------------------------------------

void CollectFileFormatService::visit( ::fwData::Object::sptr obj)
{
    OSLM_TRACE( "CollectFileFormatService Visitor Visiting : Class " << obj->className() <<
            "(" <<  ::fwTools::UUID::get(obj)    <<
            ") HAS<FileFormatService>" <<  (::fwServices::OSR::has(obj, "::fwXML::IFileFormatService")?"yes":"no") <<
            "ParentClass: " <<  (m_source?m_source->className():"NULL")   <<
            "(" << (m_source ? ::fwTools::UUID::get(m_source):"NoSOURCENOUUID") << ")"
    );

    SLM_ASSERT("Object is null",obj);
    if ( ::fwServices::OSR::has(obj, "::fwXML::IFileFormatService") )
    {
        m_objWithFileFormatService[obj] = ::fwServices::get< ::fwXML::IFileFormatService >( obj );
    }


    // VISIT FIELDS
    BOOST_FOREACH( ::fwData::Object::FieldMapType::value_type item, obj->getFields() )
    {
        ::fwData::visitor::accept( item.second , this);
    }

    // VISIT OTHER DATA
    ::fwData::Composite::sptr composite;
    ::fwData::Vector::sptr vector;
    ::fwData::List::sptr list;
    ::fwData::Resection::sptr resection;
    ::fwData::Graph::sptr graph;
    ::fwData::Node::sptr node;
    ::fwData::Mesh::sptr mesh;
    ::fwData::Image::sptr image;
    ::fwData::Reconstruction::sptr reconstruction;
    ::fwData::Acquisition::sptr acquisition;
    ::fwData::Study::sptr study;
    ::fwData::Patient::sptr patient;
    ::fwData::PatientDB::sptr patientDB;

    if ( (composite = ::fwData::Composite::dynamicCast( obj )) )
    {
        ::fwData::Composite::Container::iterator i;
        for ( i = composite->getRefMap().begin(); i != composite->getRefMap().end(); ++i)
        {
            ::fwData::visitor::accept( i->second , this);
        }
    }
    else if ( (vector = ::fwData::Vector::dynamicCast( obj )))
    {
        ::fwData::Vector::Container::iterator i;
        for ( i = vector->getRefContainer().begin(); i != vector->getRefContainer().end(); ++i)
        {
            ::fwData::visitor::accept( *i , this);
        }
    }
    else if ( (list  = ::fwData::List::dynamicCast( obj )))
    {
        ::fwData::List::Container::iterator i;
        for ( i = list->begin(); i != list->end(); ++i)
        {
            ::fwData::visitor::accept( *i , this);
        }
    }
    else if ( (resection = ::fwData::Resection::dynamicCast( obj )))
    {
        ::fwData::Resection::ResectionInputs::const_iterator i;
        for ( i = resection->getCRefInputs().begin(); i != resection->getCRefInputs().end(); ++i)
        {
            ::fwData::visitor::accept( *i , this);
        }
        ::fwData::Resection::ResectionOutputs::const_iterator j;
        for ( j = resection->getCRefOutputs().begin(); j != resection->getCRefOutputs().end(); ++j)
        {
            ::fwData::visitor::accept( *j , this);
        }
    }
    else if ( (graph  = ::fwData::Graph::dynamicCast( obj )))
    {
        ::fwData::Graph::NodeContainer::iterator i;
        for ( i = graph->getRefNodes().begin(); i != graph->getRefNodes().end(); ++i)
        {
            ::fwData::visitor::accept( *i , this);
        }
    }
    else if ( (node = ::fwData::Node::dynamicCast( obj ) ) && node->getObject() )
    {
        ::fwData::visitor::accept( node->getObject() , this);
    }
    else if ( (mesh = ::fwData::Mesh::dynamicCast( obj ) ))
    {
        ::fwData::visitor::accept( mesh->getPointsArray() , this);
        ::fwData::visitor::accept( mesh->getCellTypesArray() , this);
        ::fwData::visitor::accept( mesh->getCellDataArray() , this);
        ::fwData::visitor::accept( mesh->getCellDataOffsetsArray() , this);
        if(mesh->getPointColorsArray())
        {
            ::fwData::visitor::accept( mesh->getPointColorsArray() , this);
        }
        if(mesh->getCellColorsArray())
        {
            ::fwData::visitor::accept( mesh->getCellColorsArray() , this);
        }
        if(mesh->getPointNormalsArray())
        {
            ::fwData::visitor::accept( mesh->getPointNormalsArray() , this);
        }
        if(mesh->getCellNormalsArray())
        {
            ::fwData::visitor::accept( mesh->getCellNormalsArray() , this);
        }
        std::vector<std::string> vectNames = mesh->getDataArrayNames();
        BOOST_FOREACH(std::string name, vectNames)
        {
            ::fwData::Array::sptr array = mesh->getDataArray(name);
            OSLM_ASSERT("Array "<<name<<" not initialized in Mesh.",array);
            ::fwData::visitor::accept( array, this);
        }
    }
    else if ( (image = ::fwData::Image::dynamicCast( obj ) ))
    {
        if(image->getDataArray())
        {
            ::fwData::visitor::accept( image->getDataArray() , this);
        }
    }
    else if ( ( reconstruction = ::fwData::Reconstruction::dynamicCast( obj ) ) )
    {
        if( reconstruction->getImage() )
        {
            ::fwData::visitor::accept( reconstruction->getImage() , this);
        }
        if( reconstruction->getMesh() )
        {
            ::fwData::visitor::accept( reconstruction->getMesh() , this);
        }
    }
    else if ( ( acquisition = ::fwData::Acquisition::dynamicCast( obj ) ) )
    {
        ::fwData::visitor::accept( acquisition->getImage() , this);

        BOOST_FOREACH( ::fwData::Reconstruction::sptr rec, acquisition->getReconstructions() )
        {
            ::fwData::visitor::accept( rec, this );
        }

        if( acquisition->getStructAnat() )
        {
            ::fwData::visitor::accept( acquisition->getStructAnat(), this );
        }
    }
    else if ( ( study = ::fwData::Study::dynamicCast( obj ) ) )
    {

        BOOST_FOREACH( ::fwData::Acquisition::sptr acq, study->getAcquisitions() )
        {
            ::fwData::visitor::accept( acq, this );
        }

    }
    else if ( ( patient = ::fwData::Patient::dynamicCast( obj ) ) )
    {
        BOOST_FOREACH( ::fwData::Study::sptr pStudy, patient->getStudies() )
        {
            ::fwData::visitor::accept( pStudy, this );
        }
        if(patient->getScenarios() )
        {
            ::fwData::visitor::accept( patient->getScenarios(), this );
        }
        if( patient->getToolBox() )
        {
            ::fwData::visitor::accept( patient->getToolBox(), this );
        }
    }
    else if ( ( patientDB = ::fwData::PatientDB::dynamicCast( obj ) ) )
    {

        BOOST_FOREACH( ::fwData::Patient::sptr pPatient, patientDB->getPatients() )
        {
            ::fwData::visitor::accept( pPatient, this );
        }

    }
}


}

