use std::fmt;
use std::{cell::RefCell, cmp::Ordering::*, env, fs::File, io::Read};

#[derive(Debug, PartialEq)]
enum IList {
    List(Vec<RefCell<IList>>),
    Integer(u32),
}

impl IList {
    fn parse_num(curr: Option<u32>, val: u32) -> Option<u32> {
        match curr {
            Some(x) => Some(x * 10 + val),
            _ => Some(val),
        }
    }

    pub fn new(line: &str) -> Option<Self> {
        if line.len() < 2 {
            return None;
        }
        let line = &line[1..line.len() - 1]; // Couldn't get it to work without pushing an empty list to the stack first, so we need to trim 1
        let mut stack: Vec<RefCell<IList>> = Vec::new();
        let mut current_list = RefCell::new(IList::List(Vec::new()));
        let mut num: Option<u32> = None;
        for token in line.chars() {
            if token.is_digit(10) {
                num = IList::parse_num(num, token.to_digit(10).unwrap());
            } else {
                if let Some(val) = num {
                    if let IList::List(list) = current_list.get_mut() {
                        list.push(RefCell::new(IList::Integer(val)));
                    }
                    num = None;
                }
            }
            match token {
                '[' => {
                    let new_list = RefCell::new(IList::List(Vec::new()));
                    stack.push(current_list);
                    current_list = new_list;
                }
                ']' => {
                    let completed_list = current_list;
                    current_list = stack.pop().unwrap();
                    if let IList::List(ref mut vec) = *current_list.borrow_mut() {
                        vec.push(completed_list);
                    }
                }
                _ => (),
            }
        }
        if let Some(val) = num {
            if let IList::List(list) = current_list.get_mut() {
                list.push(RefCell::new(IList::Integer(val)));
            }
        }
        Some(current_list.into_inner())
    }
}

impl fmt::Display for IList {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            IList::List(vec) => {
                write!(f, "[")?;
                for (i, cell) in vec.iter().enumerate() {
                    if i > 0 {
                        write!(f, ",")?;
                    }
                    write!(f, "{}", cell.borrow())?;
                }
                write!(f, "]")
            }
            IList::Integer(val) => write!(f, "{}", val),
        }
    }
}

impl PartialOrd for IList {
    fn partial_cmp(&self, other: &Self) -> Option<std::cmp::Ordering> {
        match (&self, &other) {
            (Self::Integer(lhs), IList::Integer(rhs)) => lhs.partial_cmp(rhs),
            (Self::List(lhs), IList::List(rhs)) => {
                let mut a = lhs.iter();
                let mut b = rhs.iter();
                loop {
                    match (a.next(), b.next()) {
                        (Some(a), Some(b)) => {
                            if let Some(ordering) = a.partial_cmp(b) {
                                if ordering != Equal {
                                    return Some(ordering);
                                }
                            }
                        }
                        (Some(_), None) => return Some(Greater),
                        (None, Some(_)) => return Some(Less),
                        (None, None) => return Some(Equal),
                    }
                }
            }
            (Self::Integer(a), other) => {
                Self::List(vec![Self::Integer(*a).into()]).partial_cmp(other)
            }
            (other, Self::Integer(a)) => {
                let new = Self::List(vec![Self::Integer(*a).into()]);
                other.partial_cmp(&&new)
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
        .filter_map(|line| IList::new(line))
        .collect();

    let ans1: usize = ilists
        .chunks_exact(2)
        .enumerate()
        .map(|(i, w)| {
            match w[0].partial_cmp(&w[1]) {
                Some(Equal) => print!("{i}: {} == {}", w[0], w[1]),
                Some(Greater) => print!("{i}: {} > {}", w[0], w[1]),
                Some(Less) => print!("{i}: {} < {}", w[0], w[1]),
                None => print!("{i}: COULDN'T MATCH! {} {}", w[0], w[1]),
            }
            let ret = (w[0] < w[1]) as usize * (i + 1);
            println!(" ==> {ret}");
            ret
        })
        .sum();
    println!("part1: {ans1}");
}
