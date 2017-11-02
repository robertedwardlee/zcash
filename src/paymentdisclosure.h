// Copyright (c) 2017 The Zcash developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef ZCASH_PAYMENTDISCLOSURE_H
#define ZCASH_PAYMENTDISCLOSURE_H

#include "uint256.h"
#include "clientversion.h"
#include "serialize.h"
#include "streams.h"
#include "version.h"

// For JSOutPoint
#include "wallet/wallet.h"

#include <cstdint>
#include <string>


#define PAYMENT_DISCLOSURE_VERSION_EXPERIMENTAL 0

typedef JSOutPoint PaymentDisclosureKey;

struct PaymentDisclosureInfo {
    uint8_t version;          // 0 = experimental, 1 = first production version, etc.
    uint256 esk;              // zcash/NoteEncryption.cpp
    uint256 joinSplitPrivKey; // primitives/transaction.h
    // ed25519 - not tied to implementation e.g. libsodium, see ed25519 rfc

    libzcash::PaymentAddress zaddr;

    PaymentDisclosureInfo() : version(PAYMENT_DISCLOSURE_VERSION_EXPERIMENTAL) {
    }

    PaymentDisclosureInfo(uint8_t v, uint256 esk, uint256 key, libzcash::PaymentAddress zaddr) : version(v), esk(esk), joinSplitPrivKey(key), zaddr(zaddr) { }

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
        READWRITE(version);
        READWRITE(esk);
        READWRITE(joinSplitPrivKey);
        READWRITE(zaddr);
    }

    std::string ToString() const;

    friend bool operator==(const PaymentDisclosureInfo& a, const PaymentDisclosureInfo& b) {
        return (a.version == b.version && a.esk == b.esk && a.joinSplitPrivKey == b.joinSplitPrivKey && a.zaddr == b.zaddr);
    }

    friend bool operator!=(const PaymentDisclosureInfo& a, const PaymentDisclosureInfo& b) {
        return !(a == b);
    }

};


struct PaymentDisclosurePayload {
    uint8_t version;        // 0 = experimental, 1 = first production version, etc.
    uint256 esk;            // zcash/NoteEncryption.cpp
    uint256 txid;           // primitives/transaction.h
    size_t js;              // Index into CTransaction.vjoinsplit
    uint8_t n;              // Index into JSDescription fields of length ZC_NUM_JS_OUTPUTS
    libzcash::PaymentAddress zaddr; // zcash/Address.hpp
    std::string message;     // parameter to RPC call

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
        READWRITE(version);
        READWRITE(esk);
        READWRITE(txid);
        READWRITE(js);
        READWRITE(n);
        READWRITE(zaddr);
        READWRITE(message);
    }

    std::string ToString() const;

    friend bool operator==(const PaymentDisclosurePayload& a, const PaymentDisclosurePayload& b) {
        return (
            a.version == b.version &&
            a.esk == b.esk &&
            a.txid == b.txid &&
            a.js == b.js &&
            a.n == b.n &&
            a.zaddr == b.zaddr &&
            a.message == b.message
            );
    }

    friend bool operator!=(const PaymentDisclosurePayload& a, const PaymentDisclosurePayload& b) {
        return !(a == b);
    }
};

struct PaymentDisclosure {
    PaymentDisclosurePayload            payload;
    boost::array<unsigned char, 64>     payloadSig;
    // We use boost array because serialize doesn't like char buffer, otherwise we could do: unsigned char payloadSig[64];

    PaymentDisclosure() {};
    PaymentDisclosure(const PaymentDisclosurePayload payload, const boost::array<unsigned char, 64> sig) : payload(payload), payloadSig(sig) {};
    PaymentDisclosure(const uint256& joinSplitPubKey, const PaymentDisclosureKey& key, const PaymentDisclosureInfo& info, const std::string& message);

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
        READWRITE(payload);
        READWRITE(payloadSig);
    }

    std::string ToString() const;

    friend bool operator==(const PaymentDisclosure& a, const PaymentDisclosure& b) {
        return (a.payload == b.payload && a.payloadSig == b.payloadSig);
    }

    friend bool operator!=(const PaymentDisclosure& a, const PaymentDisclosure& b) {
        return !(a == b);
    }
};



typedef std::pair<PaymentDisclosureKey, PaymentDisclosureInfo> PaymentDisclosureKeyInfo;


#endif // ZCASH_PAYMENTDISCLOSURE_H
