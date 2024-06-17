
#include <iostream>
#include <vector>
#include <string>
#include <format>
#include <sstream>
#include <map>
#include <stack>
#include <algorithm>

#include <windows.h>

enum colour
{
	dark_blue,
	dark_green,
	empty,
	light_blue,
	light_green,
	magenta,
	orange,
	pink,
	cream,
	yellow
};

std::ostream& operator<<(std::ostream& out, const colour colour)
{
	switch (colour)
	{
	case dark_blue:
	{
		out << "dark blue";
		break;
	}
	case dark_green:
	{
		out << "dark green";
		break;
	}
	case empty:
	{
		out << "empty";
		break;
	}
	case light_blue:
	{
		out << "light blue";
		break;
	}
	case light_green:
	{
		out << "light green";
		break;
	}
	case magenta:
	{
		out << "magenta";
		break;
	}
	case orange:
	{
		out << "orange";
		break;
	}
	case pink:
	{
		out << "pink";
		break;
	}
	case cream:
	{
		out << "cream";
		break;
	}
	case yellow:
	{
		out << "yellow";
		break;
	}
	}
	return out;
}

class coord
{
public:
	size_t tube_index {};
	coord(size_t tube_index) : tube_index {tube_index}
	{}
	coord()
	{}
};

class piece
{
public:
	piece(coord position, colour colour) : position {position}, colour {colour}
	{}
	coord position {};
	colour colour {empty};
	void exchange_colours_with(piece& other) { std::swap(this->colour, other.colour); }
};

class move
{
public:
	coord from;
	coord to;
	size_t move_size {};
	move(coord from, coord to, size_t move_size) : from {from}, to {to}, move_size {move_size}
	{}
	std::ostream& display(std::ostream& dest) const
	{
		dest << std::format("[{} -> {} : {}]", from.tube_index + 1, to.tube_index + 1, move_size); // they are 0 indexed, but it's easier to read them as 1 indexed.
		return dest;
	}
};

constexpr bool operator==(const move& lhs, const move& rhs)
{
	return lhs.from.tube_index == rhs.from.tube_index &&
		lhs.to.tube_index == rhs.to.tube_index &&
		lhs.move_size == rhs.move_size;
}

std::ostream& operator<<(std::ostream& out, const move& move)
{
	return move.display(out);
}

class solution
{
public:
	solution(std::vector<move> moves) : moves {moves}
	{}
	std::vector<move> moves;
	std::ostream& display(std::ostream& dest) const
	{
		dest << "{";
		for (int i {0}; i < moves.size(); i++)
		{
			dest << i + 1 << ": " << moves[i]; // easier to read them as 1 indexed
			if (i != moves.size() - 1)
			{
				dest << ", ";
			}
		}
		dest << "}" << std::endl;
		return dest;
	}
};

constexpr bool operator==(const solution& lhs, const solution& rhs)
{
	return lhs.moves == rhs.moves;
}

std::ostream& operator<<(std::ostream& out, const solution& solution)
{
	return solution.display(out);
}

class test_tube
{
public:
	std::vector<piece> contents; // index 0 is the bottom of the tube
	size_t tube_id {};
	std::ostream& display(std::ostream& dest) const
	{
		std::string tube {"{"};
		for (auto i {0}; i < contents.size(); ++i)
		{
			std::ostringstream ss;
			ss << contents[i].colour;
			tube += ss.str();
			if (i != contents.size() - 1)
			{
				tube += ", ";
			}
		}
		tube += "}";

		return dest << tube;
	}

