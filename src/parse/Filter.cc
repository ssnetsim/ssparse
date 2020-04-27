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
#include "parse/Filter.h"

#include <ex/Exception.h>
#include <strop/strop.h>

#include <cassert>

#include <vector>

#include "parse/util.h"


Filter::Filter(const std::string& _description)
    : description_(_description) {
  // parse the accept/reject flag
  char accept = _description.at(0);
  if (accept == '+') {
    accept_ = true;
  } else if (accept == '-') {
    accept_ = false;
  } else {
    throw ex::Exception("invalid accept/reject flag: %c", accept);
  }

  // parse the type
  std::vector<std::string> split = strop::split(_description, '=');
  if (split.size() != 2) {
    throw ex::Exception("invalid format, must have one and only one '=': %s",
                        _description.c_str());
  }
  std::string type = strop::toLower(split.at(0).substr(1));
  bool useFloats;
  if (type == "application" || type == "app") {
    type_ = Filter::Type::APPLICATION;
    useFloats = false;
  } else if (type == "start" || type == "send") {
    type_ = Filter::Type::START;
    useFloats = true;
  } else if (type == "end" || type == "recv") {
    type_ = Filter::Type::END;
    useFloats = true;
  } else if (type == "protocolclass" || type == "pc") {
    type_ = Filter::Type::PROTOCOLCLASS;
    useFloats = false;
  } else if (type == "source" || type == "src") {
    type_ = Filter::Type::SOURCE;
    useFloats = false;
  } else if (type == "destination" || type == "dst") {
    type_ = Filter::Type::DESTINATION;
    useFloats = false;
  } else if (type == "hopcount" || type == "hc") {
    type_ = Filter::Type::HOPCOUNT;
    useFloats = false;
  } else if (type == "minhopcount" || type == "mhc") {
    type_ = Filter::Type::MINHOPCOUNT;
    useFloats = false;
  } else if (type == "nonminhopcount" || type == "nmhc") {
    type_ = Filter::Type::NONMINHOPCOUNT;
    useFloats = false;
  }  else if (type == "messagecount" || type == "msgcnt") {
    type_ = Filter::Type::MESSAGECOUNT;
    useFloats = false;
  } else if (type == "packetcount" || type == "pktcnt") {
    type_ = Filter::Type::PACKETCOUNT;
    useFloats = false;
  } else if (type == "flitcount" || type == "flitcnt") {
    type_ = Filter::Type::FLITCOUNT;
    useFloats = false;
  } else {
    throw ex::Exception("invalid type field: %s", type.c_str());
  }

  // parse the number specifier
  std::string allNumbers = split.at(1);
  std::vector<std::string> rangeStrs = strop::split(allNumbers, ',');
  for (const auto& rangeStr : rangeStrs) {
    std::vector<std::string> firstLast = strop::split(rangeStr, '-');

    if (useFloats) {
      if (firstLast.size() != 2) {
        throw ex::Exception("time based specifications must define a range. "
                            "ex: 100-200");
      }
      f64 start;
      try {
        start = toF64(firstLast.at(0));
      } catch (ex::Exception& _ex) {
        throw ex::Exception("invalid range start: %s", firstLast.at(0).c_str());
      }
      f64 end;
      try {
        end = toF64(firstLast.at(1));
      } catch (ex::Exception& _ex) {
        throw ex::Exception("invalid range end: %s", firstLast.at(1).c_str());
      }
      if (start < 0 || end < 0) {
        throw ex::Exception("time must be greater than 0");
      }
      if (start > end) {
        throw ex::Exception("invalid range bounds: %f > %f", start, end);
      }
      if (inFloatRange(start) || inFloatRange(end)) {
        throw ex::Exception("overlapping range detected");
      }
      floats_.push_back(std::make_pair(start, end));
    } else {
      if (firstLast.size() == 1) {
        u64 val;
        try {
          val = toU64(firstLast.at(0));
        } catch (ex::Exception& _ex) {
          throw ex::Exception("invalid u64 number: %s",
                              firstLast.at(0).c_str());
        }
        if (!ints_.insert(val).second) {
          throw ex::Exception("duplicate numbers detected: %lu", val);
        }
      } else if (firstLast.size() == 2) {
        u64 start, end;
        try {
          start = toU64(firstLast.at(0));
        } catch (ex::Exception& _ex) {
          throw ex::Exception("invalid u64 number: %s",
                              firstLast.at(0).c_str());
        }
        try {
          end = toU64(firstLast.at(1));
        } catch (ex::Exception& _ex) {
          throw ex::Exception("invalid u64 number: %s",
                              firstLast.at(1).c_str());
        }
        if (start > end) {
          throw ex::Exception("invalid range bounds: %lu > %lu", start, end);
        }
        for (u64 val = start; val <= end; val++) {
          if (!ints_.insert(val).second) {
            throw ex::Exception("duplicate numbers detected: %lu", val);
          }
        }
      } else {
        throw ex::Exception("invalid range spec: %s", rangeStr.c_str());
      }
    }
  }
}

