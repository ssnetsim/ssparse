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
#include "parse/Engine.h"

#include <ex/Exception.h>
#include <mut/mut.h>

#include <algorithm>

/*** State machine classes ***/

Engine::TransFsm::TransFsm() {
  reset();
}

Engine::TransFsm::~TransFsm() {}

void Engine::TransFsm::reset() {
  start = F64_POS_INF;
  end = F64_NEG_INF;
  msgCount = 0;
  pktCount = 0;
  flitCount = 0;
}

Engine::MsgFsm::MsgFsm() {
  reset();
}

Engine::MsgFsm::~MsgFsm() {}

void Engine::MsgFsm::reset() {
  enabled = false;
  start = F64_POS_INF;
  end = F64_NEG_INF;
  transId = U64_MAX;
  pktCount = 0;
  flitCount = 0;
}

Engine::PktFsm::PktFsm() {
  reset();
}

Engine::PktFsm::~PktFsm() {}

void Engine::PktFsm::reset() {
  enabled = false;
  headStart = F64_POS_INF;
  headEnd = F64_NEG_INF;
  tailEnd = F64_NEG_INF;
  flitCount = 0;
}

/*** Engine class ***/

Engine::Engine(const std::string& _transactionsFile,
               const std::string& _messagesFile,
               const std::string& _packetsFile,
               const std::string& _aggregateFile,
               f64 _scalar, bool _packetHeaderLatency,
               const std::vector<std::string>& _filters)
    : scalar_(_scalar), packetHeaderLatency_(_packetHeaderLatency) {
  if (_transactionsFile.size() > 0) {
    transFile_ = std::make_shared<fio::OutFile>(_transactionsFile);
  } else {
    transFile_ = nullptr;
  }

  if (_messagesFile.size() > 0) {
    msgsFile_ = std::make_shared<fio::OutFile>(_messagesFile);
  } else {
    msgsFile_ = nullptr;
  }

  if (_packetsFile.size() > 0) {
    pktsFile_ = std::make_shared<fio::OutFile>(_packetsFile);
  } else {
    pktsFile_ = nullptr;
  }

  if (_aggregateFile.size() > 0) {
    aggFile_ = std::make_shared<fio::OutFile>(_aggregateFile);
  } else {
    aggFile_ = nullptr;
  }

  for (const std::string& filter : _filters) {
    filters_.push_back(std::make_shared<Filter>(filter));
  }

  msgFsm_.reset();
  pktFsm_.reset();
}

Engine::~Engine() {}

void Engine::transactionStart(u64 _transId, u64 _transStart) {
  // add a new transaction FSM
  transFsms_.emplace(_transId, TransFsm());
  f64 transStartScaled = _transStart * scalar_;
  transFsms_.at(_transId).start = transStartScaled;
}

void Engine::transactionEnd(u64 _transId, u64 _transEnd) {
  Engine::TransFsm& transFsm = transFsms_.at(_transId);

  // finish the end time
  f64 transEndScaled = _transEnd * scalar_;
  assert(transEndScaled >= transFsm.end);
  transFsm.end = transEndScaled;

  // determine if transaction will be logged
  bool logTransaction = true;
  for (auto f : filters_) {
    if (!f->transaction(
            _transId, transFsm.start, transFsm.end, transFsm.msgCount,
            transFsm.pktCount, transFsm.flitCount)) {
      logTransaction = false;
      break;
    }
  }

  // save transaction latency
  if (logTransaction) {
    transLatencies_.push_back(transFsm.end - transFsm.start);
    if (transFile_) {
      transFile_->write(std::to_string(transFsm.start) + "," +
                        std::to_string(transFsm.end) + "\n");
    }
  }

  // remove the transaction FSM
  transFsms_.erase(_transId);
}

void Engine::messageStart(u32 _msgId, u32 _msgSrc, u32 _msgDst, u64 _transId,
                          u32 _trafficClass) {
  if (msgFsm_.enabled == true) {
    throw ex::Exception("Two '+M's without '-M'. File corrupted :(\n");
  }
  msgFsm_.enabled = true;
  msgFsm_.src = _msgSrc;
  msgFsm_.dst = _msgDst;
  msgFsm_.transId = _transId;
  msgFsm_.trafficClass = _trafficClass;

  // count this message in the transaction
  Engine::TransFsm& transFsm = transFsms_.at(_transId);
  transFsm.msgCount++;
}

void Engine::messageEnd() {
  if (msgFsm_.enabled == false) {
    throw ex::Exception("Missing '+M'. File corrupted :(\n");
  }

  // determine if message will be logged
  bool logMessage = true;
  for (auto f : filters_) {
    if (!f->message(
            msgFsm_.src, msgFsm_.dst, msgFsm_.transId, msgFsm_.trafficClass,
            msgFsm_.start, msgFsm_.end, msgFsm_.pktCount, msgFsm_.flitCount)) {
      logMessage = false;
      break;
    }
  }

  // save message latency
  if (logMessage) {
    msgLatencies_.push_back(msgFsm_.end - msgFsm_.start);
    if (msgsFile_) {
      msgsFile_->write(std::to_string(msgFsm_.start) + "," +
                       std::to_string(msgFsm_.end) + "\n");
    }
  }

  // update the transaction times
  Engine::TransFsm& transFsm = transFsms_.at(msgFsm_.transId);
  assert(msgFsm_.start >= transFsm.start);
  if (msgFsm_.end > transFsm.end) {
    transFsm.end = msgFsm_.end;
  }

  // reset the state machine
  msgFsm_.reset();
}

