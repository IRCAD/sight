/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include "fwXML/visitor/CollectFileFormatService.hpp"
#include <fwServices/helper.hpp>
#include <fwTools/UUID.hpp>
#include <fwData/visitor/accept.hpp>
#include <fwData/Composite.hpp>
#include <fwData/Resection.hpp>
#include <fwData/Vector.hpp>
#include <fwData/List.hpp>
#include <fwData/Node.hpp>
#include <fwData/Graph.hpp>

//#include <libxml/tree.h>

namespace visitor
{

CollectFileFormatService::CollectFileFormatService()
{
}

CollectFileFormatService::~CollectFileFormatService()
{
}



void CollectFileFormatService::visit( ::boost::shared_ptr< ::fwTools::Object> obj)
{
    std::string uuid = ::fwTools::UUID::get(obj);
    std::string srcUuid = m_source?::fwTools::UUID::get(m_source):"NoSOURCENOUUID";
    OSLM_TRACE( "CollectFileFormatService Visitor Visiting : Class " << obj->className() <<
                "(" <<  uuid    <<
                ") HASt<FileFormatService>" <<  (fwServices::has< ::fwXML::IFileFormatService >(obj)?"yes":"no") <<
                "ParentClass: " <<  (m_source?m_source->className():"NULL")   << "(" << srcUuid << ")"
                );


    assert(obj);
    if ( fwServices::has< ::fwXML::IFileFormatService >( obj ) )
    {
        m_objWithFileFormatService[obj] = fwServices::get< ::fwXML::IFileFormatService >( obj );
    }
}

void CollectFileFormatService::next( ::fwTools::Object::sptr src, ::fwTools::Object::ChildContainer &fields)
{
    this->::fwData::visitor::BreathFirst::next( src, src->children() );
    ::fwTools::Object::ChildContainer tmpContainer;

    ::fwData::Composite::sptr composite = ::fwData::Composite::dynamicCast( src );
    if ( composite )
    {
        ::fwData::Composite::Container::iterator i;
        for ( i = composite->getRefMap().begin(); i != composite->getRefMap().end(); ++i)
        {
            ::fwData::visitor::accept( i->second , this);
        }
    }

    ::fwData::Vector::sptr vector = ::fwData::Vector::dynamicCast( src );
    if ( vector )
    {
        ::fwData::Vector::Container::iterator i;
        for ( i = vector->getRefContainer().begin(); i != vector->getRefContainer().end(); ++i)
        {
            ::fwData::visitor::accept( *i , this);
        }
    }

    ::fwData::List::sptr list = ::fwData::List::dynamicCast( src );
    if ( list )
    {
        ::fwData::List::Container::iterator i;
        for ( i = list->getRefContainer().begin(); i != list->getRefContainer().end(); ++i)
        {
            ::fwData::visitor::accept( *i , this);
        }
    }

    ::fwData::Resection::sptr resection = ::fwData::Resection::dynamicCast( src );
    if ( resection )
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

    ::fwData::Graph::sptr graph = ::fwData::Graph::dynamicCast( src );
    if ( graph )
    {
        ::fwData::Graph::NodeContainer::iterator i;
        for ( i = graph->getRefNodes().begin(); i != graph->getRefNodes().end(); ++i)
        {
            ::fwData::visitor::accept( *i , this);
        }
    }


    ::fwData::Node::sptr node = ::fwData::Node::dynamicCast( src );
    if ( node )
    {
        ::fwData::visitor::accept( node->getObject() , this);
    }
}



}

