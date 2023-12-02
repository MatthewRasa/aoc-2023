#pragma once

#include <algorithm>
#include <cstring>
#include <functional>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

/* --- Input readers --- */

bool has_input(std::istream &in) {
	return in.peek() != -1;
}

std::string read_line(std::istream &in) {
	std::string line;
	std::getline(in, line);
	return line;
}

std::vector<std::string> read_lines(std::istream &in) {
	std::vector<std::string> lines;
	for (std::string line; std::getline(in, line); )
		lines.push_back(std::move(line));
	return lines;
}

template<typename Transform_FuncT = std::function<std::string(std::string &&)>>
auto read_tokens(std::istream &in, char delim = ' ', const Transform_FuncT &transform_func = [](std::string &&token) { return std::move(token); }) {
	using Token_Type = std::decay_t<std::invoke_result_t<decltype(transform_func), std::string &&>>;
	std::vector<Token_Type> tokens;
	for (std::string token; std::getline(in, token, delim); )
		tokens.push_back(transform_func(std::move(token)));
	return tokens;
}

template<typename Transform_FuncT = std::function<std::string(std::string &&)>>
auto read_tokens(const std::string &in, char delim = ' ', const Transform_FuncT &transform_func = [](std::string &&token) { return std::move(token); }) {
	std::stringstream ss{in};
	return read_tokens(ss, delim, transform_func);
}

template<typename Transform_FuncT = std::function<char(char)>>
auto read_grid(std::istream &in, const Transform_FuncT &transform_func = [](char c) { return c; }) {
	using Element_Type = std::decay_t<std::invoke_result_t<decltype(transform_func), char>>;
	std::vector<std::vector<Element_Type>> grid;
	for (std::string line; std::getline(in, line) && !line.empty(); ) {
		std::vector<Element_Type> grid_line(line.size());
		std::transform(line.begin(), line.end(), grid_line.begin(), transform_func);
		grid.push_back(std::move(grid_line));
	}
	return grid;
}

std::vector<std::vector<int>> read_integer_grid(std::istream &in) {
	return read_grid(in, [](char c) { return c - '0'; });
}

template<class CRTP, char DelimV = ' '>
struct Token_Reader {
	static CRTP create_from_stream(std::istream &in) {
		std::string line;
		if (!std::getline(in, line))
			throw std::logic_error{"EOF encountered in Token_Reader"};
		std::istringstream ss{line};

		CRTP instance{};
		for (std::string token; std::getline(ss, token, DelimV); ++instance.token_num_)
			instance.read_token(token);
		instance.read_end();
		return instance;
	}

	static CRTP create_from_string(const std::string &in) {
		std::stringstream ss{in};
		return create_from_stream(ss);
	}

	virtual ~Token_Reader() = default;

	virtual void read_token(const std::string &token) = 0;

	virtual void read_end() { };

	[[nodiscard]] std::size_t token_num() const noexcept {
		return token_num_;
	}

private:
	std::size_t token_num_{0};
};

template<class CRTP>
struct Line_Reader {
	static CRTP create_from_stream(std::istream &in) {
		std::string line;
		if (!std::getline(in, line))
			throw std::logic_error{"EOF encountered in Line_Reader"};

		CRTP instance{};
		instance.read_line(line);
		return instance;
	}

	static CRTP create_from_string(const std::string &in) {
		std::stringstream ss{in};
		return create_from_stream(ss);
	}

	virtual ~Line_Reader() = default;

	virtual void read_line(const std::string &line) = 0;
};

template<class CRTP>
struct Paragraph_Reader {
	static CRTP create_from_stream(std::istream &in) {
		CRTP instance{};
		for (std::string line; std::getline(in, line) && !line.empty(); ++instance.line_num_)
			instance.read_line(line);
		instance.read_end();
		return instance;
	}

	static CRTP create_from_string(const std::string &in) {
		std::stringstream ss{in};
		return create_from_stream(ss);
	}

	virtual ~Paragraph_Reader() = default;

	virtual void read_line(const std::string &line) = 0;

	virtual void read_end() { };

	[[nodiscard]] std::size_t line_num() const noexcept {
		return line_num_;
	}

private:
	std::size_t line_num_{0};
};

/* --- Numeric Conversions --- */

[[nodiscard]] bool is_digit(char c) noexcept {
	return '0' <= c && c <= '9';
}

/* --- Algorithm Functors --- */

struct Is_Digit {
	[[nodiscard]] bool operator()(char c) const noexcept {
		return is_digit(c);
	}
};

/* --- Position --- */

struct Position {
	int x, y;
	[[nodiscard]] bool operator==(const Position &other) const noexcept {
		return x == other.x && y == other.y;
	}
};

