#include "common.h"

[[nodiscard]] static std::vector<uint> read_separate_values(std::istream &in) {
	const auto strs = read_tokens(read_line(in), ' ');
	std::vector<uint> values;
	for (auto str_it = std::next(strs.begin()); str_it != strs.end(); ++str_it) {
		if (!str_it->empty())
			values.push_back(std::stoi(*str_it));
	}
	return values;
}

[[nodiscard]] static ulong read_single_value(std::istream &in) {
	const auto strs = read_tokens(read_line(in), ' ');
	std::stringstream ss;
	std::copy(std::next(strs.begin()), strs.end(), std::ostream_iterator<std::string>(ss));
	return std::stoul(ss.str());
}

[[nodiscard]] static std::size_t count_ways_to_win(ulong time, ulong best_distance) {
	std::size_t count{0};
	for (ulong t = 1; t < time; ++t) {
		if (t * (time - t) > best_distance)
			++count;
		else if (count != 0)
			break;
	}
	return count;
}

int main(int argc, char *argv[]) {
	if (select_part(argc, argv) == 1) {
		const auto times = read_separate_values(std::cin);
		const auto distances = read_separate_values(std::cin);
		uint result{1};
		for (std::size_t i = 0; i < times.size(); ++i)
			result *= count_ways_to_win(times[i], distances[i]);
		std::cout << result << std::endl;
	} else {
		const auto time = read_single_value(std::cin);
		const auto distance = read_single_value(std::cin);
		std::cout << count_ways_to_win(time, distance) << std::endl;
	}
	return 0;
}