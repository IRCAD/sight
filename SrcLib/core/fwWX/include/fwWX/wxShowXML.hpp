/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef FWWX_WXSHOWXML_HPP_
#define FWWX_WXSHOWXML_HPP_

#include "fwWX/config.hpp"

#include <wx/wx.h>
#include <wx/treectrl.h>
#include <wx/string.h>
#include <libxml/tree.h>

namespace fwWX
{

/**
 * @brief   This class shows data content.
 * @class   wxShowXML.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 * @todo wxShowXML is not commented.
 */

class FWWX_CLASS_API wxShowXML : public ::wxPanel

{
public:
    /**
    * @brief    Constructor
    */
    FWWX_API wxShowXML(wxWindow* parent, xmlNodePtr node);

private :
    /**
     * @brief   construct the wxTree with xmlNodePtr
     */
    void addNodeToWidget(wxTreeCtrl *tree, wxTreeItemId pItem , xmlNodePtr pXmlNode) ;
    /**
     * @brief   Convert a wxmlChar(Ascii) To wxString
     */
    wxString xmlCharTowxString(const xmlChar * ch) ;
    /**
     * @brief   return {name&value} of a node property in wxString(or empty wxString)
     */
    wxString getNodePropertiesInWxString(xmlNodePtr pXmlNode) ;
    /**
     * @brief   return Element Value of a node in wxString(or empty wxString)
     */
    wxString getElementValue(xmlNodePtr pXmlNode) ;
};

} // namespace fwWX

#endif /*FWWX_WXSHOWXML_HPP_*/
