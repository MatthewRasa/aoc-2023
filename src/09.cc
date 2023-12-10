#include "common.h"
#include <numeric>

struct History : Token_Reader<History> {

	History() {
		levels_.emplace_back();
	}

	[[nodiscard]] int future_prediction() const {
		return std::accumulate(levels_.begin(), levels_.end(), 0,
							   [](auto result, const auto &level) { return result + level.back(); });
	}

	[[nodiscard]] int past_prediction() const {
		return std::accumulate(std::next(levels_.rbegin()), levels_.rend(), 0,
							   [](auto result, const auto &level) { return level.front() - result; });
	}

	void read_token(const std::string &token) final {
		levels_.back().push_back(std::stoi(token));
	}

	void read_end() final {
		while (std::ranges::any_of(levels_.back(), [](auto val) { return val != 0; })) {
			std::vector<int> level;
			level.reserve(levels_.back().size() - 1);
			for (std::size_t i = 0; i < levels_.back().size() - 1; ++i)
				level.push_back(levels_.back()[i + 1] - levels_.back()[i]);
			levels_.push_back(std::move(level));
		}
	}

private:
	std::vector<std::vector<int>> levels_;
};

int main(int argc, char *argv[]) {
	auto histories = read_as<History>(std::cin);
	switch (select_part(argc, argv)) {
	case 1:
		std::cout << std::accumulate(histories.begin(), histories.end(), 0, [](auto sum, const auto &history) {
			return sum + history.future_prediction();
		}) << std::endl;
		break;
	case 2:
		std::cout << std::accumulate(histories.begin(), histories.end(), 0, [](auto sum, const auto &history) {
			return sum + history.past_prediction();
		}) << std::endl;
		break;
	}
	return 0;
}