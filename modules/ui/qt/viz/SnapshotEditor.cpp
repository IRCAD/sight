/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "modules/ui/qt/viz/SnapshotEditor.hpp"

#include <core/base.hpp>
#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Signals.hpp>
#include <core/runtime/operations.hpp>

#include <data/location/Folder.hpp>
#include <data/location/SingleFile.hpp>

#include <service/macros.hpp>

#include <QHBoxLayout>
#include <QIcon>
#include <QPushButton>
#include <QString>
#include <QWidget>

#include <ui/base/dialog/LocationDialog.hpp>
#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/qt/container/QtContainer.hpp>

#include <filesystem>

namespace sight::modules::ui::qt::viz
{

//------------------------------------------------------------------------------

const core::com::Signals::SignalKeyType SnapshotEditor::s_SNAPPED_SIG = "snapped";

fwServicesRegisterMacro( ::sight::ui::base::editor::IEditor, ::sight::modules::ui::qt::viz::SnapshotEditor )

//------------------------------------------------------------------------------

SnapshotEditor::SnapshotEditor() noexcept
{
    m_sigSnapped = newSignal< SnappedSignalType >(s_SNAPPED_SIG);
}

//------------------------------------------------------------------------------

SnapshotEditor::~SnapshotEditor() noexcept
{
}

//------------------------------------------------------------------------------

void SnapshotEditor::starting()
{
    this->create();

    auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(this->getContainer() );

    std::filesystem::path path = core::runtime::getModuleResourceFilePath("module_ui_qt", "camera-photo.png");
    QIcon icon(QString::fromStdString(path.string()));
    m_snapButton = new QPushButton(icon, "");
    m_snapButton->setToolTip(QObject::tr("Snapshot"));

    QHBoxLayout* hlayout = new QHBoxLayout();
    hlayout->addWidget(m_snapButton);
    hlayout->setContentsMargins(0, 0, 0, 0);

    qtContainer->setLayout(hlayout);

    QObject::connect(m_snapButton, SIGNAL(clicked()), this, SLOT(onSnapButton()));

}

//------------------------------------------------------------------------------

void SnapshotEditor::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

void SnapshotEditor::configuring()
{
    this->initialize();
}

//------------------------------------------------------------------------------

void SnapshotEditor::updating()
{
}

//------------------------------------------------------------------------------

void SnapshotEditor::swapping()
{

}

//------------------------------------------------------------------------------

void SnapshotEditor::info( std::ostream& _sstream )
{
}

//------------------------------------------------------------------------------

void SnapshotEditor::onSnapButton()
{
    auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(
        this->getContainer() );
    QWidget* container = qtContainer->getQtContainer();
    SLM_ASSERT("container not instanced", container);
    if( container->isVisible() )
    {
        std::string filename = this->requestFileName();

        if(!filename.empty())
        {
            m_sigSnapped->asyncEmit(filename);
        }
    }
    else
    {
        std::string msgInfo("It is not possible to snapshot the negato view. This view is not shown on screen.");
        sight::ui::base::dialog::MessageDialog messageBox;
        messageBox.setTitle("Negato view snapshot");
        messageBox.setMessage( msgInfo );
        messageBox.setIcon(sight::ui::base::dialog::IMessageDialog::WARNING);
        messageBox.addButton(sight::ui::base::dialog::IMessageDialog::OK);
        messageBox.show();
    }
}

//------------------------------------------------------------------------------

std::string SnapshotEditor::requestFileName()
{
    std::string fileName = "";

    sight::ui::base::dialog::LocationDialog dialogFile;
    dialogFile.setTitle("Save snapshot as");
    dialogFile.addFilter("Image file", "*.jpg *.jpeg *.bmp *.png *.tiff");
    dialogFile.addFilter("jpeg", "*.jpg *.jpeg");
    dialogFile.addFilter("bmp", "*.bmp");
    dialogFile.addFilter("png", "*.png");
    dialogFile.addFilter("tiff", "*.tiff");
    dialogFile.addFilter("all", "*.*");
    dialogFile.setOption(sight::ui::base::dialog::ILocationDialog::WRITE);

    data::location::SingleFile::sptr result;
    result = data::location::SingleFile::dynamicCast( dialogFile.show() );
    if (result)
    {
        fileName = result->getPath().string();
        dialogFile.saveDefaultLocation( data::location::Folder::New(result->getPath().parent_path()) );
    }

    return fileName;
}
//------------------------------------------------------------------------------

} // namespace sight::modules::ui::qt::viz
