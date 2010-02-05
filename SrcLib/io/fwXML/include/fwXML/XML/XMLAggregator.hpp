#ifndef XMLAGGREGATOR_HPP_
#define XMLAGGREGATOR_HPP_

#include <set>
#include <string>
#include <libxml/tree.h>
#include <boost/filesystem/path.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include "fwXML/config.hpp"
#include "fwXML/FSLocation.hpp"


#include <boost/lexical_cast.hpp> //FIXME VAG used by Version


namespace fwData
{
    class Object;
}


namespace fwXML
{

/**
 * @brief interface for XML integration
 * During visitor::Serialize scanning each object traversed an xml node is created
 * This node only contain intrinsic information of object ( aggregation ignored )
 * @author IRCAD (Research and Development Team).
 */
class FWXML_CLASS_API XMLAggregator : public FSLocation
{
public:

    /// define XML Aggregator Version
    class  Version
    {
    public :

        int m_major;
        int m_minor;

        Version() : m_major(0),m_minor(0) {};

        Version( int _major, int _minor ) : m_major(_major),m_minor(_minor) {};

        std::string string()
        {
            return ::boost::lexical_cast<std::string>(m_major) + "."
                   + ::boost::lexical_cast<std::string>(m_minor);
        }

        static Version current()
        {
            return Version(1,0);
        }

    };




    FWXML_API XMLAggregator();

    FWXML_API virtual ~XMLAggregator();

    /**
     * @brief append object in the XML Aggregator
     * Object hierarchy is implicitly stored in object its self
     */
    FWXML_API virtual void append( ::boost::weak_ptr<fwTools::Object>  obj );

    /**
     * @brief return a document viewable
     * xml is lazy created from list of registred Object. Hierarchy of xml is the same as object one.
     * if a child of object is not in the same XMLAggregator. it add an xi:include with path according to the other XMLAggregator
     *
     */
    FWXML_API xmlDocPtr getXMLDoc( /*std::list<boost::filesystem::path>  xincludePaths */);


    /**
     *  @brief helper wich retreive the root object from m_objects using child information.
     *   (used by  getXMLDoc)
     */
     ::boost::weak_ptr<fwTools::Object>  getRootObject();


protected :




    /**
     * @brief An helper wich give the relative path form this aggregator to obj aggregator
     */
    std::string getXMLIncludePathValue(::boost::weak_ptr< fwTools::Object > obj);

    /**
     * @brief manage xi::include
     */
    xmlNodePtr getXMLInclude( ::boost::weak_ptr<fwTools::Object> obj);

    /// container of aggregator elements
    typedef std::set< ::boost::weak_ptr<fwTools::Object >  > Elements;

    /// store objects which are in this Aggregator
    std::set< ::boost::weak_ptr<fwTools::Object >  > m_objects;

    /// the protocol version used for this XML aggregator
    Version m_version;

};

}

#endif /*XMLAGGREGATOR_HPP_*/