	test_tube(size_t tube_id, std::vector<colour> colours) : tube_id {tube_id}
	{
		for (auto colour : colours)
		{
			contents.push_back({tube_id, colour});
		}
	}
	colour pouring_colour()
	{
		for (auto back_iter {contents.rbegin()}; back_iter != contents.rend(); back_iter++)
		{
			auto colour {back_iter->colour};
			if (colour == empty)
			{
				continue;
			}
			else
			{
				return colour;
			}
		}

		return empty;
	}
	bool is_empty() { return pouring_colour() == empty; }
	bool has_an_empty_space() { return contents.back().colour == empty; }
	bool can_pour_into(test_tube tube)
	{
		const bool has_space {tube.has_an_empty_space()};
		const bool is_same_colour {tube.pouring_colour() == this->pouring_colour()};
		const bool is_empty {tube.is_empty()};

		const bool has_space_for_my_colour {has_space && is_same_colour};

		return has_space_for_my_colour || is_empty;
	}
	piece& piece(size_t piece_id) { return contents[piece_id]; }
	size_t empty_spaces()
	{
		size_t empty_spaces {0};
		for (auto back_iter {contents.rbegin()}; back_iter != contents.rend(); back_iter++)
		{
			if (back_iter->colour == empty)
			{
				++empty_spaces;
			}
			else
			{
				break;
			}
		}
		return empty_spaces;
	}
	std::pair<colour, size_t> get_colour_and_depth()
	{
		// not interested in empty spots, only colours // maybe this could be used for empties too?
		colour colour {empty};
		size_t depth {};
		for (auto back_iter {contents.rbegin()}; back_iter != contents.rend(); back_iter++)
		{
			if (back_iter->colour == empty) // haven't hit a colour yet
			{
				continue;
			}
			else if (depth == 0) // hit the first colour
			{
				colour = back_iter->colour;
				depth++;
			}
			else if (back_iter->colour != colour) // hit a different colour
			{
				break;
			}
			else if (colour == back_iter->colour) // the next piece is the same colour
			{
				depth++;
			}
		}

		if (colour == empty)
		{
			throw std::runtime_error("you called the wrong function");
		}

		return {colour, depth};
	}
	move generate_move_to(test_tube destination)
	{
		auto source {*this};
		auto [source_colour, source_depth] {source.get_colour_and_depth()};
		auto destination_depth {destination.empty_spaces()};

		auto move_size {(std::min)(source_depth, destination_depth)};
		if (move_size == 0)
		{
			throw std::runtime_error("you did the wrong thing");
		}

		return {source.tube_id, destination.tube_id, move_size};
	}
	bool is_finished()
	{
		colour my_colour {contents.front().colour};
		for (const auto& piece : contents)
		{
			if (piece.colour != my_colour)
			{
				return false;
			}
		}
		return true;
	}
};

std::ostream& operator<<(std::ostream& out, const test_tube& tube)
{
	return tube.display(out);
}

class game_state
{
public:
	std::vector<move> possible_moves;
	bool moves_have_been_generated {false};
	std::vector<test_tube> test_tubes;
	game_state(std::vector<std::vector<colour>> tubes)
	{
		for (const auto& tube : tubes)
		{
			test_tubes.push_back({test_tubes.size(), tube});
			if (test_tubes.back().is_empty())
			{
				count_of_initial_empty_tubes++;
			}
		}
	}
	std::ostream& display(std::ostream& dest) const
	{
		dest << "{";
		for (auto i {0}; i < test_tubes.size(); ++i)
		{
			const auto& tube {test_tubes[i]};
			dest << tube;
			if (i != test_tubes.size() - 1)
			{
				dest << ", ";
			}
		}
		dest << "}";

		return dest;
	}
	bool recursively_examine_game_state(std::map<std::string, size_t>& examined_boards, std::vector<move>& solution, game_state state_to_examine);

