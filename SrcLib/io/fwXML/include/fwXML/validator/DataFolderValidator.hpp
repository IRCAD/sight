/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef DATAFOLDERVALIDATOR_H_
#define DATAFOLDERVALIDATOR_H_

#include <map>
#include <boost/filesystem/path.hpp>
#include <fwRuntime/io/Validator.hpp>

namespace fwXML
{

/**
 * @brief the purpose of this class is to provide a way to validate DataFolder
 * @author IRCAD (Research and Development Team).
 */
class DataFolderValidator
{
public:
    DataFolderValidator();

    virtual ~DataFolderValidator();

    /// erase schema and search them on given folder
    void collecteSchema(const ::boost::filesystem::path &folder);

    /// append schema to schema validation list
    void registerSchema(const ::boost::filesystem::path &xsdfilename);

    /// clear error Log
    void clearErrorLog();

    /// get errorLog
    const std::string getErrorLog() const;

    /// validate node using all registred schema
    const bool validate( xmlNodePtr node );

    /**
     * @brief install schema in a dataFolder
     * @param dstFolder : DataFolder path to install schema
     * @param srcFolder ( optional ) if specified copy all XSD file from srcFolder to dstFolder else use default Folder aka share/fwXMLXXX
     */
    static void copySchema(const ::boost::filesystem::path &dstFolder, const ::boost::filesystem::path &srcFolder =  ::boost::filesystem::path() );

protected :

    /// validate node using "node".xsd schema
    const bool validateSingle( xmlNodePtr node );

    /// store className <-> xsd location
    std::map< std::string, ::boost::filesystem::path > m_schemaPath;

    /// store log from validators
    std::string m_errorLog;
};

}

#endif /*DATAFOLDERVALIDATOR_H_*/
