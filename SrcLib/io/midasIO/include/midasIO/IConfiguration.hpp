/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __MIDASIO_ICONFIGURATION_HPP__
#define __MIDASIO_ICONFIGURATION_HPP__

#include <string>
#include <map>

#include <fwCore/base.hpp>

#include <fwTools/macros.hpp>

#include "midasIO/registry/detail.hpp"
#include "midasIO/factory/new.hpp"
#include "midasIO/config.hpp"

#define __MIDASIO_GET_SET_MACRO(Name, Type) \
    const Type &get ## Name() \
    { \
        return m_attr ## Name; \
    } \
    void set ## Name(const Type &value) \
    { \
        m_attr ## Name = value; \
    } \


namespace midasIO
{

/**
 * @brief Base class for Midas client configuration.
 */
class MIDASIO_CLASS_API IConfiguration : public ::fwCore::BaseObject
{

public:
    /// Factory key used by IConfiguration implementations
    typedef ::midasIO::configurationFactory::Key Key;

    /// Defines a community in Midas (name to folder ID)
    typedef std::map<std::string, std::string> CommunityListType;

    /// Defines identifiers for Midas supported licenses.
    enum LicenseIDType
    {
        PUBLIC_PDDL          = 0,
        PUBLIC_ODC_BY        = 1,
        PUBLIC_ODBL          = 2,
        PRIVATE_ARR          = 3,
        PUBLIC_CC_BY_3       = 4,
        PUBLIC_CC_BY_SA_3    = 5,
        PUBLIC_CC_BY_ND_3    = 6,
        PUBLIC_CC_BY_NC_3    = 7,
        PUBLIC_CC_BY_NC_SA_3 = 8,
        PUBLIC_CC_BY_NC_ND_3 = 9
    };

    /// Maps an identification number to a licence description.
    typedef std::map< int, std::string > LicenseContainerType;

    /**
     * @brief Class used to register a class factory in factory registry.
     * This class defines also the object factory ( 'create' )
     *
     * @tparam T Factory product type
     */
    template <typename T>
    class Registrar
    {
    public:
        Registrar(std::string functorKey)
        {
            ::midasIO::registry::getConfigurationRegistry()->addFactory(
                functorKey, &::midasIO::configurationFactory::New<T>);
        }
    };

    fwCoreNonInstanciableClassDefinitionsMacro((IConfiguration));
    fwCoreAllowSharedFromThis();

    /**
     * @name Constructor/Destructor
     * @{ */
    MIDASIO_API IConfiguration();

    virtual ~IConfiguration()
    {
    }
    /**  @} */

    /// Privacy type.
    enum PrivacyType
    {
        PUBLIC,
        PRIVATE,
        PARENT
    };

    /// Available licenses on Midas.
    MIDASIO_API static const LicenseContainerType s_LICENSES;

    /// Maps privacy to privacy name
    typedef std::map< PrivacyType, std::string > Privacy2NameType;

    /**
     * @name Getters/Setters.
     * @{ */
    __MIDASIO_GET_SET_MACRO(Url, std::string);
    __MIDASIO_GET_SET_MACRO(Email, std::string);
    __MIDASIO_GET_SET_MACRO(Password, std::string);
    __MIDASIO_GET_SET_MACRO(RootFolderName, std::string);
    __MIDASIO_GET_SET_MACRO(Privacy, PrivacyType);

    __MIDASIO_GET_SET_MACRO(ApiKey, std::string);
    __MIDASIO_GET_SET_MACRO(AppName, std::string);
    __MIDASIO_GET_SET_MACRO(Token, std::string);
    __MIDASIO_GET_SET_MACRO(RootFolderId, std::string);

    __MIDASIO_GET_SET_MACRO(DefaultLicense, int);
    /**  @} */

    /**
     * @brief Returns URL to retrieve Midas API key.
     * @return API key URL.
     */
    MIDASIO_API virtual std::string getApiKeyUrl() const = 0;

    /**
     * @brief Returns URL to login to Midas server (needs API key URL).
     * @return Login URL.
     *
     * @see IConfiguration::getApiKeyUrl()
     */
    MIDASIO_API virtual std::string getTokenUrl() const = 0;

