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
        self.numbers.iter().enumerate().find_map(|(y, row)| {
            row.iter()
                .enumerate()
                .find_map(|(x, &val)| (val == Some(number)).then_some((y, x)))
        })
    }

    fn row_done(&self, y: usize) -> bool {
        self.numbers[y].iter().all(Option::is_none)
    }

    fn column_done(&self, x: usize) -> bool {
        self.numbers.iter().all(|&row| row[x].is_none())
    }

    fn is_done(&mut self, pos: Point) -> bool {
        let (y, x) = pos;
        self.done = self.column_done(x) || self.row_done(y);
        self.done
    }

    fn accept(&mut self, number: i32) -> Option<i32> {
        self.find(number).and_then(|(y, x)| {
            self.numbers[y][x] = None;
            self.is_done((y, x)).then_some(self.get_score())
        })
    }

    fn get_score(&self) -> i32 {
        self.numbers.iter().flatten().filter_map(|&id| id).sum()
    }
}

fn parse_int(s: &str) -> i32 {
    s.trim().parse().unwrap()
}

fn parse_numbers(input: &[String]) -> Vec<i32> {
    input[0].split(',').map(parse_int).collect()
}

fn parse_bingo_cards(input: &[String]) -> Vec<BingoCard> {
    input
        .split(String::is_empty)
        .map(get_numbers)
        .map(BingoCard::new)
        .collect()
}

fn get_numbers(card: &[String]) -> OptVec2D {
    card.iter().map(parse_ints).collect()
}

fn parse_ints(line: &String) -> OptVec {
    line.split_whitespace()
        .map(|s| Some(s.trim().parse::<i32>().unwrap()))
        .collect()
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
        .map(str::to_string)
        .collect::<Vec<String>>()
}

fn main() -> Result<(), Error> {
    let lines = parse_file();
    let nums = parse_numbers(&lines[..1]);
    let mut bingo_cards: Vec<BingoCard> = parse_bingo_cards(&lines[2..]);

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
