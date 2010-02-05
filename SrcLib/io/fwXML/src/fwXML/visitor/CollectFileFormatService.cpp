#include <fwCore/base.hpp>

#include "fwXML/visitor/CollectFileFormatService.hpp"
#include <fwServices/helper.hpp>
#include <fwData/visitor/accept.hpp>
#include <fwData/Composite.hpp>
#include <fwData/Resection.hpp>

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
}



}

