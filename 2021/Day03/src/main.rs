use std::{env, fs::File, io::Read};

fn is_one_at(num: &i32, index: usize) -> bool {
    (num.to_owned() & (1 << index)) != 0
}

fn count_ones_at(nums: &[i32], bit_index: usize) -> usize {
    nums.iter().filter(|x| is_one_at(x, bit_index)).count()
}

fn try_calc_and_reduce_options(
    nums: &mut Vec<i32>,
    index: usize,
    keep_ones_if: impl Fn(f32, f32) -> bool,
) -> Option<i32> {
    let ones = count_ones_at(&nums, index) as f32;
    let half = nums.len() as f32 / 2f32;
    match ones {
        x if keep_ones_if(x, half) => nums.retain(|num| is_one_at(num, index)),
        _ => nums.retain(|num| !is_one_at(num, index)),
    }
    if nums.len() == 1 {
        return Some(nums[0].to_owned());
    }
    None
}

fn main() {
    let filepath = env::args()
        .nth(1)
        .expect("Please provide a valid file path");
    let mut contents = String::new();
    File::open(&filepath)
        .expect("Couldn't open file")
        .read_to_string(&mut contents)
        .expect("Couldn't read file");

    let width = contents.find('\n').expect("Error parsing the file.") - 1;
    let height = contents.lines().count();
    println!("width: {width} height {height}");

    let indices = 0..width;
    let gamma = indices
        .into_iter()
        .map(|i| {
            contents
                .lines()
                .filter(|line| '1' != line.chars().nth(i).unwrap())
                .count()
                < height / 2
        })
        .fold(0, |acc, x| x as i32 + (acc << 1));
    let mask = 2i32.pow(width.try_into().unwrap()) - 1;

    // calculate epsilon by getting the inverse bit representation of gamma
    let ans1 = gamma * (mask ^ gamma);
    println!("Part1: {ans1}");

    let mut g_values: Vec<i32> = contents
        .lines()
        .map(|line| {
            line.chars()
                .fold(0, |acc, c| (acc << 1) + (c == '1') as i32)
        })
        .collect();

    let mut s_values = g_values.clone();
    let mut generator: i32 = 0;
    let mut scrubber: i32 = 0;

    for i in 0..width {
        let bit_index = width - i - 1;
        generator =
            try_calc_and_reduce_options(&mut g_values, bit_index, |ones, half| ones >= half)
                .unwrap_or(generator);
        scrubber = try_calc_and_reduce_options(&mut s_values, bit_index, |ones, half| ones < half)
            .unwrap_or(scrubber);
    }
    println!("part 2 :{:?}", generator * scrubber);
}
