#include "common.h"
#include <numeric>

struct Record : Token_Reader<Record> {

	[[nodiscard]] std::size_t num_arrangements() const {
		std::unordered_map<std::string, std::size_t> cache;
		return num_arrangements(0, 0, cache);
	}

	void unfold(std::size_t n) {
		const auto condition_orig{condition_};
		const auto group_sizes_orig{group_sizes_};
		for (std::size_t i = 1; i < n; ++i) {
			condition_ += "?" + condition_orig;
			group_sizes_.insert(group_sizes_.end(), group_sizes_orig.begin(), group_sizes_orig.end());
		}
	}

	void read_token(const std::string &token) final {
		switch (token_num()) {
		case 0:
			condition_ = token;
			break;
		case 1:
			group_sizes_ = read_tokens(token, ',', [](const auto &token) { return std::stoul(token); });
			break;
		}
	}

private:
	std::string condition_;
	std::vector<std::size_t> group_sizes_;

	[[nodiscard]] std::string cache_key(std::size_t condition_start, std::size_t group_idx) const {
		auto key = condition_.substr(condition_start);
		for (auto group_it = group_sizes_.begin() + static_cast<ssize_t>(group_idx); group_it != group_sizes_.end(); ++group_it)
			key += "," + std::to_string(*group_it);
		return key;
	}

	[[nodiscard]] std::size_t num_arrangements(std::size_t condition_start, std::size_t group_idx, std::unordered_map<std::string, std::size_t> &cache) const {
		if (condition_start > 0 && condition_[condition_start - 1] == '#')
			return 0;
		if (group_idx == group_sizes_.size())
			return std::none_of(condition_.begin() + static_cast<ssize_t>(condition_start), condition_.end(), [](auto c) { return c == '#'; });
		if (condition_start >= condition_.size())
			return 0;

		const auto key = cache_key(condition_start, group_idx);
		auto cache_it = cache.find(key);
		if (cache_it == cache.end()) {
			std::size_t count{0};
			for (auto condition_idx = condition_start; condition_idx <= condition_.size() - group_sizes_[group_idx]; ++condition_idx) {
				const auto group_start = condition_.begin() + static_cast<ssize_t>(condition_idx);
				if (std::all_of(group_start, group_start + static_cast<ssize_t>(group_sizes_[group_idx]),
								[](auto c) { return c == '#' || c == '?'; }))
					count += num_arrangements(condition_idx + group_sizes_[group_idx] + 1, group_idx + 1, cache);
				if (condition_[condition_idx] == '#')
					break;
			}
			cache_it = cache.emplace(key, count).first;
		}
		return cache_it->second;
	}
};

int main(int argc, char *argv[]) {
	const auto part = select_part(argc, argv);
	auto records = read_as<Record>(std::cin);
	if (part == 2) {
		for (auto &record : records)
			record.unfold(5);
	}
	std::cout << std::accumulate(records.begin(), records.end(), std::size_t{0},
								 [](auto sum, const auto &record) { return sum + record.num_arrangements(); }) << std::endl;
	return 0;
}