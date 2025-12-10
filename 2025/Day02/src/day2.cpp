#include "../include/aocutils.h"
#include <cassert>
#include <cmath>
#include <concepts>

class IdRange
{
public:
    constexpr IdRange(u_int64_t from, u_int64_t to) : from(from), to(to) {}
    constexpr IdRange(std::string_view range)
    {
        const auto seperator = range.find('-');
        std::from_chars(range.begin(), range.begin() + seperator, from);
        std::from_chars(range.begin() + seperator + 1, range.end(), to);
    }
    ~IdRange() = default;
    IdRange(const IdRange &other) = default;
    IdRange &operator=(const IdRange &other) = default;
    IdRange(IdRange &&other) noexcept = default;

    [[nodiscard]] constexpr auto possible_id_sum_part1() const -> u_int64_t
    {
        return possible_id_sum(is_base_10_repetition);
    }

    [[nodiscard]] constexpr auto possible_id_sum_part2() const -> u_int64_t
    {
        return possible_id_sum(all_base_10_repetitions);
    }

    u_int64_t from;
    u_int64_t to;

private:
    template <std::invocable<u_int64_t> F>
    [[nodiscard]] constexpr auto possible_id_sum(F f) const -> u_int64_t
    {
        auto ids = std::views::iota(from, to) | std::views::filter(f) |
                   std::views::common;
        return std::accumulate(ids.begin(), ids.end(), u_int64_t{});
    }

    [[nodiscard]] constexpr static auto base_10_digits(u_int64_t num) -> u_int64_t
    {
        // log10 not constexpr :(
        return static_cast<u_int64_t>(std::log10(num)) + 1;
    }

    [[nodiscard]] constexpr static auto is_base_10_repetition(u_int64_t num) -> bool
    {
        const auto digits = base_10_digits(num);

        if (digits % 2 == 1)
            return false;

        // Create a "Zero sandwich" => 1001 =>  1234 * 1234 = 12341234 => valid ID!
        const auto sandwich = static_cast<uint64_t>(std::pow(10, digits / 2)) + 1; // pow not constexpr :(
        return (num % sandwich) == 0;
    }

    [[nodiscard]] constexpr static auto all_base_10_repetitions(u_int64_t num) -> bool
    {
        const auto digits = base_10_digits(num);
        const auto num_string = std::to_string(num); // to_string not constexpr :(
        const auto middle = digits / 2;

        for (size_t i = 1; i <= middle; ++i)
        {
            if (has_equal_chunks(num_string, i))
                return true;
        }
        return false;
    }

    [[nodiscard]] constexpr static auto has_equal_chunks(std::string num, size_t window_size) -> bool
    {
        {
            if (num.size() % window_size != 0)
                return false;

            auto chunks = num | std::views::chunk(window_size);
            auto first_chunk = *chunks.begin();

            return std::all_of(chunks.begin(), chunks.end(), [&](auto chunk)
                               { return std::ranges::equal(chunk, first_chunk); });
        }
    }
};

std::ostream &operator<<(std::ostream &os, const IdRange &obj)
{
    os << '[' << obj.from << '-' << obj.to << ']';
    return os;
}

auto solve(int argc, char const *argv[]) -> void
{
    const std::string input_path{argc <= 1 ? "../input.txt" : argv[1]};
    std::ifstream input_file(input_path);
    std::string content((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
    auto strings = std::views::split(content, ',');

    u_int64_t part1{};
    u_int64_t part2{};

    for (auto s : strings)
    {
        const auto view = std::string_view(&*s.begin(), std::ranges::distance(s));
        const IdRange range{view};
        part1 += range.possible_id_sum_part1();
        part2 += range.possible_id_sum_part2();
    }

    std::cout << std::format("part 1: {}\n", part1);
    std::cout << std::format("part 2: {}\n", part2);
}

int main(int argc, char const *argv[])
{
    try
    {
        solve(argc, argv);
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << std::format("{}\n", e.what());
    }

    return EXIT_SUCCESS;
}
