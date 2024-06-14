use std::{
    env,
    fs::File,
    io::{Error, Read},
};

#[derive(Debug)]
struct BingoCard {
    done: bool,
    numbers: [[Option<i32>; 5]; 5],
}

type Point = (usize, usize);
type OptVec = Vec<Option<i32>>;
type OptVec2D = Vec<OptVec>;

impl BingoCard {
    fn new(input: OptVec2D) -> Self {
        Self {
            done: false,
            numbers: input
                .into_iter()
                .map(|v| <[Option<i32>; 5]>::try_from(v).unwrap())
                .collect::<Vec<_>>()
                .try_into()
                .unwrap(),
        }
    }

    fn find(&self, number: i32) -> Option<Point> {
        for (y, row) in self.numbers.iter().enumerate() {
            for (x, val) in row.iter().enumerate() {
                if *val == Some(number) {
                    return Some((y, x));
                }
            }
        }
        None
    }

    fn row_done(&self, y: usize) -> bool {
        self.numbers
            .get(y)
            .expect("Row index out of bounds")
            .iter()
            .filter(|x| x.is_some())
            .count()
            == 0
    }

    fn column_done(&self, x: usize) -> bool {
        self.numbers
            .iter()
            .filter(|line| line.get(x).unwrap().is_some())
            .count()
            == 0
    }

    fn is_done(&mut self, pos: Point) -> bool {
        let (y, x) = pos;
        self.done = self.column_done(x) || self.row_done(y);
        self.done
    }

    fn accept(&mut self, number: i32) -> Option<i32> {
        self.find(number).and_then(|(y, x)| {
            self.numbers[y][x] = None;
            match self.is_done((y, x)) {
                true => Some(self.get_score()),
                false => None,
            }
        })
    }

    fn get_score(&self) -> i32 {
        self.numbers.iter().fold(0, |acc, row| {
            acc + row.iter().fold(0, |acc, num| acc + num.unwrap_or(0))
        })
    }
}

fn parse_numbers(input: &[String]) -> Vec<i32> {
    input
        .iter()
        .nth(0)
        .unwrap()
        .split(',')
        .map(|s| s.trim().parse::<i32>().unwrap())
        .collect::<Vec<i32>>()
}

fn parse_bingo_cards(input: &[String]) -> Vec<BingoCard> {
    input
        .split(|s| s.is_empty())
        .map(|card| {
            let numbers = get_numbers(card);
            BingoCard::new(numbers)
        })
        .collect::<Vec<BingoCard>>()
}

fn get_numbers(card: &[String]) -> OptVec2D {
    card.iter()
        .map(|line| parse_ints(line))
        .collect::<OptVec2D>()
}

fn parse_ints(line: &String) -> OptVec {
    line.split_whitespace()
        .map(|s| Some(s.trim().parse::<i32>().unwrap()))
        .collect::<OptVec>()
}

fn parse_file() -> Vec<String> {
    let mut contents = String::new();

    let filepath = env::args()
        .nth(1)
        .expect("Please provide a valid file path");

    File::open(&filepath)
        .expect("Couldn't open file")
        .read_to_string(&mut contents)
        .expect("Couldn't read file");

    contents
        .lines()
        .map(|line| line.to_string())
        .collect::<Vec<String>>()
}

fn main() -> Result<(), Error> {
    let lines = parse_file();
    let nums = parse_numbers(&lines[..1]);
    let mut bingo_cards = parse_bingo_cards(&lines[2..]);

    let mut part2 = 0;
    for num in nums {
        bingo_cards
            .iter_mut()
            .filter_map(|card| card.accept(num))
            .into_iter()
            .for_each(|score| part2 = score * num);

        bingo_cards.retain(|card| !card.done);
    }

    println!("part 2: {:?}", part2);

    Ok(())
}
