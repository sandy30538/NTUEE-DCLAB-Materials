#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <tuple>
#include <array>
#include <queue>
#include <stdlib.h>
#include <time.h>

using namespace std;

int main(int argc, char** argv)
{
	uint32_t 	read_len 	= 128;
    uint32_t 	ref_len 	= 128;
	uint32_t 	data_num	= 100;

    for(int i=0;i<argc;i++) {
        std::string argv_str(argv[i]);

		// Turn the argv into uint32_t (stoul) and int (stoi)		
		if(argv_str == "-read_len") read_len = std::stoul(argv[i+1]);
		if(argv_str == "-ref_len") 	ref_len = std::stoul(argv[i+1]);
		if(argv_str == "-data_num") data_num = std::stoul(argv[i+1]);
    }
	
	std::cerr << "\n";
	std::cerr << "========= Start Generating dataset =========" << "\n";
    std::cerr << "Read Length: " << read_len << "\n";
	std::cerr << "Ref Length: " << ref_len << "\n";
	std::cerr << "Data num (set of ref-read): " << data_num << "\n";
	std::cerr << "========================================" << "\n\n";

	if (read_len != ref_len) {
		std::cerr << "Ref length and Read length should be the same! \n";
		return 1;
	}
	else if (read_len <= 20) {
		std::cerr << "Read length should be > 20! \n";
		return 1;
	}
	else if ((read_len % 4) != 0) {
		std::cerr << "Read length % 4 should be 0! \n";
		return 1;
	}

	std::ofstream fout_data("./test_data/random_pattern.txt");
	std::ofstream fout_bin("./test_data/random_pattern.bin", ios::out | ios::binary);
	if(!fout_bin) {
      cout << "Cannot open binary file!" << endl;
      return 1;
   	}

	srand (2);		//random seed
	int random_num;	// random number
	std::string reference_seq, read_seq, replace_seq;

	char* ref_buffer = new char[32];
	char* read_buffer = new char[32];
	int temp_byte = 0;
	
	for (int i=0; i<data_num; i++){
		
		if (i == 0){ // Exact match case
			for(int k=0; k<ref_len; k++) {
				random_num = rand();

				if ((random_num % 4) == 0)		reference_seq.append("A");
				else if ((random_num % 4) == 1)	reference_seq.append("C");
				else if ((random_num % 4) == 2)	reference_seq.append("G");
				else if ((random_num % 4) == 3)	reference_seq.append("T");
			}
			read_seq = reference_seq;
		}
		else if (i <= 10){ // 1~10 mismatch
			read_seq = reference_seq;

			for (int m=0; m<i; m++){
				random_num = rand();
				int offset = (random_num % (read_len/10)) + (m *(read_len/10));	// position in read seq
				
				if (reference_seq[offset] == 'A')	read_seq.replace(offset, 1, "C");
				else								read_seq.replace(offset, 1, "A");
			}
		}
		else if (i <= 20){	//replaced by a sequence 11~20 
			read_seq = reference_seq;
			int offset = (read_len/10)*(i-11);

			for (int m=0; m<(i-10); m++){
				read_seq.replace(offset+m, 1, "A");
			}
		}
		else {	// random read seq
			random_num = rand();

			for(int k=0; k<read_len; k++) {
				random_num = rand();

				if ((random_num % 4) == 0) 		read_seq.append("A");
				else if ((random_num % 4) == 1) read_seq.append("C");
				else if ((random_num % 4) == 2) read_seq.append("G");
				else if ((random_num % 4) == 3) read_seq.append("T");
			}
		}

		// write binary file (Ref)
		for (int k=0; k<ref_len; k++){
			if (reference_seq[k] == 'A')		temp_byte += 0;
			else if (reference_seq[k] == 'C')	temp_byte += 1;
			else if (reference_seq[k] == 'G')	temp_byte += 2;
			else if (reference_seq[k] == 'T')	temp_byte += 3;

			if (k % 4 == 3) {
				ref_buffer[k/4] = temp_byte;
				temp_byte = 0;
			}
			else temp_byte = temp_byte * 4;
		}
		fout_bin.write (ref_buffer, (ref_len/4));

		// write binary file (Read)
		temp_byte = 0;
		for (int k=0; k<read_len; k++){
			if (read_seq[k] == 'A')			temp_byte += 0;
			else if (read_seq[k] == 'C')	temp_byte += 1;
			else if (read_seq[k] == 'G')	temp_byte += 2;
			else if (read_seq[k] == 'T')	temp_byte += 3;

			if (k % 4 == 3) {
				read_buffer[k/4] = temp_byte;
				temp_byte = 0;
			}
			else temp_byte = temp_byte * 4;
		}
		fout_bin.write (read_buffer, (read_len/4));
		
		// Write data
		fout_data << reference_seq << "\n";
		fout_data << read_seq << "\n\n";

		// reset read seq
		read_seq = "";
	}
	
	fout_data.close();
	fout_bin.close();

	delete[] read_buffer;
	delete[] ref_buffer;

    return 0;
}
