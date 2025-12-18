/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

 #include "query.hpp"
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>

namespace sight::module::io::dicomweb::detail
{

//------------------------------------------------------------------------------

std::string query_instance_uid(
    const std::string& _pacs_server,
    const std::string& _instance_server_id,
    sight::io::http::client_qt& _client_qt
)
{
    QJsonDocument json_response;

    // Retrieve the SOP Instance UID
    const std::string instance_url(_pacs_server + "/instances/" + _instance_server_id);
    const QByteArray& instance_answer = _client_qt.get(sight::io::http::request::New(instance_url));
    json_response = QJsonDocument::fromJson(instance_answer);
    const QJsonObject& instance_answer_json_obj = json_response.object();

    QString sop_instance_uid;
    if(instance_answer_json_obj.contains("MainDicomTags"))
    {
        const QJsonObject main_dicom_tags = instance_answer_json_obj["MainDicomTags"].toObject();
        sop_instance_uid = main_dicom_tags["SOPInstanceUID"].toString();
    }

    return sop_instance_uid.toStdString();
}

} // namespace sight::module::io::dicomweb::detail
