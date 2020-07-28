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

#include <gtest/gtest.h>
#include <prim/prim.h>

TEST(Filter, plusApplication) {
  std::string desc = "+application=0-9,10-18,100-250";
  Filter filter(desc);
  ASSERT_EQ(desc, filter.description());

  ASSERT_TRUE(filter.transaction(18lu << 56, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.transaction(19lu << 56, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(100lu << 56, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.transaction(99lu << 56, 0, 0, 0, 0, 0));

  ASSERT_TRUE(filter.message(0, 0, 18lu << 56, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.message(0, 0, 19lu << 56, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.message(0, 0, 100lu << 56, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.message(0, 0, 99lu << 56, 0, 0, 0, 0, 0, 0, 0));

  ASSERT_TRUE(filter.packet(0, 0, 18lu << 56, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.packet(0, 0, 19lu << 56, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.packet(0, 0, 100lu << 56, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.packet(0, 0, 99lu << 56, 0, 0, 0, 0, 0, 0, 0, 0));
}

TEST(Filter, plusApp) {
  std::string desc = "+app=0-9,10-18,100-250";
  Filter filter(desc);
  ASSERT_EQ(desc, filter.description());

  ASSERT_TRUE(filter.transaction(18lu << 56, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.transaction(19lu << 56, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(100lu << 56, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.transaction(99lu << 56, 0, 0, 0, 0, 0));

  ASSERT_TRUE(filter.message(0, 0, 18lu << 56, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.message(0, 0, 19lu << 56, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.message(0, 0, 100lu << 56, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.message(0, 0, 99lu << 56, 0, 0, 0, 0, 0, 0, 0));

  ASSERT_TRUE(filter.packet(0, 0, 18lu << 56, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.packet(0, 0, 19lu << 56, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.packet(0, 0, 100lu << 56, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.packet(0, 0, 99lu << 56, 0, 0, 0, 0, 0, 0, 0, 0));
}

TEST(Filter, minusApp) {
  std::string desc = "-app=0-9,10-18,100-250";
  Filter filter(desc);
  ASSERT_EQ(desc, filter.description());

  ASSERT_FALSE(filter.transaction(18lu << 56, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(19lu << 56, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.transaction(100lu << 56, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(99lu << 56, 0, 0, 0, 0, 0));

  ASSERT_FALSE(filter.message(0, 0, 18lu << 56, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.message(0, 0, 19lu << 56, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.message(0, 0, 100lu << 56, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.message(0, 0, 99lu << 56, 0, 0, 0, 0, 0, 0, 0));

  ASSERT_FALSE(filter.packet(0, 0, 18lu << 56, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.packet(0, 0, 19lu << 56, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.packet(0, 0, 100lu << 56, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.packet(0, 0, 99lu << 56, 0, 0, 0, 0, 0, 0, 0, 0));
}

TEST(Filter, plusStart) {
  std::string desc = "+start=0-9,10-18,100-250";
  Filter filter(desc);
  ASSERT_EQ(desc, filter.description());

  ASSERT_TRUE(filter.transaction(0, 17.9, 0, 0, 0, 0));
  ASSERT_FALSE(filter.transaction(0, 19.1, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 100.1, 0, 0, 0, 0));
  ASSERT_FALSE(filter.transaction(0, 99.9, 0, 0, 0, 0));

  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 17.9, 0, 0, 0, 0));
  ASSERT_FALSE(filter.message(0, 0, 0, 0, 0, 19.1, 0, 0, 0, 0));
  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 100.1, 0, 0, 0, 0));
  ASSERT_FALSE(filter.message(0, 0, 0, 0, 0, 99.9, 0, 0, 0, 0));

  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 17.9, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.packet(0, 0, 0, 0, 0, 19.1, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 100.1, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.packet(0, 0, 0, 0, 0, 99.9, 0, 0, 0, 0, 0));
}

TEST(Filter, plusSend) {
  std::string desc = "+send=0-9,10-18,100-250";
  Filter filter(desc);
  ASSERT_EQ(desc, filter.description());

  ASSERT_TRUE(filter.transaction(0, 17.9, 0, 0, 0, 0));
  ASSERT_FALSE(filter.transaction(0, 19.1, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 100.1, 0, 0, 0, 0));
  ASSERT_FALSE(filter.transaction(0, 99.9, 0, 0, 0, 0));

  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 17.9, 0, 0, 0, 0));
  ASSERT_FALSE(filter.message(0, 0, 0, 0, 0, 19.1, 0, 0, 0, 0));
  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 100.1, 0, 0, 0, 0));
  ASSERT_FALSE(filter.message(0, 0, 0, 0, 0, 99.9, 0, 0, 0, 0));

  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 17.9, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.packet(0, 0, 0, 0, 0, 19.1, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 100.1, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.packet(0, 0, 0, 0, 0, 99.9, 0, 0, 0, 0, 0));
}

TEST(Filter, minusStart) {
  std::string desc = "-start=0-9,10-18,100-250";
  Filter filter(desc);
  ASSERT_EQ(desc, filter.description());

  ASSERT_FALSE(filter.transaction(0, 17.9, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 19.1, 0, 0, 0, 0));
  ASSERT_FALSE(filter.transaction(0, 100.1, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 99.9, 0, 0, 0, 0));

  ASSERT_FALSE(filter.message(0, 0, 0, 0, 0, 17.9, 0, 0, 0, 0));
  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 19.1, 0, 0, 0, 0));
  ASSERT_FALSE(filter.message(0, 0, 0, 0, 0, 100.1, 0, 0, 0, 0));
  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 99.9, 0, 0, 0, 0));

  ASSERT_FALSE(filter.packet(0, 0, 0, 0, 0, 17.9, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 19.1, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.packet(0, 0, 0, 0, 0, 100.1, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 99.9, 0, 0, 0, 0, 0));
}

TEST(Filter, plusEnd) {
  std::string desc = "+end=0-9,10-18,100-250";
  Filter filter(desc);
  ASSERT_EQ(desc, filter.description());

  ASSERT_TRUE(filter.transaction(0, 0, 17.9, 0, 0, 0));
  ASSERT_FALSE(filter.transaction(0, 0, 19.1, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 100.1, 0, 0, 0));
  ASSERT_FALSE(filter.transaction(0, 0, 99.9, 0, 0, 0));

  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 17.9, 0, 0, 0));
  ASSERT_FALSE(filter.message(0, 0, 0, 0, 0, 0, 19.1, 0, 0, 0));
  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 100.1, 0, 0, 0));
  ASSERT_FALSE(filter.message(0, 0, 0, 0, 0, 0, 99.9, 0, 0, 0));

  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 0, 17.9, 0, 0, 0, 0));
  ASSERT_FALSE(filter.packet(0, 0, 0, 0, 0, 0, 19.1, 0, 0, 0, 0));
  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 0, 100.1, 0, 0, 0, 0));
  ASSERT_FALSE(filter.packet(0, 0, 0, 0, 0, 0, 99.9, 0, 0, 0, 0));
}

TEST(Filter, plusRecv) {
  std::string desc = "+recv=0-9,10-18,100-250";
  Filter filter(desc);
  ASSERT_EQ(desc, filter.description());

  ASSERT_TRUE(filter.transaction(0, 0, 17.9, 0, 0, 0));
  ASSERT_FALSE(filter.transaction(0, 0, 19.1, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 100.1, 0, 0, 0));
  ASSERT_FALSE(filter.transaction(0, 0, 99.9, 0, 0, 0));

  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 17.9, 0, 0, 0));
  ASSERT_FALSE(filter.message(0, 0, 0, 0, 0, 0, 19.1, 0, 0, 0));
  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 100.1, 0, 0, 0));
  ASSERT_FALSE(filter.message(0, 0, 0, 0, 0, 0, 99.9, 0, 0, 0));

  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 0, 17.9, 0, 0, 0, 0));
  ASSERT_FALSE(filter.packet(0, 0, 0, 0, 0, 0, 19.1, 0, 0, 0, 0));
  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 0, 100.1, 0, 0, 0, 0));
  ASSERT_FALSE(filter.packet(0, 0, 0, 0, 0, 0, 99.9, 0, 0, 0, 0));
}

