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
//  re = random engine
//  id = int distribution
//  ck = clock
//  tp = time point
//  sc = just a number (scalar)
//  ar = plain array
//  is = yes or no boolean

#define GRIDSIZE 26
#define BADNUM 99
#define NDEBUG 1

void backtrack_row(vector<vector<int>> &ma_grid,
		vector<vector<int>>::iterator &it_row,
		vector<int>::iterator &it_cell) {
	// TO DO
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
	auto it_cell = it_row->begin();
	for ( ; it_cell != it_row->end(); ++it_cell) {
		// move random member from ar_cands into it_cell
		if (ar_cands.size() > 0) {
			auto ix_random = roll() % ar_cands.size();
			*it_cell = ar_cands[ix_random];
			ar_cands.erase(ar_cands.begin() + ix_random);
		} else {
			// if no possibilities, backtrack instead
			// move item back, subtract one from it_cell,
			// continue
			ar_cands.push_back(*it_cell);
			*it_cell = BADNUM;
			it_cell--;
			ar_cands.push_back(*it_cell);
			*it_cell = BADNUM;
			it_cell--;
			continue;
		}

		// test if it works ok
		// - if not, try again
		// check column above - that's it! find what index
		// we're at in the grid.
		auto ix_pos = it_cell - it_row->begin();
		if (!test_cell(ma_grid, it_row, ix_pos, *it_cell)) {
			backtrack_row(ma_grid, it_row, it_cell);
		}
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
