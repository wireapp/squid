/*
 * Copyright (C) 1996-2018 The Squid Software Foundation and contributors
 *
 * Squid software is distributed under GPLv2+ license and includes
 * contributions from numerous individuals and organizations.
 * Please see the COPYING and CONTRIBUTORS files for details.
 */

#ifndef SQUID_PROXYP_MESSAGE_H
#define SQUID_PROXYP_MESSAGE_H

#include "base/RefCount.h"
#include "ip/Address.h"
#include "proxyp/Elements.h"
#include "sbuf/SBuf.h"

namespace ProxyProtocol {

/// PROXY protocol v1 or v2 message
class Message: public RefCountable
{
public:
    typedef RefCount<Message> Pointer;
    typedef std::vector<Two::Tlv> Tlvs;

    Message(const SBuf &ver, const Two::Command cmd);

    /// HTTP header-like string representation of the message.
    /// The returned string has one line per pseudo header and
    /// one line per TLV (if any).
    SBuf toMime() const;

    /// \returns a delimiter-separated list of values of TLVs of the given type
    SBuf getValues(const uint32_t headerType, const char delimiter = ',') const;

    /// Searches for the first key=value pair occurrence within the
    /// value for the provided TLV type. Assumes that the TLV value
    /// is a delimiter-separated list.
    /// \returns the value of the found pair or the empty string.
    SBuf getElem(const uint32_t headerType, const char *member, const char delimiter) const;

    /// PROXY protocol version
    const SBuf &version() const { return version_; }

    /// whether source and destination addresses are valid addresses of the original "client" connection
    bool hasForwardedAddresses() const { return !localConnection() && hasAddresses(); }

    /// marks the message as lacking address information
    void ignoreAddresses() { ignoreAddresses_ = true; }

    /// whether the message relays address information (including LOCAL connections)
    bool hasAddresses() const { return !ignoreAddresses_; }

    /// \returns "4" or "6" if both source and destination addresses are IPv4 or IPv6
    /// \returns "mix" otherwise
    const SBuf &addressFamily() const;

    /// source address of the client connection
    Ip::Address sourceAddress;
    /// intended destination address of the client connection
    Ip::Address destinationAddress;
    /// empty in v1 messages and when ignored in v2 messages
    Tlvs tlvs;

private:
    /// Whether the connection over PROXY protocol is 'cmdLocal'.
    /// Such connections are established without being relayed.
    /// Received addresses and TLVs are discarded in this mode.
    bool localConnection() const { return command_ == Two::cmdLocal; }

    /// PROXY protocol version
    SBuf version_;

    /// for v2 messages: the command field
    /// for v1 messages: Two::cmdProxy
    Two::Command command_;

    /// true if the message relays no address information
    bool ignoreAddresses_;
};

} // namespace ProxyProtocol

#endif

