/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwWX/wxDialogXmlTree.hpp"

namespace fwWX
{

wxDialogXmlTree::wxDialogXmlTree(wxWindow* parent, xmlNodePtr node)
//: wxMiniFrame(parent, wxID_ANY, _T("Xml Tree"), wxPoint(10,10), wxSize(500,600), wxDEFAULT_FRAME_STYLE)
: wxFrame(parent, wxID_ANY, _T("Xml Tree")  , wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE)

{
        new wxXmlTree(this, node ) ;
}

} //namespace fwWX

