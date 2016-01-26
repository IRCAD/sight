/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <midasIO/registry/macros.hpp>
#include <midasIO/IResponseHandler.hpp>

namespace ioMidas
{

namespace responseHandlers
{

class Midas328 : public ::midasIO::IResponseHandler
{

public:

    Midas328(::midasIO::IResponseHandler::Key key)
    {
    }
    ~Midas328()
    {
    }

    virtual bool isSuccess() const;
    virtual std::string getErrorMsg() const;
    virtual ::midasIO::IResponseHandler::ObjectType get() const;
    virtual ::midasIO::IResponseHandler::ObjectType getObjectData() const;
    virtual ::midasIO::IResponseHandler::ArrayType getArrayData() const;
    virtual void setErrorMsg(const std::string& errorMsg);
};

// Register Midas326 class as an IResponseHandler for Midas version "3.2.8"
responseHandlerRegisterMacro( ::ioMidas::responseHandlers::Midas328, "3.2.8" );

bool Midas328::isSuccess() const
{
    return m_obj.find("error") == m_obj.end();
}

std::string Midas328::getErrorMsg() const
{
    std::string msg = "";
    ::json_spirit::mObject::const_iterator itErr = m_obj.find("error");

    if(itErr != m_obj.end())
    {
        ::json_spirit::mObject error                 = itErr->second.get_obj();
        ::json_spirit::mObject::const_iterator itMsg = error.find("msg");

        if(itMsg != error.end())
        {
            msg = itMsg->second.get_str();
        }
    }

    return msg;
}

void Midas328::setErrorMsg(const std::string& errorMsg)
{
    ::json_spirit::mObject error;
    error["msg"] = ::json_spirit::mValue(errorMsg);

    m_obj["error"] = error;
}

::midasIO::IResponseHandler::ObjectType Midas328::get() const
{
    return m_obj;
}

::midasIO::IResponseHandler::ObjectType Midas328::getObjectData() const
{
    ::json_spirit::mObject::const_iterator itData = m_obj.find("data");
    const bool data = itData != m_obj.end() && itData->second.type() == ::json_spirit::obj_type;
    return data ? itData->second.get_obj() : ::json_spirit::mObject();
}

::midasIO::IResponseHandler::ArrayType Midas328::getArrayData() const
{
    ::json_spirit::mObject::const_iterator itData = m_obj.find("data");
    const bool data = itData != m_obj.end() && itData->second.type() == ::json_spirit::array_type;
    return data ? itData->second.get_array() : ::json_spirit::mArray();
}

} // namespace responseHandlers

} // namespace ioMidas

