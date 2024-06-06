use itertools::{EitherOrBoth::*, Itertools};
use std::{cmp::Ordering::*, env, fs::File, io::Read};

#[derive(Debug, PartialEq)]
enum IList {
    List(Vec<IList>),
    Num(u32),
}

impl IList {
    pub fn new(line: &str) -> Option<Self> {
        let mut stack: Vec<Vec<IList>> = Vec::new();
        let mut num: Option<u32> = None;
        for ch in line.chars() {
            match ch {
                '[' => stack.push(Vec::new()),
                ']' | ',' => {
                    if let Some(val) = num {
                        stack.last_mut().unwrap().push(IList::Num(val));
                        num = None;
                    }
                    if ch == ']' {
                        let top = stack.pop().unwrap();
                        if let Some(last) = stack.last_mut() {
                            last.push(IList::List(top));
                        } else {
                            return Some(IList::List(top));
                        }
                    }
                }
                x if x.is_digit(10) => num = Some(num.unwrap_or(0) * 10 + ch.to_digit(10).unwrap()),
                _ => return None,
            }
        }
        None
    }
}

impl PartialOrd for IList {
    fn partial_cmp(&self, other: &Self) -> Option<std::cmp::Ordering> {
        match (&self, other) {
            (Self::Num(lhs), IList::Num(rhs)) => lhs.partial_cmp(rhs),
            (Self::Num(a), other) => Self::List(vec![Self::Num(*a).into()]).partial_cmp(other),
            (other, Self::Num(a)) => other.partial_cmp(&&Self::List(vec![Self::Num(*a)])),
            (Self::List(lhs), IList::List(rhs)) => {
                for pair in lhs.iter().zip_longest(rhs.iter()) {
                    // Compare Lists element by element or check the length
                    match pair {
                        Both(a, b) => {
                            if let Some(ordering) = a.partial_cmp(b) {
                                if ordering != Equal {
                                    return Some(ordering);
                                }
                            }
                        }
                        Left(_) => return Some(Greater),
                        Right(_) => return Some(Less),
                    }
                }
                None
            }
        }
    }
}

fn main() {
    let args: Vec<String> = env::args().collect();

    let mut contents: String = String::new();

    let _ = File::open(args.get(1).expect("Please provide a valid file path"))
        .expect("Couldn't open file")
        .read_to_string(&mut contents);

    let ilists: Vec<IList> = contents
        .lines()
        .filter_map(|line| IList::new(line)) // create ILists and filter out empty
        .collect();

    let ans1: usize = ilists
        .chunks_exact(2) // group in pairs of 2
        .enumerate()
        .map(|(i, w)| (w[0] < w[1]) as usize * (i + 1)) // check if ILists are in order, if so: Sum up index (index * 0 or 1 (bool to int))
        .sum();

    println!("part1: {ans1}");
}