void Engine::packetStart(u32 _pktId, u32 _pktHopCount) {
  if (msgFsm_.enabled == false) {
    throw ex::Exception("Missing '+M'. File corrupted :(\n");
  }
  if (pktFsm_.enabled == true) {
    throw ex::Exception("Two '+P's without '-S'. File corrupted :(\n");
  }
  pktFsm_.enabled = true;
  pktFsm_.hopCount = _pktHopCount;

  // count this packet in the transaction and message
  Engine::TransFsm& transFsm = transFsms_.at(msgFsm_.transId);
  transFsm.pktCount++;
  msgFsm_.pktCount++;
}

void Engine::packetEnd() {
  if (msgFsm_.enabled == false) {
    throw ex::Exception("Missing '+M'. File corrupted :(\n");
  }
  if (pktFsm_.enabled == false) {
    throw ex::Exception("Missing '+P'. File corrupted :(\n");
  }

  // determine the right packet end time
  f64 pktEnd = packetHeaderLatency_ ? pktFsm_.headEnd : pktFsm_.tailEnd;

  // determine if the packet will be logged
  bool logPacket = true;
  for (auto f : filters_) {
    if (!f->packet(
            msgFsm_.src, msgFsm_.dst, msgFsm_.transId, msgFsm_.trafficClass,
            pktFsm_.headStart, pktEnd, pktFsm_.hopCount, pktFsm_.flitCount)) {
      logPacket = false;
      break;
    }
  }

  // save the packet latency
  if (logPacket) {
    pktLatencies_.push_back(pktEnd - pktFsm_.headStart);
    if (pktsFile_) {
      pktsFile_->write(std::to_string(pktFsm_.headStart) + "," +
                       std::to_string(pktEnd) + "\n");
    }
  }

  // update the message times
  if (pktFsm_.headStart < msgFsm_.start) {
    msgFsm_.start = pktFsm_.headStart;
  }
  if (pktFsm_.tailEnd > msgFsm_.end) {
    msgFsm_.end = pktFsm_.tailEnd;
  }

  // reset the state machine
  pktFsm_.reset();
}

void Engine::flit(u32 _flitId, u64 _flitSendTime, u64 _flitReceiveTime) {
  if (pktFsm_.enabled == false) {
    throw ex::Exception("Missing '+P'. File corrupted :(\n");
  }
  if (pktFsm_.enabled == false) {
    throw ex::Exception("Missing '+P'. File corrupted :(\n");
  }

  // count this flit in the transaction, message, and packet
  Engine::TransFsm& transFsm = transFsms_.at(msgFsm_.transId);
  transFsm.flitCount++;
  msgFsm_.flitCount++;
  pktFsm_.flitCount++;

  // scale the flit time
  if (_flitSendTime > _flitReceiveTime) {
    throw ex::Exception("Flit received before it was sent? "
                        "File corrupted :(\n");
  }
  f64 sendTime = _flitSendTime * scalar_;
  f64 recvTime = _flitReceiveTime * scalar_;

  // update th packet times
  if (_flitId == 0) {
    pktFsm_.headStart = sendTime;
    pktFsm_.headEnd = recvTime;
  } else {
    // flit 0 should always be earliest
    assert(sendTime >= pktFsm_.headStart);
  }
  if (recvTime > pktFsm_.tailEnd) {
    pktFsm_.tailEnd = recvTime;
  }
}