TEST(Filter, minusEnd) {
  std::string desc = "-end=0-9,10-18,100-250";
  Filter filter(desc);
  ASSERT_EQ(desc, filter.description());

  ASSERT_FALSE(filter.transaction(0, 0, 17.9, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 19.1, 0, 0, 0));
  ASSERT_FALSE(filter.transaction(0, 0, 100.1, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 99.9, 0, 0, 0));

  ASSERT_FALSE(filter.message(0, 0, 0, 0, 0, 0, 17.9, 0, 0, 0));
  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 19.1, 0, 0, 0));
  ASSERT_FALSE(filter.message(0, 0, 0, 0, 0, 0, 100.1, 0, 0, 0));
  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 99.9, 0, 0, 0));

  ASSERT_FALSE(filter.packet(0, 0, 0, 0, 0, 0, 17.9, 0, 0, 0, 0));
  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 0, 19.1, 0, 0, 0, 0));
  ASSERT_FALSE(filter.packet(0, 0, 0, 0, 0, 0, 100.1, 0, 0, 0, 0));
  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 0, 99.9, 0, 0, 0, 0));
}

TEST(Filter, plusProtocolClass) {
  std::string desc = "+protocolclass=0-9,10-18,100-250";
  Filter filter(desc);
  ASSERT_EQ(desc, filter.description());

  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));

  ASSERT_TRUE(filter.message(0, 0, 0, 18lu, 0, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.message(0, 0, 0, 19lu, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.message(0, 0, 0, 100lu, 0, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.message(0, 0, 0, 99lu, 0, 0, 0, 0, 0, 0));

  ASSERT_TRUE(filter.packet(0, 0, 0, 18lu, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.packet(0, 0, 0, 19lu, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.packet(0, 0, 0, 100lu, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.packet(0, 0, 0, 99lu, 0, 0, 0, 0, 0, 0, 0));
}

TEST(Filter, plusPc) {
  std::string desc = "+pc=0-9,10-18,100-250";
  Filter filter(desc);
  ASSERT_EQ(desc, filter.description());

  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));

  ASSERT_TRUE(filter.message(0, 0, 0, 18lu, 0, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.message(0, 0, 0, 19lu, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.message(0, 0, 0, 100lu, 0, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.message(0, 0, 0, 99lu, 0, 0, 0, 0, 0, 0));

  ASSERT_TRUE(filter.packet(0, 0, 0, 18lu, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.packet(0, 0, 0, 19lu, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.packet(0, 0, 0, 100lu, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.packet(0, 0, 0, 99lu, 0, 0, 0, 0, 0, 0, 0));
}

TEST(Filter, minusProtocolClass) {
  std::string desc = "-protocolclass=0-9,10-18,100-250";
  Filter filter(desc);
  ASSERT_EQ(desc, filter.description());

  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));

  ASSERT_FALSE(filter.message(0, 0, 0, 18lu, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.message(0, 0, 0, 19lu, 0, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.message(0, 0, 0, 100lu, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.message(0, 0, 0, 99lu, 0, 0, 0, 0, 0, 0));

  ASSERT_FALSE(filter.packet(0, 0, 0, 18lu, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.packet(0, 0, 0, 19lu, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.packet(0, 0, 0, 100lu, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.packet(0, 0, 0, 99lu, 0, 0, 0, 0, 0, 0, 0));
}

TEST(Filter, plusOpcode) {
  std::string desc = "+opcode=0-9,10-18,100-250";
  Filter filter(desc);
  ASSERT_EQ(desc, filter.description());

  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));

  ASSERT_TRUE(filter.message(0, 0, 0, 0, 18lu, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.message(0, 0, 0, 0, 19lu, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.message(0, 0, 0, 0, 100lu, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.message(0, 0, 0, 0, 99lu, 0, 0, 0, 0, 0));

  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 18lu, 0, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.packet(0, 0, 0, 0, 19lu, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 100lu, 0, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.packet(0, 0, 0, 0, 99lu, 0, 0, 0, 0, 0, 0));
}

TEST(Filter, plusOp) {
  std::string desc = "+op=0-9,10-18,100-250";
  Filter filter(desc);
  ASSERT_EQ(desc, filter.description());

  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));

  ASSERT_TRUE(filter.message(0, 0, 0, 0, 18lu, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.message(0, 0, 0, 0, 19lu, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.message(0, 0, 0, 0, 100lu, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.message(0, 0, 0, 0, 99lu, 0, 0, 0, 0, 0));

  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 18lu, 0, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.packet(0, 0, 0, 0, 19lu, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 100lu, 0, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.packet(0, 0, 0, 0, 99lu, 0, 0, 0, 0, 0, 0));
}

TEST(Filter, minusOpcode) {
  std::string desc = "-opcode=0-9,10-18,100-250";
  Filter filter(desc);
  ASSERT_EQ(desc, filter.description());

  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));

  ASSERT_FALSE(filter.message(0, 0, 0, 0, 18lu, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.message(0, 0, 0, 0, 19lu, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.message(0, 0, 0, 0, 100lu, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.message(0, 0, 0, 0, 99lu, 0, 0, 0, 0, 0));

  ASSERT_FALSE(filter.packet(0, 0, 0, 0, 18lu, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 19lu, 0, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.packet(0, 0, 0, 0, 100lu, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 99lu, 0, 0, 0, 0, 0, 0));
}

TEST(Filter, plusSource) {
  std::string desc = "+source=0-9,10-18,100-250";
  Filter filter(desc);
  ASSERT_EQ(desc, filter.description());

  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));

  ASSERT_TRUE(filter.message(18lu, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.message(19lu, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.message(100lu, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.message(99lu, 0, 0, 0, 0, 0, 0, 0, 0, 0));

  ASSERT_TRUE(filter.packet(18lu, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.packet(19lu, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.packet(100lu, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.packet(99lu, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
}

TEST(Filter, plusSrc) {
  std::string desc = "+src=0-9,10-18,100-250";
  Filter filter(desc);
  ASSERT_EQ(desc, filter.description());

  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));

  ASSERT_TRUE(filter.message(18lu, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.message(19lu, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.message(100lu, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.message(99lu, 0, 0, 0, 0, 0, 0, 0, 0, 0));

  ASSERT_TRUE(filter.packet(18lu, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.packet(19lu, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.packet(100lu, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.packet(99lu, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
}

TEST(Filter, minusSource) {
  std::string desc = "-source=0-9,10-18,100-250";
  Filter filter(desc);
  ASSERT_EQ(desc, filter.description());

  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));

  ASSERT_FALSE(filter.message(18lu, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.message(19lu, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.message(100lu, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.message(99lu, 0, 0, 0, 0, 0, 0, 0, 0, 0));

  ASSERT_FALSE(filter.packet(18lu, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.packet(19lu, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.packet(100lu, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.packet(99lu, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
}

TEST(Filter, plusDestination) {
  std::string desc = "+destination=0-9,10-18,100-250";
  Filter filter(desc);
  ASSERT_EQ(desc, filter.description());

  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));

  ASSERT_TRUE(filter.message(0, 18lu, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.message(0, 19lu, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.message(0, 100lu, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.message(0, 99lu, 0, 0, 0, 0, 0, 0, 0, 0));

  ASSERT_TRUE(filter.packet(0, 18lu, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.packet(0, 19lu, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.packet(0, 100lu, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.packet(0, 99lu, 0, 0, 0, 0, 0, 0, 0, 0, 0));
}

TEST(Filter, plusDst) {
  std::string desc = "+dst=0-9,10-18,100-250";
  Filter filter(desc);
  ASSERT_EQ(desc, filter.description());

  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));

  ASSERT_TRUE(filter.message(0, 18lu, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.message(0, 19lu, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.message(0, 100lu, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.message(0, 99lu, 0, 0, 0, 0, 0, 0, 0, 0));

  ASSERT_TRUE(filter.packet(0, 18lu, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.packet(0, 19lu, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.packet(0, 100lu, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.packet(0, 99lu, 0, 0, 0, 0, 0, 0, 0, 0, 0));
}

TEST(Filter, minusDestination) {
  std::string desc = "-destination=0-9,10-18,100-250";
  Filter filter(desc);
  ASSERT_EQ(desc, filter.description());

  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));

  ASSERT_FALSE(filter.message(0, 18lu, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.message(0, 19lu, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.message(0, 100lu, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.message(0, 99lu, 0, 0, 0, 0, 0, 0, 0, 0));

  ASSERT_FALSE(filter.packet(0, 18lu, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.packet(0, 19lu, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_FALSE(filter.packet(0, 100lu, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.packet(0, 99lu, 0, 0, 0, 0, 0, 0, 0, 0, 0));
}

TEST(Filter, plusHopCount) {
  std::string desc = "+hopcount=0-9,10-18,100-250";
  Filter filter(desc);
  ASSERT_EQ(desc, filter.description());

  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));

  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 0, 0));

  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 18lu, 0, 0));
  ASSERT_FALSE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 19lu, 0, 0));
  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 100lu, 0, 0));
  ASSERT_FALSE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 99lu, 0, 0));
}

TEST(Filter, plusHc) {
  std::string desc = "+hc=0-9,10-18,100-250";
  Filter filter(desc);
  ASSERT_EQ(desc, filter.description());

  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));

  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 0, 0));

  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 18lu, 0, 0));
  ASSERT_FALSE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 19lu, 0, 0));
  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 100lu, 0, 0));
  ASSERT_FALSE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 99lu, 0, 0));
}

TEST(Filter, minusHopCount) {
  std::string desc = "-hopcount=0-9,10-18,100-250";
  Filter filter(desc);
  ASSERT_EQ(desc, filter.description());

  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));

  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 0, 0));

  ASSERT_FALSE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 18lu, 0, 0));
  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 19lu, 0, 0));
  ASSERT_FALSE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 100lu, 0, 0));
  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 99lu, 0, 0));
}

TEST(Filter, plusMinHopCount) {
  std::string desc = "+minhopcount=0-9,10-18,100-250";
  Filter filter(desc);
  ASSERT_EQ(desc, filter.description());

  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));

  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 0, 18lu));
  ASSERT_FALSE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 0, 19lu));
  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 0, 100lu));
  ASSERT_FALSE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 0, 99lu));

  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 0, 18lu, 0));
  ASSERT_FALSE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 0, 19lu, 0));
  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 0, 100lu, 0));
  ASSERT_FALSE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 0, 99lu, 0));
}

