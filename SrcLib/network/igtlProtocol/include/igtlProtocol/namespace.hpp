/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IGTLPROTOCOL_NAMESPACE_HPP__
#define __IGTLPROTOCOL_NAMESPACE_HPP__

/**
 * @brief for support of OpenIGTLink protocol
 **/
namespace igtlProtocol
{
/**
 * @brief converter  namespace for IConverter derived classes
 **/
namespace converter
{

} // namespace converter

/**
 * @brief archiver namespace for IReadArchive/IWriteArchiver (in-memory) implementation
 */
namespace archiver
{

} // namespace archiver

/**
 * @brief contain utilities
 */
namespace helper
{
/**
 * @brief some detail to hide to the user of ScalarToBytes utility class
 */
namespace detail
{

} // namespace detail

} // namespace helper

} // namespace igtlProtocol

#endif // __IGTLPROTOCOL_NAMESPACE_HPP__
