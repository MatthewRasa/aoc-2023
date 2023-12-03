#include "common.h"
#include <cstdint>
#include <numeric>

struct Engine_Schematic : Paragraph_Reader<Engine_Schematic> {

	[[nodiscard]] std::vector<int> part_numbers() const {
		std::vector<int> numbers{};
		for (const auto &[number, position] : number_positions_) {
			if (touches_symbols(number, position))
				numbers.push_back(number);
		}
		return numbers;
	}

	[[nodiscard]] std::vector<int> gear_ratios() const {
		std::unordered_map<Grid_Position, std::vector<int>> star_adjacent_parts;
		for (const auto &[number, position] : number_positions_) {
			for (const auto &star_position : adjacent_symbols(number, position, '*'))
				star_adjacent_parts[star_position].push_back(number);
		}

		std::vector<int> ratios;
		for (const auto &[position, parts] : star_adjacent_parts) {
			if (parts.size() == 2)
				ratios.push_back(parts[0] * parts[1]);
		}
		return ratios;
	}

	void read_line(const std::string &line) final {
		std::string current_nunber;
		std::vector<char> symbol_row(line.size(), ' ');
		for (std::size_t column_idx = 0; column_idx < line.size(); ++column_idx) {
			if (is_digit(line[column_idx])) {
				current_nunber += line[column_idx];
			} else {
				if (!current_nunber.empty()) {
					number_positions_.emplace_back(std::stoi(current_nunber), Grid_Position{line_num(), column_idx - current_nunber.size()});
					current_nunber.clear();
				}
				if (is_symbol(line[column_idx]))
					symbol_row[column_idx] = line[column_idx];
			}
		}
		if (!current_nunber.empty())
			number_positions_.emplace_back(std::stoi(current_nunber), Grid_Position{line_num(), line.size() - current_nunber.size()});
		symbol_map_.push_back(std::move(symbol_row));
	}

private:
	std::vector<std::vector<char>> symbol_map_;
	std::vector<std::pair<int, Grid_Position>> number_positions_;

	[[nodiscard]] bool touches_symbols(int number, const Grid_Position &position) const noexcept {
		const auto width = count_digits(number);
		for (auto column_index = position.c > 0 ? position.c - 1 : 0; column_index <= std::min(position.c + width, symbol_map_[position.r].size() - 1); ++column_index) {
			if ((position.r > 0 && symbol_map_[position.r - 1][column_index] != ' ')
				|| symbol_map_[position.r][column_index] != ' '
				|| (position.r < symbol_map_.size() - 1 && symbol_map_[position.r + 1][column_index] != ' '))
				return true;
		}
		return false;
	}

	[[nodiscard]] std::vector<Grid_Position> adjacent_symbols(int number, const Grid_Position &position, char symbol) const noexcept {
		std::vector<Grid_Position> symbol_positions;
		const auto width = count_digits(number);
		for (auto column_index = position.c > 0 ? position.c - 1 : 0; column_index <= std::min(position.c + width, symbol_map_[position.r].size() - 1); ++column_index) {
			if (position.r > 0 && symbol_map_[position.r - 1][column_index] == symbol)
				symbol_positions.push_back(Grid_Position{position.r - 1, column_index});
			if (symbol_map_[position.r][column_index] == symbol)
				symbol_positions.push_back(Grid_Position{position.r, column_index});
			if (position.r < symbol_map_.size() - 1 && symbol_map_[position.r + 1][column_index] == symbol)
				symbol_positions.push_back(Grid_Position{position.r + 1, column_index});
		}
		return symbol_positions;
	}
};

int main(int argc, char *argv[]) {
	const auto part = select_part(argc, argv);
	auto schematic = Engine_Schematic::create_from_stream(std::cin);
	auto values = part == 1 ? schematic.part_numbers() : schematic.gear_ratios();
	std::cout << std::accumulate(values.begin(), values.end(), 0, [](int sum, int number) { return sum + number; }) << std::endl;
	return 0;
}
