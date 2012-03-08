/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwTools/Object.hpp"
#include "fwTools/Field.hpp"

namespace fwTools
{

//------------------------------------------------------------------------------

Field::Field()
{}

//------------------------------------------------------------------------------

Field::Field(const std::string &newLabel ) :  m_label(newLabel)
{}

//------------------------------------------------------------------------------

Field::~Field()
{}

//------------------------------------------------------------------------------

std::string & Field::label()
{
    return m_label;
}

//------------------------------------------------------------------------------

const std::string & Field::label() const
{
    return m_label;
}

//------------------------------------------------------------------------------

} //namespace fwTools



