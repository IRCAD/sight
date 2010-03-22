/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#ifndef BOOST_ARCHIVE_FW_XML_OARCHIVE_HPP
#define BOOST_ARCHIVE_FW_XML_OARCHIVE_HPP

#include <ostream>
#include <sstream>

// default implementation
#include <boost/version.hpp>
#include <boost/archive/detail/oserializer.hpp>
#include <boost/archive/detail/interface_oarchive.hpp>
#include <boost/archive/detail/common_oarchive.hpp>

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

#include "fwXML/XML/XMLParser.hpp"



#include <sstream>
#include <iostream> // REMOVE ME DEBUG

namespace boost
{
namespace archive
{


/** @brief IRCAD R&D team framework boost::archive. The purpose of this class is to
 * create an xml archive based on the libxml instead a stream like in native boost
 * @author IRCAD (Research and Development Team).
 */


class fw_xml_oarchive : public detail::common_oarchive< fw_xml_oarchive > // , public  basic_text_oprimitive< std::stringstream >
{
    // permit serialization system priviledged access to permit
    // implementation of inline templates for maximum speed.
      friend class boost::archive::save_access; // for operator & : ARCHIVEInstance & object :  is ok
      friend class boost::archive::detail::archive_pointer_oserializer<boost::archive::fw_xml_oarchive>;


public:

     //// default saving of primitives.
    template<class T>
    void save( T &t)
    {
        std::stringstream ss;
        ss << t;
        xmlNodeAddContent( m_parent, xmlCharStrdup( ss.str().c_str()) );
    }

    /////////////////////////////////////////////////////////
    // fundamental types that need special treatment
    void save(const signed char t);

    void save(const unsigned char t);

    void save(const char t);


    typedef detail::common_oarchive<fw_xml_oarchive> detail_common_oarchive;

    // Anything not an attribute and not a name-value pair is an
    // error and should be trapped here.
    template<class T>
    void save_override(
#ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
            const
#endif
    T & t, BOOST_PFTO int)
    {
#ifdef BOOST135ENABLED
        // If your program fails to compile here, its most likely due to
        // not specifying an nvp wrapper around the variable to
        // be serialized.
        BOOST_MPL_ASSERT((serialization::is_wrapper<T>));//
        this->detail_common_oarchive::save_override(t, 0); // BOOST 1.35 API
#else
        BOOST_MPL_ASSERT((sizeof(T)==0)); // old boost API with no wrapper should no be enter here
#endif
    }

    template<class T>
    void save_override(
#ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
                const
#endif
        ::boost::serialization::nvp<T> & nvp,
         int
        )
    {
            assert( nvp.first  ); // if NULL it can be serialized via pointer !!! 07102008 can be null if pass throw ptr
    //#6  0xb71f389a in boost::archive::detail::pointer_oserializer<boost::archive::fw_xml_oarchive, ::fwData::Node>::save_object_ptr (this=0xb72ab4d0,
    //    ar=@0xbffbce18, x=0x80fb3e0) at /usr/include/boost/archive/detail/oserializer.hpp:204
    //204         ar_impl << boost::serialization::make_nvp(NULL, * t);

            assert( nvp.second );

            // save parent
            xmlNodePtr old_parent = m_parent;

            // create new node and set new parent
            m_parent = createNodeAndAppendTo(  nvp.first , m_parent );

            archive::save( *this , nvp.const_value());

            // restore old parent
            m_parent = old_parent;
    }

    // override NVP function : note WinSux does not deal with Partial Function Template Ordering !!!!
    template< class R>
    void save_override( boost::serialization::nvp< R > &nvp , BOOST_PFTO int)
    {
        assert( nvp.first  ); // if NULL it can be serialized via pointer !!!
        assert( nvp.second );

        // save parent
        xmlNodePtr old_parent = m_parent;

        // create new node and set new parent
        m_parent = createNodeAndAppendTo(  nvp.first , m_parent );
        // seg fault xmlXIncludeProcessTree( m_parent );

        // save sub value
        archive::save(* this->This(), nvp.const_value());

        // restore old parent
        m_parent = old_parent;
    }


    //// overload this functions : perform nothing
    void save_override(const version_type & t, BOOST_PFTO int)          {/* nothing todo */ }
    void save_override(const object_id_type & t, BOOST_PFTO int)        {/* nothing todo */ }
    void save_override(const object_reference_type & t, BOOST_PFTO int) {/* nothing todo */ }
    void save_override(const class_id_type & t, BOOST_PFTO int)         {/* nothing todo */ }
    void save_override(const class_id_reference_type & t, BOOST_PFTO int){/* nothing todo */}
    void save_override(const class_id_optional_type & t, BOOST_PFTO int){/* nothing todo */ }
    void save_override(const class_name_type &  t , BOOST_PFTO int)     {/* nothing todo */ }
    void save_override(const tracking_type & t, BOOST_PFTO int)         {/* nothing todo */ }


    // create a node named by name and append it to parent, return the new created one
    xmlNodePtr createNodeAndAppendTo(const std::string &name, xmlNodePtr parent);

    void save_binary(void *address, std::size_t size);

//    BOOST_ARCHIVE_OR_WARCHIVE_DECL(void)
//    init();

public:

    fw_xml_oarchive( unsigned int flags = 0);

    ~fw_xml_oarchive();


    /**
     * @brief return the xml node pointer
     */
    xmlNodePtr getXMLNode();


protected :

    // the xml root of the archive
    xmlNodePtr m_root;

    /// the node where to attach(child) information ( internal use )
    xmlNodePtr m_parent;

};


} // namespace archive
} // namespace boost




#endif // BOOST_ARCHIVE_FW_XML_OARCHIVE_HPP