TEST(Filter, plusMhc) {
  std::string desc = "+mhc=0-9,10-18,100-250";
  Filter filter(desc);
  ASSERT_EQ(desc, filter.description());

  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));

  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 0, 18lu));
  ASSERT_FALSE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 0, 19lu));
  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 0, 100lu));
  ASSERT_FALSE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 0, 99lu));

  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 0, 18lu, 0));
  ASSERT_FALSE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 0, 19lu, 0));
  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 0, 100lu, 0));
  ASSERT_FALSE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 0, 99lu, 0));
}

TEST(Filter, minusMinHopCount) {
  std::string desc = "-minhopcount=0-9,10-18,100-250";
  Filter filter(desc);
  ASSERT_EQ(desc, filter.description());

  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));

  ASSERT_FALSE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 0, 18lu));
  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 0, 19lu));
  ASSERT_FALSE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 0, 100lu));
  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 0, 99lu));

  ASSERT_FALSE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 0, 18lu, 0));
  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 0, 19lu, 0));
  ASSERT_FALSE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 0, 100lu, 0));
  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 0, 99lu, 0));
}

TEST(Filter, plusNonMinHopCount) {
  std::string desc = "+nonminhopcount=0-9,10-18,100-250";
  Filter filter(desc);
  ASSERT_EQ(desc, filter.description());

  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));

  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 0, 0));

  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 18lu));
  ASSERT_FALSE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19lu));
  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100lu));
  ASSERT_FALSE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 99lu));
}

