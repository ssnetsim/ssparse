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
#ifndef PARSE_FILTER_H_
#define PARSE_FILTER_H_

#include <prim/prim.h>

#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

class Filter {
 public:
  explicit Filter(const std::string& _description);
  ~Filter();

  const std::string& description() const;

  bool transaction(u64 _transId, f64 _start, f64 _end, u32 _numMsgs,
                   u32 _numPkts, u32 _numFlits);

  bool message(u32 _src, u32 _dst, u64 _transId, u32 _protocolClass,
               u32 _opcode, f64 _start, f64 _end, u32 _numPkts, u32 _numFlits,
               u32 _minHopCount);

  bool packet(u32 _src, u32 _dst, u64 _transId, u32 _protocolClass, u32 _opcode,
              f64 _start, f64 _end, u32 _numFlits, u32 _hopCount,
              u32 _minHopCount, u32 _nonMinHopCount);

 private:
  enum class Type {
    APPLICATION,
    START,
    END,
    PROTOCOLCLASS,
    OPCODE,
    SOURCE,
    DESTINATION,
    HOPCOUNT,
    MINHOPCOUNT,
    NONMINHOPCOUNT,
    MESSAGECOUNT,
    PACKETCOUNT,
    FLITCOUNT
  };

  bool inFloatRange(f64 _val) const;

  std::string description_;
  Type type_;
  bool accept_;
  std::unordered_set<u64> ints_;
  std::vector<std::pair<f64, f64> > floats_;
};

#endif  // PARSE_FILTER_H_
