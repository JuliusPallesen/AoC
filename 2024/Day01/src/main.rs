use std::fs::File;
use std::io::{BufRead, BufReader};

fn main() -> () {
    let file = File::open("file.txt").unwrap();
    let reader = BufReader::new(file);

    for line in reader.lines() {
        let line = line.unwrap();
        let numbers: Vec<i32> = line
            .split_whitespace()
            .map(|x| x.parse().unwrap())
            .collect();
        let (a, b) = (numbers[0], numbers[1]);
    }
}
