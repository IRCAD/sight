#include "fwXML/ObjectToStream.hpp"


#include <fwTools/ClassFactoryRegistry.hpp>
#include <fwCore/base.hpp>

#include "fwXML/visitor/Serialize.hpp"
#include <fwData/visitor/accept.hpp>

#include "fwXML/XML/XMLHierarchy.hpp"
#include "fwXML/XML/XMLStream.hpp"
#include "fwXML/XML/XMLTranslator.hpp"
#include "fwXML/XML/XMLParser.hpp"
#include "fwXML/XML/TrivialXMLTranslator.hpp"

#include "fwXML/policy/NeverSplitPolicy.hpp"
#include "fwXML/XML/XMLPartitioner.hpp"

namespace fwXML
{

const std::string ObjectToStream::toString( ::boost::shared_ptr< fwTools::Object > object, unsigned int option )
{
    std::stringstream os;

    // FIXME ( for test )
    ::fwXML::XMLPartitioner::getDefault()->setSplitPolicy( ::boost::shared_ptr< ::fwXML::ISplitPolicy > ( new ::fwXML::NeverSplitPolicy )  );

    if ( option == 1 || option == 2 )
    {
        // recompute one the fly
        // reset all hierarchy informations
        ::fwXML::XMLHierarchy::getDefault()->mapObjectAggregator().clear();
        ::fwXML::XMLHierarchy::getDefault()->mapChildFather().clear();
        ::fwXML::XMLHierarchy::getDefault()->mapObjectXMLNode().clear();

        ::visitor::Serialize visitor;
        //object->accept( &visitor );
        ::fwData::visitor::accept(object, &visitor);

        // For option 3 only
        std::set< ::boost::shared_ptr< ::fwXML::XMLAggregator > > savedAggregator;

        // find node
        for(    ::fwXML::XMLHierarchy::ObjectAggregatorMap::iterator aggIter = ::fwXML::XMLHierarchy::getDefault()->mapObjectAggregator().begin();
                aggIter != ::fwXML::XMLHierarchy::getDefault()->mapObjectAggregator().end();
                ++aggIter )
        {
            // Get aggregator
            ::boost::shared_ptr< ::fwXML::XMLAggregator > aggregator =  aggIter->second;

            if ( option == 1 ) // Option 2
            {
                if ( object == aggIter->first.lock() )
                {
                    ::fwXML::XMLStream::toStream(  aggregator->getXMLDoc() , os );
                }
            }
            else if ( option == 2 ) // Option 3
            {
                if ( savedAggregator.count( aggregator ) == 0 ) // not already saved
                {
                    os << std::endl << "Aggregator name : " << aggregator->getFullPath().string() << std::endl;
                    ::fwXML::XMLStream::toStream(  aggregator->getXMLDoc() , os );
                    savedAggregator.insert( aggregator );
                }
            }
        }
    }
    else if ( option == 0 )
    {

        ::boost::shared_ptr< ::fwXML::XMLTranslator > translator = fwTools::ClassFactoryRegistry::create< ::fwXML::XMLTranslator  >(  typeid(*object)  );

        if ( translator == NULL )
        {
            translator = ::boost::shared_ptr< ::fwXML::XMLTranslator >( new ::fwXML::TrivialXMLTranslator );
            OSLM_WARN( "no XMLTranslator for" << object->className() << "(" << object.get() << ") use Trivial XML Translator");
        }

        xmlNodePtr nodeXML = translator->getXMLFrom( object );

        os << ::fwXML::XMLParser::toString(nodeXML) << std::endl;
    }

    return os.str();
}

} // namespace fwXML

