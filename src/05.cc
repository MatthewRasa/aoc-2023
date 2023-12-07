#include "common.h"
#include <algorithm>
#include <iterator>

struct Range {
	ulong start;
	ulong length;

	[[nodiscard]] bool operator<(const Range &other) const noexcept {
		return start < other.start;
	}
};

struct Map : Paragraph_Reader<Map> {

	struct Converter : Token_Reader<Converter> {

		[[nodiscard]] bool operator<(const Converter &other) const noexcept {
			return src_start_ < other.src_start_;
		}

		[[nodiscard]] bool contains(ulong value) const noexcept {
			return src_start_ <= value && value < src_start_ + length_;
		}

		[[nodiscard]] ulong destination_start() const noexcept {
			return dst_start_;
		}

		[[nodiscard]] ulong source_start() const noexcept {
			return src_start_;
		}

		[[nodiscard]] ulong length() const noexcept {
			return length_;
		}

		[[nodiscard]] ulong convert(ulong value) const noexcept {
			return dst_start_ + (value - src_start_);
		}

		void read_token(const std::string &token) final {
			switch (token_num()) {
			case 0:
				dst_start_ = std::stoul(token);
				break;
			case 1:
				src_start_ = std::stoul(token);
				break;
			case 2:
				length_ = std::stoul(token);
				break;
			}
		}

	private:
		ulong dst_start_;
		ulong src_start_;
		ulong length_;
	};

	[[nodiscard]] std::vector<Range> map_range(Range input_range) const /*noexcept*/ {
		std::vector<Range> output_ranges;
		for (const auto &converter : converters_) {
			if (input_range.start < converter.source_start()) {
				const auto pre_convert_length = converter.source_start() - input_range.start;
				if (input_range.length <= pre_convert_length) {
					output_ranges.push_back(input_range);
					return output_ranges;
				}
				output_ranges.push_back(Range{input_range.start, pre_convert_length});
				input_range.start += pre_convert_length;
				input_range.length -= pre_convert_length;
			}
			if (converter.contains(input_range.start)) {
				const auto convert_length = converter.length() - (input_range.start - converter.source_start());
				if (input_range.length <= convert_length) {
					output_ranges.push_back(Range{converter.convert(input_range.start), input_range.length});
					return output_ranges;
				}
				output_ranges.push_back(Range{converter.convert(input_range.start), convert_length});
				input_range.start += convert_length;
				input_range.length -= convert_length;
			}
		}
		output_ranges.push_back(input_range);
		return output_ranges;
	}

	void read_line(const std::string &line) final {
		if (line_num() != 0)
			converters_.push_back(Converter::create_from_string(line));
	}

	void read_end() final {
		std::sort(converters_.begin(), converters_.end());
	}

private:
	std::vector<Converter> converters_;
};

[[nodiscard]] static std::vector<Range> read_seed_values(std::istream &in) {
	const auto seed_strs = read_tokens(read_line(in), ' ');
	std::vector<Range> seeds(seed_strs.size() - 1);
	std::transform(std::next(seed_strs.begin()), seed_strs.end(), seeds.begin(),
				   [](const auto &seed_str) { return Range{std::stoul(seed_str), 1}; });
	return seeds;
}

[[nodiscard]] static std::vector<Range> read_seed_ranges(std::istream &in) {
	const auto seed_strs = read_tokens(read_line(in), ' ');
	std::vector<Range> seed_ranges;
	seed_ranges.reserve((seed_strs.size() - 1) / 2);
	for (auto str_it = std::next(seed_strs.begin()); str_it != seed_strs.end(); str_it += 2)
		seed_ranges.push_back(Range{std::stoul(*str_it), std::stoul(*std::next(str_it))});
	return seed_ranges;
}

[[nodiscard]] static ulong min_location(const std::vector<Range> &seed_ranges, const std::vector<Map> &maps) {
	std::vector<Range> ranges{seed_ranges};
	for (const auto &map : maps) {
		std::vector<Range> new_ranges;
		for (const auto &range: ranges)
			std::ranges::move(map.map_range(range), std::back_inserter(new_ranges));
		ranges = std::move(new_ranges);
	}
	return std::min_element(ranges.begin(), ranges.end())->start;
}

int main(int argc, char *argv[]) {
	const auto seed_ranges = select_part(argc, argv) == 1 ? read_seed_values(std::cin) : read_seed_ranges(std::cin);
	read_line(std::cin);
	std::vector<Map> maps;
	while (has_input(std::cin))
		maps.push_back(Map::create_from_stream(std::cin));

	std::cout << min_location(seed_ranges, maps) << std::endl;
	return 0;
}