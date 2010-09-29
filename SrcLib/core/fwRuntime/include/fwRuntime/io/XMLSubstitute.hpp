/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#ifndef XMLSUBSTITUTE_HPP_
#define XMLSUBSTITUTE_HPP_

#include <boost/shared_ptr.hpp>
#include <boost/filesystem/path.hpp>

#include <fwRuntime/config.hpp>

#include <map>
#include <list>
#include <string>

#include <libxml/tree.h>


namespace fwRuntime
{
namespace io
{

/// Helper class to contain substitution rules
struct FWRUNTIME_CLASS_API Substitute
{
    std::string xpath; /// path to the XML node to patch
    std::string dictEntry; /// dictionary Entry to retrieve the new value. This value is used to replace the old one
    std::string status; // dictEntry presence requirement = "optional" , "required"
};

class FWRUNTIME_CLASS_API XMLSubstitute
{

public:

    FWRUNTIME_API static ::boost::shared_ptr< ::fwRuntime::io::XMLSubstitute > getDefault();

    /// default destructor
    FWRUNTIME_API virtual ~XMLSubstitute();

    FWRUNTIME_API std::map< std::string, std::string > &getRefDictionary();

    /**
     * @brief XML substitution function
     * From From substitution rules and a dictionary for values translation the XMLorignal node is patched
     * @param[in,out] original the XML tree to patch. The modification occurs in place
     * @param[in] substitutionRules the rules of substitution (in XML , see exemple ). Each substitution contain a :
     * \li nodePath : to localize the XML node ( element, attribute, text) to patch ( <nodePath> node in XML)
     * \li rules : to define how to replace the value node using dictionary["attribute of replace node dictEntry attribut"] ( <replace> node in XML)
     * @note  dictEntry can be "optional" or "required" - status attribut in <replace> node :
     * \li if dictEntry is missing and status="optional" : translation are ignored
     * \li if dictEntry is missing and status="required" : the application stop in FATAL mode
     *
     * example: the XML code
     * \verbatim
          <object id="tutoDataServiceBasicImage" type="::fwData::Image">
            <service implementation="::ioVTK::ImageReaderService" type="::io::IReader" uid="myReaderPathFile" autoComChannel="no" >
                <filename id="./TutoData/patient1.vtk"/>
            </service>
            <service type="::gui::frame::IFrame" implementation="::gui::frame::MainFrame" autoComChannel="no" >
                <name>tutoDataServiceBasic</name>
                <icon>Bundles/Tuto_0-1/tuto.ico</icon>
                <views>
                    <view guiContainerId="900" />
                </views>
            </service>
            ....
        \endverbatim
        with this XML substitution rules :
         \verbatim
        <Substitutions>
          <substitute>
             <nodePath>//object[@id='tutoDataServiceBasicImage']/service[@uid='myReaderPathFile']/filename/@id</nodePath>
             <replace dictEntry="imageFile" type="path" status="required" />
          </substitute>
          <substitute>
            <nodePath>//icon/text()</nodePath>
             <replace dictEntry="NAME" type="string" status="optional" />
          </substitute>
        </Substitutions>
         \endverbatim
         with dictionary["imageFile"]="OtherImage.vtk" and   dictionary["NAME"]="Other.ico" will provide :
         \verbatim
          <object id="tutoDataServiceBasicImage" type="::fwData::Image">
            <service implementation="::ioVTK::ImageReaderService" type="::io::IReader" uid="myReaderPathFile" autoComChannel="no" >
                <filename id="OtherImage.vtk"/>
            </service>
            <service type="::gui::frame::IFrame" implementation="::gui::frame::MainFrame" autoComChannel="no" >
                <name>tutoDataServiceBasic</name>
                <icon>Other.ico</icon>
                <views>
                    <view guiContainerId="900" />
                </views>
            </service>
            ....
        \endverbatim
     */
    FWRUNTIME_API static void substitute( xmlNodePtr original, xmlNodePtr substitutionRules, std::map< std::string, std::string > &dictionary);


    /**
     * @brief parse substitution rules (in XML) and provide a list  (STL container)
     * @param[in] substitutionRules the node containing XML substitution rules
     * @return a list of Substitute classes
     */
    FWRUNTIME_API static std::list< ::fwRuntime::io::Substitute > getSubstitutions( xmlNodePtr substitutionRules );

    FWRUNTIME_API xmlDocPtr load( const ::boost::filesystem::path& xmlFile);

private :

    XMLSubstitute();

    static ::boost::shared_ptr< ::fwRuntime::io::XMLSubstitute > m_instance;

    std::map< std::string, std::string > m_dictionary;

}; // XMLSubstitute

}
}


#endif /* XMLSUBSTITUTE_HPP_ */
