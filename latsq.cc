#include <iostream>
#include <random>
#include <vector>
#include <functional> // std::bind
#include <chrono>
#include <string>
#include <sstream>

using std::string;
using std::ostringstream;
using std::vector;
using std::cout;
using std::cerr;
using std::endl;
using std::default_random_engine;
using std::uniform_int_distribution;
using std::chrono::high_resolution_clock;

// tags:
//  ma = matrix
//  ms = matrix size
//  mr = matrix row
//  mc = matrix cell
//  re = random engine
//  id = int distribution
//  ck = clock
//  tp = time point
//  sc = just a number (scalar)
//  ar = plain array
//  is = yes or no boolean

#define GRIDSIZE 3
#define BADNUM 199
//#define NDEBUG 1

template <class Outputable>
void debug_output(Outputable msg) {
#ifndef NDEBUG
	cerr << msg << endl;
#endif
}

void output(unsigned char *ma_grid, unsigned char ms_grid) {
	for (unsigned int ix_count = ((ms_grid * ms_grid) + 1); --ix_count; ) {
		cout << (unsigned int)ma_grid[ix_count - 1] << " ";
		if ((ix_count - 1) % GRIDSIZE == 0) {
			cout << endl;
		}
	}
}

unsigned int get_cell(unsigned int ms_grid, unsigned int ix_row, unsigned int ix_col) {
	return (ix_row - 1) * (ms_grid - 1) + ix_col - 1;
}

unsigned int check(signed char ix_recurse, unsigned char *ma_grid, unsigned char ms_grid) {
	unsigned int ix_row = ix_recurse / (ms_grid - 1) + 1;
	unsigned int ok = 1;
	// Compare col
	ok = ma_grid[ix_recurse] != ix_row;
	if (!ok) {
		return 0;
	}
	unsigned int ix_col = 1;
	unsigned int mc_cell = get_cell(ms_grid, ix_row, ix_col);
	while (mc_cell < ix_recurse && ok) {
		ok = ma_grid[mc_cell] != ma_grid[ix_recurse];
		++ix_col;
		mc_cell = get_cell(ms_grid, ix_row, ix_col);
	}
	if (!ok) {
		return 0;
	}
	// Compare row
	ix_col = ix_recurse % (ms_grid - 1) + 1;
	ok = ma_grid[ix_recurse] != ix_col;
	if (!ok) {
		return 0;
	}
	ix_row = 1;
	mc_cell = get_cell(ms_grid, ix_row, ix_col);
	while (mc_cell < ix_recurse && ok) {
		ok = ma_grid[mc_cell] != ma_grid[ix_recurse];
		++ix_row;
		mc_cell = get_cell(ms_grid, ix_row, ix_col);
	}
	return ok;
}

void scramble(unsigned char *ar_candidates) {
	typedef high_resolution_clock hrc;
	hrc::time_point tp_seed = hrc::now();
	unsigned int sc_seed = tp_seed.time_since_epoch().count();
	default_random_engine re_random;
	re_random.seed(sc_seed);
	uniform_int_distribution<int> id_random(0, GRIDSIZE - 1);
	auto roll = std::bind(id_random, re_random);

	auto rolls = 80;
//	debug_output("Scrambling...");
	for ( ; --rolls; ) {
		auto x = roll();
		auto y = roll();
		if (x == y) {
			++rolls;
			continue;
		}
		ar_candidates[x] ^= ar_candidates[y];
		ar_candidates[y] ^= ar_candidates[x];
		ar_candidates[x] ^= ar_candidates[y];
	}
//	for (auto ix_count = GRIDSIZE + 1; --ix_count; ) {
//		debug_output(ar_candidates[ix_count - 1]);
//	}
}

bool complete = false;

