/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwXML/boostSerializer/archive/fw_xml_iarchive.hpp>

#ifndef WIN32
 #include <boost/archive/impl/archive_pointer_iserializer.ipp>
 #include <boost/archive/impl/basic_text_iprimitive.ipp>
#endif



#include <sstream>
#include <iostream> // REMOVE ME FOR DEBUG ONLY

#include <fwCore/base.hpp>




//// explicit instanciation like xml_iarchive.cpp
#ifndef WIN32
  // VAG  this generate an insternal compilator error under windows, to avoid this create
  // an external file. This class should is required if we want to load a pointer in the archive
  template
  class boost::archive::detail::archive_pointer_iserializer<boost::archive::fw_xml_iarchive>;
#endif

// VAG disable this generate an insternal compilator error under windows, to avoid this create
// an external file. This class should be required if we want to load binary data in a UUENCODE form
#ifndef WIN32
template
class boost::archive::basic_text_iprimitive<  std::stringstream >;
#endif

// basic_text_oprimitive have load_binary method public but its constructor is protected :/
static std::stringstream sstreamFWXMLArchiveHelper;


namespace boost {
namespace archive {


/** @brief IRCAD R&D team framework boost::archive. The purpose of this class is to
 * create an xml archive based on the libxml instread a stream like in native boost
 * @author IRCAD (Research and Development Team).
 */



/////////////////////////////////////////////////////////
// fundamental types that need special treatment
void fw_xml_iarchive::load(const signed char &t)
{
   short int t2;
   load(t2);
   const_cast< signed char &>(t) = t2;
}

void fw_xml_iarchive::load(const unsigned char &t)
{
   short unsigned int t2;
   load(t2);
   const_cast< unsigned char &>(t) = t2;
}

void fw_xml_iarchive::load(const char &t)
{
    short int t2;
    load(t2);
    const_cast< char &>(t) = t2;
}

/////////////////////////////////////////////////////////
// fundamental types that need special treatment
void fw_xml_iarchive::load(const std::string &t)
{
    assert(m_current);
    const_cast<std::string &>(t) = ::fwXML::XMLParser::getTextValue( m_current );
//  std::cout << "STRINGvoid load( T &t))0 XMLNODESTART" <<std::endl;
//  std::cout << ::fwXML::XMLParser::toString( m_current );
//  std::cout << " XMLNODEEND\n \nreaded string " << t << std::endl;
}




xmlNodePtr  fw_xml_iarchive::findNode(  xmlNodePtr start , std::string name )
{
    xmlNodePtr elt = start->children; // we parse elements
    while( elt != NULL)
    {
        if (  !xmlStrcmp( elt->name, BAD_CAST name.c_str() ) && m_visitedNode.count(elt) == 0 )
        {
            return elt;
        }
        elt=elt->next;
    }
    return elt; // node not found
}



void fw_xml_iarchive::load_binary(void *address, std::size_t size)
{
    OSLM_FATAL( " fw_xml_iarchive::load_binary() Disabled (Implemented versiondo not deal with large data)" );
    assert(false);
}



fw_xml_iarchive::fw_xml_iarchive( unsigned int flags) :  detail::common_iarchive< fw_xml_iarchive >(flags) ,
//  basic_text_oprimitive< std::stringstream >(sstreamFWXMLArchiveHelper, false),
m_root(NULL), m_current(NULL)
{
    OSLM_DEBUG( " fw_xml_iarchive::Contructor" );
}


fw_xml_iarchive::~fw_xml_iarchive()
{
}

void fw_xml_iarchive::setXMLNode(xmlNodePtr boostRoot)
{
    m_root =  boostRoot;
    m_visitedNode.clear();
}



} // namespace archive
} // namespace boost


