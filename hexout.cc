/*
** Copyright 2019 Javier Alvarado
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/
#include "libhexer/hexout.h"

#include <assert.h>

using namespace libhexer;

static const char XUPPER[] = { '0', '1', '2', '3', '4', '5', '6', '7',
                               '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'  };
static const char XLOWER[] = { '0', '1', '2', '3', '4', '5', '6', '7',
                               '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'  };

const HexOut::Config HexOut::CONFIG_DEFAULT;

HexOut XOUT;

const char * HexOut::_GetHexChars(LetterCase lcase) const
{
    const char * xchar = nullptr;

    switch (lcase) {
        case HexOut::LOWER:
            xchar = XLOWER;
            break;
        case HexOut::UPPER:
            xchar = XUPPER;
            break;
        default:
            throw std::runtime_error("invalid letter case");
    }

    return xchar;
}

string HexOut::_IntN(size_t len, uint64_t val) const
{
    assert(len <= 8);

    uint8_t buffer[8];
    for (size_t i = 1; i <= len; i++) {
        buffer[i-1] = val >> ((len - i) * 8) & 0xFF;
    }

    return Data(buffer, len);
}

inline size_t _CalcStrLen(size_t nbytes, size_t grpsz, size_t seplen)
{
    size_t ngroups = 1;
    if (grpsz > 0) {
        ngroups = nbytes / grpsz + (nbytes % grpsz ? 1 : 0);
    }
    return nbytes * 2 + seplen * (ngroups - 1);
}

// TODO Break up into grouping and non-grouping versions?
string HexOut::Data(void * ptr, size_t len) const
{
    string hex;
    hex.reserve(_CalcStrLen(len, _group_size, _group_separator.length()));

    size_t i = 0, j = 0;
    auto bytes = (uint8_t *)ptr;
    if (_group_size > 0 && _partial_group == FRONT) {
        i = _group_size - (len % _group_size);
    }
    while (j < len) {
        if (_group_size > 0 && j > 0 && i % _group_size == 0) {
            hex += _group_separator;
        }
        hex += _xchars[bytes[j] >> 4 & 0xF];
        hex += _xchars[bytes[j] >> 0 & 0xF];
        i++, j++;
    }

    return hex;
}