// recurse
void generate(signed char ix_recurse, unsigned char *ma_grid, unsigned char ms_grid) {
//	if (ix_recurse < 0) {
//		ma_grid[ix_recurse] = roll();
//		generate(ix_recurse + 1, ma_grid, ms_grid, roll);
//	} else {
//		ma_grid[ix_recurse] = roll();
//		generate(ix_recurse + 1, ma_grid, ms_grid, roll);
//	}
/*	if (ix_recurse < 0 || test(ix_recurse, ma_grid, ms_grid)) {
		if (ix_recurse == (ms_grid * ms_grid) - 1) {
			output(ma_grid, ms_grid);
		} else {
			for (unsigned char val = 0; val < ms_grid; ++val) {
				ma_grid[ix_recurse + 1] = val;
				generate(ix_recurse + 1, ma_grid, ms_grid);
			}
		}
	}
*/
	std::ostringstream ostr;
	ostr << "Finding cell: " << (ix_recurse + 1);
	ostr << endl << "Value: " << (int)ma_grid[ix_recurse + 1];
	debug_output(ostr.str());
	ostr.str("");
	output(ma_grid, ms_grid);
	if (ix_recurse < 0 || check(ix_recurse, ma_grid, ms_grid)) {
		debug_output("Cell checks ok or cell 0");
		if (ix_recurse == (ms_grid * ms_grid) - 1) {
			debug_output("Got one!");
			complete = true;
			return;
		}
		// try all the numbers - in random order?
		unsigned char ar_candidates[GRIDSIZE];
		for (unsigned char ix_size = GRIDSIZE + 1; --ix_size; ) {
			ar_candidates[ix_size - 1] = ix_size - 1;
		}
		scramble(ar_candidates);
		for (auto ix_count = GRIDSIZE + 1; --ix_count; ) {
			ma_grid[ix_recurse + 1] = ar_candidates[ix_count - 1];
			ostr << "Cell value: " << (int)ma_grid[ix_recurse + 1];
			debug_output(ostr.str());
			ostr.str("");
			generate(ix_recurse + 1, ma_grid, ms_grid);
			if (complete) {
				break;
			}
		}
	} else {
		debug_output("Cell bad");
	}
}

void initialise(unsigned char *ma_grid, unsigned char ms_grid) {
	for (unsigned int ix_count = ((ms_grid * ms_grid) + 1); --ix_count; ) {
		ma_grid[ix_count - 1] = BADNUM;
		debug_output(ix_count - 1);
	}
}

int main(int, char **) {
	unsigned char *ma_grid{new unsigned char[GRIDSIZE*GRIDSIZE]};
	if (ma_grid == nullptr) {
		return 1;
	}
	debug_output("Allocating grid");
	initialise(ma_grid, GRIDSIZE);
	debug_output("Generating grid");
	generate(-1, ma_grid, GRIDSIZE);
	debug_output("Outputting grid");
	output(ma_grid, GRIDSIZE);
	delete[] ma_grid;
	return 0;
}

