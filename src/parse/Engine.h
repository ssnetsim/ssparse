/*
 * Copyright (c) 2012-2016, Nic McDonald
 * All rights reserved.
 *
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
 * - Neither the name of prim nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
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
#ifndef PARSE_ENGINE_H_
#define PARSE_ENGINE_H_

#include <fio/OutFile.h>
#include <prim/prim.h>

#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "parse/Filter.h"

class Engine {
 public:
  Engine(const std::string& _transactionsFile,
         const std::string& _messagesFile,
         const std::string& _packetsFile,
         const std::string& _aggregateFile,
         f64 _scalar,
         const std::vector<std::string>& _filters);
  ~Engine();

  void transactionStart(u64 _transId);
  void transactionEnd(u64 _transId);
  void messageStart(u32 _msgId, u32 _msgSrc, u32 _msgDst, u64 _transId,
                    u32 _trafficClass);
  void messageEnd();
  void packetStart(u32 _pktId, u32 _pktHopCount);
  void packetEnd();
  void flit(u32 _flitId, u64 _flitSendTime, u64 _flitReceiveTime);
  void complete();

 private:
  std::shared_ptr<fio::OutFile> transFile_;
  std::shared_ptr<fio::OutFile> msgsFile_;
  std::shared_ptr<fio::OutFile> pktsFile_;
  std::shared_ptr<fio::OutFile> aggFile_;

  f64 scalar_;
  std::vector<std::shared_ptr<Filter> > filters_;

  // latency vectors for aggregate computations
  std::vector<f64> transLatencies_;
  std::vector<f64> msgLatencies_;
  std::vector<f64> pktLatencies_;

  // transaction state machines
  struct TransFsm {
    TransFsm();
    ~TransFsm();
    void reset();
    f64 start;
    f64 end;
    u32 msgCount;
    u32 pktCount;
    u32 flitCount;
  };
  std::unordered_map<u64, TransFsm > transFsms_;

  // message state machine
  struct MsgFsm {
    MsgFsm();
    ~MsgFsm();
    void reset();

    bool enabled;
    f64 start;
    f64 end;
    u32 src;
    u32 dst;
    u64 transId;
    u32 trafficClass;
    u32 pktCount;
    u32 flitCount;
  };
  MsgFsm msgFsm_;

  // packet state machine
  struct PktFsm {
    PktFsm();
    ~PktFsm();
    void reset();

    bool enabled;
    f64 start;
    f64 end;
    u32 hopCount;
    u32 flitCount;
  };
  PktFsm pktFsm_;
};

#endif  // PARSE_ENGINE_H_
