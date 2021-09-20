/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#include "SSnapshot.hpp"

#include <core/base.hpp>
#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/location/SingleFile.hpp>
#include <core/runtime/ConfigurationElement.hpp>
#include <core/runtime/operations.hpp>

#include <service/macros.hpp>

#include <ui/base/dialog/LocationDialog.hpp>

namespace sight::module::ui::base::viz
{

const core::com::Signals::SignalKeyType SSnapshot::s_SNAPPED_SIG = "snapped";

SSnapshot::SSnapshot() noexcept
{
    m_sigSnapped = newSignal<SnappedSignalType>(s_SNAPPED_SIG);
}

//------------------------------------------------------------------------------

SSnapshot::~SSnapshot() noexcept
{
}

//------------------------------------------------------------------------------

void SSnapshot::starting()
{
    sight::ui::base::IAction::actionServiceStarting();
}

//------------------------------------------------------------------------------

void SSnapshot::stopping()
{
    sight::ui::base::IAction::actionServiceStopping();
}

//------------------------------------------------------------------------------

void SSnapshot::configuring()
{
    sight::ui::base::IAction::initialize();
}

//------------------------------------------------------------------------------

void SSnapshot::updating()
{
    std::string filename = this->requestFileName();

    if(!filename.empty())
    {
        m_sigSnapped->asyncEmit(filename);
    }
}

//------------------------------------------------------------------------------

void SSnapshot::info(std::ostream&)
{
}

//------------------------------------------------------------------------------

std::string SSnapshot::requestFileName()
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

    auto result = core::location::SingleFile::dynamicCast(dialogFile.show());
    if(result)
    {
        fileName = result->getFile().string();
    }

    return fileName;
}

//------------------------------------------------------------------------------

} // uiVisu
