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
#include <ex/Exception.h>
#include <fio/InFile.h>
#include <prim/prim.h>
#include <strop/strop.h>
#include <tclap/CmdLine.h>

#include <string>
#include <vector>

#include "parse/Engine.h"
#include "parse/util.h"

s32 main(s32 _argc, char** _argv) {
  std::string inputFile;
  std::string transactionFile;
  std::string messageFile;
  std::string packetFile;
  std::string latencyfile;
  std::string hopcountfile;
  f64 scalar;
  bool packetHeaderLatency;
  std::vector<std::string> filterStrs;

  std::string description =
      ("Parse and analyze SuperSim output files (.mpf). "
       "Nic McDonald. See LICENSE and NOTICE files for copyright details.");

  try {
    // create the command line parser
    TCLAP::CmdLine cmd(description, ' ', "1.0");

    // define command line args
    TCLAP::UnlabeledValueArg<std::string> inputFileArg(
        "inputfile", "input file to be parsed", true, "", "filename", cmd);
    TCLAP::ValueArg<std::string> transactionFileArg(
        "t", "transactionfile", "output transaction latencies file", false, "",
        "filename", cmd);
    TCLAP::ValueArg<std::string> messageFileArg("m", "messagefile",
                                                "output message latencies file",
                                                false, "", "filename", cmd);
    TCLAP::ValueArg<std::string> packetFileArg("p", "packetfile",
                                               "output packet latencies file",
                                               false, "", "filename", cmd);
    TCLAP::ValueArg<std::string> latencyFileArg(
        "l", "latencyfile", "output aggregate latencies file", false, "",
        "filename", cmd);
    TCLAP::ValueArg<std::string> hopcountFileArg(
        "c", "hopcountfile", "output aggregate hopcounts file", false, "",
        "filename", cmd);
    TCLAP::ValueArg<f64> scalarArg("s", "scalar", "latency scalar", false, 1.0,
                                   "f64", cmd);
    TCLAP::SwitchArg packetHeaderLatencyArg(
        "", "headerlatency", "use header latency for packets", cmd, false);
    TCLAP::MultiArg<std::string> filterStrsArg(
        "f", "filter", "acceptance filters", false, "filter description", cmd);

    // parse the command line
    cmd.parse(_argc, _argv);

    // copy the values out to variables
    inputFile = inputFileArg.getValue();
    transactionFile = transactionFileArg.getValue();
    messageFile = messageFileArg.getValue();
    packetFile = packetFileArg.getValue();
    latencyfile = latencyFileArg.getValue();
    hopcountfile = hopcountFileArg.getValue();
    scalar = scalarArg.getValue();
    packetHeaderLatency = packetHeaderLatencyArg.getValue();
    filterStrs = filterStrsArg.getValue();
  } catch (TCLAP::ArgException& e) {
    throw std::runtime_error(e.error().c_str());
  }

  // create a processing engine
  Engine engine(transactionFile, messageFile, packetFile, latencyfile,
                hopcountfile, scalar, packetHeaderLatency, filterStrs);

  // create input file object
  if (inputFile.size() == 0) {
    throw ex::Exception("How do you expect to open a file without a name?\n");
  }
  fio::InFile inFile(inputFile);

  // feed the contents of the file into the processing engine line by line
  std::string line;
  std::vector<std::string> words;
  for (s64 lineNum = 1; true; lineNum++) {
    fio::InFile::Status sts = inFile.getLine(&line);
    if (sts == fio::InFile::Status::ERROR) {
      throw ex::Exception("Error while reading input file\n");
    }
    if (sts == fio::InFile::Status::END) {
      break;
    }

    line = strop::trim(line);
    words.clear();
    u64 wordCount = split(line, &words);
    if (wordCount == 0) {
      continue;  // probably the last line
    }

    if (words.at(0) == "+T") {
      // parse the transaction start command
      u64 transId = toU64(words.at(1));
      u64 transStart = toU64(words.at(2));
      engine.transactionStart(transId, transStart);
    } else if (words.at(0) == "-T") {
      // parse the transaction end command
      u64 transId = toU64(words.at(1));
      u64 transEnd = toU64(words.at(2));
      engine.transactionEnd(transId, transEnd);
    } else if (words.at(0) == "+M") {
      // parse the message start command
      u32 msgId = toU32(words.at(1));
      u32 msgSrc = toU32(words.at(2));
      u32 msgDst = toU32(words.at(3));
      u64 transId = toU64(words.at(4));
      u32 protocolClass = toU32(words.at(5));
      u32 minimalHops = toU32(words.at(6));
      u32 opCode = toU32(words.at(7));
      engine.messageStart(msgId, msgSrc, msgDst, transId, protocolClass,
                          minimalHops, opCode);
    } else if (words.at(0) == "-M") {
      // parse the message end command
      engine.messageEnd();
    } else if (words.at(0) == "+P") {
      // parse the packet start command
      u32 pktId = toU32(words.at(1));
      u32 hopCount = toU32(words.at(2));
      engine.packetStart(pktId, hopCount);
    } else if (words.at(0) == "-P") {
      // parse the packet end command
      engine.packetEnd();
    } else if (words.at(0) == "F") {
      // parse the flit occurrence command
      u32 flitId = toU32(words.at(1));
      u64 flitSend = toU64(words.at(2));
      u64 flitRecv = toU64(words.at(3));
      engine.flit(flitId, flitSend, flitRecv);
    } else {
      throw ex::Exception("Invalid line command. File corrupted :(\n");
    }
  }
  engine.complete();

  return 0;
}
