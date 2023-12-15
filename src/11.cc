#include "common.h"
#include <map>
#include <numeric>

struct Universe {

	explicit Universe(const std::vector<std::vector<char>> &grid) {
		height_ = grid.size();
		width_ = grid[0].size();
		for (std::size_t r = 0; r < grid.size(); ++r) {
			for (std::size_t c = 0; c < grid[0].size(); ++c) {
				if (grid[r][c] == '#') {
					galaxy_locations_.push_back(Grid_Position{r, c});
					galaxy_column_order_.push_back(c);
				}
			}
		}
		std::ranges::sort(galaxy_column_order_);
		galaxy_column_order_.erase(std::ranges::unique(galaxy_column_order_).begin(), galaxy_column_order_.end());
	}

	[[nodiscard]] std::vector<std::size_t> galaxy_distances() const {
		std::vector<std::size_t> distances;
		ranges_foreach_combination(galaxy_locations_, [&distances](const auto &lhs, const auto &rhs) {
			distances.push_back(galaxy_distance(lhs, rhs));
		});
		return distances;
	}

	void expand(std::size_t n) {
		ssize_t last_r{-1};
		std::size_t inc{0};
		for (auto &[r, c] : galaxy_locations_) {
			const auto dist = r - last_r;
			if (dist > 1)
				inc += (dist - 1) * n;
			last_r = static_cast<ssize_t>(r);
			r += inc;
		}

		ssize_t last_c{-1};
		inc = 0;
		std::unordered_map<std::size_t, std::size_t> incs;
		for (auto c : galaxy_column_order_) {
			const auto dist = c - last_c;
			if (dist > 1)
				inc += (dist - 1) * n;
			last_c = static_cast<ssize_t>(c);
			incs[c] = inc;
		}

		for (auto &[r, c] : galaxy_locations_)
			c += incs[c];
	}

private:
	[[nodiscard]] static std::size_t galaxy_distance(const Grid_Position &lhs, const Grid_Position &rhs) noexcept {
		return std::abs(static_cast<long>(lhs.r) - static_cast<long>(rhs.r)) + std::abs(static_cast<long>(lhs.c) - static_cast<long>(rhs.c));
	}

	std::size_t width_, height_;
	std::vector<Grid_Position> galaxy_locations_;
	std::vector<std::size_t> galaxy_column_order_;
};

int main(int argc, char *argv[]) {
	const auto part = select_part(argc, argv);
	Universe universe(read_grid(std::cin));
	universe.expand(part == 1 ? 1 : 999999);

	const auto distances = universe.galaxy_distances();
	std::cout << std::accumulate(distances.begin(), distances.end(), std::size_t{0},
								 [](auto sum, auto distance) { return sum + distance; }) << std::endl;
}