    /**
     * @brief Returns URL to retrieve community list (may need connection token).
     * @return Community list URL.
     *
     * @see IConfiguration::getToken()
     */
    MIDASIO_API virtual std::string getCommunityListUrl() const = 0;

    /**
     * @brief Returns URL to create a folder with given name into the folder of ID parentFolderId.
     *
     * @param name name of the folder to be created.
     * @param description description of the folder
     * @param parentFolderId ID of the folder where the new folder will be created
     * @return URL to perform this operation
     */
    MIDASIO_API virtual std::string getCreateFolderUrl(
        const std::string& name, const std::string& description, const std::string& parentFolderId) const = 0;

    /**
     * @brief Returns URL to get children of given folder ID.
     * @param fodlerId ID of the folder
     * @return URL to perform this operation
     */
    MIDASIO_API virtual std::string getFolderChildrenUrl(const std::string& folderId) const = 0;

    /**
     * @brief Returns URL to create an item on Midas with given name into given parent folder ID.
     * @param parentId ID of the parent folder where the item will be created
     * @param name name of the item to create
     * @return URL to perform this operation
     */
    MIDASIO_API virtual std::string getCreateItemUrl(const std::string& parentId, const std::string& name) const = 0;

    /**
     * @brief Returns URL to search an item on Midas using given name.
     * @param itemName name of the item to search
     * @return URL to perform this operation
     */
    MIDASIO_API virtual std::string getSearchItemByNameUrl(const std::string& itemName) const = 0;

    /**
     * @brief Returns URL to search an item on Midas using given ID.
     * @param itemId ID of the item to search
     * @return URL to perform this operation
     */
    MIDASIO_API virtual std::string getSearchItemByIdUrl(const std::string& itemId) const = 0;

    /**
     * @brief Returns URL to generate an upload token for a file of given filename with ID 'itemID'.
     * @param itemId ID of the item (file to upload) on Midas
     * @param filename item name
     * @return URL to perform this operation
     *
     * @see getCreateItemUrl()
     * @todo : implement checksum support
     */
    MIDASIO_API virtual std::string getUploadTokenUrl(const std::string& itemId, const std::string& filename) const = 0;

    /**
     * @brief  Returns URL to set license on given item ID.
     *
     * @param itemId ID of the item to set a license
     * @param licenseId ID of the license to set
     *
     * @return URL to perform this operation
     *
     * @see IConfiguration::s_LICENSES
     * @note This method is not available on all Midas servers.
     */
    MIDASIO_API virtual std::string getSetLicenseUrl(const std::string& itemId, const int& licenseId) const = 0;

    /**
     * @brief Returns URL to upload file with given name and length on Midas, using an upload token
     * @param token upload token to upload file
     * @param filename name of file to upload
     * @param length in bytes of file
     * @return URL to perform this operation
     *
     * @see getUploadTokenUrl()
     */
    MIDASIO_API virtual std::string getPerformUploadUrl(
        const std::string& token, const std::string& filename, const std::string& itemId, const int& length) const = 0;

    /// Maps privacies to display name.
    MIDASIO_API static const Privacy2NameType s_PRIVACY_NAMES;

    /// Midas version
    MIDASIO_API virtual std::string getMidasVersion() const = 0;

protected:

    /**
     * @brief Returns URL to access Midas API
     * @return API URL
     */
    virtual std::string getApiPathUrl() const = 0;

    /**
     * @name Configuration parameters
     * Default values : "Patients" for root folder name, "Default" for application name, "PRIVATE" for privacy.
     * @{ */
    std::string m_attrUrl;
    std::string m_attrEmail;
    std::string m_attrPassword;
    std::string m_attrRootFolderName;
    std::string m_attrRootFolderId;

    PrivacyType m_attrPrivacy;

    std::string m_attrApiKey;
    std::string m_attrAppName;
    std::string m_attrToken;

    int m_attrDefaultLicense;
    /**  @} */
};

} // namespace midasIO

#endif // __MIDASIO_ICONFIGURATION_HPP__

