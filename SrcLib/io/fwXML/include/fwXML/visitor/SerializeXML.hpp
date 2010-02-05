#ifndef SERIALIZEXMLBREATH_FIRST_H
#define SERIALIZEXMLBREATH_FIRST_H

#include <map>
#include <boost/shared_ptr.hpp>

#include "fwXML/config.hpp"
#include <fwData/visitor/BreathFirst.hpp>
#include <libxml/tree.h>


namespace visitor
{



/**
 * @brief visit all objects then call complete an XML tree
 * @author IRCAD (Research and Development Team).
 */
class FWXML_CLASS_API SerializeXML : public  ::fwData::visitor::BreathFirst
{
public:
    FWXML_API SerializeXML();

    FWXML_API virtual ~SerializeXML();

    // override call serialize service for object
    FWXML_API virtual void visit( ::boost::shared_ptr< ::fwTools::Object> obj);

    /// correspondance within node object and xmlNode
    std::map< ::boost::shared_ptr< ::fwTools::Object> , xmlNodePtr>  m_correspondance;


};

}

#endif /*SERIALIZEXMLBREATH_FIRST_H*/
