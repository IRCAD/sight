/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "core/runtime/detail/io/ProfileReader.hpp"

#include "core/runtime/detail/io/Validator.hpp"
#include "core/runtime/detail/profile/Activater.hpp"
#include "core/runtime/detail/profile/Profile.hpp"
#include "core/runtime/operations.hpp"
#include "core/runtime/profile/Profile.hpp"
#include "core/runtime/Runtime.hpp"
#include "core/runtime/RuntimeException.hpp"

#include <libxml/parser.h>

#include <filesystem>
#include <sstream>
#include <string>

namespace sight::core::runtime
{

namespace detail
{

namespace io
{

std::string ProfileReader::ID("id");
std::string ProfileReader::NAME("name");
std::string ProfileReader::VALUE("value");
std::string ProfileReader::VERSION("version");
std::string ProfileReader::CHECK_SINGLE_INSTANCE("check-single-instance");
std::string ProfileReader::ACTIVATE("activate");
std::string ProfileReader::START("start");
std::string ProfileReader::PARAM("param");
std::string ProfileReader::DIS_EXT_PT("disable-extension-point");
std::string ProfileReader::DIS_EXT("disable-extension");

//------------------------------------------------------------------------------

namespace profile = core::runtime::detail::profile;

//------------------------------------------------------------------------------

std::shared_ptr<profile::Profile> ProfileReader::createProfile(const std::filesystem::path& path)
{
    // Normalizes the path.
    std::filesystem::path normalizedPath(std::filesystem::weakly_canonical(path));

    // Asserts that the repository is a valid directory path.
    if(std::filesystem::exists(normalizedPath) == false || std::filesystem::is_directory(normalizedPath) == true)
    {
        throw RuntimeException("'" + normalizedPath.string() + "': not a a file.");
    }

    // Validation
    auto profileXSDLocation = core::runtime::getLibraryResourceFilePath("core/profile.xsd");

    Validator validator(profileXSDLocation);

    if(validator.validate(normalizedPath) == false)
    {
        throw RuntimeException(validator.getErrorLog());
    }

    // Get the document.
    xmlDocPtr document = xmlParseFile(normalizedPath.string().c_str());
    if(document == nullptr)
    {
        throw RuntimeException("Unable to read the profile file.");
    }

    try
    {
        // Get the root node.
        xmlNodePtr rootNode = xmlDocGetRootElement(document);

        char* pName    = reinterpret_cast<char*>(xmlGetProp(rootNode, reinterpret_cast<const xmlChar*>(NAME.c_str())));
        char* pVersion =
            reinterpret_cast<char*>(xmlGetProp(rootNode, reinterpret_cast<const xmlChar*>(VERSION.c_str())));
        char* pChkInst =
            reinterpret_cast<char*>(xmlGetProp(
                                        rootNode,
                                        reinterpret_cast<const xmlChar*>(CHECK_SINGLE_INSTANCE.c_str())
            ));

        SIGHT_ASSERT("Application profile MUST have a name attribute", pName);
        SIGHT_ASSERT("Application profile MUST have a version attribute", pVersion);

        std::string sName(pName);
        std::string sVersion(pVersion);
        bool checkSingleInstance = pChkInst && std::string(pChkInst) == "true";

        xmlFree(pName);
        xmlFree(pVersion);
        xmlFree(pChkInst);

        // Creates and process the profile element.
        std::shared_ptr<profile::Profile> profile = processProfile(rootNode);

        profile->setFilePath(normalizedPath);
        profile->setName(sName);
        profile->setVersion(sVersion);
        profile->setCheckSingleInstance(checkSingleInstance);

        detail::profile::setCurrentProfile(profile);

        // Job's done!
        xmlFreeDoc(document);
        return profile;
    }
    catch(std::exception& exception)
    {
        xmlFreeDoc(document);
        throw exception;
    }
}

//------------------------------------------------------------------------------

std::shared_ptr<profile::Profile> ProfileReader::processProfile(xmlNodePtr node)
{
    using namespace profile;

    auto profile = std::dynamic_pointer_cast<detail::profile::Profile>(core::runtime::getCurrentProfile());

    // Process child nodes.
    xmlNodePtr curChild = node->children;
    for(curChild = node->children ; curChild != nullptr ; curChild = curChild->next)
    {
        if(xmlStrcmp(curChild->name, reinterpret_cast<const xmlChar*>(ACTIVATE.c_str())) == 0)
        {
            profile->add(ProfileReader::processActivater(curChild));
            continue;
        }

        if(xmlStrcmp(curChild->name, reinterpret_cast<const xmlChar*>(START.c_str())) == 0)
        {
            profile->addStarter(processStarter(curChild));
            continue;
        }
    }

    return profile;
}

//------------------------------------------------------------------------------

std::string ProfileReader::processStarter(xmlNodePtr node)
{
    // Processes all attributes.
    xmlAttrPtr curAttr;
    std::string identifier;
    std::string version;
    for(curAttr = node->properties ; curAttr != nullptr ; curAttr = curAttr->next)
    {
        if(xmlStrcmp(curAttr->name, reinterpret_cast<const xmlChar*>(ID.c_str())) == 0)
        {
            identifier = reinterpret_cast<const char*>(curAttr->children->content);
            continue;
        }

        if(xmlStrcmp(curAttr->name, reinterpret_cast<const xmlChar*>(VERSION.c_str())) == 0)
        {
            version = reinterpret_cast<const char*>(curAttr->children->content);
            continue;
        }
    }

    return identifier;
}

//------------------------------------------------------------------------------

std::shared_ptr<detail::profile::Activater> ProfileReader::processActivater(xmlNodePtr node)
{
    // Processes all attributes.
    xmlAttrPtr curAttr;
    std::string identifier;
    std::string version;
    for(curAttr = node->properties ; curAttr != nullptr ; curAttr = curAttr->next)
    {
        if(xmlStrcmp(curAttr->name, reinterpret_cast<const xmlChar*>(ID.c_str())) == 0)
        {
            identifier = reinterpret_cast<const char*>(curAttr->children->content);
            continue;
        }

        if(xmlStrcmp(curAttr->name, reinterpret_cast<const xmlChar*>(VERSION.c_str())) == 0)
        {
            version = reinterpret_cast<const char*>(curAttr->children->content);
            continue;
        }
    }

    // Creates the activater object.
    auto activater = std::make_shared<detail::profile::Activater>(identifier, version);

    // Processes child node that are the parameters
    xmlNodePtr curChild = node->children;
    for(curChild = node->children ; curChild != nullptr ; curChild = curChild->next)
    {
        if(xmlStrcmp(curChild->name, reinterpret_cast<const xmlChar*>(PARAM.c_str())) == 0)
        {
            processActivaterParam(curChild, activater);
            continue;
        }

        if(xmlStrcmp(curChild->name, reinterpret_cast<const xmlChar*>(DIS_EXT_PT.c_str())) == 0)
        {
            processActivaterDisableExtensionPoint(curChild, activater);
            continue;
        }

        if(xmlStrcmp(curChild->name, reinterpret_cast<const xmlChar*>(DIS_EXT.c_str())) == 0)
        {
            processActivaterDisableExtension(curChild, activater);
            continue;
        }
    }

    // Job's done.
    return activater;
}

//------------------------------------------------------------------------------

void ProfileReader::processActivaterParam(xmlNodePtr node, std::shared_ptr<detail::profile::Activater> activater)
{
    // Processes all attributes.
    xmlAttrPtr curAttr;
    std::string identifier;
    std::string value;
    for(curAttr = node->properties ; curAttr != nullptr ; curAttr = curAttr->next)
    {
        if(xmlStrcmp(curAttr->name, reinterpret_cast<const xmlChar*>(ID.c_str())) == 0)
        {
            identifier = reinterpret_cast<const char*>(curAttr->children->content);
            continue;
        }

        if(xmlStrcmp(curAttr->name, reinterpret_cast<const xmlChar*>(VALUE.c_str())) == 0)
        {
            value = reinterpret_cast<const char*>(curAttr->children->content);
            continue;
        }
    }

    // Stores the parameter into the activater.
    activater->addParameter(identifier, value);
}

//------------------------------------------------------------------------------

void ProfileReader::processActivaterDisableExtensionPoint(
    xmlNodePtr node,
    std::shared_ptr<detail::profile::Activater> activater
)
{
    // Processes all attributes.
    xmlAttrPtr curAttr;
    std::string identifier;
    for(curAttr = node->properties ; curAttr != nullptr ; curAttr = curAttr->next)
    {
        if(xmlStrcmp(curAttr->name, reinterpret_cast<const xmlChar*>(ID.c_str())) == 0)
        {
            identifier = reinterpret_cast<const char*>(curAttr->children->content);
            continue;
        }
    }

    // Stores the parameter into the activater.
    activater->addDisableExtensionPoint(identifier);
}

//------------------------------------------------------------------------------

void ProfileReader::processActivaterDisableExtension(
    xmlNodePtr node,
    std::shared_ptr<profile::Activater> activater
)
{
    // Processes all attributes.
    xmlAttrPtr curAttr;
    std::string identifier;
    for(curAttr = node->properties ; curAttr != nullptr ; curAttr = curAttr->next)
    {
        if(xmlStrcmp(curAttr->name, reinterpret_cast<const xmlChar*>(ID.c_str())) == 0)
        {
            identifier = reinterpret_cast<const char*>(curAttr->children->content);
            continue;
        }
    }

    // Stores the parameter into the activater.
    activater->addDisableExtension(identifier);
}

//------------------------------------------------------------------------------

} // namespace io

} // namespace detail

//------------------------------------------------------------------------------

std::shared_ptr<core::runtime::Profile> io::ProfileReader::createProfile(const std::filesystem::path& path)
{
    return detail::io::ProfileReader::createProfile(path);
}

} // namespace sight::core::runtime
