#include "common.h"
#include <cstdint>
#include <deque>
#include <limits>
#include <numeric>

struct Tile {
	static constexpr std::size_t INF{std::numeric_limits<std::size_t>::max()};

	explicit Tile(char symbol) noexcept
		: symbol_{symbol} { }

	[[nodiscard]] bool is_start() const noexcept {
		return symbol_ == 'S';
	}

	[[nodiscard]] std::size_t distance_from_start() const noexcept {
		return distance_from_start_;
	}

	[[nodiscard]] bool on_loop() const noexcept {
		return distance_from_start_ != INF;
	}

	void link_neighbors(std::vector<std::vector<Tile>> &grid, std::size_t row, std::size_t column) {
		if (symbol_one_of<'|', '7', 'F', 'S'>(symbol_) && row < grid.size() - 1 && symbol_one_of<'|', 'L', 'J', 'S'>(grid[row + 1][column].symbol_))
			link_neighbor(grid[row + 1][column]);
		if (symbol_one_of<'-', 'L', 'F', 'S'>(symbol_) && column < grid[0].size() - 1 && symbol_one_of<'-', 'J', '7', 'S'>(grid[row][column + 1].symbol_))
			link_neighbor(grid[row][column + 1]);
	}

	void calculate_loop_distances() {
		if (symbol_ != 'S')
			throw std::logic_error{"Must invoke from start tile"};

		std::deque<const Tile *> to_visit;
		for (auto neighbor_ptr : neighbors_) {
			neighbor_ptr->distance_from_start_ = 1;
			to_visit.push_back(neighbor_ptr);
		}

		while (!to_visit.empty()) {
			auto current = to_visit.front();
			to_visit.pop_front();
			for (auto next_ptr : current->neighbors_) {
				if (current->distance_from_start_ + 1 < next_ptr->distance_from_start_) {
					next_ptr->distance_from_start_ = current->distance_from_start_ + 1;
					to_visit.push_back(next_ptr);
				}
			}
		}
	}

	[[nodiscard]] bool connects_to(const Tile &other) const noexcept {
		return std::ranges::find(neighbors_, &other) != neighbors_.end();
	}

private:
	template<char ...symbols>
	[[nodiscard]] static bool symbol_one_of(char symbol) {
		return ((symbol == symbols) || ...);
	}

	void link_neighbor(Tile &other) noexcept {
		neighbors_.emplace_back(&other);
		other.neighbors_.emplace_back(this);
	}

	char symbol_;
	std::vector<Tile *> neighbors_;
	std::size_t distance_from_start_{INF};
};

[[nodiscard]] static Tile &find_start(std::vector<std::vector<Tile>> &grid) {
	for (auto &row : grid) {
		auto it = std::ranges::find_if(row, [](const auto &tile) { return tile.is_start(); });
		if (it != row.end())
			return *it;
	}
	throw std::logic_error{"Unable to find starting position"};
}

[[nodiscard]] static std::size_t find_max_distance(std::vector<std::vector<Tile>> &grid) {
	std::size_t result{0};
	for (const auto &row : grid) {
		for (const auto &tile : row) {
			if (tile.distance_from_start() != Tile::INF)
				result = std::max(result, tile.distance_from_start());
		}
	}
	return result;
}

[[nodiscard]] static std::size_t count_loop_size(const std::vector<std::vector<Tile>> &grid) {
	return std::accumulate(grid.begin(), grid.end(), std::size_t{0}, [](auto sum, const auto &row) {
		return sum + std::accumulate(row.begin(), row.end(), std::size_t{0},
									 [](auto sum, const auto &tile) { return sum + tile.on_loop(); });
	});
}

[[nodiscard]] static std::size_t count_enclosed(const std::vector<std::vector<Tile>> &grid) {
	std::vector<std::vector<std::uint8_t>> visited(grid.size() * 2 + 1, std::vector<std::uint8_t>(grid[0].size() * 2 + 1, false));
	std::deque<Grid_Position> to_visit{Grid_Position{0, 0}};
	std::size_t non_enclosed_count{0};
	while (!to_visit.empty()) {
		const auto position = to_visit.front();
		to_visit.pop_front();
		if (visited[position.r][position.c])
			continue;
		visited[position.r][position.c] = true;

		if (position.r % 2 == 1 && position.c % 2 == 1) {
			if (grid[(position.r - 1) / 2][(position.c - 1) / 2].on_loop())
				continue;
			++non_enclosed_count;
		} else if (position.r % 2 == 0 && position.c % 2 == 1) {
			if (0 < position.r && position.r < visited.size() - 1 && grid[(position.r - 1) / 2][(position.c - 1) / 2].connects_to(grid[position.r / 2][(position.c - 1) / 2]))
				continue;
		} else if (position.r % 2 == 1 && position.c % 2 == 0) {
			if (0 < position.c && position.c < visited[0].size() - 1 && grid[(position.r - 1) / 2][(position.c - 1) / 2].connects_to(grid[(position.r - 1) / 2][position.c / 2]))
				continue;
		}

		if (position.r > 0)
			to_visit.push_back({Grid_Position{position.r - 1, position.c}});
		if (position.r < visited.size() - 1)
			to_visit.push_back({Grid_Position{position.r + 1, position.c}});
		if (position.c > 0)
			to_visit.push_back({Grid_Position{position.r, position.c - 1}});
		if (position.c < visited[0].size() - 1)
			to_visit.push_back({Grid_Position{position.r, position.c + 1}});
	}
	return grid.size() * grid[0].size() - non_enclosed_count - count_loop_size(grid);
}

int main(int argc, char *argv[]) {
	const auto part = select_part(argc, argv);
	auto grid = read_grid(std::cin, [](auto c) { return Tile{c}; });
	for (std::size_t row = 0; row < grid.size(); ++row) {
		for (std::size_t column = 0; column < grid[0].size(); ++column)
			grid[row][column].link_neighbors(grid, row, column);
	}

	find_start(grid).calculate_loop_distances();
	std::cout << (part == 1 ? find_max_distance(grid) : count_enclosed(grid)) << std::endl;
	return 0;
}