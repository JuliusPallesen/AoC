#include <cassert>
#include <cmath>
#include <concepts>
#include <cstdint>
#include <vector>
#include <string>
#include <fstream>
#include <format>
#include <algorithm>
#include <iostream>
#include <ranges>
#include <concepts>
#include <queue>
#include <climits>

// TODO noexcept ? template ?
[[nodiscard]] constexpr auto ipow(std::uint64_t num, std::uint64_t exp) -> std::uint64_t
{
    return std::ranges::fold_left(std::views::iota(0ull, exp), 1ull, [&](auto acc, auto)
                                  { return acc * num; });
}

[[nodiscard]] constexpr auto digit_count(std::uint64_t num, std::uint64_t base = 10) -> std::uint64_t
{
    auto iter = std::views::iota(0ull) | std::views::take_while([&](auto)
                                                                { return (num /= base) > 0; });
    return 1 + std::ranges::distance(iter);
}

[[nodiscard]] constexpr auto to_digits(std::uint64_t n, std::uint64_t base = 10)
{
    return std::views::iota(0ull, digit_count(n, base)) | std::views::transform([=](auto exp)
                                                                                { return (n / ipow(base, exp)) % base; });
}

class IdRange
{
public:
    constexpr IdRange(std::uint64_t from, std::uint64_t to) : from(from), to(to) {}
    constexpr IdRange(std::string_view range)
    {
        const auto separator = range.find('-');
        auto parse = [](std::string_view s)
        {
            return std::ranges::fold_left(s, 0ull, [](auto acc, char c)
                                          { return acc * 10 + (c - '0'); });
        };
        from = parse(range.substr(0, separator));
        to = parse(range.substr(separator + 1));
    }
    ~IdRange() = default;
    IdRange(const IdRange &other) = default;
    IdRange &operator=(const IdRange &other) = default;
    IdRange(IdRange &&other) noexcept = default;

    [[nodiscard]] constexpr auto part1_sum() const -> std::uint64_t
    {
        return id_sum(valid_part1);
    }

    [[nodiscard]] constexpr auto part2_sum() const -> std::uint64_t
    {
        return id_sum(valid_part2);
    }

    std::uint64_t from;
    std::uint64_t to;

private:
    template <std::invocable<std::uint64_t> F>
    [[nodiscard]] constexpr auto id_sum(F f) const -> std::uint64_t
    {
        auto ids = std::views::iota(from, to + 1) | std::views::filter(f);
        return std::ranges::fold_left(ids.begin(), ids.end(), std::uint64_t{}, [](auto acc, auto id)
                                      { return acc + id; });
    }

    [[nodiscard]] constexpr static auto valid_part1(std::uint64_t num) -> bool
    {
        const auto digits = digit_count(num);

        if (digits % 2 == 1)
            return false;

        // Create a "Zero sandwich" => 1001 =>  1234 * 1234 = 12341234 => valid ID!
        const auto sandwich = ipow(10, digits / 2) + 1;
        return (num % sandwich) == 0;
    }

    [[nodiscard]] constexpr static auto valid_part2(std::uint64_t num) -> bool
    {
        const auto middle = digit_count(num) / 2;

        for (const auto &i : std::views::iota(1ull, middle + 1))
            if (has_equal_chunks(to_digits(num), i))
                return true;

        return false;
    }

    [[nodiscard]] constexpr static auto has_equal_chunks(auto num, size_t window_size) -> bool
    {
        auto chunks = num | std::views::chunk(window_size);
        auto first_chunk = *chunks.begin();

        return std::ranges::all_of(chunks, [first_chunk](auto chunk)
                                   { return std::ranges::equal(chunk, first_chunk); });
    }
};

[[nodiscard]] constexpr auto part1(std::string_view input) -> std::uint64_t
{
    auto strings = std::views::split(input, ',');
    return std::ranges::fold_left(strings, 0ull, [](const auto acc, auto str)
                                  {
            const auto view = std::string_view{str.begin(), str.end()};
            const IdRange range{view}; 
            return acc + range.part1_sum(); });
}

[[nodiscard]] constexpr auto part2(std::string_view input) -> std::uint64_t
{
    auto strings = std::views::split(input, ',');
    return std::ranges::fold_left(strings, 0ull, [](auto acc, auto str)
                                  {
            const auto view = std::string_view{str.begin(), str.end()};
            const IdRange range{view}; 
            return acc + range.part2_sum(); });
}

constexpr const static std::string_view example{"11-22,95-115,998-1012,1188511880-1188511890,222220-222224,1698522-1698528,446443-446449,38593856-38593862,565653-565659,824824821-824824827,2121212118-2121212124"};
static_assert(part1(example) == 1227775554);
static_assert(part2(example) == 4174379265);

auto solve(int argc, char const *argv[]) -> void
{
    const std::string input_path{argc <= 1 ? "../input.txt" : argv[1]};
    std::ifstream input_file(input_path);
    std::string content((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
    std::cout << std::format("part 1: {}\n", part1(content));
    std::cout << std::format("part 2: {}\n", part2(content));
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
