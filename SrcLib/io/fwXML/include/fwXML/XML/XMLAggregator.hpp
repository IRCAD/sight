/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef XMLAGGREGATOR_HPP_
#define XMLAGGREGATOR_HPP_

#include <set>
#include <string>
#include <libxml/tree.h>
#include <boost/filesystem/path.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>

#include "fwXML/config.hpp"
#include "fwXML/FSLocation.hpp"


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
class FWXML_CLASS_API XMLAggregator : public ::fwCore::BaseObject, public FSLocation
{
public:

    fwCoreClassDefinitionsWithFactoryMacro( (XMLAggregator)(::fwCore::BaseObject), (()), new XMLAggregator );

    /// define XML Aggregator Version
    class  Version
    {
    public :

        unsigned int m_version;

        Version() : m_version(0) {};

        Version( unsigned int _version ) : m_version(_version) {};

        Version( std::string _version )
        {
            if(_version == "1.0")
            {
                m_version = 1;
            }
            else
            {
                m_version = ::boost::lexical_cast<unsigned int>(_version);
            }
        }

        std::string string()
        {
            std::string version;
            if(m_version == 1)
            {
                version = "1.0";
            }
            else
            {
                version = ::boost::lexical_cast<std::string>(m_version);
            }
            return version;
        }

        static Version current()
        {
            return Version(3);
        }

        bool operator==(const Version& ver)
        {
            return ( this->m_version == ver.m_version );
        }
        bool operator!=(const Version& ver)
        {
            return ( this->m_version != ver.m_version );
        }
        bool operator>=(const Version& ver)
        {
            return ( this->m_version >= ver.m_version );
        }
        bool operator<=(const Version& ver)
        {
            return ( this->m_version <= ver.m_version );
        }
        bool operator>(const Version& ver)
        {
            return ( this->m_version > ver.m_version );
        }
        bool operator<(const Version& ver)
        {
            return ( this->m_version < ver.m_version );
        }
    };

    FWXML_API XMLAggregator();

    FWXML_API virtual ~XMLAggregator();

    /**
     * @brief append object in the XML Aggregator
     * Object hierarchy is implicitly stored in object its self
     */
    FWXML_API virtual void append( ::boost::weak_ptr< ::fwData::Object>  obj );

    /**
     * @brief return a document viewable
     * xml is lazy created from list of registred Object. Hierarchy of xml is the same as object one.
     * if a child of object is not in the same XMLAggregator. it add an xi:include with path according to the other XMLAggregator
     *
     */
    FWXML_API xmlDocPtr getXMLDoc();


    /**
     *  @brief helper wich retreive the root object from m_objects using child information.
     *   (used by  getXMLDoc)
     */
     ::boost::weak_ptr< ::fwData::Object>  getRootObject();


protected :

    /**
     * @brief An helper wich give the relative path form this aggregator to obj aggregator
     */
    std::string getXMLIncludePathValue(::boost::weak_ptr< ::fwData::Object > obj);

    /**
     * @brief manage xi::include
     */
    xmlNodePtr getXMLInclude( ::boost::weak_ptr< ::fwData::Object> obj);

    /// container of aggregator elements
    typedef std::set< ::boost::weak_ptr< ::fwData::Object >  > Elements;

    /// store objects which are in this Aggregator
    std::set< ::boost::weak_ptr< ::fwData::Object >  > m_objects;

    /// the protocol version used for this XML aggregator
    Version m_version;

};

}

#endif /*XMLAGGREGATOR_HPP_*/
