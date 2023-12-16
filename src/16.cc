#include "common.h"
#include <cstdint>
#include <numeric>

struct Direction {
	enum : uint8_t {
		NONE =  0b0000,
		UP =    0b0001,
		LEFT =  0b0010,
		DOWN =  0b0100,
		RIGHT = 0b1000
	};
};

[[nodiscard]] static std::uint8_t next_light_direction(std::uint8_t direction, char tile) {
	switch (tile) {
	case '.':
		return direction;
	case '/':
		switch (direction) {
		case Direction::UP:
			return Direction::RIGHT;
		case Direction::LEFT:
			return Direction::DOWN;
		case Direction::DOWN:
			return Direction::LEFT;
		case Direction::RIGHT:
			return Direction::UP;
		default:
			throw std::logic_error{"Unexpected direction"};
		}
	case '\\':
		switch (direction) {
		case Direction::UP:
			return Direction::LEFT;
		case Direction::LEFT:
			return Direction::UP;
		case Direction::DOWN:
			return Direction::RIGHT;
		case Direction::RIGHT:
			return Direction::DOWN;
		default:
			throw std::logic_error{"Unexpected direction"};
		}
	case '|':
		switch (direction) {
		case Direction::UP:
		case Direction::DOWN:
			return direction;
		case Direction::LEFT:
		case Direction::RIGHT:
			return Direction::UP | Direction::DOWN;
		default:
			throw std::logic_error{"Unexpected direction"};
		}
	case '-':
		switch (direction) {
		case Direction::UP:
		case Direction::DOWN:
			return Direction::LEFT | Direction::RIGHT;
		case Direction::LEFT:
		case Direction::RIGHT:
			return direction;
		default:
			throw std::logic_error{"Unexpected direction"};
		}
	default:
		throw std::logic_error{"Unexpected tile"};
	}
}

static void plot_energy(const std::vector<std::vector<char>> &grid, const Grid_Position &position,
						std::uint8_t direction, std::vector<std::vector<std::uint8_t>> &energy) {
	const auto next_direction = next_light_direction(direction, grid[position.r][position.c]);
	if ((energy[position.r][position.c] & next_direction) != next_direction) {
		energy[position.r][position.c] |= next_direction;
		if ((next_direction & Direction::UP) && position.r > 0)
			plot_energy(grid, position.move_up(), Direction::UP, energy);
		if ((next_direction & Direction::LEFT) && position.c > 0)
			plot_energy(grid, position.move_left(), Direction::LEFT, energy);
		if ((next_direction & Direction::DOWN) && position.r < grid.size() - 1)
			plot_energy(grid, position.move_down(), Direction::DOWN, energy);
		if ((next_direction & Direction::RIGHT) && position.c < grid[0].size() - 1)
			plot_energy(grid, position.move_right(), Direction::RIGHT, energy);
	}
}

[[nodiscard]] static std::size_t count_energized(const std::vector<std::vector<char>> &grid,
												 const Grid_Position &start_position, std::uint8_t start_direction) {
	std::vector<std::vector<std::uint8_t>> energy(grid.size(), std::vector<std::uint8_t>(grid[0].size(), Direction::NONE));
	plot_energy(grid, start_position, start_direction, energy);
	return std::accumulate(energy.begin(), energy.end(), std::size_t{0}, [](auto sum, const auto &row) {
		return sum + std::accumulate(row.begin(), row.end(), std::size_t{0},
									 [](auto sum, auto direction) { return sum + (direction != Direction::NONE); });
	});
}

[[nodiscard]] static std::size_t count_max_energized(const std::vector<std::vector<char>> &grid) {
	std::size_t count{0};
	for (std::size_t c = 0; c < grid[0].size(); ++c)
		count = std::max(count, count_energized(grid, Grid_Position{0, c}, Direction::DOWN));
	for (std::size_t r = 0; r < grid.size(); ++r)
		count = std::max(count, count_energized(grid, Grid_Position{r, 0}, Direction::RIGHT));
	for (std::size_t c = 0; c < grid[0].size(); ++c)
		count = std::max(count, count_energized(grid, Grid_Position{grid.size() - 1, c}, Direction::UP));
	for (std::size_t r = 0; r < grid.size(); ++r)
		count = std::max(count, count_energized(grid, Grid_Position{r, grid[0].size() - 1}, Direction::LEFT));
	return count;
}

int main(int argc, char *argv[]) {
	const auto part = select_part(argc, argv);

	auto grid = read_grid(std::cin);
	std::cout << (part == 1 ? count_energized(grid, Grid_Position{0, 0}, Direction::RIGHT) : count_max_energized(grid)) << std::endl;
	return 0;
}