TEST(Filter, plusNmhc) {
  std::string desc = "+nmhc=0-9,10-18,100-250";
  Filter filter(desc);
  ASSERT_EQ(desc, filter.description());

  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));

  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 0, 0));

  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 18lu));
  ASSERT_FALSE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19lu));
  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100lu));
  ASSERT_FALSE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 99lu));
}

TEST(Filter, minusNonMinHopCount) {
  std::string desc = "-nonminhopcount=0-9,10-18,100-250";
  Filter filter(desc);
  ASSERT_EQ(desc, filter.description());

  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 0));

  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 0, 18lu));
  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 0, 19lu));
  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 0, 100lu));
  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 0, 99lu));

  ASSERT_FALSE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 18lu));
  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19lu));
  ASSERT_FALSE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100lu));
  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 99lu));
}

TEST(Filter, plusMessageCount) {
  std::string desc = "+messagecount=0-9,10-18,100-250";
  Filter filter(desc);
  ASSERT_EQ(desc, filter.description());

  ASSERT_TRUE(filter.transaction(0, 0, 0, 18lu, 0, 0));
  ASSERT_FALSE(filter.transaction(0, 0, 0, 19lu, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 100lu, 0, 0));
  ASSERT_FALSE(filter.transaction(0, 0, 0, 99lu, 0, 0));

  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 0, 0));

  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
}

