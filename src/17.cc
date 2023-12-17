#include "common.h"
#include <cstdint>
#include <deque>
#include <limits>

[[nodiscard]] static std::uint8_t compute_hash(int straight_count, Grid_Direction direction) noexcept {
	return (static_cast<std::uint8_t>(straight_count) << 2) + direction;  // 6-bit hash
}

[[nodiscard]] static int min_heat_loss(const std::vector<std::vector<int>> &grid, int min_straight, int max_straight) {
	std::vector<std::vector<std::vector<int>>> min_losses(
			grid.size(), std::vector<std::vector<int>>(grid[0].size(),
													   std::vector<int>(64, std::numeric_limits<int>::max())));
	std::deque<std::tuple<Grid_Position, Grid_Direction, int, int>> to_visit{
		{Grid_Position{0, 1}, Grid_Direction::RIGHT, 0, 1},
		{Grid_Position{1, 0}, Grid_Direction::DOWN, 0, 1}
	};
	while (!to_visit.empty()) {
		const auto [position, direction, prev_cost, straight_count] = to_visit.front();
		to_visit.pop_front();

		const auto hash = compute_hash(straight_count, direction);
		if (const auto cost = prev_cost + grid[position.r][position.c]; cost < min_losses[position.r][position.c][hash]) {
			min_losses[position.r][position.c][hash] = cost;
			if (straight_count < max_straight && position.can_move(direction, grid.size(), grid[0].size()))
				to_visit.emplace_back(position.move(direction), direction, cost, straight_count + 1);
			if (straight_count >= min_straight) {
				if (auto ccw_direction = direction.turn_counter_clockwise(); position.can_move(ccw_direction, grid.size(), grid[0].size()))
					to_visit.emplace_back(position.move(ccw_direction), ccw_direction, cost, 1);
				if (auto cw_direction = direction.turn_clockwise(); position.can_move(cw_direction, grid.size(), grid[0].size()))
					to_visit.emplace_back(position.move(cw_direction), cw_direction, cost, 1);
			}
		}
	}
	const auto &destination = min_losses[grid.size() - 1][grid[0].size() - 1];
	return *std::min_element(destination.begin() + compute_hash(min_straight, Grid_Direction{0}), destination.end());
}

int main(int argc, char *argv[]) {
	const auto part = select_part(argc, argv);

	auto grid = read_integer_grid(std::cin);
	std::cout << (part == 1 ? min_heat_loss(grid, 1, 3) : min_heat_loss(grid, 4, 10)) << std::endl;
	return 0;
}