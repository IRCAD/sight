/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __MIDASIO_REQUESTHANDLER_HPP__
#define __MIDASIO_REQUESTHANDLER_HPP__

#include <boost/filesystem.hpp>

#include <fwTools/Failed.hpp>

#include "midasIO/IConfiguration.hpp"
#include "midasIO/IResponseHandler.hpp"
#include "midasIO/config.hpp"

namespace midasIO
{

class IResponseHandler;

/**
 * @brief Executes HTTP requests to a Midas server.
 * @class RequestHandler
 *
 * All requests returns a ::midasIO::IResponseHandler which contains the returned data as JSON.
 */
class MIDASIO_CLASS_API RequestHandler
{
public:

    /// HTTP request type.
    enum RequestType { POST, GET };

    /**
     * @name Constructor/Destructor
     * @{ */
    MIDASIO_API RequestHandler(SPTR(::midasIO::IConfiguration)config);
    MIDASIO_API ~RequestHandler();
    /**  @} */

    /**
     * @brief Logs in to the Midas server.
     * @return a response handler
     */
    MIDASIO_API SPTR(::midasIO::IResponseHandler) login();

    /**
     * @brief Creates a folder on Midas.
     *
     * @param name folder name
     * @param desc folder description
     * @param parentFolderId ID of the parent folder
     *
     * @return a response handler
     */
    MIDASIO_API SPTR(::midasIO::IResponseHandler) createFolder(
        const std::string& name, const std::string& desc, const std::string& parentFolderId);

    /**
     * @brief Gets the list of Midas communities.
     *
     * @return a response handler
     */
    MIDASIO_API SPTR(::midasIO::IResponseHandler) getCommunityList();

    /**
     * @brief Retrieves children of a given folder ID on Midas.
     *
     * @param folderId folder ID to retrieve children
     *
     * @return a response handler
     */
    MIDASIO_API SPTR(::midasIO::IResponseHandler) getChildren(const std::string& folderId);

    /**
     * @brief Uploads a file on Midas.
     *
     * @param filePath path of file to upload
     * @param targetDirId target directory ID on Midas
     * @param licenseId license to set on file to upload (default set to ::midasIO::IConfiguration::PRIVATE)
     *
     * @return a response handler
     *
     * @throw ::fwCore::Exception if any exception occurs while processing files
     */
    MIDASIO_API SPTR(::midasIO::IResponseHandler) uploadFile(
        const ::boost::filesystem::path& filePath,
        const std::string& targetDirId,
        const int& licenseId = IConfiguration::PRIVATE_ARR);

private:

    /**
     * @brief Set license on given item ID.
     *
     * @param itemId ID of the item to set license
     * @param licenseId ID of the license to set
     *
     * @note This method is not supported on all Midas servers
     * @see ::midasIO::IConfiguration::s_LICENSES
     */
    void setLicence(const std::string& itemId, const int& licenseId);

    /**
     * @brief Executes an HTTP request and fills the response handler m_handler with returned data.
     *
     * @param url url for HTTP request
     * @param reqType request type : GET or POST (default is GET)
     * @param postData if request type is POST, postData can be passed here, else, uses an empty string
     */
    void performRequest(const std::string& url, RequestType reqType = GET, const std::string& postData = "");

    /// Midas configuration.
    SPTR(::midasIO::IConfiguration) m_config;

    /// Response handler parsing server reponses according to Midas version.
    ::midasIO::IResponseHandler::sptr m_handler;
};

} // namespace midasIO

#endif  // __MIDASIO_REQUESTHANDLER_HPP__