TEST(Filter, plusMsgCnt) {
  std::string desc = "+msgcnt=0-9,10-18,100-250";
  Filter filter(desc);
  ASSERT_EQ(desc, filter.description());

  ASSERT_TRUE(filter.transaction(0, 0, 0, 18lu, 0, 0));
  ASSERT_FALSE(filter.transaction(0, 0, 0, 19lu, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 100lu, 0, 0));
  ASSERT_FALSE(filter.transaction(0, 0, 0, 99lu, 0, 0));

  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 0, 0));

  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
}

TEST(Filter, minusMessageCount) {
  std::string desc = "-messagecount=0-9,10-18,100-250";
  Filter filter(desc);
  ASSERT_EQ(desc, filter.description());

  ASSERT_FALSE(filter.transaction(0, 0, 0, 18lu, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 19lu, 0, 0));
  ASSERT_FALSE(filter.transaction(0, 0, 0, 100lu, 0, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 99lu, 0, 0));

  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 0, 0));

  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
}

TEST(Filter, plusPacketCount) {
  std::string desc = "+packetcount=0-9,10-18,100-250";
  Filter filter(desc);
  ASSERT_EQ(desc, filter.description());

  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 18lu, 0));
  ASSERT_FALSE(filter.transaction(0, 0, 0, 0, 19lu, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 100lu, 0));
  ASSERT_FALSE(filter.transaction(0, 0, 0, 0, 99lu, 0));

  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 0, 18lu, 0, 0));
  ASSERT_FALSE(filter.message(0, 0, 0, 0, 0, 0, 0, 19lu, 0, 0));
  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 0, 100lu, 0, 0));
  ASSERT_FALSE(filter.message(0, 0, 0, 0, 0, 0, 0, 99lu, 0, 0));

  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
}

