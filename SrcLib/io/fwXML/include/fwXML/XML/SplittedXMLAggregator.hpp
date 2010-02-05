#ifndef SPLITTEDXMLAGGREGATOR_HPP_
#define SPLITTEDXMLAGGREGATOR_HPP_


#include "fwXML/config.hpp"
#include "fwXML/XML/IXMLAggregator.hpp"
#include <libxml/tree.h>


namespace fwXML
{

/**
 * @brief XML integration each node splitted in a file
 * During visitor::Serialize scanning each object traversed an xml node is created
 * This node only contain intrinsic information of object ( aggregation ignored )
 * @auhtor Vincent Agnus
 */
class FWXML_CLASS_API SplittedXMLAggregator : public IXMLAggregator
{
public:

    FWXML_API SplittedXMLAggregator();

    FWXML_API virtual ~SplittedXMLAggregator();

    /**
     * @brief append xml node in the xml tree as child
     */
    FWXML_API virtual void append( xmlNodePtr node );

    FWXML_API virtual void append( xmlNodePtr parent, xmlNodePtr node );

protected :


};

}

#endif /*SPLITTEDXMLAGGREGATOR_HPP_*/
