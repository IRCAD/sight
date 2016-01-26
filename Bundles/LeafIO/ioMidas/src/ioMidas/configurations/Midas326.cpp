/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <string>

#include <boost/lexical_cast.hpp>

#include <midasIO/IConfiguration.hpp>
#include <midasIO/registry/macros.hpp>

namespace ioMidas
{

namespace configurations
{

class Midas326 : public ::midasIO::IConfiguration
{
public:
    Midas326(::midasIO::IConfiguration::Key key)
    {
    }
    ~Midas326()
    {
    }

    virtual std::string getApiKeyUrl() const;
    virtual std::string getTokenUrl() const;
    virtual std::string getApiPathUrl() const;
    virtual std::string getCommunityListUrl() const;

    virtual std::string getCreateFolderUrl(
        const std::string& name, const std::string& desc, const std::string& parentFolderId) const;

    virtual std::string getFolderChildrenUrl(const std::string& folderId) const;

    virtual std::string getUploadTokenUrl(const std::string& itemId, const std::string& filename) const;

    virtual std::string getCreateItemUrl(const std::string& parentId, const std::string& name) const;

    virtual std::string getPerformUploadUrl(
        const std::string& token, const std::string& filename, const std::string &itemId, const int &length) const;

    virtual std::string getSearchItemByNameUrl(const std::string& itemName) const;

    virtual std::string getSearchItemByIdUrl(const std::string& itemId) const;

    virtual std::string getSetLicenseUrl(const std::string& itemId, const int& licenseId) const;

    virtual std::string getMidasVersion() const;
};

// Register Midas326 class as an IConfiguration for Midas version "3.2.6"
MidasIOConfigurationRegisterMacro( ::ioMidas::configurations::Midas326, "3.2.6" );

//---------------------------------------------------------------------------------------------------

std::string Midas326::getApiKeyUrl() const
{
    return m_attrUrl + getApiPathUrl() + "midas.user.apikey.default"
           + "&email=" + m_attrEmail
           + "&password=" + m_attrPassword;
}

//---------------------------------------------------------------------------------------------------

std::string Midas326::getTokenUrl() const
{
    return m_attrUrl + getApiPathUrl() + "midas.login"
           + "&email=" + m_attrEmail
           + "&appname=" + m_attrAppName
           + "&apikey=" + m_attrApiKey;
}

//---------------------------------------------------------------------------------------------------

std::string Midas326::getApiPathUrl() const
{
    return "/api/json?method=";
}

//---------------------------------------------------------------------------------------------------

std::string Midas326::getCommunityListUrl() const
{
    return m_attrUrl + getApiPathUrl() + "midas.community.list&token=" + m_attrToken;
}

//---------------------------------------------------------------------------------------------------

std::string Midas326::getCreateFolderUrl(
    const std::string& name, const std::string& description, const std::string& parentFolderId) const
{
    OSLM_ASSERT("Privacy '" << m_attrPrivacy << "' not found",
                s_PRIVACY_NAMES.find(m_attrPrivacy) != s_PRIVACY_NAMES.end());

    const std::string& privacy = s_PRIVACY_NAMES.find(m_attrPrivacy)->second;

    return m_attrUrl + getApiPathUrl() + "midas.folder.create"
           + "&token=" + m_attrToken
           + "&name=" + name
           + (!description.empty() ? ("&description=" + description) : "")
           + "&privacy=" + privacy
           + "&parentid=" + parentFolderId;
}

//---------------------------------------------------------------------------------------------------

std::string Midas326::getFolderChildrenUrl(const std::string& folderId) const
{
    return m_attrUrl + getApiPathUrl() + "midas.folder.children"
           + "&token=" + m_attrToken
           + "&id=" + folderId;
}

//---------------------------------------------------------------------------------------------------

std::string Midas326::getCreateItemUrl(const std::string& parentId, const std::string& name) const
{
    return m_attrUrl + getApiPathUrl() + "midas.item.create"
           + "&token=" + m_attrToken
           + "&parentid=" + parentId
           + "&name=" + name;
}

//---------------------------------------------------------------------------------------------------

std::string Midas326::getUploadTokenUrl(const std::string& itemId, const std::string& filename) const
{
    return m_attrUrl + getApiPathUrl() + "midas.upload.generatetoken"
           + "&token=" + m_attrToken
           + "&itemid=" + itemId
           + "&filename=" + filename;
}

//---------------------------------------------------------------------------------------------------

std::string Midas326::getPerformUploadUrl(
    const std::string& token, const std::string& filename, const std::string& itemId, const int& length) const
{
    return m_attrUrl + getApiPathUrl() + "midas.upload.perform"
           + "&uploadtoken=" + token
           + "&filename=" + filename
           + "&itemid=" + itemId
           + "&length=" + ::boost::lexical_cast< std::string >(length);
}

//---------------------------------------------------------------------------------------------------

std::string Midas326::getSearchItemByNameUrl(const std::string& itemName) const
{
    return m_attrUrl + getApiPathUrl() + "midas.item.searchbyname"
           + "&token=" + m_attrToken
           + "&name=" + itemName;
}

//---------------------------------------------------------------------------------------------------

std::string Midas326::getSearchItemByIdUrl(const std::string& itemId) const
{
    return m_attrUrl + getApiPathUrl() + "midas.item.get"
           + "&token=" + m_attrToken
           + "&id=" + itemId;
}

//---------------------------------------------------------------------------------------------------

std::string Midas326::getSetLicenseUrl(const std::string& itemId, const int& licenseId) const
{
    return m_attrUrl + getApiPathUrl() + "midas.item.set.license"
           + "&token="  + m_attrToken
           + "&item_id=" + itemId
           + "&license=" + ::boost::lexical_cast< std::string >(licenseId);
}

//---------------------------------------------------------------------------------------------------

std::string Midas326::getMidasVersion() const
{
    return "3.2.6";
}

} // namespace configurations

} // namespace ioMidas

