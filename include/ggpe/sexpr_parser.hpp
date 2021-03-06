#ifndef SEXPR_PARSER_HPP_
#define SEXPR_PARSER_HPP_

#include <string>
#include <unordered_set>
#include <unordered_map>
#include <vector>

namespace sexpr_parser {

/**
 * (relation name, arg index)
 */
using ArgPos = std::pair<std::string, int>;

using ArgPosPair = std::pair<ArgPos, ArgPos>;

/**
 * A node in KIF trees
 */
class TreeNode {
public:
  /**
   * Leaf node
   * @param value
   */
  TreeNode(const std::string& value);

  /**
   * Non-leaf node
   * @param children
   */
  TreeNode(const std::vector<TreeNode>& children);

  /**
   * @return whether this node is a leaf
   */
  bool IsLeaf() const;

  /**
   * @return whether this leaf node represents a variable
   */
  bool IsVariable() const;

  /**
   * @return the value of this leaf node
   */
  const std::string& GetValue() const;

  /**
   * @return children of this non-leaf node
   */
  const std::vector<TreeNode>& GetChildren() const;

  /**
   * @return a string that represents the structure of this node
   */
  std::string ToString() const;

  /**
   * @return a string in S-expression
   */
  std::string ToSexpr() const;

  /**
   * @return S-expressions of children, separated by space
   */
  std::string ChildrenToSexpr() const;

  /**
   * Convert this to a Prolog atom string. This node must be a leaf.
   * @param quotes_atoms if true, add single quotations around the atom
   * @param atom_prefix this string is added as to the head of the atom
   * @return a string as a Prolog atom
   */
  std::string ToPrologAtom(
      const bool quotes_atoms,
      const std::string& atom_prefix) const;

  /**
   * Convert this to a Prolog functor atom string. This node must be a
   * non-variable leaf.
   * @param quotes_atoms
   * @param functor_prefix
   * @return
   */
  std::string ToPrologFunctor(
      const bool quotes_atoms,
      const std::string& functor_prefix) const;

  /**
   * Convert this to a Prolog clause.
   * @param quotes_atoms
   * @param functor_prefix
   * @param atom_prefix
   * @return
   */
  std::string ToPrologClause(
      const bool quotes_atoms,
      const std::string& functor_prefix,
      const std::string& atom_prefix) const;

  /**
   * Convert this to a Prolog term.
   * @param quotes_atoms
   * @param functor_prefix
   * @param atom_prefix
   * @return
   */
  std::string ToPrologTerm(
      const bool quotes_atoms,
      const std::string& functor_prefix,
      const std::string& atom_prefix) const;

  /**
   * Collect all atoms from this node and its children.
   * @return
   */
  std::unordered_set<std::string> CollectAtoms() const;

  /**
   * Collect non-functor atoms from this node and its children.
   * @return
   */
  std::unordered_set<std::string> CollectNonFunctorAtoms() const;

  /**
   * Collect negated relations (relations that occurs inside negation).
   * @return
   */
   void CollectNegatedFunctors(std::unordered_map<std::string, int>& output) const;

//   void CollectVariables(std::unordered_set<std::string>& output) const;
   void CollectVariables(
       const std::unordered_set<std::string>& ignored,
       std::unordered_set<std::string>& output) const;
  void CollectBoundAndUnboundVariables(
      std::unordered_set<std::string>& bound_variables,
      std::unordered_set<std::string>& unbound_variables) const;
//    void CollectUnboundVariables(
//        const std::unordered_set<std::string>& bound_vairables,
//        std::unordered_set<std::string>& output) const;

   bool ChangeBodyOrderSoThatAllVariablesAreBound();
   bool IsImplication() const;

  /**
   * Collect functor atoms and their argument numbers from this node and its
   * children.
   * @return
   */
  std::unordered_map<std::string, int> CollectFunctorAtoms() const;
  void CollectFunctorAtoms(std::unordered_map<std::string, int>& output) const;

  std::unordered_map<std::string, std::unordered_set<ArgPos>> CollectVariableArgs() const;
  std::unordered_set<ArgPosPair> CollectSameDomainArgsInBody() const;
  std::unordered_set<ArgPosPair> CollectSameDomainArgsBetweenHeadAndBody() const;
  TreeNode ReplaceAtoms(const std::string& before, const std::string& after) const;
  bool CollectDynamicRelations(std::unordered_set<std::string>& local_relation_functors) const;
  const std::string& GetFunctor() const;
  std::string ToPrologRequirementTerm(
      const bool quotes_atoms,
      const std::string& functor_prefix,
      const std::string& atom_prefix,
      const std::unordered_set<std::string>& dynamic_relations,
      const bool negated) const;
  std::string ToPrologRequirementClause(
      const bool quotes_atoms,
      const std::string& functor_prefix,
      const std::string& atom_prefix,
      const std::unordered_set<std::string>& dynamic_relations) const;
  bool ContainsAnyAtomOf(const std::unordered_set<std::string>& atoms) const;

  /**
   * Check if this node contains given functors.
   * @param functors
   * @return
   */
  bool ContainsFunctors(const std::unordered_set<std::string>& functors) const;
  bool operator==(const TreeNode& another) const;
private:
  bool is_leaf_;
  std::string value_;
  std::vector<TreeNode> children_;
};

std::string RemoveComments(const std::string& sexpr);
std::vector<TreeNode> Parse(const std::string& sexpr, const bool flatten_tuple_with_one_child = false);
std::vector<TreeNode> ParseKIF(const std::string& kif);
std::string ToProlog(
    const std::vector<TreeNode>& nodes,
    const bool quotes_atoms,
    const std::string& functor_prefix = "",
    const std::string& atom_prefix = "",
    const bool adds_helper_clauses = false,
    const bool enables_tabling = false);
std::unordered_set<std::string> CollectAtoms(const std::vector<TreeNode>& nodes);
std::unordered_set<std::string> CollectNonFunctorAtoms(const std::vector<TreeNode>& nodes);
std::unordered_map<std::string, int> CollectFunctorAtoms(const std::vector<TreeNode>& nodes);
std::vector<TreeNode> ReplaceAtoms(const std::vector<TreeNode>& nodes, const std::string& before, const std::string& after);

/**
 * Collect all dynamic relations, including reserved ones.
 * Dynamic relations depend on 'true' and/or 'does' directly or indirectly.
 * @param nodes
 * @return
 */
std::unordered_set<std::string> CollectDynamicRelations(
    const std::vector<TreeNode>& nodes);

/**
 * Collect all static relations, not including reserved ones.
 * Static relations do not depend on 'true' nor 'does', thus their evaluation
 * results can be safely cached.
 * @param nodes
 * @return
 */
std::unordered_map<std::string, int> CollectStaticRelations(
    const std::vector<TreeNode>& nodes);

}

#endif /* SEXPR_PARSER_HPP_ */
