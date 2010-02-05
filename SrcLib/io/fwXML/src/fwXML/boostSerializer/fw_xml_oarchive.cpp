#include <fwXML/boostSerializer/archive/fw_xml_oarchive.hpp>

#ifndef WIN32
 #include <boost/archive/impl/archive_pointer_oserializer.ipp>
 #include <boost/archive/impl/basic_text_oprimitive.ipp>
#endif



#include <sstream>
#include <iostream> // REMOVE ME FOR DEBUG ONLY

#include <fwCore/base.hpp>




//// explicit instanciation like xml_oarchive.cpp
#ifndef WIN32
  // VAG  this generate an insternal compilator error under windows, to avoid this create
  // an external file. This class should is required if we want to save a pointer in the archive
  template
  class boost::archive::detail::archive_pointer_oserializer<boost::archive::fw_xml_oarchive>;
#endif

// VAG disable this generate an insternal compilator error under windows, to avoid this create
// an external file. This class should be required if we want to save binary data in a UUENCODE form
#ifndef WIN32
template
class boost::archive::basic_text_oprimitive<  std::stringstream >;
#endif

// basic_text_oprimitive have save_binary method public but its constructor is protected :/
static std::stringstream sstreamFWXMLArchiveHelper;


namespace boost {
namespace archive {


/** @brief IRCAD R&D team framework boost::archive. The purpose of this class is to
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

    void fw_xml_oarchive::save(const unsigned char t)
    {
       short unsigned int t2 = static_cast<short unsigned int>(t);
       save(t2);
    }

    void fw_xml_oarchive::save(const char t)
    {
        short int t2 = static_cast<short int>(t);
        save(t2);
    }




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
        //std::cout << "createNodeAndAppendTo(" << name << "\"" << "parent=" << parent << std::endl;
        return xmlAddChild( parent, newNode );
    }



    void fw_xml_oarchive::save_binary(void *address, std::size_t size)
    {
        OSLM_WARN( " fw_xml_oarchive::save_binary() Disabled (Implemented versiondo not deal with large data)" );
        // TODO to continue
//      this->basic_text_oprimitive<std::stringstream >::save_binary( address, size );
//      //std::cout <<sstreamFWXMLArchiveHelper.str();
//      xmlNodeSetContent( m_parent, BAD_CAST sstreamFWXMLArchiveHelper.str().c_str() );
    }



    fw_xml_oarchive::fw_xml_oarchive( unsigned int flags) :  detail::common_oarchive< fw_xml_oarchive >(flags) ,
//  basic_text_oprimitive< std::stringstream >(sstreamFWXMLArchiveHelper, false),
    m_root(NULL), m_parent(NULL)
    {
    }


    fw_xml_oarchive::~fw_xml_oarchive()
    {
        // WARNING can change status of m_root then envalidate futur serialization !!!
//      xmlThrDefIndentTreeOutput(4);
//      xmlDocPtr pDoc = xmlNewDoc((const xmlChar*)"1.0");
//      // set root
//      xmlDocSetRootElement(pDoc , m_root );
//      std::stringstream ss ("/tmp/Debug-");
//      ss << this << ".xml";
//      xmlSaveFile( "-" , pDoc );
//      //xmlSaveFormatFileEnc( ss.str().c_str() , pDoc, "UTF-8", 1);
    }

    xmlNodePtr fw_xml_oarchive::getXMLNode()
    {
//      xmlDocPtr pDoc = xmlNewDoc((const xmlChar*)"1.0");
//      // set root
//      xmlDocSetRootElement(pDoc , m_root � );
//      xmlSaveFile( "-" , pDoc );
        assert( m_root );
        return m_root;
    }



} // namespace archive
} // namespace boost


