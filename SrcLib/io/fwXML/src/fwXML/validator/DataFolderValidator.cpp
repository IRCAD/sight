#include <fwCore/base.hpp>

#include <boost/version.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>

#include "fwXML/validator/DataFolderValidator.hpp"

#include <iostream>

namespace fwXML
{

DataFolderValidator::DataFolderValidator()
{
}


DataFolderValidator::~DataFolderValidator()
{
}


/// erase schema and search them on given folder
void DataFolderValidator::collecteSchema(const boost::filesystem::path &folder)
{
    namespace fs = boost::filesystem;

    assert( fs::is_directory(folder) );

    fs::directory_iterator end_iter;
    for ( fs::directory_iterator dir_itr( folder );   dir_itr != end_iter;   ++dir_itr )
    {
#if BOOST_VERSION < 103600
        if (fs::extension( dir_itr->leaf()  ) == ".xsd" )
#else
        if (fs::extension( dir_itr->filename()  ) == ".xsd" )
#endif
        {
            boost::filesystem::path p=*dir_itr;
            registerSchema( *dir_itr );
        }
    }
}


/// append schema
void DataFolderValidator::registerSchema(const boost::filesystem::path &xsdfilename)
{
    std::string className = boost::filesystem::basename( xsdfilename.leaf() );

    OSLM_INFO("Registring schema from " << xsdfilename.string() << " for class " << className );
    m_schemaPath[className] = xsdfilename;

}


void DataFolderValidator::clearErrorLog()
{
    m_errorLog.clear();
}


const std::string DataFolderValidator::getErrorLog() const
{
    return m_errorLog;
}


//const bool DataFolderValidator::validate( const boost::filesystem::path & xmlFile )
//{
//
//
//}



const bool DataFolderValidator::validate( xmlNodePtr node )
{
    bool result = validateSingle(node);
    node = node->children;
    while ( result && node)
    {
        if ( node->type != XML_ELEMENT_NODE )
        {
            node = node->next;
        }
        else
        {
            // create a sub doc to validate
            // see http://mail.gnome.org/archives/xml/2006-May/msg00094.html for pb detail
            xmlDocPtr  doc = xmlNewDoc(BAD_CAST "1.0");
            xmlNodePtr newSubNode =  xmlCopyNode(node,1);
            xmlDocSetRootElement(doc, newSubNode );

            result &= validate( newSubNode );

            xmlFreeDoc(doc);

            if ( result==false )
            {
                return false; // with correct errlog setted
            }
            node = node->next;
        }
    }

    return result;
}



const bool DataFolderValidator::validateSingle( xmlNodePtr node )
{
    assert(node);
    std::string key( (const char*)(node->name) );
    boost::filesystem::path xsdPath = m_schemaPath[key];

    OSLM_DEBUG("key" << key << " - name : " << xsdPath.string());

    if ( !xsdPath.empty() )
    {
        //xmlNodePtr newNode = xmlCopyNode(node,1);
        ::fwRuntime::io::Validator validator(xsdPath);
        OSLM_INFO(" DataFolderValidator::validateSingle " << key );

        //xmlSaveFile( "debug.xml", (xmlDocPtr )node);

        bool result = validator.validate(node);
        m_errorLog = validator.getErrorLog();
        OSLM_INFO(" DataFolderValidator " << key << " validation result(OK=1)=" << result);
        return result;
    }
    else
    {
        OSLM_WARN("No schema validation for XML node " << key << " : assuming content correct" );
        return true;
    }
}


void DataFolderValidator::copySchema(const boost::filesystem::path &dstFolder, const boost::filesystem::path &srcFolder  )
{
    namespace fs = boost::filesystem;

    // set the correct source
    // * srcFolder if specified
    // share/fwXML-XXX/ else

    boost::filesystem::path sourcePath;
    if ( srcFolder.empty() )
    {
#ifndef FWXML_VER
        sourcePath =  fs::current_path() / "share/fwXML_0-0" ;
#else
        std::ostringstream pathLocation;
        pathLocation << "share/fwXML_" <<  FWXML_VER;
        sourcePath = fs::current_path() / pathLocation.str();
#endif
    }
    else
    {
        sourcePath = srcFolder;
    }


    // copy all XSD file from srcFolder to dstFolder
    assert( fs::is_directory(sourcePath) );
    assert( fs::is_directory(dstFolder) );

    fs::directory_iterator end_iter;
    for ( fs::directory_iterator dir_itr( sourcePath );   dir_itr != end_iter;   ++dir_itr )
    {

        if ( !fs::is_directory( *dir_itr ) )
        {
#if BOOST_VERSION < 103600
            if (fs::extension( dir_itr->leaf()  ) == ".xsd" )
#else
            if (fs::extension( dir_itr->filename()  ) == ".xsd" )
#endif
            {
#if BOOST_VERSION < 103600
                fs::path dstFile = dstFolder / dir_itr->leaf();
#else
                fs::path dstFile = dstFolder / dir_itr->filename();
#endif

                if (  fs::exists(dstFile ) )
                {
                    fs::remove( dstFile ); // allow overwritting schema
                }
                fs::copy_file( *dir_itr,  dstFile );
            }
        }
    }
}




}