	static std::vector<solution> work_out_all_solutions(game_state& given_state);
private:
	game_state(const std::vector<test_tube>& test_tubes, size_t count_of_initial_empty_tubes) :
		test_tubes {test_tubes}, count_of_initial_empty_tubes {count_of_initial_empty_tubes}
	{}
	bool is_finished {false};
	size_t count_of_initial_empty_tubes {0};
	std::vector<game_state> generate_possible_next_boards(std::vector<move> moves)
	{
		std::vector<game_state> next_boards;
		for (const auto& move : moves)
		{
			next_boards.push_back(generate_new_board_from_move(move));
		}
		return next_boards;
	}
	void generate_possible_moves()
	{
		if (moves_have_been_generated)
		{
			throw std::runtime_error("moves have already been generated");
		}

		int finished_tubes {0};
		for (auto& potential_source : test_tubes)
		{
			if (potential_source.is_finished())
			{
				finished_tubes++;
				if (finished_tubes == test_tubes.size() - count_of_initial_empty_tubes)
				{
					is_finished = true;
				}

				continue;
			}

			for (auto& potential_destination : test_tubes)
			{
				if (potential_source.tube_id == potential_destination.tube_id)
				{
					continue;
				}

				if (potential_source.can_pour_into(potential_destination))
				{
					move m {potential_source.generate_move_to(potential_destination)};
					possible_moves.push_back(m);
				}
			}
		}

		moves_have_been_generated = true;
	}
	game_state generate_new_board_from_move(move m)
	{
		game_state new_board {this->test_tubes, this->count_of_initial_empty_tubes};
		new_board.apply_move(m);
		return new_board;
	}
	void apply_move(move move)
	{
		auto& source {tube(move.from.tube_index)};
		auto& dest {tube(move.to.tube_index)};
		auto move_depth {move.move_size};

		auto source_iter {source.contents.rbegin()};
		auto dest_iter {dest.contents.begin()};
		for (size_t move_count {0}; move_count < move_depth; ++move_count)
		{
			// go down the source container and up the destination container (empty the source and fill the destination)
			while (dest_iter->colour != empty)
			{
				dest_iter++;
			}

			while (source_iter->colour == empty)
			{
				source_iter++;
			}

			source_iter->exchange_colours_with(*dest_iter);
			source_iter++;
			dest_iter++;
		}
	}
	test_tube& tube(size_t tube_id) { return test_tubes[tube_id]; }
};

std::ostream& operator<<(std::ostream& out, const game_state& state)
{
	return state.display(out);
}

bool game_state_has_already_been_examined(std::map<std::string, size_t>& examined_boards, game_state& game_state, size_t length_of_path_to_state)
{
	std::ostringstream oss;
	oss << game_state;
	auto key {examined_boards.find(oss.str())};
	if (key != examined_boards.end())
	{
		if (length_of_path_to_state < key->second)
		{
			key->second = length_of_path_to_state;
			return false;
		}
		else
		{
			return true;
		}
	}
	else
	{
		examined_boards[oss.str()] = length_of_path_to_state;
		return false;
	}
}

