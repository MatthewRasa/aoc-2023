#include "common.h"
#include <algorithm>
#include <numeric>

[[nodiscard]] static uint parse_calibration_simple(const std::string &line) {
	return std::stoi(std::string{} + *std::ranges::find_if(line, Is_Digit{}) + *std::ranges::find_if(line.rbegin(), line.rend(), Is_Digit{}));
}

[[nodiscard]] static uint parse_calibration_advanced(const std::string &line) {
	static constexpr std::array<std::string, 9> spelled_digits{"one", "two", "three", "four", "five", "six", "seven", "eight", "nine"};

	auto first_it = std::ranges::find_if(line, Is_Digit{});
	auto second_it = std::ranges::find_if(line.rbegin(), line.rend(), Is_Digit{});
	auto first_value = *first_it;
	auto second_value = *second_it;
	for (auto spelled_digit_it = spelled_digits.begin(); spelled_digit_it != spelled_digits.end(); ++spelled_digit_it) {
		if (auto it = std::ranges::search(line, *spelled_digit_it).begin(); it < first_it) {
			first_it = it;
			first_value = '0' + std::distance(spelled_digits.begin(), spelled_digit_it) + 1;
		}
		if (auto it = std::ranges::search(line.rbegin(), line.rend(), spelled_digit_it->rbegin(), spelled_digit_it->rend()).begin(); it < second_it) {
			second_it = it;
			second_value = '0' + std::distance(spelled_digits.begin(), spelled_digit_it) + 1;
		}
	}
	return std::stoi(std::string{} + first_value + second_value);
}

template<typename ParserT>
[[nodiscard]] static std::vector<uint> read_calibration_values(std::istream &in, const ParserT &parser) {
	const auto lines = read_lines(in);
	std::vector<uint> values(lines.size());
	std::ranges::transform(lines, values.begin(), parser);
	return values;
}

int main(int argc, char *argv[]) {
	const auto part = select_part(argc, argv);
	const auto values = read_calibration_values(std::cin, part == 1 ? parse_calibration_simple : parse_calibration_advanced);
	std::cout << std::reduce(values.begin(), values.end()) << std::endl;
	return 0;
}
