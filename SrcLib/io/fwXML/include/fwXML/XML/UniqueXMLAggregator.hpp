#ifndef UNIQUEXMLAGGREGATOR_HPP_
#define UNIQUEXMLAGGREGATOR_HPP_


#include "fwXML/config.hpp"
#include "fwXML/XML/IXMLAggregator.hpp"
#include <libxml/tree.h>


namespace fwXML
{

/**
 * @brief interface for XML integration
 * During visitor::Serialize scanning each object traversed an xml node is created
 * This node only contain intrinsic information of object ( aggregation ignored )
 * @auhtor Vincent Agnus
 */
class FWXML_CLASS_API UniqueXMLAggregator : public IXMLAggregator
{
public:

    FWXML_API UniqueXMLAggregator();

    FWXML_API virtual ~UniqueXMLAggregator();

    /**
     * @brief append xml node in the xml tree as child
     */
    FWXML_API virtual void append( xmlNodePtr node );

    FWXML_API virtual void append( xmlNodePtr parent, xmlNodePtr node );

protected :


};

}

#endif /*UNIQUEXMLAGGREGATOR_HPP_*/
