/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>
#include <fwTools/UUID.hpp>

#include <fwData/visitor/accept.hpp>
#include <fwData/Composite.hpp>
#include <fwData/Resection.hpp>
#include <fwData/Vector.hpp>
#include <fwData/List.hpp>
#include <fwData/Node.hpp>
#include <fwData/Graph.hpp>

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

void CollectFileFormatService::visit( ::fwTools::Object::sptr obj)
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
}

//------------------------------------------------------------------------------

void CollectFileFormatService::next( ::fwTools::Object::sptr src, ::fwTools::Object::ChildContainer &fields)
{
    this->::fwData::visitor::BreathFirst::next( src, src->children() );

    ::fwData::Composite::sptr composite;
    ::fwData::Vector::sptr vector;
    ::fwData::List::sptr list;
    ::fwData::Resection::sptr resection;
    ::fwData::Graph::sptr graph;
    ::fwData::Node::sptr node;

    if ( composite = ::fwData::Composite::dynamicCast( src ))
    {
        ::fwData::Composite::Container::iterator i;
        for ( i = composite->getRefMap().begin(); i != composite->getRefMap().end(); ++i)
        {
            ::fwData::visitor::accept( i->second , this);
        }
    }
    else if ( vector = ::fwData::Vector::dynamicCast( src ))
    {
        ::fwData::Vector::Container::iterator i;
        for ( i = vector->getRefContainer().begin(); i != vector->getRefContainer().end(); ++i)
        {
            ::fwData::visitor::accept( *i , this);
        }
    }
    else if ( list  = ::fwData::List::dynamicCast( src ))
    {
        ::fwData::List::Container::iterator i;
        for ( i = list->getRefContainer().begin(); i != list->getRefContainer().end(); ++i)
        {
            ::fwData::visitor::accept( *i , this);
        }
    }
    else if ( resection = ::fwData::Resection::dynamicCast( src ))
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
    else if ( graph  = ::fwData::Graph::dynamicCast( src ))
    {
        ::fwData::Graph::NodeContainer::iterator i;
        for ( i = graph->getRefNodes().begin(); i != graph->getRefNodes().end(); ++i)
        {
            ::fwData::visitor::accept( *i , this);
        }
    }
    else if ( (node = ::fwData::Node::dynamicCast( src ) ) && node->getObject() )
    {
        ::fwData::visitor::accept( node->getObject() , this);
    }
}

//------------------------------------------------------------------------------

}

