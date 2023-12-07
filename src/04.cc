#include "common.h"
#include <algorithm>
#include <numeric>
#include <unordered_set>

struct Card : Token_Reader<Card, ':'> {

	struct Number_Set : Token_Reader<Number_Set> {
		[[nodiscard]] bool contains(int number) const noexcept {
			return numbers_.contains(number);
		}

		[[nodiscard]] const std::unordered_set<int> &set() const noexcept {
			return numbers_;
		}

		void read_token(const std::string &token) final {
			if (!token.empty())
				numbers_.insert(std::stoi(token));
		}
	private:
		std::unordered_set<int> numbers_;
	};

	[[nodiscard]] std::size_t num_matches() const {
		return std::ranges::count_if(your_numbers_.set(),
									 [this](auto number) { return winning_numbers_.contains(number); });
	}

	[[nodiscard]] std::size_t points() const {
		const auto matches = num_matches();
		return matches == 0 ? matches : 1 << (matches - 1);
	}

	void read_token(const std::string &token) final {
		switch (token_num()) {
		case 1:
			auto number_sets = read_tokens(token, '|', [](const auto &token) { return Number_Set::create_from_string(token); });
			winning_numbers_ = std::move(number_sets[0]);
			your_numbers_ = std::move(number_sets[1]);
			break;
		}
	}

private:
	Number_Set winning_numbers_, your_numbers_;
};

[[nodiscard]] static std::size_t total_points(const std::vector<Card> &cards) {
	return std::accumulate(cards.begin(), cards.end(), 0ul,
						   [](auto sum, const auto &card) { return sum + card.points(); });
}

[[nodiscard]] static std::size_t total_cards(const std::vector<Card> &cards) {
	std::vector<std::size_t> counts(cards.size(), 1);
	for (std::size_t i = 0; i < cards.size(); ++i) {
		for (std::size_t j = i + 1; j < i + 1 + cards[i].num_matches(); ++j)
			counts[j] += counts[i];
	}
	return std::accumulate(counts.begin(), counts.end(), 0ul);
}

int main(int argc, char *argv[]) {
	const auto part = select_part(argc, argv);
	std::vector<Card> cards;
	while (has_input(std::cin))
		cards.push_back(Card::create_from_stream(std::cin));
	std::cout << (part == 1 ? total_points(cards) : total_cards(cards)) << std::endl;
	return 0;
}
