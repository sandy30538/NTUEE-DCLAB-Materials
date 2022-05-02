#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <tuple>
#include <array>
#include <queue>

#include "SW.cpp"
using namespace std;

// For VC parameters
#define CONST_MATCH_SCORE		1
#define CONST_MISMATCH_SCORE	-4
#define CONST_GAP_OPEN			-6
#define CONST_GAP_EXTEND		-1

int main(int argc, char** argv)
{
	std::string	data_path 	= "";
    uint32_t 	data_num 	= 100;

    for(int i=0;i<argc;i++) {
        std::string argv_str(argv[i]);

		// Turn the argv into uint32_t (stoul) and int (stoi)
        if(argv_str == "-data_path") data_path = argv[i+1];
		if(argv_str == "-data_num") data_num = std::stoul(argv[i+1]);
    }

	// Score parameters
	int match_score 		= CONST_MATCH_SCORE;
	int mismatch_score 		= CONST_MISMATCH_SCORE;
	int gap_open_score		= CONST_GAP_OPEN;
	int gap_extend_score 	= CONST_GAP_EXTEND;
	
	std::cout << "========= Start Smith-Waterman =========" << "\n";
    std::cout << "Data path: " << data_path << "\n";
	std::cout << "Data num (set of ref-read): " << data_num << "\n\n";
	std::cout << "Match score: " << match_score << "\n";
	std::cout << "Mismatch score: " << mismatch_score << "\n";
	std::cout << "Gap open score: " << gap_open_score << "\n";
	std::cout << "Gap extend score: " << gap_extend_score << "\n";
	std::cout << "========================================" << "\n\n";

	// Read in File
    std::ifstream fin_data(data_path);
    if(fin_data) std::cerr << "Testdata file ok!\n";
    else {
        std::cerr << "Testdata file err!\n";
        return 1;
    }

	// Write File
	std::ofstream fout_golden("./test_data/pattern_ans.txt");
	if(!fout_golden) {
      cout << "Cannot open file!" << endl;
      return 1;
   	}

	std::string reference_seq;
	std::string read_seq;
	std::string new_line;

	fout_golden << std::dec << data_num << "\n";
	for (uint32_t i=0; i<data_num; i++){
		fin_data >> reference_seq;
		fin_data >> read_seq;

		std::cout << "Set " << std::dec << i << "\n";
		std::cout << "Reference seq: \n" << reference_seq << "\n";
		std::cout << "Read seq: \n" << read_seq << "\n";

		// Execute Smith-Waterman
		auto [highest_score, row_end, column_end, alignment_result] = smith_waterman(reference_seq, read_seq, match_score, mismatch_score, gap_open_score, gap_extend_score);
		std::cout << "Highest score: " << std::dec << highest_score << "\n";
		std::cout << "Row end: " << std::dec << row_end << "\n";
		std::cout << "Column end: " << std::dec << column_end << "\n";
		std::cout << "Alignment result: \n" << alignment_result << "\n\n";

		fout_golden << std::dec << highest_score << "\n";
		fout_golden << std::dec << row_end << "\n";
		fout_golden << std::dec << column_end << "\n";
	}

	fin_data.close();
	fout_golden.close();

    return 0;
}