void Engine::complete() {
  // check that all state machines completed
  if ((transFsms_.size() != 0) ||
      (msgFsm_.enabled == true) ||
      (pktFsm_.enabled == true)) {
    throw ex::Exception("ERROR: State machines didn't complete. "
                        "Input file is likely corrupted.\n");
  }

  // generate aggregate log
  if (pktLatencies_.size() > 0 && aggFile_) {
    // sort data
    std::sort(transLatencies_.begin(), transLatencies_.end());
    std::sort(msgLatencies_.begin(), msgLatencies_.end());
    std::sort(pktLatencies_.begin(), pktLatencies_.end());

    // complete arithmetic mean, variance, and standard deviation
    f64 transMean = mut::arithmeticMean<f64>(transLatencies_);
    f64 msgMean = mut::arithmeticMean<f64>(msgLatencies_);
    f64 pktMean = mut::arithmeticMean<f64>(pktLatencies_);

    f64 transVariance = mut::variance<f64>(transLatencies_, transMean);
    f64 msgVariance = mut::variance<f64>(msgLatencies_, msgMean);
    f64 pktVariance = mut::variance<f64>(pktLatencies_, pktMean);

    f64 transStdDev = mut::standardDeviation<f64>(transVariance);
    f64 msgStdDev = mut::standardDeviation<f64>(msgVariance);
    f64 pktStdDev = mut::standardDeviation<f64>(pktVariance);

    // write header
    aggFile_->write("Type,");
    aggFile_->write("Count,");
    aggFile_->write("Minimum,");
    aggFile_->write("Maximum,");
    aggFile_->write("Median,");
    aggFile_->write("90th%,");
    aggFile_->write("99th%,");
    aggFile_->write("99.9th%,");
    aggFile_->write("99.99th%,");
    aggFile_->write("99.999th%,");
    aggFile_->write("Mean,");
    aggFile_->write("Variance,");
    aggFile_->write("StdDev\n");

    f64 pmin, pmax, p50, p90, p99, p999, p9999, p99999;
    u64 size;

    // write packet statistics
    size = pktLatencies_.size();
    pmin = 0;
    pmax = size - 1;
    p50 = round(pmax * 0.50);
    p90 = round(pmax * 0.90);
    p99 = round(pmax * 0.99);
    p999 = round(pmax * 0.999);
    p9999 = round(pmax * 0.9999);
    p99999 = round(pmax * 0.99999);

    aggFile_->write("Packet,");
    aggFile_->write(std::to_string(pktLatencies_.size()) + ",");
    aggFile_->write(std::to_string(pktLatencies_.at(pmin)) + ",");
    aggFile_->write(std::to_string(pktLatencies_.at(pmax)) + ",");
    aggFile_->write(std::to_string(pktLatencies_.at(p50)) + ",");
    aggFile_->write(std::to_string(pktLatencies_.at(p90)) + ",");
    aggFile_->write(std::to_string(pktLatencies_.at(p99)) + ",");
    aggFile_->write(std::to_string(pktLatencies_.at(p999)) + ",");
    aggFile_->write(std::to_string(pktLatencies_.at(p9999)) + ",");
    aggFile_->write(std::to_string(pktLatencies_.at(p99999)) + ",");
    aggFile_->write(std::to_string(pktMean) + ",");
    aggFile_->write(std::to_string(pktVariance) + ",");
    aggFile_->write(std::to_string(pktStdDev) + "\n");

    size = msgLatencies_.size();
    pmin = 0;
    pmax = size - 1;
    p50 = round(pmax * 0.50);
    p90 = round(pmax * 0.90);
    p99 = round(pmax * 0.99);
    p999 = round(pmax * 0.999);
    p9999 = round(pmax * 0.9999);
    p99999 = round(pmax * 0.99999);

    aggFile_->write("Message,");
    aggFile_->write(std::to_string(msgLatencies_.size()) + ",");
    aggFile_->write(std::to_string(msgLatencies_.at(pmin)) + ",");
    aggFile_->write(std::to_string(msgLatencies_.at(pmax)) + ",");
    aggFile_->write(std::to_string(msgLatencies_.at(p50)) + ",");
    aggFile_->write(std::to_string(msgLatencies_.at(p90)) + ",");
    aggFile_->write(std::to_string(msgLatencies_.at(p99)) + ",");
    aggFile_->write(std::to_string(msgLatencies_.at(p999)) + ",");
    aggFile_->write(std::to_string(msgLatencies_.at(p9999)) + ",");
    aggFile_->write(std::to_string(msgLatencies_.at(p99999)) + ",");
    aggFile_->write(std::to_string(msgMean) + ",");
    aggFile_->write(std::to_string(msgVariance) + ",");
    aggFile_->write(std::to_string(msgStdDev) + "\n");

    size = transLatencies_.size();
    pmin = 0;
    pmax = size - 1;
    p50 = round(pmax * 0.50);
    p90 = round(pmax * 0.90);
    p99 = round(pmax * 0.99);
    p999 = round(pmax * 0.999);
    p9999 = round(pmax * 0.9999);
    p99999 = round(pmax * 0.99999);

    aggFile_->write("Transaction,");
    aggFile_->write(std::to_string(transLatencies_.size()) + ",");
    aggFile_->write(std::to_string(transLatencies_.at(pmin)) + ",");
    aggFile_->write(std::to_string(transLatencies_.at(pmax)) + ",");
    aggFile_->write(std::to_string(transLatencies_.at(p50)) + ",");
    aggFile_->write(std::to_string(transLatencies_.at(p90)) + ",");
    aggFile_->write(std::to_string(transLatencies_.at(p99)) + ",");
    aggFile_->write(std::to_string(transLatencies_.at(p999)) + ",");
    aggFile_->write(std::to_string(transLatencies_.at(p9999)) + ",");
    aggFile_->write(std::to_string(transLatencies_.at(p99999)) + ",");
    aggFile_->write(std::to_string(transMean) + ",");
    aggFile_->write(std::to_string(transVariance) + ",");
    aggFile_->write(std::to_string(transStdDev) + "\n");
  }
}
