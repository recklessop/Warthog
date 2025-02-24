#pragma once
#include "general/funds.hpp"
#include <cstdint>
class Writer;
Writer& operator<<(Writer&, CompactUInt);
class CompactUInt {
    static std::optional<Funds> uncompact_value(uint16_t val)
    { // OK
        uint64_t e = (val & uint64_t(0xFC00u)) >> 10;
        uint64_t m = (val & uint64_t(0x03FFu)) + uint64_t(0x0400u);
        if (e < 10) {
            return Funds::from_value(m >> (10 - e));
        } else {
            return Funds::from_value(m << (e - 10));
        }
    }
    constexpr CompactUInt(uint16_t val)
        : val(val)
    {
    }

public:
    static constexpr auto smallest(){return CompactUInt(0);}
    static CompactUInt from_value_assert(uint16_t val)
    {
        auto v { from_value(val) };
        assert(v.has_value());
        return *v;
    }
    static CompactUInt from_value_throw(uint16_t val)
    {
        auto v { from_value(val) };
        if (!v)
            throw Error(EBADFEE);
        return *v;
    }
    static std::optional<CompactUInt> from_value(uint16_t val)
    {
        if (uncompact_value(val).has_value())
            return CompactUInt { val };
        return {};
    }
    Funds uncompact() const
    {
        auto v { uncompact_value(val) };
        assert(v.has_value());
        return *v;
    };
    auto to_string() const { return uncompact().to_string(); }
    static CompactUInt compact(Funds);
    auto next() const
    {
        auto res(*this);
        res.val += 1;
        return res;
        assert(res.val != 0);
    }
    uint16_t value() const { return val; }

    // default comparison is correct even without uncompacting.
    auto operator<=>(const CompactUInt&) const = default;

private:
    uint16_t val;
};
