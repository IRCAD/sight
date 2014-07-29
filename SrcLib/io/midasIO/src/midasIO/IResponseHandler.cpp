/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "midasIO/IResponseHandler.hpp"

namespace midasIO
{

IResponseHandler::IResponseHandler()
{
}

void IResponseHandler::setJsonData(const std::string& data)
{
    if(!data.empty())
    {
        ::json_spirit::mValue v;
        bool result = ::json_spirit::read_string(data, v);
        m_obj = v.get_obj();
    }
}

} // namespace midasIO