std::vector<solution> game_state::work_out_all_solutions(game_state& given_state)
{
	std::vector<solution> solutions;
	std::vector<move> possible_solution;
	std::map<std::string, size_t> examined_boards;
	std::stack<game_state> board_stack;

	given_state.generate_possible_moves();
	if (given_state.is_finished)
	{
		throw std::runtime_error("this state is already solved");
	}

	static_cast<void>(game_state_has_already_been_examined(examined_boards, given_state, possible_solution.size()));
	board_stack.push(given_state);

	// todo 2: limit the max length of solution we're after, jeez

	while (!board_stack.empty())
	{
		auto& state_to_examine {board_stack.top()};
		while (!state_to_examine.possible_moves.empty())
		{
			auto move_to_examine {state_to_examine.possible_moves.back()}; // arbitrarily choose the last one
			// possible_solution.push_back(move_to_examine); // the move stays in the possible solution until all of its children have been examined.
			state_to_examine.possible_moves.pop_back(); // take the move off the game_state so it's not examined again.

			auto new_board {state_to_examine.generate_new_board_from_move(move_to_examine)};
			new_board.generate_possible_moves();
			if (new_board.is_finished)
			{
				possible_solution.push_back(move_to_examine); // add the move to get to the solution so we can copy it off
				solutions.push_back(possible_solution);
				possible_solution.pop_back(); // we're looking for all solutions, so take the winning move back off the list because we want to continue on our search.
			}
			else if (new_board.possible_moves.size() == 0)
			{
				// this board has no possible moves, and it's not finished, it's a loser.
				//possible_solution.pop_back(); // the move that got us here lead to a dead end.
			}
			else if (game_state_has_already_been_examined(examined_boards, new_board, possible_solution.size() + 1))
			{
				// the problem here is that we might take a circuitous route to get to a state, when further along our examininations,
				// we might have found a more direct route to the same state. This problem will be unnecessarily culling shorter solutions.
				// I can think of a couple of options here:
				// 1. get rid of this check and let the max solution length check take care of situations where we cycle back to the same state,
				// this seems unsatisfactory, or
				// 2. maybe make a std::map<std::string, size_t> to hold both the examined state and the shortest route to it we've seen.
				// then, if we have examined the state before but our current path to it is shorter, we can consider it.

				// if we execute this check before checking whether the state is finished, 
				// we exclude all solutions that end in a duplicate end state. That's bad because
				// we would only record the first path to that end state, possibly missing shorter paths.

				// if we execute this check after the check for 0 moves, we save ourselves the time of
				// serialising the game_board and the memory of saving it away (which is larger than i expected).
				// this check is really to stop us cycling endlessly between the same game states.

				//possible_solution.pop_back(); // this is not the droid we're looking for
			}
			else
			{
				if (possible_solution.size() >= 80) // no thank you sir
				{
					// board_stack should be exactly the same size;

					// I am not interested in solutions this long
					state_to_examine.possible_moves.clear(); // and the horse you rode in on
					break;
				}
				else
				{
					possible_solution.push_back(move_to_examine);
					board_stack.push(new_board);
					break; // we stop examining the moves of this board and start examining the new board
				}
			}
		}
		if (state_to_examine.possible_moves.empty())
		{
			board_stack.pop();
			if (!board_stack.empty())
			{
				possible_solution.pop_back(); // if we've finished with a board, we've finished with the move that lead to it
			}
			else
			{
				// The initial game state wasn't generated by applying a move from a previous board, so we can't pop it off 
			}
		}
	}
	return solutions;
}

//bool game_state::recursively_examine_game_state(std::map<std::string, size_t>& examined_boards, std::vector<move>& solution, game_state state_to_examine)
//{
//	if (game_state_has_already_been_examined(examined_boards, state_to_examine, ))
//	{
//		return false;
//	}
//
//	state_to_examine.generate_possible_moves();
//	if (state_to_examine.is_finished)
//	{
//		return true;
//	}
//	else
//	{
//		for (const auto& move : state_to_examine.possible_moves)
//		{
//			auto new_state {state_to_examine.generate_new_board_from_move(move)};
//			if (recursively_examine_game_state(examined_boards, solution, new_state))
//			{
//				solution.push_back(move);
//				return true;
//			}
//		}
//		return false;
//	}
//}

solution& work_out_best_solution(std::vector<solution>& solutions)
{
	size_t index_of_best_solution {};
	size_t shortest_solution_length_so_far {};
	for (size_t i {0}; i < solutions.size(); i++)
	{
		if (i == 0)
		{
			shortest_solution_length_so_far = solutions[i].moves.size();
			index_of_best_solution = i;
		}
		else
		{
			auto this_solution_length {solutions[i].moves.size()};
			if (this_solution_length < shortest_solution_length_so_far)
			{
				shortest_solution_length_so_far = this_solution_length;
				index_of_best_solution = i;
			}
		}
	}
	return solutions[index_of_best_solution];
}

void report_best_solution(std::vector<solution> solutions)
{
	auto& best_solution {work_out_best_solution(solutions)};

	std::cout << "found " << solutions.size() << " solutions. The shortest is: " << std::endl;
	std::cout << best_solution;
}

