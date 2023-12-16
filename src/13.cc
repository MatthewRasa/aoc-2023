#include "common.h"
#include <numeric>

struct Mirror {
	explicit Mirror(std::vector<std::vector<char>> grid)
			: grid_{std::move(grid)} { }

	[[nodiscard]] size_t vertical_reflect_columns(std::size_t tolerance) const noexcept {
		for (std::size_t column = 0; column < grid_[0].size() - 1; ++column) {
			if (vertical_reflection_errors(column, column + 1) == tolerance)
				return column + 1;
		}
		return 0;
	}

	[[nodiscard]] size_t horizontal_reflect_rows(std::size_t tolerance) const noexcept {
		for (std::size_t row = 0; row < grid_.size() - 1; ++row) {
			if (horizontal_reflection_errors(row, row + 1) == tolerance)
				return row + 1;
		}
		return 0;
	}

private:
	std::vector<std::vector<char>> grid_;

	[[nodiscard]] std::size_t column_errors(std::size_t column1, std::size_t column2) const noexcept {
		return std::ranges::count_if(grid_, [column1, column2](const auto &row) { return row[column1] != row[column2]; });
	}

	[[nodiscard]] std::size_t row_errors(std::size_t row1, std::size_t row2) const noexcept {
		std::size_t num_errors{0};
		for (std::size_t column = 0; column < grid_[0].size(); ++column) {
			if (grid_[row1][column] != grid_[row2][column])
				++num_errors;
		}
		return num_errors;
	}

	[[nodiscard]] std::size_t vertical_reflection_errors(std::size_t column_left, std::size_t column_right) const noexcept {
		std::size_t num_errors{0};
		for (;; --column_left, ++column_right) {
			num_errors += column_errors(column_left, column_right);
			if (column_left == 0 || column_right == grid_[0].size() - 1)
				break;
		}
		return num_errors;
	}

	[[nodiscard]] std::size_t horizontal_reflection_errors(std::size_t row_above, std::size_t row_below) const noexcept {
		std::size_t num_errors{0};
		for (;; --row_above, ++row_below) {
			num_errors += row_errors(row_above, row_below);
			if (row_above == 0 || row_below == grid_.size() - 1)
				break;
		}
		return num_errors;
	}
};

int main(int argc, char *argv[]) {
	const auto tolerance = select_part(argc, argv) == 1 ? 0 : 1;

	std::vector<Mirror> mirrors;
	while (has_input(std::cin))
		mirrors.emplace_back(read_grid(std::cin));

	std::cout << std::accumulate(mirrors.begin(), mirrors.end(), std::size_t{0}, [tolerance](auto sum, const auto &mirror) {
		const auto val = mirror.vertical_reflect_columns(tolerance);
		return sum + (val != 0 ? val : mirror.horizontal_reflect_rows(tolerance) * 100);
	}) << std::endl;
	return 0;
}