/*
void backtrack_row(vector<vector<int>> &ma_grid,
		vector<vector<int>>::iterator &it_row,
		vector<int>::iterator &it_cell,
		vector<int> &ar_cands, vector<int> &ar_nope) {
	it_cell = it_row->begin();
	for ( ; it_cell != it_row->end(); ++it_cell) {
		if (*it_cell == BADNUM) break;
		ar_cands.push_back(*it_cell);
	}
	it_cell = it_row->begin();
*
	// TO DO
	if (it_cell > it_row->begin() + 5) {
		ar_cands.push_back(*it_cell);
		*it_cell = BADNUM;
		--it_cell;
	}
	if (it_cell > it_row->begin() + 4) {
		ar_cands.push_back(*it_cell);
		*it_cell = BADNUM;
		--it_cell;
	}
	if (it_cell > it_row->begin() + 3) {
		ar_cands.push_back(*it_cell);
		*it_cell = BADNUM;
		--it_cell;
	}
	if (it_cell > it_row->begin() + 2) {
		ar_cands.push_back(*it_cell);
		*it_cell = BADNUM;
		--it_cell;
	}
	if (it_cell > it_row->begin() + 1) {
		ar_cands.push_back(*it_cell);
		*it_cell = BADNUM;
		--it_cell;
	}
	if (it_cell > it_row->begin() + 0) {
		ar_cands.push_back(*it_cell);
		*it_cell = BADNUM;
		--it_cell;
	}
*
*
	// if no possibilities, backtrack instead
	// move item back, subtract one from it_cell,
	// continue
//	ar_cands.push_back(*it_cell);
//	*it_cell = BADNUM;
//	it_cell--;
//	ar_cands.push_back(*it_cell);
//	*it_cell = BADNUM;
//	it_cell--;
	backtrack_row(ma_grid, it_row, it_cell);
	continue;

	// test if it works ok
	// - if not, try again
	// check column above - that's it! find what index
	// we're at in the grid.
	auto ix_pos = it_cell - it_row->begin();
	if (!test_cell(ma_grid, it_row, ix_pos, *it_cell)) {
		backtrack_row(ma_grid, it_row, it_cell);
	}
*
}

template <class Stringable>
void debug_output(const Stringable &msg) {
#ifndef NDEBUG
	cerr << msg << endl;
#endif
}

void output_row(vector<int> &mr_row) {
	for (auto &r : mr_row) {
		cout << char(97 + r);
		cout << " ";
	}
}

void output_grid(vector<vector<int>> &ma_grid) {
	for (auto &g : ma_grid) {
		output_row(g);
		cout << endl;
	}
}

bool test_cell_row(vector<vector<int>>::iterator &it_row,
		vector<vector<int>>::iterator &it_cell_row,
		int ix_pos, int sc_cell) {
	auto it_cell = it_row->begin() + ix_pos;
	bool is_same_value = (sc_cell == *it_cell);
	cout << sc_cell << " is the same as " << *it_cell << ": ";
	cout << std::boolalpha << is_same_value << endl;
	bool is_same_cell = (it_cell_row == it_row);
	if (is_same_value && is_same_cell) {
		cout << "But same cell compared, so it's ok." << endl;
	}
	if (is_same_value && !is_same_cell) {
		cout << "Bad!" << endl;
		return false;
	}
	return true;
}

bool test_cell(vector<vector<int>> &ma_grid,
		vector<vector<int>>::iterator it_cell_row,
		int ix_pos, int sc_cell) {
	debug_output(ix_pos);
	output_grid(ma_grid);
	cout << endl;
	auto it_row = ma_grid.begin();
	for ( ; it_row != ma_grid.end(); ++it_row) {
		if (!test_cell_row(it_row, it_cell_row, ix_pos,
				sc_cell)) {
			return false;
		}
	}
	return true;
}

void select_cand(vector<int> &ar_cands, vector<int>::iterator &it_cell,
		int ix_random) {
	*it_cell = ar_cands[ix_random];
	ar_cands.erase(ar_cands.begin() + ix_random);
}

void generate_matrix_row(vector<vector<int>> &ma_grid,
		vector<vector<int>>::iterator &it_row) {
	// generate rows, test each char as we go and backtrack
	typedef high_resolution_clock hrc;
	hrc::time_point tp_seed = hrc::now();
	unsigned int sc_seed = tp_seed.time_since_epoch().count();

	ostringstream ostr;
	ostr.str();
	ostr << "Seed: " << sc_seed;
	debug_output(ostr.str());

	default_random_engine re_random;
	re_random.seed(sc_seed);
	uniform_int_distribution<int> id_random(0, 25);
	auto roll = std::bind(id_random, re_random);

	vector<int> ar_cands = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
				12, 13, 14, 15, 16, 17, 18, 19, 20,
				21, 22, 23, 24, 25};
	vector<int> ar_nope = {};
	auto it_cell = it_row->begin();
	for ( ; it_cell != it_row->end(); ++it_cell) {
		// move random member from ar_cands into it_cell
		if (ar_cands.size() == 0) {
			backtrack_row(ma_grid, it_row, it_cell, ar_cands, ar_nope);
			continue;
		}
		auto ix_random = roll() % ar_cands.size();
		select_cand(ar_cands, it_cell, ix_random);
		auto ix_pos = it_cell - it_row->begin();
		if (!test_cell(ma_grid, it_row, ix_pos, *it_cell)) {
			backtrack_row(ma_grid, it_row, it_cell, ar_cands, ar_nope);
		}
		ar_cands.insert(ar_cands.end(), ar_nope.begin(), ar_nope.end());
		ar_nope.clear();
	}
}

void generate_matrix(vector<vector<int>> &ma_grid) {
	auto it_row = ma_grid.begin();
	for ( ; it_row != ma_grid.end(); ++it_row) {
		generate_matrix_row(ma_grid, it_row);
	}
}

int main(int, char **) {
	ostringstream ostr;
	const int ms_grid = GRIDSIZE;

	debug_output("Allocating grid");

	vector<vector<int>> ma_grid{{}};
	ma_grid.resize(ms_grid);
	for (auto count = ms_grid + 1; --count; ) {
		auto row = count - 1;

		ostr.str();
		ostr << "Resizing row " << row;
		debug_output(ostr.str());

		ma_grid[row].resize(ms_grid);
		for (auto count2 = ms_grid + 1; --count2; ) {
			ma_grid[row][count2 - 1] = BADNUM;
		}
	}
	generate_matrix(ma_grid);
	output_grid(ma_grid);
	return 0;
}
*/

