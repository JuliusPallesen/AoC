use std::{env, fs::File, io::Read};

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
}