struct Position3D {
	int x, y, z;
	[[nodiscard]] bool operator==(const Position3D &other) const noexcept {
		return x == other.x && y == other.y && z == other.z;
	}
};

struct Grid_Position {
	std::size_t r, c;
	[[nodiscard]] bool operator==(const Grid_Position &other) const noexcept {
		return r == other.r && c == other.c;
	}
};

namespace std {
	template<>
	struct hash<Position> {
		[[nodiscard]] std::size_t operator()(const Position &position) const noexcept {
			return (static_cast<std::size_t>(position.x) << 32) + static_cast<std::size_t>(position.y);
		}
	};

	template<>
	struct hash<Position3D> {
		[[nodiscard]] std::size_t operator()(const Position3D &position) const noexcept {
			return std::hash<std::string>{}(std::to_string(position.x) + "," + std::to_string(position.y) + "," + std::to_string(position.z));
		}
	};

	template<>
	struct hash<Grid_Position> {
		[[nodiscard]] std::size_t operator()(const Grid_Position &position) const noexcept {
			return (position.r << 32) + static_cast<std::size_t>(position.c);
		}
	};

	std::ostream &operator<<(std::ostream &out, const Position &position) {
		out << "<" << position.x << "," << position.y << ">";
		return out;
	}

	std::ostream &operator<<(std::ostream &out, const Position3D &position) {
		out << "<" << position.x << "," << position.y << "," << position.z << ">";
		return out;
	}

	std::ostream &operator<<(std::ostream &out, const Grid_Position &position) {
		out << "<" << position.r << "," << position.c << ">";
		return out;
	}
}

/* --- Circular Queue --- */

template<typename T, typename QueueT = std::vector<T>>
struct Circular_Queue {

	explicit Circular_Queue(const QueueT &queue)
		: queue_{queue},
		  idx_{0} { }

	explicit Circular_Queue(QueueT &&queue)
		: queue_{std::move(queue)},
		  idx_{0} { }

	[[nodiscard]] std::size_t size() const noexcept {
		return queue_.size();
	}

	[[nodiscard]] bool at_start() const noexcept {
		return idx_ == 0;
	}

	[[nodiscard]] std::size_t position() const noexcept {
		return idx_;
	}

	[[nodiscard]] const T &current() const noexcept {
		return queue_[idx_];
	}

	void next() noexcept {
		idx_ = (idx_ + 1) % queue_.size();
	}

	const T &take() noexcept {
		auto prev_idx = idx_;
		next();
		return queue_[prev_idx];
	}

private:
	QueueT queue_;
	std::size_t idx_;
};

/* --- Visual debugging */

template<typename ItemT>
void print_grid(const std::vector<std::vector<ItemT>> &grid) {
	for (const auto &row : grid) {
		std::copy(row.begin(), row.end(), std::ostream_iterator<ItemT>(std::cout));
		std::cout << std::endl;
	}
}

template<typename PositionsT>
void print_grid_positions(const PositionsT &positions, char display_char = '#') {
	print_grid_positions(positions,
						 std::min_element(positions.begin(), positions.end(),
										  [](const auto &p0, const auto &p1) { return p0.x < p1.x; })->x,
						 std::min_element(positions.begin(), positions.end(),
										  [](const auto &p0, const auto &p1) { return p0.y < p1.y; })->y,
						 std::max_element(positions.begin(), positions.end(),
										  [](const auto &p0, const auto &p1) { return p0.x < p1.x; })->x,
						 std::max_element(positions.begin(), positions.end(),
										  [](const auto &p0, const auto &p1) { return p0.y < p1.y; })->y,
						 display_char);
}

template<typename PositionsT>
void print_grid_positions(const PositionsT &positions, std::size_t min_x, std::size_t min_y, std::size_t max_x, std::size_t max_y, char display_char = '#') {
	std::vector<std::vector<char>> grid(max_y - min_y + 1, std::vector<char>(max_x - min_x + 1, '.'));
	for (const auto &position : positions)
		grid[position.y - min_y][position.x - min_x] = display_char;
	for (const auto &row : grid) {
		std::copy(row.begin(), row.end(), std::ostream_iterator<char>(std::cout));
		std::cout << std::endl;
	}
}

/* --- Boilerplate --- */

uint select_part(int argc, char *argv[]) {
	if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << " <1|2>" << std::endl;
		std::exit(1);
	}

	if (strncmp(argv[1], "1", 1) == 0) {
		return 1;
	} else if (strncmp(argv[1], "2", 1) == 0) {
		return 2;
	} else {
		std::cerr << argv[0] << ": invalid argument '" << argv[1] << "'" << std::endl;
		std::exit(1);
	}
}
