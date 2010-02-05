#ifndef CollectFileFormatServiceBREATH_FIRST_H
#define CollectFileFormatServiceBREATH_FIRST_H

#include "fwXML/config.hpp"
#include <fwData/visitor/BreathFirst.hpp>
#include <fwXML/IFileFormatService.hpp>


namespace visitor
{



/**
 * @brief visit all objects then store FileFormatService if any
 * @author IRCAD (Research and Development Team).
 */
class FWXML_CLASS_API CollectFileFormatService : public  ::fwData::visitor::BreathFirst // BreathFirst to prove genericity ,DepthFirst is Better
{
public:
    FWXML_API CollectFileFormatService();

    FWXML_API virtual ~CollectFileFormatService();

    // override call CollectFileFormatService service for object
    FWXML_API virtual void visit( ::boost::shared_ptr< ::fwTools::Object > obj);

    /// override the behavior when visiting composite by visit its fields *and* values in its map
    FWXML_API virtual void next( ::fwTools::Object::sptr src, ::fwTools::Object::ChildContainer &fields);

    typedef  std::map< ::boost::shared_ptr< ::fwTools::Object > , ::boost::shared_ptr<  ::fwXML::IFileFormatService > > MapObjectFileFormatService;
    MapObjectFileFormatService m_objWithFileFormatService;


};

}

#endif /*CollectFileFormatServiceBREATH_FIRST_H*/
