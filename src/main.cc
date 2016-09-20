/*
 * Copyright (c) 2012-2015, Nic McDonald
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
#include <fio/InFile.h>
#include <fio/OutFile.h>
#include <prim/prim.h>
#include <unistd.h>

#include <cassert>
#include <climits>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <cctype>

#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <locale>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

bool toU64(const std::string _str, u64* _value) {
  char* end;
  const char* cstr = _str.c_str();
  u64 val = strtoul(cstr, &end, 0);
  if ((end - cstr) < static_cast<s64>(_str.size())) {
    return false;
  }
  *_value = val;
  return true;
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

std::string trim(std::string _str) {
  _str.erase(_str.begin(), std::find_if(
      _str.begin(), _str.end(),
      std::not1(std::ptr_fun<s32, s32>(std::isspace))));
  _str.erase(std::find_if(
      _str.rbegin(), _str.rend(),
      std::not1(std::ptr_fun<s32, s32>(std::isspace))).base(),
             _str.end());
  return _str;
}

struct Stats {
  f64 mean;
  f64 stdDev;
};

Stats meanStddev(const std::vector<u64>& _vec) {
  Stats stats;

  f64 summation = 0;
  for (auto it = _vec.begin(); it != _vec.end(); ++it) {
    summation += *it;
  }
  stats.mean = summation / _vec.size();

  f64 diffSquareSum = 0;

  for (auto it = _vec.begin(); it != _vec.end(); ++it) {
    u64 element = *it;
    diffSquareSum += pow((element - stats.mean), 2);
  }

  stats.stdDev = sqrt(diffSquareSum / _vec.size());

  return stats;
}

void extractLatency(const char* _inputFile,
                    const char* _transactionFile,
                    const char* _messageFile,
                    const char* _packetFile,
                    const char* _aggregateFile) {
  assert(_inputFile != NULL);

  fio::InFile inFile(_inputFile);

  fio::OutFile* transactionOut = nullptr;
  if (_transactionFile != NULL) {
    transactionOut = new fio::OutFile(_transactionFile);
  }

  fio::OutFile* messageOut = nullptr;
  if (_messageFile != NULL) {
    messageOut = new fio::OutFile(_messageFile);
  }

  fio::OutFile* packetOut = nullptr;
  if (_packetFile != NULL) {
    packetOut = new fio::OutFile(_packetFile);
  }

  fio::OutFile* aggregateOut = nullptr;
  if (_aggregateFile != NULL) {
    aggregateOut = new fio::OutFile(_aggregateFile);
  }

  std::unordered_map<u64, std::pair<u64, u64> > transTimes;

  std::vector<u64> pktLatency;
  std::vector<u64> msgLatency;
  std::vector<u64> transLatency;


  bool inMsg = false;
  u64 msgEnd = 0;
  u64 msgStart = U64_MAX;
  u64 msgTrans = U64_MAX;

  bool inPkt = false;
  u64 pktEnd = 0;
  u64 pktStart = U64_MAX;

  std::string line;
  std::vector<std::string> words;
  for (s64 lineNum = 1; true; lineNum++) {
    fio::InFile::Status sts = inFile.getLine(&line);
    assert(sts != fio::InFile::Status::ERROR);
    if (sts == fio::InFile::Status::END) {
      break;
    }

    line = trim(line);

    words.clear();
    u64 wordCount = split(line, &words);
    if (wordCount == 0) {
      continue;  // probably the last line
    }

    if (words.at(0) == "+T") {
      // create the transaction entry
      u64 trans;
      if (toU64(words.at(1), & trans) == false) {
        assert(false);
      }
      bool res = transTimes.insert(
          std::make_pair(trans, std::make_pair(U64_MAX, U64_MAX))).second;
      (void)res;  // unused
      assert(res);
    } else if (words.at(0) == "-T") {
      // lookup the transaction entry
      u64 trans;
      if (toU64(words.at(1), & trans) == false) {
        assert(false);
      }
      const std::pair<u64, u64>& transTime = transTimes.at(trans);
      if (transTime.second < transTime.first) {
        fprintf(stderr, "last=%lu < first=%lu\n",
                transTime.second, transTime.first);
      }
      assert(transTime.second >= transTime.first);

      // save transaction latency
      transLatency.push_back(transTime.second - transTime.first);

      // write transaction latency
      if (transactionOut) {
        transactionOut->write(std::to_string(transTime.first) + "," +
                              std::to_string(transTime.second) + "\n");
      }

      // remoce transaction entry
      u64 res = transTimes.erase(trans);
      (void)res;  // unused
      assert(res == 1);
    } else if (words.at(0) == "+M") {
      assert(inMsg == false);
      // handle the message state machine
      inMsg = true;
      msgEnd = 0;
      msgStart = U64_MAX;
      if (toU64(words.at(4), &msgTrans) == false) {
        assert(false);
      }
    } else if (words.at(0) == "-M") {
      // handle the message state machine
      assert(inMsg == true);
      assert(msgStart != U64_MAX);
      assert(msgEnd != 0);
      inMsg = false;
      assert(msgEnd >= msgStart);

      // save message latency
      msgLatency.push_back(msgEnd - msgStart);

      // write message latency
      if (messageOut) {
        messageOut->write(std::to_string(msgStart) + "," +
                          std::to_string(msgEnd) + "\n");
      }

      // update the transaction entry
      std::pair<u64, u64>& transTime = transTimes.at(msgTrans);
      if ((transTime.first == U64_MAX) || (msgStart < transTime.first)) {
        transTime.first = msgStart;
      }
      if ((transTime.second == U64_MAX) || (msgEnd > transTime.second)) {
        transTime.second = msgEnd;
      }
    } else if (words.at(0) == "+P") {
      // handle the packet state machine
      assert(inMsg == true);
      assert(inPkt == false);
      inPkt = true;
      pktEnd = 0;
      pktStart = U64_MAX;
    } else if (words.at(0) == "-P") {
      assert(inMsg == true);
      assert(inPkt == true);
      assert(pktStart != U64_MAX);
      assert(pktEnd != 0);
      inPkt = false;
      assert(pktEnd >= pktStart);
      pktLatency.push_back(pktEnd - pktStart);
      if (packetOut) {
        packetOut->write(std::to_string(pktStart) + "," +
                         std::to_string(pktEnd) + "\n");
      }
    } else if (words.at(0) == "F") {
      assert(inMsg == true);
      assert(inPkt == true);
      u64 flitStart = 0;
      u64 flitEnd = 0;
      if (toU64(words.at(2), &flitStart) == false) {
        assert(false);
      }
      if (toU64(words.at(3), &flitEnd) == false) {
        assert(false);
      }
      assert(flitEnd >= flitStart);
      if (flitStart < msgStart) {
        msgStart = flitStart;
      }
      if (flitStart < pktStart) {
        pktStart = flitStart;
      }
      if (flitEnd > msgEnd) {
        msgEnd = flitEnd;
      }
      if (flitEnd > pktEnd) {
        pktEnd = flitEnd;
      }
    } else {
      assert(false);
    }
  }

  assert(inMsg == false);
  assert(inPkt == false);

  std::sort(pktLatency.begin(), pktLatency.end());
  std::sort(msgLatency.begin(), msgLatency.end());
  std::sort(transLatency.begin(), transLatency.end());

  if ((pktLatency.size() > 0) && (aggregateOut)) {
    Stats pktStats = meanStddev(pktLatency);
    Stats msgStats = meanStddev(msgLatency);
    Stats transStats = meanStddev(transLatency);

    aggregateOut->write("Type,");
    aggregateOut->write("Count,");
    aggregateOut->write("Minimum,");
    aggregateOut->write("Maximum,");
    aggregateOut->write("Median,");
    aggregateOut->write("90th%,");
    aggregateOut->write("99th%,");
    aggregateOut->write("99.9th%,");
    aggregateOut->write("99.99th%,");
    aggregateOut->write("99.999th%,");
    aggregateOut->write("Mean,");
    aggregateOut->write("StdDev\n");

    u64 pmin, pmax, p50, p90, p99, p999, p9999, p99999;
    u64 size;

    size = pktLatency.size();
    pmin = 0;
    pmax = size - 1;
    p50 = static_cast<u64>(round(pmax * 0.50));
    p90 = static_cast<u64>(round(pmax * 0.90));
    p99 = static_cast<u64>(round(pmax * 0.99));
    p999 = static_cast<u64>(round(pmax * 0.999));
    p9999 = static_cast<u64>(round(pmax * 0.9999));
    p99999 = static_cast<u64>(round(pmax * 0.99999));

    aggregateOut->write("Packet,");
    aggregateOut->write(std::to_string(pktLatency.size()) + ",");
    aggregateOut->write(std::to_string(pktLatency.at(pmin)) + ",");
    aggregateOut->write(std::to_string(pktLatency.at(pmax)) + ",");
    aggregateOut->write(std::to_string(pktLatency.at(p50)) + ",");
    aggregateOut->write(std::to_string(pktLatency.at(p90)) + ",");
    aggregateOut->write(std::to_string(pktLatency.at(p99)) + ",");
    aggregateOut->write(std::to_string(pktLatency.at(p999)) + ",");
    aggregateOut->write(std::to_string(pktLatency.at(p9999)) + ",");
    aggregateOut->write(std::to_string(pktLatency.at(p99999)) + ",");
    aggregateOut->write(std::to_string(pktStats.mean) + ",");
    aggregateOut->write(std::to_string(pktStats.stdDev) + "\n");

    size = msgLatency.size();
    pmin = 0;
    pmax = size - 1;
    p50 = static_cast<u64>(round(pmax * 0.50));
    p90 = static_cast<u64>(round(pmax * 0.90));
    p99 = static_cast<u64>(round(pmax * 0.99));
    p999 = static_cast<u64>(round(pmax * 0.999));
    p9999 = static_cast<u64>(round(pmax * 0.9999));
    p99999 = static_cast<u64>(round(pmax * 0.99999));

    aggregateOut->write("Message,");
    aggregateOut->write(std::to_string(msgLatency.size()) + ",");
    aggregateOut->write(std::to_string(msgLatency.at(pmin)) + ",");
    aggregateOut->write(std::to_string(msgLatency.at(pmax)) + ",");
    aggregateOut->write(std::to_string(msgLatency.at(p50)) + ",");
    aggregateOut->write(std::to_string(msgLatency.at(p90)) + ",");
    aggregateOut->write(std::to_string(msgLatency.at(p99)) + ",");
    aggregateOut->write(std::to_string(msgLatency.at(p999)) + ",");
    aggregateOut->write(std::to_string(msgLatency.at(p9999)) + ",");
    aggregateOut->write(std::to_string(msgLatency.at(p99999)) + ",");
    aggregateOut->write(std::to_string(msgStats.mean) + ",");
    aggregateOut->write(std::to_string(msgStats.stdDev) + "\n");

    size = transLatency.size();
    pmin = 0;
    pmax = size - 1;
    p50 = static_cast<u64>(round(pmax * 0.50));
    p90 = static_cast<u64>(round(pmax * 0.90));
    p99 = static_cast<u64>(round(pmax * 0.99));
    p999 = static_cast<u64>(round(pmax * 0.999));
    p9999 = static_cast<u64>(round(pmax * 0.9999));
    p99999 = static_cast<u64>(round(pmax * 0.99999));

    aggregateOut->write("Transaction,");
    aggregateOut->write(std::to_string(transLatency.size()) + ",");
    aggregateOut->write(std::to_string(transLatency.at(pmin)) + ",");
    aggregateOut->write(std::to_string(transLatency.at(pmax)) + ",");
    aggregateOut->write(std::to_string(transLatency.at(p50)) + ",");
    aggregateOut->write(std::to_string(transLatency.at(p90)) + ",");
    aggregateOut->write(std::to_string(transLatency.at(p99)) + ",");
    aggregateOut->write(std::to_string(transLatency.at(p999)) + ",");
    aggregateOut->write(std::to_string(transLatency.at(p9999)) + ",");
    aggregateOut->write(std::to_string(transLatency.at(p99999)) + ",");
    aggregateOut->write(std::to_string(transStats.mean) + ",");
    aggregateOut->write(std::to_string(transStats.stdDev) + "\n");
  }

  if (transactionOut) {
    delete transactionOut;
  }
  if (messageOut) {
    delete messageOut;
  }
  if (packetOut) {
    delete packetOut;
  }
  if (aggregateOut) {
    delete aggregateOut;
  }
}

void usage(char* exe) {
  printf("Usage:\n");
  printf("  %s [options] <input file>\n", exe);
  printf("\n");
  printf("  Options:\n");
  printf("    -h         prints this message and exits\n");
  printf("    -t FILE    specifies transaction output file (optional)\n");
  printf("    -m FILE    specifies message output file (optional)\n");
  printf("    -p FILE    specifies packet output file (optional)\n");
  printf("    -a FILE    specifies aggregate output file (optional)\n");
  printf("\n");
}

s32 main(s32 _argc, char** _argv) {
  char* transactionFile = NULL;
  char* messageFile = NULL;
  char* packetFile = NULL;
  char* aggregateFile = NULL;

  s32 help = 0;

  s32 c;
  while ((c = getopt(_argc, _argv, "ht:m:p:a:")) != -1) {
    switch (c) {
      case 'h':
        help = 1;
        break;
      case 't':
        transactionFile = optarg;
        break;
      case 'm':
        messageFile = optarg;
        break;
      case 'p':
        packetFile = optarg;
        break;
      case 'a':
        aggregateFile = optarg;
        break;
      default:
        printf("unknown flag: %c\n", c);
        usage(_argv[0]);
        return -1;
    }
  }

  if (help) {
    usage(_argv[0]);
    return 0;
  }

  s32 numArgs = (_argc - optind);
  if (numArgs != 1) {
    printf("this program requires at least one argument\n");
    usage(_argv[0]);
    return -1;
  }

  char* inputFile = _argv[optind];

  extractLatency(inputFile, transactionFile, messageFile, packetFile,
                 aggregateFile);
  return 0;
}
