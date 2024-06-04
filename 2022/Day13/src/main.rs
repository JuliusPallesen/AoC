use std::{
    cell::RefCell, cmp::Ordering::*, env, fs::File, io::Read, rc::Rc
};

#[derive(Debug, PartialEq)]
enum IList {
    List(Vec<Rc<RefCell<IList>>>),
    Integer(i32),
}

impl IList {
    fn new(line: &str) -> Option<Self> {
        let mut stack: Vec<Rc<RefCell<IList>>> = Vec::new();
        let mut current_list = Rc::new(RefCell::new(IList::List(Vec::new())));

        let mut num_buffer = String::new();

        for token in line.chars() {
            match token {
                '[' => {
                    let new_list = Rc::new(RefCell::new(IList::List(Vec::new())));
                    stack.push(Rc::clone(&current_list));
                    current_list = new_list;
                }
                ']' => {
                    if !num_buffer.is_empty() {
                        let num = num_buffer.parse::<i32>().unwrap();
                        if let IList::List(ref mut vec) = *current_list.borrow_mut() {
                            vec.push(Rc::new(RefCell::new(IList::Integer(num))));
                        }
                        num_buffer.clear();
                    }
                    let completed_list = Rc::clone(&current_list);
                    current_list = stack.pop().unwrap();
                    if let IList::List(ref mut vec) = *current_list.borrow_mut() {
                        vec.push(completed_list);
                    }
                }
                ',' => {
                    if !num_buffer.is_empty() {
                        let num = num_buffer.parse::<i32>().unwrap();
                        if let IList::List(ref mut vec) = *current_list.borrow_mut() {
                            vec.push(Rc::new(RefCell::new(IList::Integer(num))));
                        }
                        num_buffer.clear();
                    }
                }
                digit if digit.is_digit(10) => {
                    num_buffer.push(digit);
                }
                _ => (),
            }
        }
        Some(Rc::try_unwrap(current_list).unwrap().into_inner())
    }
}

impl PartialOrd for IList {
    fn partial_cmp(&self, other: &Self) -> Option<std::cmp::Ordering> {
        match (&self,&other) {
            (Self::Integer(lhs), IList::Integer(rhs)) => lhs.partial_cmp(rhs),
            (Self::List(lhs), IList::List(rhs))=> {
                
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
            } ,
            (Self::Integer(a), other) 
                => Self::List(vec![Rc::new(Self::Integer(*a).into())]).partial_cmp(other),
            (b,a) => a.partial_cmp(b),
        }
    }
    
    fn lt(&self, other: &Self) -> bool {
        matches!(self.partial_cmp(other), Some(Less))
    }
    
    fn le(&self, other: &Self) -> bool {
        matches!(self.partial_cmp(other), Some(Less | Equal))
    }
    
    fn gt(&self, other: &Self) -> bool {
        matches!(self.partial_cmp(other), Some(Greater))
    }
    
    fn ge(&self, other: &Self) -> bool {
        matches!(self.partial_cmp(other), Some(Greater | Equal))
    }
}

fn main() {
    println!("Hello world?");
    let args: Vec<String> = env::args().collect();
    let mut contents: String = String::new();
    let _ = File::open(args.get(1).expect("Please provide a valid file path"))
        .expect("Couldn't open file")
        .read_to_string(&mut contents);
    contents
        .lines().map(|line| IList::new(line)).for_each(|l| println!("{:?}",l));
}
