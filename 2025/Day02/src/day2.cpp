#include <concepts>
#include <cstdint>
#include <string>
#include <fstream>
#include <format>
#include <algorithm>
#include <iostream>
#include <ranges>
#include "../include/aocutils.h"

template <typename T>
concept UnsignedInteger = std::is_integral_v<T> && std::is_unsigned_v<T>;

template <UnsignedInteger T>
[[nodiscard]] constexpr auto ipow(T num, T exp) noexcept -> T
{
    return std::ranges::fold_left(std::views::repeat(num, exp), T{1}, std::multiplies{});
}

template <UnsignedInteger T>
[[nodiscard]] constexpr auto digit_count(T num, T base = T{10}) noexcept -> T
{
    auto iter = std::views::iota(0) | std::views::take_while([&](auto)
                                                             { return (num /= base) > 0; });
    return T{1} + static_cast<T>(std::ranges::distance(iter));
}

template <UnsignedInteger T>
[[nodiscard]] constexpr auto to_digits(T num, T base = T{10}) noexcept
{
    const auto count = digit_count(num, base);
    return std::views::iota(T{0}, count) | std::views::transform([num, base](auto exp)
                                                                 { return (num / ipow(base, exp)) % base; });
}

template <UnsignedInteger T>
class IdRange
{
public:
    constexpr IdRange(const std::string_view &range) noexcept
    {
        const auto parse_int = [](const std::string_view &s)
        {
            return std::ranges::fold_left(s, T{0}, [](auto acc, char c)
                                          { return acc * T{10} + static_cast<T>(c - '0'); });
        };
        const auto separator = range.find('-');
        from = parse_int(range.substr(0, separator));
        to = parse_int(range.substr(separator + 1));
    }

    [[nodiscard]] constexpr auto part1_sum() const noexcept -> T { return id_sum(valid_part1); }
    [[nodiscard]] constexpr auto part2_sum() const noexcept -> T { return id_sum(valid_part2); }

private:
    template <std::invocable<T> F>
    [[nodiscard]] constexpr auto id_sum(F f) const noexcept -> T
    {
        auto ids = std::views::iota(from, to + 1) | std::views::filter(f);
        return std::ranges::fold_left(ids, T{0}, std::plus{});
    }

    [[nodiscard]] constexpr static auto valid_part1(T num) noexcept -> bool
    {
        const auto digits = digit_count(num);
        if (digits % 2 == 1)
            return false;
        // https://www.youtube.com/watch?v=6JrFu0xj0bs
        const auto sandwich = ipow(T{10}, digits / 2) + 1;
        return (num % sandwich) == 0;
    }

    [[nodiscard]] constexpr static auto valid_part2(T num) noexcept -> bool
    {
        const auto digits = to_digits(num);
        const auto count = static_cast<std::size_t>(std::ranges::distance(digits));
        const auto middle = count / 2;

        auto chunks = std::views::iota(std::size_t{1}, middle + 1) | std::views::filter([&count](const auto chunk_size)
                                                                                        { return count % chunk_size == 0; });
        return std::ranges::any_of(chunks, [&digits](const auto i)
                                   { return has_equal_chunks(digits, i); });
    }

    [[nodiscard]] constexpr static auto has_equal_chunks(const auto &digits, std::size_t chunk_size) noexcept -> bool
    {
        auto chunks = digits | std::views::chunk(chunk_size);
        return std::ranges::all_of(chunks, [first_chunk = *chunks.begin()](const auto &chunk)
                                   { return std::ranges::equal(chunk, first_chunk); });
    }

    T from;
    T to;
};

template <UnsignedInteger T, std::invocable<IdRange<T>> F>
[[nodiscard]] constexpr auto solve_part(const std::string_view &input, F f) noexcept -> T
{
    auto sums = std::views::split(input, ',') | std::views::transform([&](auto str)
                                                                      { return f(IdRange<T>{std::string_view{str.begin(), str.end()}}); });
    return std::ranges::fold_left(sums, T{0}, std::plus{});
}

template <UnsignedInteger T>
[[nodiscard]] constexpr auto part1(const std::string_view &input) noexcept -> T
{
    return solve_part<T>(input, [](const auto &r)
                         { return r.part1_sum(); });
}

template <UnsignedInteger T>
[[nodiscard]] constexpr auto part2(const std::string_view &input) noexcept -> T
{
    return solve_part<T>(input, [](const auto &r)
                         { return r.part2_sum(); });
}

constexpr const static std::string_view example{"11-22,95-115,998-1012,1188511880-1188511890,222220-222224,1698522-1698528,446443-446449,38593856-38593862,565653-565659,824824821-824824827,2121212118-2121212124"};
static_assert(part1<std::uint64_t>(example) == std::uint64_t{1227775554});
static_assert(part2<std::uint64_t>(example) == std::uint64_t{4174379265});

auto solve(int argc, char const *argv[]) -> void
{
    const std::string input_path{argc <= 1 ? "../input.txt" : argv[1]};
    std::ifstream input_file(input_path);
    std::string content((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
    std::cout << std::format("part 1: {}\n", part1<std::uint64_t>(content));
    std::cout << std::format("part 2: {}\n", part2<std::uint64_t>(content));
}

int main(int argc, char const *argv[])
{
    const aoc::Timer t{};
    try
    {
        solve(argc, argv);
    }
    catch (const std::exception &e)
    {
        std::cerr << std::format("{}\n", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
