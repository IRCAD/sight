/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwWX/wxXmlTree.hpp"

#include <iostream>
#include <sstream>

namespace fwWX
{

wxXmlTree::wxXmlTree(wxWindow* parent, xmlNodePtr node)
 : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxMAXIMIZE_BOX)
{
    wxBoxSizer * sizer = new wxBoxSizer(wxVERTICAL);
    wxSizerFlags flagsExpand(1);
    flagsExpand.Expand();
    wxTreeCtrl * tree = new wxTreeCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(500, 500), wxTR_DEFAULT_STYLE); //, wxPoint(0,0), wxSize(400, 500) ) ; //remplacer les point et size par AUTO

    sizer->Add(tree, flagsExpand);
    SetSizer(sizer);

    wxTreeItemId root;
    addNodeToWidget(tree, root, node) ;
    //tree->SetQuickBestSize(false) ;
    //pour les tests
    //tree->ExpandAll() ;
}


void wxXmlTree::addNodeToWidget(wxTreeCtrl *tree, wxTreeItemId pItem , xmlNodePtr pXmlNode)
{
    wxString itemTreeLabel ; // concatenation of node name + node attributes {name + value}
    //--------Add Node-Name--------------------
    itemTreeLabel << xmlCharTowxString(pXmlNode->name) ;
    //--------Add Attributes if exist----------------
    itemTreeLabel << getNodePropertiesInWxString(pXmlNode) ;
    //--------Add Element Value if exist---------------------
    itemTreeLabel << getElementValue(pXmlNode) ;
    //--------AppendItem to Tree-----------

    wxTreeItemId item ;
    if (tree->GetRootItem())
    {item = tree->AppendItem(pItem, itemTreeLabel ) ;}
    else
    {
        item = tree->AddRoot( itemTreeLabel ) ;
    }

    //------children Elements of the curent node--------
    xmlNodePtr child = pXmlNode->children ;
    while (child)
    {
        if (child->type == XML_ELEMENT_NODE)
        {
            addNodeToWidget(tree, item , child) ;
        }
        child = child->next ;
    }
    //------END---children Elements of the curent node--------
}

wxString  wxXmlTree::xmlCharTowxString(const xmlChar * ch)
{
    std::stringstream ss ;
    ss.str("") ;
    ss<< ch ;
    wxString  wxstring_tmp = wxString::FromAscii(ss.str().data()) ;
    return wxstring_tmp ;
}

wxString  wxXmlTree::getNodePropertiesInWxString(xmlNodePtr pXmlNode)
{
    //if propertie/attribute type = Text, then return : 'name1' = 'content1',  'name2' = 'content2'...
    xmlAttrPtr attr_curent = pXmlNode->properties ;
    wxString tmp ;

    while(attr_curent)
    {
        if (attr_curent->children->type == XML_TEXT_NODE)
        {
            tmp << wxString::FromAscii(", ") << xmlCharTowxString(attr_curent->name) << wxString::FromAscii(" = ") << xmlCharTowxString( attr_curent->children->content ) ;
        }
        attr_curent = attr_curent->next ;
    }
    return tmp ;
}

wxString wxXmlTree::getElementValue(xmlNodePtr pXmlNode)
{
    wxString tmp ;

    if ( ( pXmlNode->type == XML_ELEMENT_NODE) && (pXmlNode->children)
            && (pXmlNode->children->type == XML_TEXT_NODE) )
    {
        tmp << wxString::FromAscii(" = ") << xmlCharTowxString(pXmlNode->children->content) ;
    }
    return tmp ;
}

} //namespace fwWX

