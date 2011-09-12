/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/version.h>

#include <fwData/String.hpp>

#include <fwComEd/Dictionary.hpp>
#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>

#if wxCHECK_VERSION(2, 9, 0)

#include <fwTools/dateAndTime.hpp>
#include "wx/dataview.h"
#include "fwWX/PatientDBTreeModel.hpp"
#include "fwWX/convert.hpp"

static const char * eye_xpm[] = {
"16 16 2 1",
"   c None",
"X  c Black",
"                ",
"                ",
"      XXXXX     ",
"   XXXXXXXXXX   ",
" XX      XXXXXX ",
"X           XXXX",
"              XX",
"    XXXXXXX     ",
"  XXXXXXXXXXX   ",
" XX  XXXXX  XX  ",
"X    XX XX    X ",
" X   XXXXX   X  ",
"  XX  XXX  XX   ",
"    XXXXXXX     ",
"                ",
"                "};

wxVariant fwPatientDBNode::getCell(unsigned int _col)
{
    wxVariant res;
    switch (_col)
    {
        case 0 : res << wxDataViewIconText("Local PACS"); break;
        default : res = wxString("");
    }

    return res;
}

wxVariant fwPatientNode::getCell(unsigned int _col)
{
    wxVariant res;
    switch (_col)
    {
//      case 0 : res << wxDataViewIconText(::fwWX::std2wx( (m_patient.lock()->getName() + " " + m_patient.lock()->getFirstname()).c_str())); break;
        case 0 : res << wxDataViewIconText(::fwWX::std2wx( (m_patient.lock()->getName() + " " + m_patient.lock()->getFirstname()).c_str())); break;
//      case 0 : res = wxString(wxConvertMB2WX(m_patient->getName().c_str())); break;
        case 3 : res = wxString(""); break;
        default : res = wxString("");
    }

    return res;
}

wxVariant fwAcqNode::getCell(unsigned int _col)
{
    wxVariant res;
    std::string zone = (getStudy().lock()->getAcquisitionZone().length())?getStudy().lock()->getAcquisitionZone():"Unknown zone";
    std::string voxelSize = "";
    std::string nbImages = "";
    std::string acqDate = boost::posix_time::to_iso_extended_string( getAcq().lock()->getCreationDate() );
    acqDate =  acqDate.substr(0,10) + " " + acqDate.substr(11,5);
    if ( getAcq().lock()->getImage()->getDimension() == 3 )
    {
        std::stringstream nbImagesStream;
        nbImagesStream << getAcq().lock()->getImage()->getSize()[0] << "  x " << getAcq().lock()->getImage()->getSize()[1] << " x " << getAcq().lock()->getImage()->getSize()[2];
        nbImages = nbImagesStream.str();

        std::stringstream voxelSizeStream;
        voxelSizeStream << getAcq().lock()->getImage()->getSpacing()[0] << " x "
                        << getAcq().lock()->getImage()->getSpacing()[1] << " x "
                        << getAcq().lock()->getImage()->getSpacing()[2];
        voxelSize = voxelSizeStream.str();
    }

    ::fwComEd::fieldHelper::MedicalImageHelpers::checkComment(getAcq().lock()->getImage());
    if ( ! getAcq().lock()->getImage()->getFieldSize( ::fwComEd::Dictionary::m_imageLabelId) )
    {
        fwPatientNode *patientNode =  static_cast< fwPatientNode * >(GetParent());
        ::fwComEd::fieldHelper::MedicalImageHelpers::setImageLabel(patientNode->getPatient().lock(), getAcq().lock()->getImage());
    }
    std::string comment = getAcq().lock()->getImage()->getFieldSingleElement< ::fwData::String >( ::fwComEd::Dictionary::m_imageLabelId )->value();
    comment += " : " + getAcq().lock()->getImage()->getFieldSingleElement< ::fwData::String >( ::fwComEd::Dictionary::m_commentId )->value();

    switch (_col)
    {
        case 0 :
            if( this->isSelected() )
            {
                res << wxDataViewIconText(::fwWX::std2wx(zone.c_str()), wxIcon( eye_xpm ));
            }
            else
            {
                res << wxDataViewIconText( ::fwWX::std2wx(zone.c_str()), wxIcon( ));
            }
            break;
        case 1 : res = wxString(wxConvertMB2WX(getStudy().lock()->getModality().c_str()));break;
        case 2 : res = wxString(wxConvertMB2WX(acqDate.c_str())); break;
        case 3 : res = wxString(wxConvertMB2WX(nbImages.c_str())); break;
        case 4 : res = wxString(wxConvertMB2WX(voxelSize.c_str())); break;
        case 5 : res = wxString(wxConvertMB2WX(comment.c_str())); break;
        default : res = wxString("TODO");
    }

    return res;
}

// ----------------------------------------------------------------------------
// PatientDBTreeModel
// ----------------------------------------------------------------------------

wxString PatientDBTreeModel::GetColumnType( unsigned int col ) const
{
    if (col == 0)
    {
        return wxT("wxDataViewIconText");
    }
    return wxT("string");
}