TEST(Filter, plusPktCnt) {
  std::string desc = "+pktcnt=0-9,10-18,100-250";
  Filter filter(desc);
  ASSERT_EQ(desc, filter.description());

  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 18lu, 0));
  ASSERT_FALSE(filter.transaction(0, 0, 0, 0, 19lu, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 100lu, 0));
  ASSERT_FALSE(filter.transaction(0, 0, 0, 0, 99lu, 0));

  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 0, 18lu, 0, 0));
  ASSERT_FALSE(filter.message(0, 0, 0, 0, 0, 0, 0, 19lu, 0, 0));
  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 0, 100lu, 0, 0));
  ASSERT_FALSE(filter.message(0, 0, 0, 0, 0, 0, 0, 99lu, 0, 0));

  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
}

TEST(Filter, minusPacketCount) {
  std::string desc = "-packetcount=0-9,10-18,100-250";
  Filter filter(desc);
  ASSERT_EQ(desc, filter.description());

  ASSERT_FALSE(filter.transaction(0, 0, 0, 0, 18lu, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 19lu, 0));
  ASSERT_FALSE(filter.transaction(0, 0, 0, 0, 100lu, 0));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 99lu, 0));

  ASSERT_FALSE(filter.message(0, 0, 0, 0, 0, 0, 0, 18lu, 0, 0));
  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 0, 19lu, 0, 0));
  ASSERT_FALSE(filter.message(0, 0, 0, 0, 0, 0, 0, 100lu, 0, 0));
  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 0, 99lu, 0, 0));

  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
}

