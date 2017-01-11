/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __MIDASIO_IRESPONSEHANDLER_HPP__
#define __MIDASIO_IRESPONSEHANDLER_HPP__

#include <string>

#include <json_spirit/json_spirit_reader_template.h>

#include <fwCore/base.hpp>

#include "midasIO/factory/new.hpp"
#include "midasIO/config.hpp"

namespace midasIO
{

/**
 * @brief Base class for Midas client response handlers.
 */
class MIDASIO_CLASS_API IResponseHandler : public ::fwCore::BaseObject
{

public:

    /// Data formatted as JSON object.
    typedef ::json_spirit::mObject ObjectType;

    /// Data formatted as JSON array.
    typedef ::json_spirit::mArray ArrayType;

    /// Factory key used by IConfiguration implementations
    typedef ::midasIO::responseHandlerFactory::Key Key;

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
            ::midasIO::registry::getResponseHandlerRegistry()->addFactory(
                functorKey, &::midasIO::responseHandlerFactory::New<T>);
        }
    };

    fwCoreNonInstanciableClassDefinitionsMacro((IResponseHandler));
    fwCoreAllowSharedFromThis();

    /**
     * @name Constructor/Destructor.
     * @{ */
    MIDASIO_API IResponseHandler();
    virtual ~IResponseHandler()
    {
    }
    /**  @} */

    /**
     * @brief Set content from given JSON data
     *
     * @param data
     */
    MIDASIO_API void setJsonData(const std::string& jsonData);

    /// Returns true if response status is success.
    MIDASIO_API virtual bool isSuccess() const = 0;

    /// Returns error message if response status is error, or an empty string.
    MIDASIO_API virtual std::string getErrorMsg() const = 0;

    /// Sets error message with given string.
    MIDASIO_API virtual void setErrorMsg(const std::string& errorMsg) = 0;

    /**
     * @brief Returns response as JSON object.
     *
     * @return full JSON response
     */
    MIDASIO_API virtual ObjectType get() const = 0;

    /**
     * @brief Returns data part of the response as JSON object.
     * Returns empty JSON object if 'data' attribute not found in response or not a JSON object.
     *
     * @return response data as JSON object
     */
    MIDASIO_API virtual ObjectType getObjectData() const = 0;

    /**
     * @brief Returns data part of the response as JSON array.
     * Returns empty JSON array if 'data' attribute not found in response or not a JSON object.
     *
     * @return response data as array
     */
    MIDASIO_API virtual ArrayType getArrayData() const = 0;

protected:

    /// JSON object matching response
    ObjectType m_obj;
};

} // namespace midasIO

#endif //__MIDASIO_IRESPONSEHANDLER_HPP__

