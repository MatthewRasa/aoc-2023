#include "common.h"
#include <numeric>

struct Box {
	Box() = default;

	explicit Box(std::size_t number) noexcept
			: number_{number} { }

	[[nodiscard]] std::size_t focusing_power() const noexcept {
		std::size_t power{0};
		for (std::size_t slot = 0; slot < lenses_.size(); ++slot)
			power += number_ * (slot + 1) * lenses_[slot].length;
		return power;
	}

	void add_lens(const std::string &label, int length) {
		if (auto lens_it = find_lens(label); lens_it != lenses_.end())
			lens_it->length = length;
		else
			lenses_.push_back(Lens{label, length});
	}

	void remove_lens(const std::string &label) {
		if (auto lens_it = find_lens(label); lens_it != lenses_.end())
			lenses_.erase(lens_it);
	}

private:
	struct Lens {
		std::string label;
		int length;
	};

	std::size_t number_{};
	std::vector<Lens> lenses_;

	decltype(lenses_)::iterator find_lens(const std::string &label) {
		return std::ranges::find_if(lenses_, [&label](const auto &lens) { return lens.label == label; });
	}
};

[[nodiscard]] static std::size_t calculate_hash(const std::string &str) noexcept {
	std::size_t val{0};
	for (auto c : str)
		val = ((val + c) * 17) % 256;
	return val;
}

[[nodiscard]] static std::size_t hash_sum(const std::vector<std::string> &init_steps) noexcept {
	return std::accumulate(init_steps.begin(), init_steps.end(), std::size_t{0},
						   [](auto sum, const auto &step) { return sum + calculate_hash(step); });
}

[[nodiscard]] std::size_t focusing_power(const std::vector<std::string> &init_steps) {
	std::array<Box, 256> boxes;
	for (std::size_t i = 0; i < boxes.size(); ++i)
		boxes[i] = Box{i + 1};

	for (const auto &step : init_steps) {
		auto dash_pos = step.find('-');
		if (dash_pos != std::string::npos) {
			const auto label = step.substr(0, dash_pos);
			boxes[calculate_hash(label)].remove_lens(label);
		} else {
			auto equal_pos = step.find('=');
			const auto label = step.substr(0, equal_pos);
			const auto length = std::stoi(step.substr(equal_pos + 1));
			boxes[calculate_hash(label)].add_lens(label, length);
		}
	}

	return std::accumulate(boxes.begin(), boxes.end(), std::size_t{0},
						   [](auto sum, const auto &box) { return sum + box.focusing_power(); });
}

int main(int argc, char *argv[]) {
	const auto part = select_part(argc, argv);

	const auto init_steps = read_tokens(read_line(std::cin), ',');
	std::cout << (part == 1 ? hash_sum(init_steps) : focusing_power(init_steps)) << std::endl;
	return 0;
}