// ----------------------------------------------------------------------------

void PatientDBTreeModel::SelectNode(fwDataNode* _node)
{
    if(_node)
    {
        for(unsigned int iPatient = 0; iPatient < m_root->GetChildren().GetCount(); iPatient++)
        {
            fwDataNode *pPatientNode = m_root->GetChildren()[iPatient];
            pPatientNode->Select(false);
            for(unsigned int iAcq = 0; iAcq < pPatientNode->GetChildren().GetCount(); iAcq++)
            {
                fwDataNode *pAcqNode = pPatientNode->GetChildren()[iAcq];
                bool b = (pAcqNode->getPosition()[0] == _node->getPosition()[0] &&
                        pAcqNode->getPosition()[1] == _node->getPosition()[1] &&
                        pAcqNode->getPosition()[2] == _node->getPosition()[2]);
                pAcqNode->Select(b);
            }
        }
    }
}

// ----------------------------------------------------------------------------

PatientDBTreeModel::PatientDBTreeModel(::fwData::PatientDB::wptr _pdb)
{
    m_root = ::boost::shared_ptr< fwPatientDBNode >( new fwPatientDBNode( NULL, _pdb ));
}

// ----------------------------------------------------------------------------

void PatientDBTreeModel::setPatientDB(::fwData::PatientDB::wptr _pdb)
{
//  wxDataViewItem itemRoot( (void*) m_root.get() );
//  ItemChanged( itemRoot );

//  if (m_root)
//      delete m_root;


    //m_root = new fwPatientDBNode( NULL, _pdb );
    //ItemAdded( GetParent(m_root), m_root );

//  size_t count = m_root->GetChildren().GetCount();
//  OSLM_TRACE("setPatientDB RootChildren count: " << count);
//  for (size_t i = 0; i < count; i++)

//  while (m_root->GetChildren().GetCount() > 0)
//  {
//      fwDataNode *child = m_root->GetChildren()[0];
//      Delete(child);
//      ItemDeleted(itemRoot, child);
//  }
    m_root = ::boost::shared_ptr< fwPatientDBNode >(new fwPatientDBNode( NULL, _pdb ));
    Cleared ();

    ::fwData::PatientDB::PatientIterator patientBegin = _pdb.lock()->getPatients().first;
    ::fwData::PatientDB::PatientIterator patientEnd     = _pdb.lock()->getPatients().second;
    ::fwData::PatientDB::PatientIterator patient        = patientBegin;

    while ( patient != patientEnd )
    {
        AddPatient(*patient);
        ++patient;
    }
//  wxDataViewItem child( (void*) m_root.get() );
//  ItemAdded( NULL, child );
    //Cleared ();
}

// ----------------------------------------------------------------------------

void PatientDBTreeModel::AddPatient( ::fwData::Patient::wptr _patient )
{
    int indexP = m_root->GetChildren().GetCount();
    fwPatientNode *patient_node = new fwPatientNode( m_root.get(), _patient, indexP );
    ::fwData::Patient::StudyIterator studyBegin = _patient.lock()->getStudies().first;
    ::fwData::Patient::StudyIterator studyEnd = _patient.lock()->getStudies().second;
    ::fwData::Patient::StudyIterator study = studyBegin;

    while ( study != studyEnd )
    {
        int indexS = study - studyBegin;

        ::fwData::Study::AcquisitionIterator acquisitionBegin = (*study)->getAcquisitions().first;
        ::fwData::Study::AcquisitionIterator acquisitionEnd = (*study)->getAcquisitions().second;
        ::fwData::Study::AcquisitionIterator acquisition = acquisitionBegin;

        while ( acquisition != acquisitionEnd )
        {
            int indexA = acquisition - acquisitionBegin;
            fwAcqNode *acq_node = new fwAcqNode( patient_node, *study, *acquisition, indexP, indexS, indexA );
            patient_node->Append( acq_node );

            ++acquisition;
        }
        ++study;
        // notify control
    }
    m_root->Append( patient_node );
    // notify control
#ifndef MULTI_ROOT
    // notify control
    wxDataViewItem child( (void*) patient_node );
    wxDataViewItem parent( (void*) m_root.get() );
    ItemAdded( parent, child );
#else
    wxDataViewItem child( (void*) patient_node );
    ItemAdded( NULL, child );
#endif
}

// ----------------------------------------------------------------------------

void PatientDBTreeModel::Delete( const wxDataViewItem &item )
{
    fwDataNode* node = (fwDataNode*) item.GetID() ;
    if (!node)      // happens if item.IsOk()==false
        return;

    if (node->GetParent())
    {
        wxDataViewItem parent( node->GetParent() );
        if (!parent.IsOk())
        {
            wxASSERT(node == m_root.get());

            // don't make the control completely empty:
            wxLogError( "Cannot remove the root item!" );
        }
        else
        {
            // first remove the node from the parent's array of children;
            // NOTE: fwDataNodePtrArray is only an array of _pointers_
            //       thus removing the node from it doesn't result in freeing it
            node->GetParent()->GetChildren().Remove( node );
            delete node;
            node = NULL;
            // free the node
            if (node != m_root.get())
            {
                // notify control
                ItemDeleted( parent, item );
            }
        }
    }
}

