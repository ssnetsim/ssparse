/*
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * - Neither the name of prim nor the names of its contributors may be used to
 * endorse or promote products derived from this software without specific prior
 * written permission.
 *
 * See the NOTICE file distributed with this work for additional information
 * regarding copyright ownership.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include "parse/util.h"

#include <ex/Exception.h>

#include <cassert>
#include <cstdio>

u64 toU64(const std::string& _str) {
  char* end;
  const char* cstr = _str.c_str();
  u64 val = strtoul(cstr, &end, 0);
  if ((end - cstr) < static_cast<s64>(_str.size())) {
    throw ex::Exception("non u64: %s\n", _str.c_str());
  }
  return val;
}

u32 toU32(const std::string& _str) {
  char* end;
  const char* cstr = _str.c_str();
  u32 val = strtol(cstr, &end, 0);
  if ((end - cstr) < static_cast<s64>(_str.size())) {
    throw ex::Exception("non u32: %s\n", _str.c_str());
  }
  return val;
}

f64 toF64(const std::string& _str) {
  char *end;
  const char* cstr = _str.c_str();
  f64 val = strtod(cstr, &end);
  if ((end - cstr) < static_cast<s64>(_str.size())) {
    throw ex::Exception("non f64: %s\n", _str.c_str());
  }
  return val;
}

u64 split(const std::string& _str, std::vector<std::string>* _words) {
  char delimiter = ',';
  u64 count = 0;
  u64 pos = 0;
  u64 idx;
  std::string token;
  while ((idx = _str.find(delimiter, pos)) != std::string::npos) {
    token = _str.substr(pos, idx-pos);
    _words->push_back(token);
    pos = idx + 1;
    count++;
  }
  if (pos < _str.size()) {
    token = _str.substr(pos, _str.size()-pos);
    _words->push_back(token);
    count++;
  }
  return count;
}
