use std::env;
use std::fs;

fn count_increases(vec: &Vec<i32>) -> usize {
    vec.windows(2)
        .map(|p| p[1] - p[0])
        .filter(|&d| d > 0)
        .count()
}

fn main() {
    let args: Vec<String> = env::args().collect();
    let file_path = args.get(1).expect("Please provide a filepath");

    let ints: Vec<i32> = fs::read_to_string(file_path)
        .unwrap_or_else(|err| panic!("Error reading file: {}", err))
        .split_whitespace()
        .into_iter()
        .map(|s| s.trim().parse::<i32>().unwrap())
        .collect();

    let ans1 = count_increases(&ints);
    let ans2 = count_increases(&ints.windows(3).map(|w| w[0] + w[1] + w[2]).collect());

    println!("AoC 2021 part 1: {}", ans1);
    println!("AoC 2021 part 1: {}", ans2);
}
