/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "fwGui/ICursor.hpp"

namespace fwGui
{

const ICursor::FactoryRegistryKeyType
ICursor::factoryRegistryKey =  "an unique key for all ICursor (factories are exclusives)";

ICursor::ICursor()
{}


ICursor::~ICursor()
{}


} // namespace fwGui