void do_the_thing()
{
	game_state g
	{{
	{magenta, yellow, dark_green, orange},
	{yellow, dark_blue, cream, dark_blue},
	{light_blue, pink, dark_blue, light_green},
	{light_blue, pink, magenta, cream},
	{yellow, light_blue, magenta, dark_blue},
	{yellow, light_green, light_green, light_green},
	{cream, pink, dark_green, light_blue},
	{pink, orange, dark_green, orange},
	{cream, dark_green, orange, magenta},
	{empty, empty, empty, empty},
	{empty, empty, empty, empty}
	}};

	auto solutions {game_state::work_out_all_solutions(g)};
	if (solutions.empty())
	{
		std::cout << "didn't find a solution";
	}
	else
	{
		auto& solution {work_out_best_solution(solutions)};
		std::cout << solution;
	}
	std::cout << std::endl;
}

namespace tests
{
void test_get_colour_and_depth()
{
	{
		test_tube top_light_green_1 {1, {orange, light_blue, magenta, light_green}};
		auto [colour, depth] {top_light_green_1.get_colour_and_depth()};
		if (colour != light_green || depth != 1)
		{
			::DebugBreak();
		}
	}

	{
		test_tube top_light_green_2 {1, {orange, light_blue, light_green, light_green}};
		auto [colour, depth] {top_light_green_2.get_colour_and_depth()};
		if (colour != light_green || depth != 2)
		{
			::DebugBreak();
		}
	}

	{
		test_tube mid_orange_1_after_empty_1 {1, {light_green, cream, orange, empty}};
		auto [colour, depth] {mid_orange_1_after_empty_1.get_colour_and_depth()};
		if (colour != orange || depth != 1)
		{
			::DebugBreak();
		}
	}

	{
		test_tube mid_cream_2_after_empty_2 {1, {cream, cream, empty, empty}};
		auto [colour, depth] {mid_cream_2_after_empty_2.get_colour_and_depth()};
		if (colour != cream || depth != 2)
		{
			::DebugBreak();
		}
	}
}

void test_pouring_colour()
{
	{
		test_tube magenta_4 {1, {magenta, magenta, magenta, magenta}};
		if (magenta_4.pouring_colour() != magenta)
		{
			::DebugBreak();
		}
	}

	{
		test_tube magenta_3_empty_1 {1, {magenta, magenta, magenta, empty}};
		if (magenta_3_empty_1.pouring_colour() != magenta)
		{
			::DebugBreak();
		}
	}

	{
		test_tube orange_2_empty_2 {1, {orange, orange, empty, empty}};
		if (orange_2_empty_2.pouring_colour() != orange)
		{
			::DebugBreak();
		}
	}

	{
		test_tube pink_1_empty_3 {1, {pink, empty, empty, empty}};
		if (pink_1_empty_3.pouring_colour() != pink)
		{
			::DebugBreak();
		}
		if (pink_1_empty_3.is_empty())
		{
			::DebugBreak();
		}
	}

	{
		test_tube empty_4 {1, {empty, empty, empty, empty}};
		if (empty_4.pouring_colour() != empty)
		{
			::DebugBreak();
		}
		if (!empty_4.is_empty())
		{
			::DebugBreak();
		}

	}
}

void test_tube_display()
{
	{
		test_tube tube_1 {1, {orange, light_green, magenta, yellow}};
		std::ostringstream ss;
		ss << tube_1;
		if (ss.str() != "{orange, light green, magenta, yellow}")
		{
			::DebugBreak();
		}
	}

	{
		test_tube tube_2 {1, {orange, light_green, magenta, empty}};
		std::ostringstream ss;
		ss << tube_2;
		if (ss.str() != "{orange, light green, magenta, empty}")
		{
			::DebugBreak();
		}
	}

	{
		test_tube tube_3 {1, {empty, empty, empty, empty}};
		std::ostringstream ss;
		ss << tube_3;
		if (ss.str() != "{empty, empty, empty, empty}")
		{
			::DebugBreak();
		}
	}
}

void test_generate_possible_moves()
{
	{
		test_tube source {1, {orange, magenta, yellow, cream}};
		test_tube dest {2, {orange, magenta, cream, empty}};
		auto move {source.generate_move_to(dest)};

		if (move.to.tube_index = !1 && move.from.tube_index != 2)
		{
			::DebugBreak();
		}

		if (move.move_size != 1)
		{
			::DebugBreak();
		}
	}

	{
		test_tube source {1, {orange, magenta, yellow, yellow}};
		test_tube dest {2, {orange, magenta, yellow, empty}};
		auto move {source.generate_move_to(dest)};

		if (move.to.tube_index = !1 && move.from.tube_index != 2)
		{
			::DebugBreak();
		}

		if (move.move_size != 1)
		{
			::DebugBreak();
		}
	}

	{
		test_tube source {1, {orange, magenta, yellow, empty}};
		test_tube dest {2, {orange, magenta, yellow, empty}};
		auto move {source.generate_move_to(dest)};

		if (move.to.tube_index = !1 && move.from.tube_index != 2)
		{
			::DebugBreak();
		}

		if (move.move_size != 1)
		{
			::DebugBreak();
		}
	}

	{
		test_tube source {1, {orange, magenta, yellow, yellow}};
		test_tube dest {2, {orange, yellow, empty, empty}};
		auto move {source.generate_move_to(dest)};

		if (move.to.tube_index = !1 && move.from.tube_index != 2)
		{
			::DebugBreak();
		}

		if (move.move_size != 2)
		{
			::DebugBreak();
		}
	}

	{
		test_tube source {1, {orange, magenta, magenta, yellow}};
		test_tube dest {2, {empty, empty, empty, empty}};
		auto move {source.generate_move_to(dest)};

		if (move.to.tube_index = !1 && move.from.tube_index != 2)
		{
			::DebugBreak();
		}

		if (move.move_size != 1)
		{
			::DebugBreak();
		}
	}

	{
		test_tube source {1, {orange, magenta, yellow, cream}};
		test_tube dest {2, {empty, empty, empty, empty}};
		auto move {source.generate_move_to(dest)};

		if (move.to.tube_index = !1 && move.from.tube_index != 2)
		{
			::DebugBreak();
		}

		if (move.move_size != 1)
		{
			::DebugBreak();
		}
	}
}

void test_work_out_all_solutions_1()
{
	game_state g
	{{
	{yellow, empty},
	{yellow, empty}
	}};

	auto solutions {game_state::work_out_all_solutions(g)};

	const solution solution_1 {{{1, 0, 1}}};
	const solution solution_2 {{{0, 1, 1}}};

	auto found_solution_1 {std::find(solutions.begin(), solutions.end(), solution_1)};
	if (*found_solution_1 != solution_1)
	{
		::DebugBreak();
	}

	auto found_solution_2 {std::find(solutions.begin(), solutions.end(), solution_2)};
	if (*found_solution_2 != solution_2)
	{
		::DebugBreak();
	}

	if (solutions.size() != 2)
	{
		::DebugBreak();
	}
}

void test_work_out_all_solutions_2()
{
	game_state g
	{{
	{yellow, yellow, empty},
	{yellow, empty, empty},
	}};

	auto solutions {game_state::work_out_all_solutions(g)};

	const solution solution_1 {{{0, 1, 2}}};
	const solution solution_2 {{{1, 0, 1}}};

	auto found_solution_1 {std::find(solutions.begin(), solutions.end(), solution_1)};
	if (*found_solution_1 != solution_1)
	{
		::DebugBreak();
	}

	auto found_solution_2 {std::find(solutions.begin(), solutions.end(), solution_2)};
	if (*found_solution_2 != solution_2)
	{
		::DebugBreak();
	}

	if (solutions.size() != 2)
	{
		::DebugBreak();
	}
}

void test_work_out_all_solutions_3()
{
	game_state g
	{{
	{yellow, yellow, empty},
	{yellow, empty, empty},
	{empty, empty, empty}
	}};

	auto solutions {game_state::work_out_all_solutions(g)};

	const solution solution_1 {{{1, 0, 2}}};
	const solution solution_2 {{{0, 1, 1}}};
	const solution solution_3 {{{0, 2, 2}, {1, 2, 1}}};
	const solution solution_4 {{{1, 2, 1}, {0, 2, 2}}};
	const solution solution_5 {{{0, 2, 2}, {1, 0, 1}, {0, 2, 1}}};
	const solution solution_6 {{{0, 2, 2}, {1, 0, 1}, {2, 0, 2}}};

	std::vector<solution> expected_solutions {solution_1, solution_2, solution_3, solution_4, solution_5, solution_6};
	for (const auto& solution : solutions)
	{
		auto found_solution {std::find(solutions.begin(), solutions.end(), solution)};
		if (found_solution == solutions.end())
		{
			::DebugBreak();
		}
	}

	if (solutions.size() != expected_solutions.size())
	{
		::DebugBreak();
	}
}

void test_work_out_all_solutions_4()
{
	game_state g
	{{
	{magenta, orange, light_green},
	{orange, light_green, orange},
	{light_green, magenta, magenta},
	{empty, empty, empty}
	}};
	// oh boy these get big real quick

	auto solutions {game_state::work_out_all_solutions(g)};

	const solution solution_1 {{{2, 3, 2}, {0, 2, 1}, {1, 0, 1}, {1, 2, 1}, {0, 1, 2}, {0, 3, 1}}};
	const solution solution_2 {{{2, 3, 2}, {0, 2, 1}, {1, 0, 1}, {1, 2, 1}, {0, 1, 2}, {3, 0, 2}}};
	// i know there are more but I haven't looked yet

	std::vector<solution> expected_solutions {solution_1, solution_2};
	for (const auto& solution : solutions)
	{
		auto found_solution {std::find(solutions.begin(), solutions.end(), solution)};
		if (found_solution == solutions.end())
		{
			::DebugBreak();
		}
	}

	if (solutions.size() != expected_solutions.size())
	{
		::DebugBreak();
	}
}

void test_work_out_all_solutions()
{
	test_work_out_all_solutions_1();
	test_work_out_all_solutions_2();
	test_work_out_all_solutions_3();
	test_work_out_all_solutions_4();
}

void test_game_state_has_already_been_examined()
{
	game_state g
	{{
	{magenta, orange, light_green},
	{orange, light_green, orange},
	{light_green, magenta, magenta},
	{empty, empty, empty}
	}};

	std::map<std::string, size_t> examined_boards;

	if (game_state_has_already_been_examined(examined_boards, g, 10)) // first time we've seen it
	{
		::DebugBreak();
	}

	if (!game_state_has_already_been_examined(examined_boards, g, 10)) // the state has the same depth
	{
		::DebugBreak();
	}

	if (!game_state_has_already_been_examined(examined_boards, g, 11)) // the path to the state is worse
	{
		::DebugBreak();
	}

	if (game_state_has_already_been_examined(examined_boards, g, 9)) // the path to the state is shorter
	{
		::DebugBreak();
	}

	game_state g2
	{{
	{magenta, orange, light_green},
	{orange, light_green, orange},
	{light_green, magenta, pink},
	{empty, empty, empty}
	}};

	if (game_state_has_already_been_examined(examined_boards, g2, 0)) // first time
	{
		::DebugBreak();
	}
}

}

int main()
{
	tests::test_get_colour_and_depth();
	tests::test_pouring_colour();
	tests::test_tube_display();
	tests::test_generate_possible_moves();
	tests::test_game_state_has_already_been_examined();
	tests::test_work_out_all_solutions();

	//do_the_thing();
	return 0;
}