/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <wx/wx.h>
#include <wx/dataview.h>

#include <fwData/PatientDB.hpp>
#include <fwData/Patient.hpp>
#include <fwData/Study.hpp>
#include <fwData/Acquisition.hpp>
#include <fwData/Reconstruction.hpp>

// ----------------------------------------------------------------------------
// fwDataNode: a node inside PatientDBTreeModel
// ----------------------------------------------------------------------------

class fwDataNode;
WX_DEFINE_ARRAY_PTR( fwDataNode*, fwDataNodePtrArray );

class fwDataNode
{
public:
        fwDataNode( fwDataNode* parent):
                m_vIndex(3,0)
        {
                m_parent = parent;
                m_container = false;
                m_isSelected = false;
        }

        virtual ~fwDataNode()
        {
        }

        virtual wxVariant getCell(unsigned int col) = 0;

        bool IsContainer() const
        { return m_container; }

        fwDataNode* GetParent()
        { return m_parent; }
        fwDataNodePtrArray& GetChildren()
        { return m_children; }
        fwDataNode* GetNthChild( unsigned int n )
        { return m_children.Item( n ); }
        void Insert( fwDataNode* child, unsigned int n)
        { m_children.Insert( child, n); }
        void Append( fwDataNode* child )
        { m_children.Add( child ); }
        unsigned int GetChildCount() const
        { return m_children.GetCount(); }
        std::vector<int> getPosition() const
        { return m_vIndex; }

        bool isSelected(){return m_isSelected;}
        void Select(bool bSelect){m_isSelected = bSelect;}

public:     // public to avoid getters/setters


        // TODO/FIXME:
        // the GTK version of wxDVC (in particular wxDataViewCtrlInternal::ItemAdded)
        // needs to know in advance if a node is or _will be_ a container.
        // Thus implementing:
        //   bool IsContainer() const
        //    { return m_children.GetCount()>0; }
        // doesn't work with wxGTK when PatientDBTreeModel::AddToClassical is called
        // AND the classical node was removed (a new node temporary without children
        // would be added to the control)
        bool m_container;

private:
        fwDataNode          *m_parent;
        fwDataNodePtrArray   m_children;
protected:
        std::vector<int>     m_vIndex;
        bool                             m_isSelected;
};

class fwPatientDBNode: public fwDataNode
{
public:
        fwPatientDBNode( fwDataNode* parent,const ::fwData::PatientDB::wptr _patientDB) :
                fwDataNode(parent)
                {
                m_patientDB = _patientDB;
                m_container = true;
                }
        ~fwPatientDBNode()
        {
                // free all our children nodes
                size_t count = GetChildCount();
                for (size_t i = 0; i < count; i++)
                {
                        fwDataNode *child = GetNthChild(i);
                        delete child;
                        child = 0;
                }
                m_patientDB.reset();
        }
        wxVariant getCell(unsigned int col);
        ::fwData::PatientDB::wptr getPatientDB(){ return m_patientDB; }

private :
        ::fwData::PatientDB::wptr m_patientDB;
};

class fwPatientNode: public fwDataNode
{
public:
        fwPatientNode( fwDataNode* parent,const ::fwData::Patient::wptr _patient, const int _indexP) :
                fwDataNode(parent)
                {
                m_patient = _patient;
                m_container = true;
                m_vIndex[0] = _indexP;
                }
        ~fwPatientNode()
        {
                // free all our children nodes
                size_t count = GetChildCount();
                for (size_t i = 0; i < count; i++)
                {
                        fwDataNode *child = GetNthChild(i);
                        delete child;
                        child = 0;
                }
                m_patient.reset();
        }
        wxVariant getCell(unsigned int col);
        ::fwData::Patient::wptr getPatient(){ return m_patient; }

private :
        ::fwData::Patient::wptr m_patient;
};

class fwAcqNode: public fwDataNode
{
public:
        fwAcqNode( fwDataNode* parent, const ::fwData::Study::wptr _study,
                        const ::fwData::Acquisition::wptr _acq
                        , const int _indexP, const int _indexS, const int _indexA) :
                fwDataNode(parent)
                {
                m_acq = _acq;
                m_study = _study;
                m_container = false;
                m_vIndex[0] = _indexP;
                m_vIndex[1] = _indexS;
                m_vIndex[2] = _indexA;
                }

        ~fwAcqNode()
        {
                // free all our children nodes
                size_t count = GetChildCount();
                for (size_t i = 0; i < count; i++)
                {
                        fwDataNode *child = GetNthChild(i);
                        delete child;
                        child = 0;
                }
                m_acq.reset();
                m_study.reset();
        }
        wxVariant getCell(unsigned int col);

        ::fwData::Acquisition::wptr getAcq(){ return m_acq; }
        ::fwData::Study::wptr getStudy(){ return m_study; }

private :
        ::fwData::Acquisition::wptr m_acq;
        ::fwData::Study::wptr m_study;
};

// ----------------------------------------------------------------------------
// PatientDBTreeModel
// ----------------------------------------------------------------------------

/**
 * @brief       This class allows us to represent ::fwData::PatientDB with a wxDataViewModel.
 * @class       PatientDBTreeModel.
 * @author      IRCAD (Research and Development Team).
 * @date        2009.
 * @note This class is not commented, because it is not used in the open framework tutorial.
 * @see This data view model is used by AcquisitionSelectorPanelDataView to manipulate PatientDB.
 * @todo PatientDBTreeModel is not commented.
 */
class PatientDBTreeModel: public wxDataViewModel
{
public:
        PatientDBTreeModel(::fwData::PatientDB::wptr _pdb);
        ~PatientDBTreeModel()
        {
                m_root.reset();
        }

        void AddPatient( ::fwData::Patient::wptr _patient );
        void setPatientDB(::fwData::PatientDB::wptr _pdb);

        void SelectNode(fwDataNode* _node);

        void Delete( const wxDataViewItem &item );

        // override sorting to always sort branches ascendingly

        int Compare( const wxDataViewItem &item1, const wxDataViewItem &item2,
                        unsigned int column, bool ascending ) const;

        // implementation of base class virtuals to define model

        virtual unsigned int GetColumnCount() const
        {
                return 6;
        }

        virtual wxString GetColumnType( unsigned int col ) const;

        virtual void GetValue( wxVariant &variant,
                        const wxDataViewItem &item, unsigned int col ) const;
        virtual bool SetValue( const wxVariant &variant,
                        const wxDataViewItem &item, unsigned int col );

        virtual wxDataViewItem GetParent( const wxDataViewItem &item ) const;
        virtual bool IsContainer( const wxDataViewItem &item ) const;
        virtual unsigned int GetChildren( const wxDataViewItem &parent,
                        wxDataViewItemArray &array ) const;

        ::boost::shared_ptr<fwDataNode> getRoot(){return m_root;}
private:
        ::boost::shared_ptr< fwDataNode >   m_root;
        bool HasItem( const wxDataViewItem &item ) const;

};


