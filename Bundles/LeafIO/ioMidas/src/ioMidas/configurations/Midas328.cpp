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

class Midas328 : public ::midasIO::IConfiguration
{
public:
    Midas328(::midasIO::IConfiguration::Key key)
    {
    }
    ~Midas328()
    {
    }

    /// POST
    virtual std::string getApiKeyUrl() const;

    /// Login process (GET)
    virtual std::string getTokenUrl() const;

    virtual std::string getApiPathUrl() const;

    /// GET
    virtual std::string getCommunityListUrl() const;

    /// POST
    virtual std::string getCreateFolderUrl(
        const std::string& name, const std::string& desc, const std::string& parentFolderId) const;

    /// GET
    virtual std::string getFolderChildrenUrl(const std::string& folderId) const;

    /// GET
    virtual std::string getUploadTokenUrl(const std::string& itemId, const std::string& filename) const;

    /// POST
    virtual std::string getCreateItemUrl(const std::string& parentId, const std::string& name) const;

    /// POST or PUT
    virtual std::string getPerformUploadUrl(
        const std::string& token, const std::string& filename, const std::string &itemId, const int &length) const;

    /// GET
    virtual std::string getSearchItemByNameUrl(const std::string& itemName) const;

    /// GET
    virtual std::string getSearchItemByIdUrl(const std::string& itemId) const;

    virtual std::string getSetLicenseUrl(const std::string& itemId, const int& licenseId) const;

    virtual std::string getMidasVersion() const;
};

// Registers Midas328 class as an IConfiguration for Midas version "3.2.8"
MidasIOConfigurationRegisterMacro( ::ioMidas::configurations::Midas328, "3.2.8" );

//---------------------------------------------------------------------------------------------------

std::string Midas328::getApiKeyUrl() const
{
    return m_attrUrl + getApiPathUrl() + "/system/defaultapikey?"
           + "email=" + m_attrEmail
           + "&password=" + m_attrPassword;
}

//---------------------------------------------------------------------------------------------------

std::string Midas328::getTokenUrl() const
{
    return m_attrUrl + getApiPathUrl() + "/system/login?"
           + "email=" + m_attrEmail
           + "&appname=" + m_attrAppName
           + "&apikey=" + m_attrApiKey;
}

//---------------------------------------------------------------------------------------------------

std::string Midas328::getApiPathUrl() const
{
    return "/rest";
}

//---------------------------------------------------------------------------------------------------

std::string Midas328::getCommunityListUrl() const
{
    return m_attrUrl + getApiPathUrl() + "/community?token=" + m_attrToken;
}

//---------------------------------------------------------------------------------------------------

std::string Midas328::getCreateFolderUrl(
    const std::string& name, const std::string& description, const std::string& parentFolderId) const
{
    OSLM_ASSERT("Privacy '" << m_attrPrivacy << "' not found",
                s_PRIVACY_NAMES.find(m_attrPrivacy) != s_PRIVACY_NAMES.end());

    const std::string& privacy = s_PRIVACY_NAMES.find(m_attrPrivacy)->second;

    return m_attrUrl + getApiPathUrl() + "/folder"
           + "?token=" + m_attrToken
           + "&name=" + name
           + (!description.empty() ? ("&description=" + description) : "")
           + "&privacy=" + privacy
           + "&parentid=" + parentFolderId
           + "&reuseExisting=true";
}

//---------------------------------------------------------------------------------------------------

std::string Midas328::getFolderChildrenUrl(const std::string& folderId) const
{
    return m_attrUrl + getApiPathUrl() + "/folder/children/" + folderId + "?token=" + m_attrToken;
}

//---------------------------------------------------------------------------------------------------

std::string Midas328::getCreateItemUrl(const std::string& parentId, const std::string& name) const
{
    return m_attrUrl + getApiPathUrl() + "/item"
           + "?token=" + m_attrToken
           + "&parentid=" + parentId
           + "&name=" + name;
}

//---------------------------------------------------------------------------------------------------

std::string Midas328::getUploadTokenUrl(const std::string& itemId, const std::string& filename) const
{
    return m_attrUrl + getApiPathUrl() + "/system/uploadtoken"
           + "?token=" + m_attrToken
           + "&itemid=" + itemId
           + "&filename=" + filename;
}

//---------------------------------------------------------------------------------------------------

std::string Midas328::getPerformUploadUrl(
    const std::string& token, const std::string& filename, const std::string& itemId, const int& length) const
{
    // itemId not needed in Midas 3.2.9 : specified when generating upload token
    return m_attrUrl + getApiPathUrl() + "/system/upload"
           + "?uploadtoken=" + token
           + "&filename=" + filename
           + "&length=" + ::boost::lexical_cast< std::string >(length);
}

//---------------------------------------------------------------------------------------------------

std::string Midas328::getSearchItemByNameUrl(const std::string& itemName) const
{
    return m_attrUrl + getApiPathUrl() + "/item/search"
           + "?token=" + m_attrToken
           + "&name=" + itemName;
}

//---------------------------------------------------------------------------------------------------

std::string Midas328::getSearchItemByIdUrl(const std::string& itemId) const
{
    return m_attrUrl + getApiPathUrl() + "/item/" + itemId + "?token=" + m_attrToken;
}

//---------------------------------------------------------------------------------------------------

std::string Midas328::getSetLicenseUrl(const std::string& itemId, const int& licenseId) const
{
    return m_attrUrl + getApiPathUrl() + "midas.item.set.license"
           + "&token="  + m_attrToken
           + "&item_id=" + itemId
           + "&license=" + ::boost::lexical_cast< std::string >(licenseId);
}

//---------------------------------------------------------------------------------------------------

std::string Midas328::getMidasVersion() const
{
    return "3.2.8";
}

} // namespace configurations

} // namespace ioMidas

