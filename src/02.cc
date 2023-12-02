#include "common.h"
#include <numeric>

struct Game_Round : Token_Reader<Game_Round, ':'> {
	enum class Color { RED, GREEN, BLUE };

	struct Set : Token_Reader<Set, ','> {
		struct Dice_Count : Token_Reader<Dice_Count> {
			[[nodiscard]] int count() const noexcept {
				return count_;
			}

			[[nodiscard]] Color color() const noexcept {
				return color_;
			}

			void read_token(const std::string &token) final {
				switch (token_num()) {
				case 1:
					count_ = std::stoi(token);
					break;
				case 2:
					if (token == "red")
						color_ = Color::RED;
					else if (token == "green")
						color_ = Color::GREEN;
					else if (token == "blue")
						color_ = Color::BLUE;
					else
						throw std::logic_error{"Parse error: expected a valid color"};
					break;
				}
			}
		private:
			int count_{};
			Color color_{};
		};

		[[nodiscard]] int color_count(Color color) const noexcept {
			const auto it = dice_counts_.find(color);
			return it != dice_counts_.end() ? dice_counts_.at(color).count() : 0;
		}

		void read_token(const std::string &token) final {
			const auto dice_count = Dice_Count::create_from_string(token);
			dice_counts_[dice_count.color()] = dice_count;
		}

	private:
		std::unordered_map<Color, Dice_Count> dice_counts_;
	};

	[[nodiscard]] int id() const noexcept {
		return id_;
	}

	[[nodiscard]] bool is_possible(int red_count, int green_count, int blue_count) const noexcept {
		return std::ranges::all_of(set_list_, [red_count, green_count, blue_count](const auto &set) {
			return set.color_count(Color::RED) <= red_count
				&& set.color_count(Color::GREEN) <= green_count
				&& set.color_count(Color::BLUE) <= blue_count;
		});
	}

	[[nodiscard]] int min_possible(Color color) const noexcept {
		return std::ranges::max_element(set_list_, [color](const auto &lhs, const auto &rhs) { return lhs.color_count(color) < rhs.color_count(color); })->color_count(color);
	}

	[[nodiscard]] int min_possible_power() const noexcept {
		return min_possible(Color::RED) * min_possible(Color::GREEN) * min_possible(Color::BLUE);
	}

	void read_token(const std::string &token) final {
		if (token_num() == 0) {
			auto id_start = token.find(' ') + 1;
			auto id_end = token.find(':', id_start + 1);
			id_ = std::stoi(token.substr(id_start, id_end - id_start));
		} else {
			set_list_ = read_tokens(token, ';', [](const std::string &token) { return Set::create_from_string(token); });
		}
	}

private:
	int id_{};
	std::vector<Set> set_list_;
};

int main(int argc, char *argv[]) {
	const auto part = select_part(argc, argv);

	std::vector<Game_Round> game_rounds;
	while (has_input(std::cin))
		game_rounds.push_back(Game_Round::create_from_stream(std::cin));

	std::cout << std::accumulate(game_rounds.begin(), game_rounds.end(), 0,
		part == 1 ? [](std::size_t sum, const Game_Round &game) {
			return sum + (game.is_possible(12, 13, 14) ? game.id() : 0);
		} : [](std::size_t sum, const Game_Round &game) {
			return sum + game.min_possible_power();
		}
	) << std::endl;
	return 0;
}
