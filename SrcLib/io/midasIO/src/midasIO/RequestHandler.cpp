/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fstream>

#include <fwNetwork/http/ClientQt.hpp>
#include <fwNetwork/http/Request.hpp>

#include "midasIO/factory/new.hpp"
#include "midasIO/IResponseHandler.hpp"
#include "midasIO/RequestHandler.hpp"

namespace midasIO
{

void RequestHandler::setLicence(const std::string& itemId, const int& licenseId)
{
    ::fwNetwork::http::ClientQt client;
    ::midasIO::IResponseHandler::sptr h = ::midasIO::responseHandlerFactory::New(m_config->getMidasVersion());
    const std::string& licenseUrl = m_config->getSetLicenseUrl(itemId, licenseId);
    std::string reply             = "";

    try
    {
        reply = client.get(::fwNetwork::http::Request::New(licenseUrl));
        h->setJsonData(reply);
    }
    catch(::fwNetwork::exceptions::Base& e)
    {
        SLM_ERROR(
            "Failed to set licence '" + std::string("" + licenseId) + "' for item '" + itemId + "' : " + e.what());
    }

    if(!h->isSuccess())
    {
        SLM_ERROR("Failed to set licence : '" + reply + "'");
    }
}

void RequestHandler::performRequest(
    const std::string& url, RequestHandler::RequestType reqType, const std::string& postData)
{
    ::fwNetwork::http::Request::sptr req = ::fwNetwork::http::Request::New(url);
    ::fwNetwork::http::ClientQt client;

    std::string data;

    try
    {
        if(reqType == GET)
        {
            SLM_DEBUG("GET request : '" + url + "'");
            data = client.get(req);
        }
        else
        {
            SLM_DEBUG("POST request : '" + url + "'");
            data = client.post(req, postData);
        }
    }
    catch( ::fwNetwork::exceptions::Base& e)
    {
        std::string what(e.what());
        SLM_WARN("Network exception : '" + what + "'");
        m_handler->setJsonData("");
        m_handler->setErrorMsg("Error : '" + what + "'");
        return;
    }

    const std::size_t start = data.find_first_of("{");
    const std::size_t end   = data.find_last_of("}");

    if(start == std::string::npos || end == std::string::npos)
    {
        m_handler->setErrorMsg("Failed to read server response.");
    }
    else
    {
        const std::string jsonStr = data.substr(start, end);
        m_handler->setJsonData(jsonStr);
        SLM_DEBUG("JSON-only response : '" + jsonStr + "'");
    }
}

RequestHandler::RequestHandler( ::midasIO::IConfiguration::sptr config )
    : m_config(config)
{
    m_handler = ::midasIO::responseHandlerFactory::New(m_config->getMidasVersion());
    SLM_ASSERT("Failed to build response handler with key '" + m_config->getMidasVersion() + "'", m_handler);
}

RequestHandler::~RequestHandler()
{
}

SPTR(::midasIO::IResponseHandler) RequestHandler::getCommunityList()
{
    const std::string& url = m_config->getCommunityListUrl();
    this->performRequest(url);
    return m_handler;
}

SPTR(::midasIO::IResponseHandler) RequestHandler::login()
{
    // Retrieve API key
    const std::string& apiKeyUrl = m_config->getApiKeyUrl();
    this->performRequest(apiKeyUrl, POST);

    if(m_handler->isSuccess())
    {
        ::midasIO::IResponseHandler::ObjectType respData = m_handler->getObjectData();
        ::midasIO::IResponseHandler::ObjectType resp     = m_handler->get();

        std::string apiKey = respData["apikey"].get_str();
        m_config->setApiKey(apiKey);

        // Retrieve token
        const std::string tokenUrl = m_config->getTokenUrl();

        this->performRequest(tokenUrl);

        if(m_handler->isSuccess())
        {
            // Extract token
            respData = m_handler->getObjectData();
            std::string token = respData["token"].get_str();
            m_config->setToken(token);
        }
    }

    return m_handler;
}

SPTR(::midasIO::IResponseHandler) RequestHandler::getChildren(const std::string& folderId)
{
    const std::string& url = m_config->getFolderChildrenUrl(folderId);
    this->performRequest(url);
    return m_handler;
}

SPTR(::midasIO::IResponseHandler) RequestHandler::createFolder(
    const std::string& name, const std::string& desc, const std::string& parentFolderId)
{
    const std::string& url = m_config->getCreateFolderUrl(name, desc, parentFolderId);
    this->performRequest(url, POST);
    return m_handler;
}

SPTR(::midasIO::IResponseHandler) RequestHandler::uploadFile(
    const ::boost::filesystem::path& filePath,
    const std::string& targetDirId,
    const int& licenseId)
{
    SLM_DEBUG("Uploading file '" + filePath.string() + "' to remote directory '" + targetDirId + "'");
    const std::string filename = filePath.filename().string();

    std::string itemId;

    // Search for file on Midas to check if it already exists
    const std::string& childrenUrl = m_config->getFolderChildrenUrl(targetDirId);
    this->performRequest(childrenUrl);

    if(!m_handler->isSuccess())
    {
        return m_handler;
    }

    ::midasIO::IResponseHandler::ObjectType obj = m_handler->getObjectData();

    bool createItem = true;

    if(obj.find("items") != obj.end())
    {
        ::json_spirit::mArray folders = obj["items"].get_array();

        for(::json_spirit::mArray::iterator it = folders.begin(); it != folders.end(); ++it)
        {
            ::json_spirit::mObject& obj = it->get_obj();
            SLM_ASSERT("Didn't find attribute 'name' in response", obj.find("name") != obj.end());
            SLM_ASSERT("Didn't find attribute 'item_id' in response", obj.find("item_id") != obj.end());
            const std::string& name = obj["name"].get_str();

            if(name == filename)
            {
                itemId     = obj["item_id"].get_str();
                createItem = false;
                break;
            }
        }
    }

    if(createItem)
    {
        // Get item ID for file
        const std::string& urlItem = m_config->getCreateItemUrl(targetDirId, filename);
        this->performRequest(urlItem, POST);

        if(m_handler->isSuccess())
        {
            ::midasIO::IResponseHandler::ObjectType dataItem = m_handler->getObjectData();
            SLM_ASSERT("Didn't find 'item_id' attribute in response",
                       dataItem.find("item_id") != dataItem.end());
            itemId = dataItem["item_id"].get_str();
        }
        else
        {
            return m_handler;
        }
    }

    const std::string& urlToken = m_config->getUploadTokenUrl(itemId, filename);
    this->performRequest(urlToken);

    if(m_handler->isSuccess())
    {
        ::midasIO::IResponseHandler::ObjectType data = m_handler->getObjectData();
        SLM_ASSERT("Didn't find 'token' attribute in response", data.find("token") != data.end());

        ::fwNetwork::http::ClientQt client;

        std::ifstream is((filePath.string()).c_str(), std::ifstream::binary);
        if (is)
        {
            is.seekg(0, is.end);
            int length = is.tellg();

            is.seekg(0, is.beg);

            char* buffer = new char [length];
            is.read(buffer,length);

            if(!is)
            {
                is.close();
                delete[] buffer;
                throw ::fwCore::Exception("Failed to read file " + filePath.string() + "'");
            }

            is.close();

            const std::string& uploadUrl = m_config->getPerformUploadUrl(
                data["token"].get_str(), filename, itemId, length);

            SLM_DEBUG("POST url : '" + uploadUrl + "'");
            ::fwNetwork::http::Request::sptr req = ::fwNetwork::http::Request::New(uploadUrl);

            this->performRequest(uploadUrl, POST, std::string(buffer));
            if(!m_handler->isSuccess())
            {
                m_handler->setErrorMsg("Failed to upload file '" + filename + "'");
                return m_handler;
            }

            delete[] buffer;

            // Set licence on item if the server handles this operation
            // Doesn't use performRequest() to avoid reporting errors through m_handler
            this->setLicence(itemId, licenseId);
        }
        else
        {
            throw ::fwCore::Exception("Failed to open file " + filePath.string() + "'");
        }
    }

    return m_handler;
}

} // namespace midasIO

