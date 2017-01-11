/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWNETWORK_HTTP_REQUEST_HPP__
#define __FWNETWORK_HTTP_REQUEST_HPP__

#include <map>
#include <fwCore/base.hpp>

#include "fwNetwork/config.hpp"

namespace fwNetwork
{

namespace http
{


/**
 * @brief HTTP request.
 */
class FWNETWORK_CLASS_API Request : public ::fwCore::BaseObject
{

public:
    fwCoreAllowSharedFromThis();
    fwCoreClassDefinitionsWithFactoryMacro((Request), (()), new Request );


    /// Maps header name to its value.
    typedef std::map<std::string, std::string> HeadersType;

    /**
     * @name Constructor/Destructor
     * @{ */
    FWNETWORK_API Request();

    /**
     * @brief Construct a new Request with given url.
     */
    FWNETWORK_API Request(const std::string& url);

    virtual ~Request()
    {
    }
    /**  @} */

    /**
     * @brief Creates a new Request with given url.
     *
     * @param url URL belonging to the request.
     *
     * @return  the newly created Request object
     */
    FWNETWORK_API static Request::sptr New(const std::string& url);

    /**
     * @brief add element in the request header
     */
    FWNETWORK_API void addHeader(const std::string& key, const std::string& value);

    /**
     *@brief headers setter.
     */
    FWNETWORK_API void setHeaders(const HeadersType& headers);

    /**
     *@brief headers getter.
     */
    FWNETWORK_API const HeadersType& getHeaders() const;

    /**
     * @brief return current url
     */
    FWNETWORK_API const std::string& getUrl() const;

    /**
     * @brief set current url
     */
    FWNETWORK_API void setUrl(const std::string& url);

private:

    HeadersType m_headers;

    std::string m_url;
};

}  // namespace http

}

// namespace fwNetwork

#endif //__FWNETWORK_HTTP_REQUEST_HPP__
