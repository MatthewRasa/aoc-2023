#include "common.h"
#include <numeric>

struct Node : Token_Reader<Node> {

	[[nodiscard]] const std::string &label() const noexcept {
		return label_;
	}

	[[nodiscard]] const Node &left() const noexcept {
		return *left_;
	}

	[[nodiscard]] const Node &right() const noexcept {
		return *right_;
	}

	void assign_children(const std::unordered_map<std::string, Node> &nodes) {
		left_ = &nodes.at(left_label_);
		right_ = &nodes.at(right_label_);
	}

	void read_token(const std::string &token) final {
		switch (token_num()) {
		case 0:
			label_ = token;
			break;
		case 2:
			left_label_ = token.substr(1, 3);
			break;
		case 3:
			right_label_ = token.substr(0, 3);
			break;
		}
	}
private:
	std::string label_, left_label_, right_label_;
	const Node *left_, *right_;
};

[[nodiscard]] std::size_t steps_required(Circular_Queue<char, std::string> instructions,
										 const std::unordered_map<std::string, Node> &nodes) {
	std::size_t steps{0};
	for (auto *current_ptr = &nodes.at("AAA"); current_ptr->label() != "ZZZ"; ++steps)
		current_ptr = &(instructions.take() == 'L' ? current_ptr->left() : current_ptr->right());
	return steps;
}

[[nodiscard]] std::vector<const Node *> starting_nodes(const std::unordered_map<std::string, Node> &nodes) {
	std::vector<const Node *> result;
	for (const auto &[label, node] : nodes) {
		if (label.back() == 'A')
			result.push_back(&node);
	}
	return result;
}

[[nodiscard]] std::size_t steps_until_end(Circular_Queue<char, std::string> instructions, const Node &node) {
	std::size_t steps{0};
	for (auto *current_ptr = &node; current_ptr->label().back() != 'Z'; ++steps)
		current_ptr = &(instructions.take() == 'L' ? current_ptr->left() : current_ptr->right());
	return steps;
}

[[nodiscard]] std::size_t simultaneous_steps_required(Circular_Queue<char, std::string> instructions,
													  const std::unordered_map<std::string, Node> &nodes) {
	auto current_nodes = starting_nodes(nodes);
	return std::accumulate(current_nodes.begin(), current_nodes.end(), 1ul, [&instructions](auto result, auto node_ptr) {
		return std::lcm(result, steps_until_end(instructions, *node_ptr));
	});
}

int main(int argc, char *argv[]) {
	const auto part = select_part(argc, argv);
	auto instructions = Circular_Queue<char, std::string>(read_line(std::cin));
	read_line(std::cin);

	std::unordered_map<std::string, Node> nodes;
	while (has_input(std::cin)) {
		auto node = Node::create_from_stream(std::cin);
		const auto label = node.label();
		nodes.emplace(label, std::move(node));
	}
	for (auto &[label, node] : nodes)
		node.assign_children(nodes);

	std::cout << (part == 1 ? steps_required(instructions, nodes) : simultaneous_steps_required(instructions, nodes)) << std::endl;
	return 0;
}