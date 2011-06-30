/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#ifndef BOOST_ARCHIVE_FW_XML_IARCHIVE_HPP
#define BOOST_ARCHIVE_FW_XML_IARCHIVE_HPP

#include <ostream>
#include <sstream>
#include <set>

// default implementation
#include <boost/version.hpp>
#include <boost/archive/detail/iserializer.hpp>
#include <boost/archive/detail/interface_iarchive.hpp>
#include <boost/archive/detail/common_iarchive.hpp>

#ifdef BOOST_VERSION
#if BOOST_VERSION >= 103500
#define BOOST135ENABLED
#endif
#else
#warning "BOOST_VERSION not found !!!"
#endif

#ifdef BOOST135ENABLED
#include <boost/serialization/wrapper.hpp>
#endif


#include "fwCore/base.hpp"
#include "fwXML/XML/XMLParser.hpp"




#include <sstream>
#include <iostream> // REMOVE ME DEBUG

namespace boost {
namespace archive {


/** @brief IRCAD R&D team framework ::boost::archive. The purpose of this class is to
 * create an xml archive based on the libxml instead a stream like in native boost
 * @author IRCAD (Research and Development Team).
 */





class fw_xml_iarchive : public detail::common_iarchive< fw_xml_iarchive > // , public  basic_text_oprimitive< std::stringstream >
{
    // permit serialization system priviledged access to permit
    // implementation of inline templates for maximum speed.
      friend class ::boost::archive::load_access; // for operator & : ARCHIVEInstance & object :  is ok
      #if BOOST_VERSION >= 104000
         friend class ::boost::archive::detail::archive_serializer_map<boost::archive::fw_xml_iarchive>;
      #else
         friend class ::boost::archive::detail::archive_pointer_iserializer<boost::archive::fw_xml_iarchive>;
      #endif


public:

     //// default saving of primitives.
    template<class T>
    void load(
            #ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
            const
            #endif
            T &t)
    {
        SLM_ASSERT("m_current not instanced", m_current);
        std::stringstream ss;
        ss << ::fwXML::XMLParser::getTextValue( m_current );
        ss >> const_cast<T &>(t);
    }

    /////////////////////////////////////////////////////////
    // fundamental types that need special treatment
    void load(const signed char &t);

    void load(const unsigned char &t);

    void load(const char &t);

    void load(const std::string &t);


    typedef detail::common_iarchive<fw_xml_iarchive> detail_common_oarchive;

    // Anything not an attribute and not a name-value pair is an
    // error and should be trapped here.
    template<class T>
    void load_override(
            #ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
            const
            #endif
            T & t, BOOST_PFTO int)
    {
#ifdef BOOST135ENABLED
        // If your program fails to compile here, its most likely due to
        // not specifying an nvp wrapper around the variable to
        // be serialized.
        BOOST_MPL_ASSERT((serialization::is_wrapper<T>));
        this->detail_common_oarchive::load_override(t, 0); // BOOST 1.35 API
#else
        BOOST_MPL_ASSERT((sizeof(T)==0)); // old boost API with no wrapper should no be enter here
#endif
    }

    template<class T>
    void load_override(
            #ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
            const
            #endif
          ::boost::serialization::nvp<T> & nvp,
            int
        )
    {
        assert( nvp.first  ); // if NULL it can be serialized via pointer !!!
        assert( nvp.second );

        if ( m_current == NULL )
        {
            m_current = m_root;
            archive::load( *this , nvp.value() );
        }
        else
        {
            SLM_ASSERT("m_current not instanced", m_current);
            xmlNodePtr child = findNode(  m_current ,nvp.first);
            m_current=(child?child:m_current); // if no child current stay unchanged typically <Dimension>3<Dimension/> elementtary type doesn't have child
            SLM_ASSERT("m_current not instanced", m_current);
#ifndef __MACOSX__
            assert( m_visitedNode.count( m_current)==0 ); // not alrady inserted
#endif
            m_visitedNode.insert( m_current );
            archive::load( *this , nvp.value());
            m_current = m_current->parent;
            //SLM_ASSERT("m_current not instanced", m_current);
        }
    }



    //// overload this functions : perform nothing
    void load_override(const version_type & t, BOOST_PFTO int)          {/* nothing todo */ }
    void load_override(const object_id_type & t, BOOST_PFTO int)        {/* nothing todo */ }
    void load_override(const object_reference_type & t, BOOST_PFTO int) {/* nothing todo */ }
    void load_override(const class_id_type & t, BOOST_PFTO int)         {/* nothing todo */ }
    void load_override(const class_id_reference_type & t, BOOST_PFTO int){/* nothing todo */}
    void load_override(const class_id_optional_type & t, BOOST_PFTO int){/* nothing todo */ }
    void load_override(const class_name_type &  t , BOOST_PFTO int)     {/* nothing todo */ }
    void load_override(const tracking_type & t, BOOST_PFTO int)         {/* nothing todo */ }


    template<class SHAREDP,class PRT>
    void reset( SHAREDP sp, PRT p )
    {
        SLM_TRACE("FIXMEME void reset( SHAREDP sp, p )");
    }


    void load_binary(void *address, std::size_t size);


    public:

    fw_xml_iarchive( unsigned int flags = 0 );

    ~fw_xml_iarchive();


    /**
    * @brief se the xml root node pointer , MUST be setted before call operator& !!!
    */
    void setXMLNode(xmlNodePtr boostRoot);


    protected :

    /// get the first child with given name of parent. This child must no be visited
    xmlNodePtr  findNode(  xmlNodePtr parent , std::string name );


    // the xml root of the archive
    xmlNodePtr m_root;

    /// the node where to read information ( internal use )
    xmlNodePtr m_current;

    std::set< xmlNodePtr > m_visitedNode;



};



} // namespace archive
} // namespace boost




#endif // BOOST_ARCHIVE_FW_XML_IARCHIVE_HPP
