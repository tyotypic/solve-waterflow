
#include <iostream>
#include <vector>
#include <string>
#include <format>
#include <sstream>
#include <set>

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
	void exchange_colours_with(piece& other) {std::swap(this->colour, other.colour);}
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
		dest << std::format("[{} -> {} : {}]", from.tube_index, to.tube_index, move_size);
		return dest;
	}
};


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
			dest << i << ": " << moves[i];
			if (i != moves.size() - 1)
			{
				dest << ", ";
			}
		}
		dest << "}" << std::endl;
		return dest;
	}
};

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
	bool examine_game_state(std::set<std::string>& examined_boards, std::vector<move>& solution, game_state state_to_examine);
	
	solution work_out_solution(game_state given_state)
	{
		std::vector<move> solution;
		std::set<std::string> examined_boards;

		if (examine_game_state(examined_boards, solution, given_state))
		{
			std::reverse(solution.begin(), solution.end());
			std::cout << solution;
		}
		else
		{
			std::cout << "no solution";
		}

		std::cout << std::endl;

		return solution;
	}
private:
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
	std::vector<move> generate_possible_moves()
	{
		std::vector<move> possible_moves;

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
		return possible_moves;
	}
	game_state generate_new_board_from_move(move m)
	{
		game_state new_board {*this};
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

bool game_state::examine_game_state(std::set<std::string>& examined_boards, std::vector<move>& solution, game_state state_to_examine)
{
	std::ostringstream oss;
	oss << state_to_examine;
	if (examined_boards.contains(oss.str()))
	{
		return false;
	}
	else
	{
		examined_boards.insert(oss.str());
	}

	auto moves {state_to_examine.generate_possible_moves()};
	if (state_to_examine.is_finished)
	{
		return true;
	}
	else
	{
		for (const auto& move : moves)
		{
			auto new_state {state_to_examine.generate_new_board_from_move(move)};
			if (examine_game_state(examined_boards, solution, new_state))
			{
				solution.push_back(move);
				return true;
			}
		}
		return false;
	}
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

	auto solution {g.work_out_solution(g)};
	//std::cout << solution << std::endl;
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

void test_work_out_solution()
{
	game_state g 
	{{
	{yellow, empty},
	{yellow, empty}
	}};
	g.work_out_solution(g);
}

void test_apply_move()
{
}

}

int main()
{
	tests::test_get_colour_and_depth();
	tests::test_pouring_colour();
	tests::test_tube_display();
	tests::test_generate_possible_moves();
	//tests::test_work_out_solution();
	//tests::test_apply_move();

	do_the_thing();
	return 0;
}