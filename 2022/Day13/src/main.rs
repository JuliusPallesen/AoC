use itertools::{EitherOrBoth::*, Itertools};
use std::{
    cmp::Ordering::*,
    env,
    fs::File,
    io::{Error, Read},
    time::Instant,
};

/*
A Recursive Integer List
*/
#[derive(Debug, PartialEq, Eq, Clone)]
enum IList {
    List(Vec<IList>),
    Num(u32),
}

impl IList {
    /*
    Parses an input str into a Recursive Integer List
    Uses a stack and move semantics to insert inner elements into outer ones

    Example:
    input [1,[2]]
               ^
               | at this position
    stack: [[1],[2]] ==> pop [2] and insert it into [1] (which is the new top of the stack)
    stack: [[1,[2]]]

    input [1,[2]]
                ^
                | at this position
    stack: [[1,[2]]] ==> pop [1,[2]]. Since the stack is now empty we can return
    */
    pub fn new(line: &str) -> Option<Self> {
        let mut stack: Vec<Vec<IList>> = Vec::new();
        let mut num: Option<u32> = None;
        for ch in line.chars() {
            match ch {
                '[' => stack.push(Vec::new()),
                ']' | ',' => {
                    if let Some(val) = num {
                        // push the last read number to the top of the stack
                        stack.last_mut().unwrap().push(IList::Num(val));
                        num = None;
                    }
                    if ch == ']' {
                        let top = stack.pop().unwrap();
                        if let Some(last) = stack.last_mut() {
                            // Turn the top of the stack into IList and insert it into the 2nd element of the stack
                            last.push(IList::List(top));
                        } else {
                            // closing bracket with an empty stack means we're done
                            return Some(IList::List(top));
                        }
                    }
                }
                // parse base-10 ints char by char left to right
                x if x.is_digit(10) => num = Some(num.unwrap_or(0) * 10 + ch.to_digit(10).unwrap()),
                _ => return None, // invalid input
            }
        }
        None
    }
}

impl PartialOrd for IList {
    /*
    Implements comparisons between ILists used to put them in order
     */
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
    /*
    Total Ord using PartialOrd
    */
    fn cmp(&self, other: &Self) -> std::cmp::Ordering {
        self.partial_cmp(other).unwrap_or(Equal)
    }
}

/*
Part 1 of AoC 2022 Day 13
Lists are paired into pairs of two, if they are in ascending order, we sum up their indices.
*/
fn part1(ilists: &Vec<IList>) -> () {
    let ans1: usize = ilists
        .chunks_exact(2) // group in pairs of 2
        .enumerate()
        // check if ILists are in order, if so: Sum up index (Branchless: index * 0 or 1 (bool cast to int))
        .map(|(i, w)| (w[0] < w[1]) as usize * (i + 1))
        .sum();

    println!("[part 1]\tanswer: {ans1}");
}

/*
Part 2 of AoC 2022 Day 13
We insert two "Divider Packets" into the List of ILists
After putting the entire List of ILists into ascending order, we find the divider packets
and multiply their indices
*/
fn part2(ilists: &mut Vec<IList>) {
    // initialize divider packets [[2]] and [[6]]
    let div1 = IList::List(vec![IList::List(vec![IList::Num(2)])]);
    let div2 = IList::List(vec![IList::List(vec![IList::Num(6)])]);

    // Clone the elements since we need them again to search for them
    ilists.push(div1.clone());
    ilists.push(div2.clone());
    ilists.sort();

    // Sorted Vector lets us use binary search
    let ans2 =
        (ilists.binary_search(&div1).unwrap() + 1) * (ilists.binary_search(&div2).unwrap() + 1);

    println!("[part 2]\tanswer: {ans2}");
}

fn main() -> Result<(), Error> {
    let start_time = Instant::now();
    let args: Vec<String> = env::args().collect();
    let mut contents: String = String::new();

    // Read input file
    let _ = File::open(args.get(1).expect("Please provide a valid file path"))
        .expect("Couldn't open file")
        .read_to_string(&mut contents)?;

    // Parse all lines (ignore empty since we know its every 2 lines anyways)
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