TEST(Filter, plusFlitCount) {
  std::string desc = "+flitcount=0-9,10-18,100-250";
  Filter filter(desc);
  ASSERT_EQ(desc, filter.description());

  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 18lu));
  ASSERT_FALSE(filter.transaction(0, 0, 0, 0, 0, 19lu));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 100lu));
  ASSERT_FALSE(filter.transaction(0, 0, 0, 0, 0, 99lu));

  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 18lu, 0));
  ASSERT_FALSE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 19lu, 0));
  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 100lu, 0));
  ASSERT_FALSE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 99lu, 0));

  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 0, 0, 18lu, 0, 0, 0));
  ASSERT_FALSE(filter.packet(0, 0, 0, 0, 0, 0, 0, 19lu, 0, 0, 0));
  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 0, 0,  100lu, 0, 0, 0));
  ASSERT_FALSE(filter.packet(0, 0, 0, 0, 0, 0, 0, 99lu, 0, 0, 0));
}

TEST(Filter, plusFlitCnt) {
  std::string desc = "+flitcnt=0-9,10-18,100-250";
  Filter filter(desc);
  ASSERT_EQ(desc, filter.description());

  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 18lu));
  ASSERT_FALSE(filter.transaction(0, 0, 0, 0, 0, 19lu));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 100lu));
  ASSERT_FALSE(filter.transaction(0, 0, 0, 0, 0, 99lu));

  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 18lu, 0));
  ASSERT_FALSE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 19lu, 0));
  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 100lu, 0));
  ASSERT_FALSE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 99lu, 0));

  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 0, 0, 18lu, 0, 0, 0));
  ASSERT_FALSE(filter.packet(0, 0, 0, 0, 0, 0, 0, 19lu, 0, 0, 0));
  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 0, 0,  100lu, 0, 0, 0));
  ASSERT_FALSE(filter.packet(0, 0, 0, 0, 0, 0, 0, 99lu, 0, 0, 0));
}

TEST(Filter, minusFlitCount) {
  std::string desc = "-flitcount=0-9,10-18,100-250";
  Filter filter(desc);
  ASSERT_EQ(desc, filter.description());

  ASSERT_FALSE(filter.transaction(0, 0, 0, 0, 0, 18lu));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 19lu));
  ASSERT_FALSE(filter.transaction(0, 0, 0, 0, 0, 100lu));
  ASSERT_TRUE(filter.transaction(0, 0, 0, 0, 0, 99lu));

  ASSERT_FALSE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 18lu, 0));
  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 19lu, 0));
  ASSERT_FALSE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 100lu, 0));
  ASSERT_TRUE(filter.message(0, 0, 0, 0, 0, 0, 0, 0, 99lu, 0));

  ASSERT_FALSE(filter.packet(0, 0, 0, 0, 0, 0, 0, 18lu, 0, 0, 0));
  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 0, 0, 19lu, 0, 0, 0));
  ASSERT_FALSE(filter.packet(0, 0, 0, 0, 0, 0, 0,  100lu, 0, 0, 0));
  ASSERT_TRUE(filter.packet(0, 0, 0, 0, 0, 0, 0, 99lu, 0, 0, 0));
}
