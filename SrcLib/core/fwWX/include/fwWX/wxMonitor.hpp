/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWWX_WXMONITOR_HPP_
#define _FWWX_WXMONITOR_HPP_

#include <map>
#include <list>

#include <wx/wx.h>
#include <wx/treectrl.h>

#include <fwServices/monitor/Monitor.hpp>

#include <fwData/Object.hpp>

#include "fwWX/config.hpp"

namespace fwWX
{


class FWWX_CLASS_API ObjectPtrItemData : public wxTreeItemData
    {
        public:
            FWWX_API ObjectPtrItemData(::boost::shared_ptr< ::fwData::Object > objPtr) : m_objPtr(objPtr) { }

            FWWX_API ::boost::shared_ptr< ::fwData::Object > GetObjectPtr() { return m_objPtr; }

        private:
          ::boost::shared_ptr< ::fwData::Object > m_objPtr;
    };


class FWWX_CLASS_API wxMonitorCallback
{
    public:

    typedef ::boost::shared_ptr< wxMonitorCallback > ptr;

    virtual FWWX_API void operator() (ObjectPtrItemData* selected, int radio_selection) {};

};

/**
 * @brief	This class is used for monitoring applications.
 * @class	wxMonitor.
 * @author	IRCAD (Research and Development Team).
 * @date	2009.
* @todo wxMonitor is not commented.
 */
class FWWX_CLASS_API wxMonitor : public ::wxPanel
{
public:

	FWWX_API wxMonitor(wxWindow* parent, ::fwServices::monitor::map_object_servicesNames & map_string) ;

	FWWX_API wxMonitor(wxWindow* parent, ::fwServices::monitor::string_map & map_string) ;

    void FWWX_API setOnSelectionCallback( wxMonitorCallback::ptr callback);

private :

	wxTreeCtrl * m_tree ;
	::boost::shared_ptr< ::fwData::Object > m_ObjPtr ;
	wxRadioBox* m_radioBox ;

    wxMonitorCallback::ptr m_onSelectionCallback;
	FWWX_API void OnSelection( wxTreeEvent& event ) ;

	~wxMonitor() ;
};

}//namespace fwWX

#endif // _FWWX_WXMONITOR_HPP_
