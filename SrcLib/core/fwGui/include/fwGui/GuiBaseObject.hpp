/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGUI_GUIBASEOBJECT_HPP__
#define __FWGUI_GUIBASEOBJECT_HPP__


#include <boost/enable_shared_from_this.hpp>

#include <fwCore/base.hpp>

#include "fwGui/factory/new.hpp"
#include "fwGui/registry/detail.hpp"
#include "fwGui/config.hpp"

namespace fwGui
{

/**
 * @brief Base class for all fwGui's classes
 */
class FWGUI_CLASS_API GuiBaseObject : public ::fwCore::BaseObject
{

public:

    typedef ::fwGui::factory::Key Key;

    /**
     * @brief Class used to register a class factory in factory registry.
     * This class defines also the object factory ( 'create' )
     *
     * @tparam T Factory product type
     */
    template <typename T>
    class Registrar
    {
    public:
        Registrar(std::string functorKey)
        {
            ::fwGui::registry::get()->addFactory(functorKey, &::fwGui::factory::New<T>);
        }
    };

    fwCoreNonInstanciableClassDefinitionsMacro( (GuiBaseObject)(::fwCore::BaseObject) );

protected:
    /**
     * @name Constructor/Destructor
     * @{ */

    GuiBaseObject()
    {
    }
    virtual ~GuiBaseObject()
    {
    }

    /**  @} */

};

} // namespace fwGui

#endif // __FWGUI_GUIBASEOBJECT_HPP__

