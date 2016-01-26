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

class Midas326 : public ::midasIO::IResponseHandler
{

public:

    Midas326(::midasIO::IResponseHandler::Key key)
    {
    }
    ~Midas326()
    {
    }

    virtual bool isSuccess() const;
    virtual std::string getErrorMsg() const;
    virtual ::midasIO::IResponseHandler::ObjectType get() const;
    virtual ::midasIO::IResponseHandler::ObjectType getObjectData() const;
    virtual ::midasIO::IResponseHandler::ArrayType getArrayData() const;
    virtual void setErrorMsg(const std::string& errorMsg);
};

// Register Midas326 class as an IResponseHandler for Midas version "3.2.6"
responseHandlerRegisterMacro( ::ioMidas::responseHandlers::Midas326, "3.2.6" );

bool Midas326::isSuccess() const
{
    ::json_spirit::mObject::const_iterator itStat = m_obj.find("stat");
    return (itStat != m_obj.end()) ? itStat->second.get_str() == "ok" : false;
}

std::string Midas326::getErrorMsg() const
{
    ::json_spirit::mObject::const_iterator itMsg = m_obj.find("message");
    const std::string msg = (itMsg != m_obj.end()) ? itMsg->second.get_str() : "";
    return msg;
}

void Midas326::setErrorMsg(const std::string& errorMsg)
{
    m_obj["stat"]    = "error";
    m_obj["message"] = ::json_spirit::mValue(errorMsg);
}

::midasIO::IResponseHandler::ObjectType Midas326::get() const
{
    return m_obj;
}

::midasIO::IResponseHandler::ObjectType Midas326::getObjectData() const
{
    ::json_spirit::mObject::const_iterator itData = m_obj.find("data");
    const bool data = itData != m_obj.end() && itData->second.type() == ::json_spirit::obj_type;
    return data ? itData->second.get_obj() : ::json_spirit::mObject();
}

::midasIO::IResponseHandler::ArrayType Midas326::getArrayData() const
{
    ::json_spirit::mObject::const_iterator itData = m_obj.find("data");
    const bool data = itData != m_obj.end() && itData->second.type() == ::json_spirit::array_type;
    return data ? itData->second.get_array() : ::json_spirit::mArray();
}

} // namespace responseHandlers

} // namespace ioMidas

