/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwQml/IQmlEditor.hpp"

namespace fwQml
{

//-----------------------------------------------------------------------------

IQmlEditor::IQmlEditor() noexcept
{
}

//-----------------------------------------------------------------------------

IQmlEditor::~IQmlEditor() noexcept
{
}

//-----------------------------------------------------------------------------

void IQmlEditor::starting()
{
    Q_EMIT started();
    this->updating();
}

//-----------------------------------------------------------------------------

void IQmlEditor::stopping()
{
    Q_EMIT stopped();
}

//-----------------------------------------------------------------------------

}
