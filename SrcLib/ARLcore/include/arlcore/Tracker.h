/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __ARLCORE_TRACKER_H__
#define __ARLCORE_TRACKER_H__
#include <arlcore/Common.h>

#include <arlcore/Object.h>

namespace arlCore
{
class Tracker : public arlCore::Object
{
/**
 * @brief   TODO commentaire ?? Tracker
 */
public:
    //! @brief Constructor
    ARLCORE_API Tracker( const std::string& name = "" );

    //! @brief Destructor
    ARLCORE_API ~Tracker( void );

    //! @return Description of the current tracker
    ARLCORE_API std::string getString( void ) const;

private:
};
} // namespace arlCore
#endif // __ARLCORE_TRACKER_H__
