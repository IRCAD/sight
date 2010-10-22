/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#ifndef FWWX_WXXMLTREE_HPP_
#define FWWX_WXXMLTREE_HPP_

#include <wx/wx.h>
#include <wx/treectrl.h>
#include <wx/string.h>
#include <libxml/tree.h>


#include "fwWX/config.hpp"


namespace fwWX
{

/**
 * @brief   This class returns a wxPanel witch contains a wxXmlTree (Xml tree in a wxTree).
 * @class   wxXmlTree.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 * @todo wxXmlTree is not commented.
 */
class FWWX_CLASS_API wxXmlTree : public ::wxPanel

{
public:
    /**
    * @brief    Constructor
    */
    FWWX_API wxXmlTree(wxWindow* parent);

    /**
    * @brief    Constructor
    */
    FWWX_API wxXmlTree(wxWindow* parent, xmlNodePtr node);

    FWWX_API void updateNode(xmlNodePtr pXmlNode);
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

    void createLayout();

    wxTreeCtrl *m_tree;
};

} //namespace fwWX

#endif /*FWWX_WXXMLTREE_HPP_*/