// ----------------------------------------------------------------------------

int PatientDBTreeModel::Compare( const wxDataViewItem &item1, const wxDataViewItem &item2,
        unsigned int column, bool ascending ) const
{
    wxASSERT(item1.IsOk() && item2.IsOk());
    // should never happen

    if (IsContainer(item1) && IsContainer(item2))
    {
        wxVariant value1, value2;
        GetValue( value1, item1, 0 );
        GetValue( value2, item2, 0 );

        wxString str1 = value1.GetString();
        wxString str2 = value2.GetString();
        int res = str1.Cmp( str2 );
        if (res) return res;

        // items must be different
        wxUIntPtr litem1 = (wxUIntPtr) item1.GetID();
        wxUIntPtr litem2 = (wxUIntPtr) item2.GetID();

        return litem1-litem2;
    }

    return wxDataViewModel::Compare( item1, item2, column, ascending );
}

// ----------------------------------------------------------------------------

void PatientDBTreeModel::GetValue( wxVariant &variant,
        const wxDataViewItem &item, unsigned int col ) const
{
    wxASSERT(item.IsOk());
    fwDataNode *node = (fwDataNode*) item.GetID();
    variant = node->getCell(col);
}

// ----------------------------------------------------------------------------

bool PatientDBTreeModel::SetValue( const wxVariant &variant,
        const wxDataViewItem &item, unsigned int col )
{
    wxASSERT(item.IsOk());

    fwDataNode *node = (fwDataNode*) item.GetID();
    switch (col)
    {
    case 0:
        //node->m_title = variant.GetString();
        return true;
    case 1:
        return true;
    case 2:
        return true;
    case 3:
        return true;
    case 4:
        return true;
    case 5:
        return true;
    default:
        wxLogError( "PatientDBTreeModel::SetValue: wrong column" );
    }
    return false;
}

// ----------------------------------------------------------------------------

bool PatientDBTreeModel::HasItem( const wxDataViewItem &item ) const
{
    if (!item.IsOk())
        return false;
    fwDataNode* node = static_cast< fwDataNode * >( item.GetID() );

    bool res =  false;
    for (size_t iPatient = 0; iPatient < m_root->GetChildren().GetCount(); iPatient++)
    {
        fwDataNode *pNodePatient = m_root->GetChildren()[iPatient];
        if(node == pNodePatient)
        {
            res = true;
            break;
        }
        for (size_t iAcq = 0; iAcq < pNodePatient->GetChildren().GetCount(); iAcq++)
        {
            fwDataNode *pNodeAcq = pNodePatient->GetChildren()[iAcq];
            if(node == pNodeAcq)
            {
                res = true;
                break;
            }
        }
    }
    return res;
}
// ----------------------------------------------------------------------------

wxDataViewItem PatientDBTreeModel::GetParent( const wxDataViewItem &item ) const
{
    // the invisible root node has no parent
    if (!item.IsOk())
        return wxDataViewItem(NULL);

    fwDataNode* node = static_cast< fwDataNode * >( item.GetID() );
    if (!node)
    {
        SLM_ASSERT("node not instanced", node);
        return wxDataViewItem(NULL);
    }

    // m_root has no parent
    if (node == m_root.get())
        return wxDataViewItem(NULL);



#ifdef MULTI_ROOT
    if(!this->HasItem(item))
    {
        //TODO fixme: unknown item?!
        //assert(false && "bad item!");
        return wxDataViewItem();
    }
    if (node->GetParent() == m_root.get())
        return wxDataViewItem();
#endif

    return wxDataViewItem( (void*) node->GetParent() );
}

// ----------------------------------------------------------------------------

bool PatientDBTreeModel::IsContainer( const wxDataViewItem &item ) const
{
    // the invisble root node can have children
    if (!item.IsOk())
        return true;

    fwDataNode *node = (fwDataNode*) item.GetID();
    return node->IsContainer();
}

unsigned int PatientDBTreeModel::GetChildren( const wxDataViewItem &parent,
        wxDataViewItemArray &array ) const
{
    fwDataNode *node = (fwDataNode*) parent.GetID();
    if (!node)
    {
#ifndef MULTI_ROOT
        array.Add( wxDataViewItem( (void*) m_root.get() ) );
        return 1;
#else
        return GetChildren(m_root.get(), array);
#endif
    }

    if (node->GetChildCount() == 0)
    {
        return 0;
    }

    unsigned int count = node->GetChildren().GetCount();
    for (unsigned int pos = 0; pos < count; pos++)
    {
        fwDataNode *child = node->GetChildren().Item( pos );
        array.Add( wxDataViewItem( (void*) child  ));
    }

    return count;
}

#endif
