/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWWX_ACQUISITIONSELECTORPANEL_HPP_
#define _FWWX_ACQUISITIONSELECTORPANEL_HPP_

#include <vector>
#include <map>
#include <wx/wx.h>
#include <wx/panel.h>
#include <wx/event.h>
#include <wx/listctrl.h>
#include <boost/shared_ptr.hpp>

#include "fwWX/config.hpp"

namespace fwData
{
    class PatientDB;
}

/**
 * @brief This namespace provides a list of classes based on WxWidgets .
 * @namespace   fwWX
 *
 * @author IRCAD (Research and Development Team).
 * @date    2009-2010.
 */
namespace fwWX
{

/**
 * @brief   This class allows us to select an acquisition in a patient data base.
 * @class   AcquisitionSelectorPanel.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 * @note This class is not commented, because it is not used in the open framework tutorial.
 * @todo AcquisitionSelectorPanel is not commented.
 */
class FWWX_CLASS_API AcquisitionSelectorPanel : public wxPanel
{


public :

    enum
    {
        ID_LIST = wxID_HIGHEST + 1
    };

    FWWX_API AcquisitionSelectorPanel( wxWindow * _container, ::boost::shared_ptr< ::fwData::PatientDB > _associatedPatientDB, ::fwServices::IService::wptr _serviceParent );

    void FWWX_API onItemSelected( wxListEvent & event );

    void FWWX_API onItemActivated( wxListEvent & event );

    void FWWX_API onKeyDown( wxListEvent & event );

    void FWWX_API itemSelectionNotification( long itemIndex );

    void FWWX_API updateData( ::boost::shared_ptr< ::fwData::PatientDB > _associatedPatientDB, bool _bResetSelection = true );

    const FWWX_API std::vector<int> getIndexOfSelectedItem();

private :

    void eraseItemSelected( const std::vector<int> & _vItemSelected );

    //int m_nbColumnMax;

    //int m_nbLineMax;

    wxListCtrl * m_wxList;

    ::boost::shared_ptr< ::fwData::PatientDB > m_associatedPatientDB;

    ::std::map < int, std::vector<int> > m_lineToAcq;

    int m_iSelectedItemIndex;

    ::fwServices::IService::wptr m_serviceParent;

    DECLARE_EVENT_TABLE()
};

} // namespace fwWX

#endif /* _FWWX_ACQUISITIONSELECTORPANEL_HPP_ */

