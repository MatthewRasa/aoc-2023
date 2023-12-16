#include "common.h"

[[nodiscard]] static std::vector<std::vector<char>> tilt_north(std::vector<std::vector<char>> grid) noexcept {
	for (std::size_t column = 0; column < grid[0].size(); ++column) {
		std::size_t free_row{0};
		for (std::size_t row = 0; row < grid.size(); ++row) {
			if (grid[row][column] == '#') {
				free_row = row + 1;
			} else if (grid[row][column] == 'O') {
				if (row != free_row) {
					grid[free_row][column] = 'O';
					grid[row][column] = '.';
				}
				++free_row;
			}
		}
	}
	return grid;
}

[[nodiscard]] static std::vector<std::vector<char>> tilt_west(std::vector<std::vector<char>> grid) noexcept {
	for (std::size_t row = 0; row < grid.size(); ++row) {
		std::size_t free_column{0};
		for (std::size_t column = 0; column < grid[0].size(); ++column) {
			if (grid[row][column] == '#') {
				free_column = column + 1;
			} else if (grid[row][column] == 'O') {
				if (column != free_column) {
					grid[row][free_column] = 'O';
					grid[row][column] = '.';
				}
				++free_column;
			}
		}
	}
	return grid;
}

[[nodiscard]] static std::vector<std::vector<char>> tilt_south(std::vector<std::vector<char>> grid) noexcept {
	for (std::size_t column = 0; column < grid[0].size(); ++column) {
		ssize_t free_row{static_cast<ssize_t>(grid.size()) - 1};
		for (ssize_t row = static_cast<ssize_t>(grid.size()) - 1; row >= 0; --row) {
			if (grid[row][column] == '#') {
				free_row = row - 1;
			} else if (grid[row][column] == 'O') {
				if (row != free_row) {
					grid[free_row][column] = 'O';
					grid[row][column] = '.';
				}
				--free_row;
			}
		}
	}
	return grid;
}

[[nodiscard]] static std::vector<std::vector<char>> tilt_east(std::vector<std::vector<char>> grid) noexcept {
	for (std::size_t row = 0; row < grid.size(); ++row) {
		ssize_t free_column{static_cast<ssize_t>(grid[0].size()) - 1};
		for (ssize_t column = static_cast<ssize_t>(grid[0].size()) - 1; column >= 0; --column) {
			if (grid[row][column] == '#') {
				free_column = column - 1;
			} else if (grid[row][column] == 'O') {
				if (column != free_column) {
					grid[row][free_column] = 'O';
					grid[row][column] = '.';
				}
				--free_column;
			}
		}
	}
	return grid;
}

[[nodiscard]] static std::size_t calculate_load(const std::vector<std::vector<char>> &grid) noexcept {
	std::size_t load{0};
	for (std::size_t row = 0; row < grid.size(); ++row)
		load += std::ranges::count(grid[row], 'O') * (grid.size() - row);
	return load;
}

[[nodiscard]] static std::size_t grid_hash(const std::vector<std::vector<char>> &grid) {
	std::string grid_str{};
	for (const auto &row : grid)
		grid_str.insert(grid_str.end(), row.begin(), row.end());
	return std::hash<std::string>{}(grid_str);
}

[[nodiscard]] static std::size_t calculate_load_after(std::vector<std::vector<char>> grid, std::size_t num_cycles) {
	std::unordered_map<std::size_t, std::size_t> cache{{grid_hash(grid), 0}};
	std::unordered_map<std::size_t, std::size_t> loads{{0, calculate_load(grid)}};
	for (std::size_t cycle = 1; cycle <= num_cycles; ++cycle) {
		grid = tilt_east(tilt_south(tilt_west(tilt_north(grid))));

		const auto hash = grid_hash(grid);
		if (auto it = cache.find(hash); it != cache.end()) {
			return loads[it->second + (num_cycles - it->second) % (cycle - it->second)];
		}
		cache[hash] = cycle;
		loads[cycle] = calculate_load(grid);
	}
	return calculate_load(grid);
}


int main(int argc, char *argv[]) {
	const auto part = select_part(argc, argv);

	auto grid = read_grid(std::cin);
	std::cout << (part == 1 ? calculate_load(tilt_north(grid)) : calculate_load_after(grid, 1000000000)) << std::endl;
	return 0;
}