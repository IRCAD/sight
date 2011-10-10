/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UIVISUQT_BASICFLOATEDITOR_HPP_
#define _UIVISUQT_BASICFLOATEDITOR_HPP_

#include <QObject>
#include <QPointer>
#include <QLineEdit>

#include <fwTools/Failed.hpp>

#include <gui/editor/IEditor.hpp>

#include "uiVisuQt/config.hpp"

namespace uiVisu
{

/**
 * @brief   BasicFloatEditor service permits to edit a float value.
 * @class   BasicFloatEditor.
 * @author  IRCAD (Research and Development Team).
 * @date    2011.
 */
class UIVISUQT_CLASS_API BasicFloatEditor : public QObject, public ::gui::editor::IEditor
{
    Q_OBJECT
public :

    fwCoreServiceClassDefinitionsMacro ( (BasicFloatEditor)(::gui::editor::IEditor) ) ;

    /// Constructor. Do nothing.
    UIVISUQT_API BasicFloatEditor() throw() ;

    /// Destructor. Do nothing.
    UIVISUQT_API virtual ~BasicFloatEditor() throw() ;

protected:

    typedef ::fwRuntime::ConfigurationElement::sptr Configuration;

    /// Install the layout.
    virtual void starting() throw(::fwTools::Failed);

    /// Destroy the layout.
    virtual void stopping() throw(::fwTools::Failed);

    /// Do nothing
    virtual void updating( ::boost::shared_ptr< const fwServices::ObjectMsg > _msg ) throw(::fwTools::Failed);

    /// Do nothing
    virtual void updating() throw(::fwTools::Failed);

    /// Do nothing
    virtual void swapping() throw(::fwTools::Failed);

    /// Configure the editor. Do nothing.
    virtual void configuring() throw(fwTools::Failed);

    /// Overrides
    virtual void info( std::ostream &_sstream ) ;

protected slots:

    /**
     * @brief This method is called when the value change.
     *
     * Check if the value is a valid float, update the Float object value and notify the modification.
     */
    void onModifyValue(QString value);

private:

    QPointer< QLineEdit > m_valueCtrl;

};

} // uiVisu

#endif // _UIVISUQT_BASICFLOATEDITOR_HPP_


