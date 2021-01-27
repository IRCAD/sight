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

#include "uiVisu/action/SSnapshot.hpp"

#include <core/base.hpp>
#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/runtime/ConfigurationElement.hpp>
#include <core/runtime/operations.hpp>

#include <data/location/SingleFile.hpp>

#include <gui/dialog/LocationDialog.hpp>

#include <services/macros.hpp>

namespace uiVisu
{
namespace action
{

const core::com::Signals::SignalKeyType SSnapshot::s_SNAPPED_SIG = "snapped";

fwServicesRegisterMacro( ::sight::gui::IActionSrv, ::uiVisu::action::SSnapshot, ::sight::data::Object )

SSnapshot::SSnapshot() noexcept
{
    m_sigSnapped = newSignal< SnappedSignalType >(s_SNAPPED_SIG);
}

//------------------------------------------------------------------------------

SSnapshot::~SSnapshot() noexcept
{
}

//------------------------------------------------------------------------------

void SSnapshot::starting()
{
    gui::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------

void SSnapshot::stopping()
{
    gui::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

void SSnapshot::configuring()
{
    gui::IActionSrv::initialize();
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

void SSnapshot::swapping()
{

}

//------------------------------------------------------------------------------

void SSnapshot::info( std::ostream& _sstream )
{
}

//------------------------------------------------------------------------------

std::string SSnapshot::requestFileName()
{
    std::string fileName = "";

    gui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle("Save snapshot as");
    dialogFile.addFilter("Image file", "*.jpg *.jpeg *.bmp *.png *.tiff");
    dialogFile.addFilter("jpeg", "*.jpg *.jpeg");
    dialogFile.addFilter("bmp", "*.bmp");
    dialogFile.addFilter("png", "*.png");
    dialogFile.addFilter("tiff", "*.tiff");
    dialogFile.addFilter("all", "*.*");
    dialogFile.setOption(gui::dialog::ILocationDialog::WRITE);

    data::location::SingleFile::sptr result;
    result = data::location::SingleFile::dynamicCast( dialogFile.show() );
    if (result)
    {
        fileName = result->getPath().string();
    }

    return fileName;
}

//------------------------------------------------------------------------------

} // action
} // uiVisu
