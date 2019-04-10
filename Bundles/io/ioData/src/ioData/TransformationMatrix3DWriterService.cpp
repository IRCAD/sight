/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "ioData/TransformationMatrix3DWriterService.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwCore/base.hpp>

#include <fwData/location/Folder.hpp>
#include <fwData/location/SingleFile.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwDataIO/writer/TransformationMatrix3DWriter.hpp>

#include <fwGui/dialog/LocationDialog.hpp>

#include <fwServices/macros.hpp>

#include <boost/filesystem/operations.hpp>

#include <fstream>
#include <iostream>

namespace ioData
{

//-----------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwIO::IWriter, ::ioData::TransformationMatrix3DWriterService,
                         ::fwData::TransformationMatrix3D );

//-----------------------------------------------------------------------------

void TransformationMatrix3DWriterService::info(std::ostream& _sstream )
{
    this->SuperClass::info( _sstream );
    _sstream << std::endl << " TransformationMatrix3D object writer";
}

//-----------------------------------------------------------------------------

void TransformationMatrix3DWriterService::starting( )
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

::fwIO::IOPathType TransformationMatrix3DWriterService::getIOPathType() const
{
    return ::fwIO::FILE;
}

//-----------------------------------------------------------------------------

void TransformationMatrix3DWriterService::configuring()
{
    ::fwIO::IWriter::configuring();
}

//------------------------------------------------------------------------------

void TransformationMatrix3DWriterService::configureWithIHM()
{
    SLM_TRACE_FUNC();
    static ::boost::filesystem::path _sDefaultPath("");

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a file to save a transformation matrix" : m_windowTitle);
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("TRF files", "*.trf");
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::WRITE);

    ::fwData::location::SingleFile::sptr result;
    result = ::fwData::location::SingleFile::dynamicCast( dialogFile.show() );
    if (result)
    {
        _sDefaultPath = result->getPath().parent_path();
        dialogFile.saveDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
        this->setFile(result->getPath());
    }
    else
    {
        m_writeFailed = true;
        this->clearLocations();
    }

}

//-----------------------------------------------------------------------------

void TransformationMatrix3DWriterService::stopping()
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void TransformationMatrix3DWriterService::updating()
{
    SLM_TRACE_FUNC();
    if(this->hasLocationDefined())
    {
        // Retrieve object
        ::fwData::TransformationMatrix3D::csptr matrix =
            this->getInput< ::fwData::TransformationMatrix3D >(::fwIO::s_DATA_KEY);
        SLM_ASSERT("The input key '" + ::fwIO::s_DATA_KEY + "' is not correctly set.", matrix);

        ::fwDataIO::writer::TransformationMatrix3DWriter::sptr writer =
            ::fwDataIO::writer::TransformationMatrix3DWriter::New();
        writer->setObject( matrix );
        writer->setFile(this->getFile());
        writer->write();
    }
    else
    {
        m_writeFailed = true;
    }
}

//-----------------------------------------------------------------------------

}
