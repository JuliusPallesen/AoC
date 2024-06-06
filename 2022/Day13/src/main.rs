use itertools::{EitherOrBoth::*, Itertools};
use std::time::Instant;
use std::{
    cmp::Ordering::*,
    env,
    fs::File,
    io::{Error, Read},
};

#[derive(Debug, PartialEq, Eq, Clone)]
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
            (Self::Num(a), other) => Self::List(vec![Self::Num(*a)]).partial_cmp(other),
            (other, Self::Num(a)) => other.partial_cmp(&&Self::List(vec![Self::Num(*a)])),
            (Self::List(lhs), IList::List(rhs)) => {
                lhs.iter()
                    .zip_longest(rhs.iter())
                    .find_map(|pair| match pair {
                        Both(a, b) => a.partial_cmp(b).filter(|&ord| ord != Equal),
                        Left(_) => Some(Greater),
                        Right(_) => Some(Less),
                    })
            }
        }
    }
}

impl Ord for IList {
    fn cmp(&self, other: &Self) -> std::cmp::Ordering {
        self.partial_cmp(other).unwrap_or(Equal)
    }
}

fn part1(ilists: &Vec<IList>) -> () {
    let ans1: usize = ilists
        .chunks_exact(2) // group in pairs of 2
        .enumerate()
        .map(|(i, w)| (w[0] < w[1]) as usize * (i + 1)) // check if ILists are in order, if so: Sum up index (Branchless: index * 0 or 1 (bool cast to int))
        .sum();

    println!("[part 1]\tanswer: {ans1}");
}

fn part2(ilists: &mut Vec<IList>) {
    // part2: insert clones of both dividers, sort the entire list vector and multiple their indices +1
    let div1 = IList::List(vec![IList::List(vec![IList::Num(2)])]);
    let div2 = IList::List(vec![IList::List(vec![IList::Num(6)])]);
    ilists.push(div1.clone());
    ilists.push(div2.clone());
    ilists.sort();

    let ans2 =
        (ilists.binary_search(&div1).unwrap() + 1) * (ilists.binary_search(&div2).unwrap() + 1);

    println!("[part 2]\tanswer: {ans2}");
}

fn main() -> Result<(), Error> {
    let start_time = Instant::now();
    let args: Vec<String> = env::args().collect();
    let mut contents: String = String::new();

    let _ = File::open(args.get(1).expect("Please provide a valid file path"))
        .expect("Couldn't open file")
        .read_to_string(&mut contents)?;

    let mut ilists: Vec<IList> = contents
        .lines()
        .filter_map(|line| IList::new(line)) // create ILists and filter out empty
        .collect();

    println!("[parsing lists]\ttime elapsed: {:?}", start_time.elapsed());

    part1(&ilists);
    println!("[part 1]\ttime elapsed: {:?}", start_time.elapsed());

    part2(&mut ilists);
    println!("[part 2]\ttime elapsed: {:?}", start_time.elapsed());
    Ok(())
}
