#include "common.h"
#include <numeric>

enum class Type {
	HIGH_CARD, ONE_PAIR, TWO_PAIR, THREE_OF_A_KIND, FULL_HOUSE, FOUR_OF_A_KIND, FIVE_OF_A_KIND
};

struct Normal_Game {
	[[nodiscard]] static uint card_value(char card) noexcept {
		switch (card) {
			case 'A':
				return 14;
			case 'K':
				return 13;
			case 'Q':
				return 12;
			case 'J':
				return 11;
			case 'T':
				return 10;
			default:
				return card - '0';
		}
	}

	[[nodiscard]] static Type determine_type(const std::string &hand) noexcept {
		std::unordered_map<char, std::size_t> card_counts;
		for (auto card : hand)
			++card_counts[card];

		bool has_three_of_a_kind{false};
		std::size_t pair_count{0};
		for (const auto &[card, count] : card_counts) {
			if (count == 5)
				return Type::FIVE_OF_A_KIND;
			if (count == 4)
				return Type::FOUR_OF_A_KIND;
			if (count == 3)
				has_three_of_a_kind = true;
			else if (count == 2)
				++pair_count;
		}
		if (has_three_of_a_kind)
			return pair_count == 1 ? Type::FULL_HOUSE : Type::THREE_OF_A_KIND;
		if (pair_count == 2)
			return Type::TWO_PAIR;
		if (pair_count == 1)
			return Type::ONE_PAIR;
		return Type::HIGH_CARD;
	}
};

struct Joker_Game {
	[[nodiscard]] static uint card_value(char card) noexcept {
		switch (card) {
			case 'A':
				return 14;
			case 'K':
				return 13;
			case 'Q':
				return 12;
			case 'J':
				return 1;
			case 'T':
				return 10;
			default:
				return card - '0';
		}
	}

	[[nodiscard]] static Type determine_type(const std::string &hand) noexcept {
		std::unordered_map<char, std::size_t> card_counts;
		std::size_t joker_count{0};
		for (auto card : hand) {
			if (card == 'J')
				++joker_count;
			else
				++card_counts[card];
		}
		if (joker_count == 5)
			return Type::FIVE_OF_A_KIND;

		Type high_type{Type::HIGH_CARD};
		for (const auto &[card, count] : card_counts) {
			if (count + joker_count == 5)
				return Type::FIVE_OF_A_KIND;
			if (count + joker_count == 4)
				return Type::FOUR_OF_A_KIND;
			if (count + joker_count == 3)
				high_type = std::max(high_type, has_other_pair(card, card_counts) ? Type::FULL_HOUSE : Type::THREE_OF_A_KIND);
			else if (count + joker_count == 2)
				high_type = std::max(high_type, has_other_pair(card, card_counts) ? Type::TWO_PAIR : Type::ONE_PAIR);
		}
		return high_type;
	}

private:
	[[nodiscard]] static bool has_other_pair(char card, const std::unordered_map<char, std::size_t> &card_counts) noexcept {
		return std::ranges::any_of(card_counts, [card](const auto &pair) { return card != pair.first && pair.second == 2; });
	}

};

struct Hand : Token_Reader<Hand> {

	template<typename GameT>
	[[nodiscard]] bool worse_than(const Hand &other) const noexcept {
		const auto type = GameT::determine_type(hand_);
		const auto other_type = GameT::determine_type(other.hand_);
		if (type < other_type)
			return true;
		if (other_type < type)
			return false;
		for (std::size_t i = 0; i < hand_.size(); ++i) {
			const auto value = GameT::card_value(hand_[i]);
			const auto other_value = GameT::card_value(other.hand_[i]);
			if (value < other_value)
				return true;
			if (other_value < value)
				return false;
		}
		return false;
	}

	[[nodiscard]] uint bid() const noexcept {
		return bid_;
	}

	void read_token(const std::string &token) final {
		switch (token_num()) {
		case 0:
			hand_ = token;
			break;
		case 1:
			bid_ = std::stoi(token);
			break;
		}
	}

private:
	std::string hand_;
	uint bid_;
};

int main(int argc, char *argv[]) {
	const auto part = select_part(argc, argv);

	std::vector<Hand> hands;
	while (has_input(std::cin))
		hands.push_back(Hand::create_from_stream(std::cin));

	if (part == 1)
		std::ranges::sort(hands, [](const auto &lhs, const auto &rhs) { return lhs.template worse_than<Normal_Game>(rhs); });
	else
		std::ranges::sort(hands, [](const auto &lhs, const auto &rhs) { return lhs.template worse_than<Joker_Game>(rhs); });

	uint rank{0};
	std::cout << std::accumulate(hands.begin(), hands.end(), 0ul, [&rank](auto sum, const auto &hand) {
		return sum + (++rank) * hand.bid();
	}) << std::endl;

	return 0;
}