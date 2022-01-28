/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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
#include <core/location/SingleFile.hpp>
#include <core/location/SingleFolder.hpp>
#include <core/runtime/operations.hpp>

#include <service/macros.hpp>

#include <ui/base/dialog/LocationDialog.hpp>
#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/qt/container/QtContainer.hpp>

#include <QHBoxLayout>
#include <QIcon>
#include <QPushButton>
#include <QString>
#include <QWidget>

#include <filesystem>

namespace sight::module::ui::qt::viz
{

//------------------------------------------------------------------------------

const core::com::Signals::SignalKeyType SnapshotEditor::s_SNAPPED_SIG = "snapped";

//------------------------------------------------------------------------------

SnapshotEditor::SnapshotEditor() noexcept
{
    m_sigSnapped = newSignal<SnappedSignalType>(s_SNAPPED_SIG);
}

//------------------------------------------------------------------------------

SnapshotEditor::~SnapshotEditor() noexcept
{
}

//------------------------------------------------------------------------------

void SnapshotEditor::starting()
{
    this->create();

    auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(this->getContainer());

    std::filesystem::path path = core::runtime::getModuleResourceFilePath("sight::module::ui::qt", "camera-photo.png");
    QIcon icon(QString::fromStdString(path.string()));
    m_snapButton = new QPushButton(icon, "");
    m_snapButton->setToolTip(QObject::tr("Snapshot"));

    QHBoxLayout* h_layout = new QHBoxLayout();
    h_layout->addWidget(m_snapButton);
    h_layout->setContentsMargins(0, 0, 0, 0);

    qtContainer->setLayout(h_layout);

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

void SnapshotEditor::info(std::ostream& _sstream)
{
}

//------------------------------------------------------------------------------

void SnapshotEditor::onSnapButton()
{
    auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(
        this->getContainer()
    );
    QWidget* container = qtContainer->getQtContainer();
    SIGHT_ASSERT("container not instanced", container);
    if(container->isVisible())
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
        messageBox.setMessage(msgInfo);
        messageBox.setIcon(sight::ui::base::dialog::IMessageDialog::WARNING);
        messageBox.addButton(sight::ui::base::dialog::IMessageDialog::OK);
        messageBox.show();
    }
}

//------------------------------------------------------------------------------

std::string SnapshotEditor::requestFileName()
{
    static auto defaultDirectory = std::make_shared<core::location::SingleFolder>();
    std::string fileName         = "";

    sight::ui::base::dialog::LocationDialog dialogFile;
    dialogFile.setTitle("Save snapshot as");
    dialogFile.setDefaultLocation(defaultDirectory);
    dialogFile.addFilter("Image file", "*.jpg *.jpeg *.bmp *.png *.tiff");
    dialogFile.addFilter("jpeg", "*.jpg *.jpeg");
    dialogFile.addFilter("bmp", "*.bmp");
    dialogFile.addFilter("png", "*.png");
    dialogFile.addFilter("tiff", "*.tiff");
    dialogFile.addFilter("all", "*.*");
    dialogFile.setOption(sight::ui::base::dialog::ILocationDialog::WRITE);

    auto result = core::location::SingleFile::dynamicCast(dialogFile.show());
    if(result)
    {
        fileName = result->getFile().string();
        defaultDirectory->setFolder(result->getFile().parent_path());
        dialogFile.saveDefaultLocation(defaultDirectory);
    }

    return fileName;
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::viz
