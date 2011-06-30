/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwXML/boostSerializer/archive/fw_xml_oarchive.hpp>

#ifndef WIN32
#if BOOST_VERSION >= 104000
 #include <boost/archive/impl/archive_serializer_map.ipp>
#else
 #include <boost/archive/impl/archive_pointer_oserializer.ipp>
#endif
 #include <boost/archive/impl/basic_text_oprimitive.ipp>
#endif



#include <sstream>
#include <iostream> // REMOVE ME FOR DEBUG ONLY

#include <fwCore/base.hpp>



//// explicit instanciation like xml_oarchive.cpp
#ifndef WIN32
  // VAG  this generate an internal compilator error under windows, to avoid this create
  // an external file. This class should is required if we want to save a pointer in the archive
  #if BOOST_VERSION >= 104000
     template
     class ::boost::archive::detail::archive_serializer_map<boost::archive::fw_xml_oarchive>;
  #else
     template
     class ::boost::archive::detail::archive_pointer_oserializer<boost::archive::fw_xml_oarchive>;
  #endif
#endif

// VAG disable this generate an insternal compilator error under windows, to avoid this create
// an external file. This class should be required if we want to save binary data in a UUENCODE form
#ifndef WIN32
template
class ::boost::archive::basic_text_oprimitive<  std::stringstream >;
#endif

// basic_text_oprimitive have save_binary method public but its constructor is protected :/
static std::stringstream sstreamFWXMLArchiveHelper;


namespace boost
{

namespace archive
{


/** @brief IRCAD R&D team framework ::boost::archive. The purpose of this class is to
 * create an xml archive based on the libxml instread a stream like in native boost
 * @author IRCAD (Research and Development Team).
 */

/////////////////////////////////////////////////////////
// fundamental types that need special treatment
void fw_xml_oarchive::save(const signed char t)
{
    short int t2 = static_cast<short int>(t);
    save(t2);
}

//------------------------------------------------------------------------------

void fw_xml_oarchive::save(const unsigned char t)
{
    short unsigned int t2 = static_cast<short unsigned int>(t);
    save(t2);
}

//------------------------------------------------------------------------------

void fw_xml_oarchive::save(const char t)
{
    short int t2 = static_cast<short int>(t);
    save(t2);
}

//------------------------------------------------------------------------------

// create a node named by name and append it to parent, return the new created one
xmlNodePtr fw_xml_oarchive::createNodeAndAppendTo(const std::string &name, xmlNodePtr parent)
{

    // create node
    xmlNodePtr newNode = xmlNewNode( NULL, BAD_CAST name.c_str() );
    // if no parent set create root if necessary else append to it
    if ( parent==NULL )
    {
        m_root = newNode;
        return m_root;
    }
    // here  parent is valid
    return xmlAddChild( parent, newNode );
}

//------------------------------------------------------------------------------

void fw_xml_oarchive::save_binary(void *address, std::size_t size)
{
    OSLM_WARN( " fw_xml_oarchive::save_binary() Disabled (Implemented versiondo not deal with large data)" );
    // TODO to continue
    //this->basic_text_oprimitive<std::stringstream >::save_binary( address, size );
    //xmlNodeSetContent( m_parent, BAD_CAST sstreamFWXMLArchiveHelper.str().c_str() );
}

//------------------------------------------------------------------------------

fw_xml_oarchive::fw_xml_oarchive( unsigned int flags) :  detail::common_oarchive< fw_xml_oarchive >(flags) ,
        //  basic_text_oprimitive< std::stringstream >(sstreamFWXMLArchiveHelper, false),
        m_root(NULL), m_parent(NULL)
{
}

//------------------------------------------------------------------------------

fw_xml_oarchive::~fw_xml_oarchive()
{
    // WARNING can change status of m_root then envalidate futur serialization !!!
    // xmlThrDefIndentTreeOutput(4);
    // xmlDocPtr pDoc = xmlNewDoc((const xmlChar*)"1.0");
    // // set root
    // xmlDocSetRootElement(pDoc , m_root );
    // std::stringstream ss ("/tmp/Debug-");
    // ss << this << ".xml";
    // xmlSaveFile( "-" , pDoc );
    // //xmlSaveFormatFileEnc( ss.str().c_str() , pDoc, "UTF-8", 1);
}

//------------------------------------------------------------------------------

xmlNodePtr fw_xml_oarchive::getXMLNode()
{
    // xmlDocPtr pDoc = xmlNewDoc((const xmlChar*)"1.0");
    // // set root
    // xmlDocSetRootElement(pDoc , m_root � );
    // xmlSaveFile( "-" , pDoc );
    SLM_ASSERT("m_root not instanced", m_root);
    return m_root;
}

} // namespace archive
} // namespace boost


