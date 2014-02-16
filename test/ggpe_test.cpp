#include "gtest/gtest.h"
#include "ggpe.hpp"

#include <algorithm>
#include <cassert>
#include <fstream>


namespace ggpe {

namespace {

std::string LoadStringFromFile(const std::string& filename) {
  std::ifstream ifs(filename);
  assert(ifs);
  return std::string((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
}

void SimpleSimulate(const State& state) {
  auto tmp_state = state;
  while (!tmp_state.IsTerminal()) {
    JointAction joint_action(GetRoleCount());
    for (const auto role_idx : GetRoleIndices()) {
      joint_action.at(role_idx) = tmp_state.GetLegalActions().at(role_idx).front();
    }
    tmp_state = tmp_state.GetNextState(joint_action);
  }
}

const auto kif_tictactoe = LoadStringFromFile("test/tictactoe.kif");
const auto kif_breakthrough = LoadStringFromFile("test/breakthrough.kif");

}

TEST(Initialize, TicTacToe) {
  Initialize(kif_tictactoe);
  ASSERT_TRUE(GetRoleCount() == 2);
  ASSERT_TRUE(GetRoleIndices().size() == 2);
  ASSERT_FALSE(IsValidRoleIndex(-1));
  ASSERT_TRUE(IsValidRoleIndex(0));
  ASSERT_TRUE(IsValidRoleIndex(1));
  ASSERT_FALSE(IsValidRoleIndex(2));
  ASSERT_TRUE(StringToRoleIndex("white") == 0);
  ASSERT_TRUE(StringToRoleIndex("black") == 1);

  State state;
  const auto& facts = state.GetFacts();
  ASSERT_TRUE(facts.size() == 10);
  const auto answer_fact_strs = std::vector<std::string>({
    "(cell 1 1 b)",
    "(cell 1 2 b)",
    "(cell 1 3 b)",
    "(cell 2 1 b)",
    "(cell 2 2 b)",
    "(cell 2 3 b)",
    "(cell 3 1 b)",
    "(cell 3 2 b)",
    "(cell 3 3 b)",
    "(control white)"
  });
  for (const auto& answer_fact_str : answer_fact_strs) {
    const auto answer_fact = StringToTuple(answer_fact_str);
    ASSERT_TRUE(std::find(facts.begin(), facts.end(), answer_fact) != facts.end());
  }
  state.GetLegalActions();
  state.Simulate();
  SimpleSimulate(State());
}

TEST(Initialize, Breakthrough) {
  Initialize(kif_breakthrough);
  State state;
  ASSERT_TRUE(GetRoleCount() == 2);
  ASSERT_TRUE(StringToRoleIndex("white") == 0);
  ASSERT_TRUE(StringToRoleIndex("black") == 1);
  const auto& facts = state.GetFacts();
  ASSERT_TRUE(facts.size() == 33);
  state.GetLegalActions();
  state.Simulate();
  SimpleSimulate(State());
}

}