Filter::~Filter() {}

const std::string& Filter::description() const {
  return description_;
}


bool Filter::transaction(u64 _transId, f64 _start, f64 _end, u32 _numMsgs,
                         u32 _numPkts, u32 _numFlits) {
  u32 appId = (u32)(_transId >> 56);

  switch (type_) {
    case Filter::Type::APPLICATION:
      return accept_ == (ints_.count(appId) == 1);

    case Filter::Type::START:
      return accept_ == inFloatRange(_start);

    case Filter::Type::END:
      return accept_ == inFloatRange(_end);

    case Filter::Type::MESSAGECOUNT:
      return accept_ == (ints_.count(_numMsgs) == 1);

    case Filter::Type::PACKETCOUNT:
      return accept_ == (ints_.count(_numPkts) == 1);

    case Filter::Type::FLITCOUNT:
      return accept_ == (ints_.count(_numFlits) == 1);

    case Filter::Type::PROTOCOLCLASS:
    case Filter::Type::SOURCE:
    case Filter::Type::DESTINATION:
    case Filter::Type::HOPCOUNT:
    case Filter::Type::MINHOPCOUNT:
    case Filter::Type::NONMINHOPCOUNT:
      return true;  // not applicable for transactions
    default:
      assert(false);
  }
}

bool Filter::message(u32 _src, u32 _dst, u64 _transId, u32 _protocolClass,
                     f64 _start, f64 _end, u32 _numPkts, u32 _numFlits,
                     u32 _minHopCount) {
  u32 appId = (u32)(_transId >> 56);

  switch (type_) {
    case Filter::Type::APPLICATION:
      return accept_ == (ints_.count(appId) == 1);

    case Filter::Type::START:
      return accept_ == inFloatRange(_start);

    case Filter::Type::END:
      return accept_ == inFloatRange(_end);

    case Filter::Type::PROTOCOLCLASS:
      return accept_ == (ints_.count(_protocolClass) == 1);

    case Filter::Type::SOURCE:
      return accept_ == (ints_.count(_src) == 1);

    case Filter::Type::DESTINATION:
      return accept_ == (ints_.count(_dst) == 1);

    case Filter::Type::PACKETCOUNT:
      return accept_ == (ints_.count(_numPkts) == 1);

    case Filter::Type::FLITCOUNT:
      return accept_ == (ints_.count(_numFlits) == 1);

    case Filter::Type::MINHOPCOUNT:
      return accept_ == (ints_.count(_minHopCount) == 1);

    case Filter::Type::HOPCOUNT:
    case Filter::Type::NONMINHOPCOUNT:
    case Filter::Type::MESSAGECOUNT:
      return true;  // not applicable for messages

    default:
      assert(false);
  }
}

bool Filter::packet(u32 _src, u32 _dst, u64 _transId, u32 _protocolClass,
                    f64 _start, f64 _end, u32 _numFlits,
                    u32 _hopCount, u32 _minHopCount, u32 _nonMinHopCount) {
  u32 appId = (u32)(_transId >> 56);

  switch (type_) {
    case Filter::Type::APPLICATION:
      return accept_ == (ints_.count(appId) == 1);

    case Filter::Type::START:
      return accept_ == inFloatRange(_start);

    case Filter::Type::END:
      return accept_ == inFloatRange(_end);

    case Filter::Type::PROTOCOLCLASS:
      return accept_ == (ints_.count(_protocolClass) == 1);

    case Filter::Type::SOURCE:
      return accept_ == (ints_.count(_src) == 1);

    case Filter::Type::DESTINATION:
      return accept_ == (ints_.count(_dst) == 1);

    case Filter::Type::FLITCOUNT:
      return accept_ == (ints_.count(_numFlits) == 1);

    case Filter::Type::HOPCOUNT:
      return accept_ == (ints_.count(_hopCount) == 1);

    case Filter::Type::MINHOPCOUNT:
      return accept_ == (ints_.count(_minHopCount) == 1);

    case Filter::Type::NONMINHOPCOUNT:
      return accept_ == (ints_.count(_nonMinHopCount) == 1);

    case Filter::Type::MESSAGECOUNT:
    case Filter::Type::PACKETCOUNT:
      return true;  // not applicable for messages

    default:
      assert(false);
  }
}

bool Filter::inFloatRange(f64 _val) const {
  for (const std::pair<f64, f64>& range : floats_) {
    if (_val >= range.first && _val < range.second) {
      return true;
    }
  }
  return false;
}
