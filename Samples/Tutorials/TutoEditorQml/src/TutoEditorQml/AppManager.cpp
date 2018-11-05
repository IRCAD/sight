/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "TutoEditorQml/AppManager.hpp"

#include <fwQml/IQmlEditor.hpp>

#include <QVariant>

namespace TutoEditorQml
{

static const std::string s_EDITED_STRING = "editedString";

//------------------------------------------------------------------------------

AppManager::AppManager() noexcept
{
}

//------------------------------------------------------------------------------

AppManager::~AppManager() noexcept
{
}

//------------------------------------------------------------------------------

void AppManager::initialize()
{
    this->create();

    m_string = ::fwData::String::New("Edit this line...");

    this->addObject(m_string, s_EDITED_STRING);

    this->startServices();
}

//------------------------------------------------------------------------------

void AppManager::uninitialize()
{
    this->destroy();
}

//------------------------------------------------------------------------------

void AppManager::onServiceCreated(const QVariant& obj)
{
    // check that the service is a IQmlEditor
    ::fwQml::IQmlEditor::sptr srv(obj.value< ::fwQml::IQmlEditor* >());
    if (srv)
    {
        // check if it is the SOrganMaterialEditor
        if (srv->isA("::TutoEditorQml::SStringEditor"))
        {
            // register the new service in the AppManager, it will be automatically started when the reconstruction is
            // added
            srv->setObjectId("string", s_EDITED_STRING);
            this->addService(srv, true);

            try
            {
                this->startService(srv);
            }
            catch (std::exception& e)
            {
                SLM_ERROR("Try to start '" + srv->getID() + "': " + std::string(e.what()));
            }
        }
    }
}

//------------------------------------------------------------------------------

} // namespace TutoEditorQml
