/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "io/IReader.hpp"

#include <fwCore/base.hpp>
#include <fwServices/macros.hpp>

namespace io
{

IReader::IReader() throw()
{}

//-----------------------------------------------------------------------------

IReader::~IReader() throw()
{}

//-----------------------------------------------------------------------------

std::string IReader::getSelectorDialogTitle()
{
    return "Title of selector dialog box";
}

//-----------------------------------------------------------------------------

std::vector< std::string > IReader::getSupportedExtensions()
{
    return std::vector< std::string >();
}

}

