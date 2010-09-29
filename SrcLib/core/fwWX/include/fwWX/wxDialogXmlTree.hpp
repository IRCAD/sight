/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef FWWX_WXDIALOGXMLTREE_HPP_
#define FWWX_WXDIALOGXMLTREE_HPP_


#include "wxXmlTree.hpp"

#include <wx/window.h>
#include <wx/frame.h>

#include "fwWX/config.hpp"

namespace fwWX
{

/**
 * @brief   This class returns a wxFrame witch contains a wxXmlTree (Xml tree in a wxTree).
 * @class   wxDialogXmlTree.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
* @todo wxDialogXmlTree is not commented.
 */
class FWWX_CLASS_API wxDialogXmlTree : public wxFrame
{
public:
    /**
    * @brief    Constructor
    *
    * @remarks   Call itself wxXmlTree constructor
    */
    FWWX_API wxDialogXmlTree(wxWindow* parent, xmlNodePtr node) ;

};

} //namespace fwWX

#endif /*FWWX_WXDIALOGXMLTREE_HPP_*/
