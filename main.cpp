#define _CRT_SECURE_NO_WARNINGS 
#include <string>
#include <cstring>
#include <stack>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include "cryptlib.h" // CryptoPP main library
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK1 // for md5 hash
#include "md5.h"
#include "sha.h" // for SHA1 and SHA256
#include "hex.h" // for HexEncoder
#include "files.h" // for FileSink
using namespace CryptoPP;


class GetControleData {
private:
	std::string file_path, dir_path;
	std::vector<std::vector<std::string>> lines_sep; // lines separated into file_name, hash type, hash summ and written in groups of 3 elements, as if with '\n'

public:
	std::stack<std::string> get_paths(std::string paths) {
		std::stack<std::string> stk; // stack to store file_path and dir_path
		std::size_t pos = paths.find(" ");
		this-> file_path = paths.substr(0, pos); // subtring from 0 position to " " - to extract first part of string, i e path to file
		stk.push(file_path);
		this-> dir_path = paths.substr(pos); // substring from " " to end - includes all possible spaces before directory path (that may occur when path is inserted to console
		dir_path = dir_path.substr(dir_path.find_first_not_of(" ")); // dir_path is a substring from "not space" to the end, i e we delete all spaces BEFORE path
		stk.push(dir_path);

		return stk; // return stack of strings
	}

	std::vector<std::vector<std::string>> prep_controle_data() {
		std::ifstream ifile(file_path, std::ios::binary);
		if (!ifile.is_open()) {
			std::cout << "failed to open file with controle data " << '\n';
		}
		std::vector<std::string> lines; // variable for each symbol combination in file (between spaces and '\n'-s)
		std::string file_cont; // file contents
		while (ifile) {
			ifile >> file_cont;
			lines.push_back(file_cont);
		}

		std::vector<std::string> temp_v; // temporary vector, each such vector stores: 1 file_name, 1 hash_type and 1 hash-sum (corresponding)
		for (int i = 0; i < lines.size(); i++) {
			temp_v.push_back(lines[i]);
			if (temp_v.size() == 3) {
				this-> lines_sep.push_back(temp_v); // we make "this->" inside loop because we want to aggregate all push_backs in the same lines_sep
				temp_v.clear();
			}
		}
		temp_v.clear();

		/*for (int j = 0; j < lines_sep.size(); j++) {
			for (int k = 0; k < 3; k++) {
				std::cout << lines_sep[j][k] << "   ";
			}
			std::cout << '\n';
		}*/
		return lines_sep; // return controle data in vector of vectors of strings
	}
};

// verify hash-sum of files, given in controle_data.txt
class CheckFileIntegrity {
private:
	std::string s; // private variable to store file contents
	std::string hash_sum; // private variable 

public:
	std::string prep_for_encoding(const char* file_name) { 
		FILE* f = fopen(file_name, "rb");
		if (!f) { // if we're not able to open file
			this->s = "NOT FOUND";
		}
		else {
			fseek(f, 0, SEEK_END); // seek the end of file
			size_t size = ftell(f); // get current position within the file , i e - its end; this way we obtain size of file contents
			this->s;
			s.resize(size); // allocate string space and set length equal to size of data stored in file
			rewind(f); // return to the beginning of the file for fread
			fread(&s[0], 1, size, f); 	// read size-times by 1 byte from f into s 
			fclose(f);
		}
		
		return s;
	}

	// encoding file contents with 1 of 3 types of hashing
	std::string get_hash_sum(std::string hash_type) {
		// encoding bytes into base 16 encoded data with HexEncoder (opponent is HexDecoder)
		std::string hash_sum = this->hash_sum; // initialize every time once again so that .Attach would not aggregate all further strings to hash_sum after the first call of the get_hash_sum function
		HexEncoder encoder;
		encoder.Attach(new StringSink(hash_sum));
		if (hash_type == "md5") {
			byte digest[MD5::DIGESTSIZE];
			MD5().CalculateDigest(digest, (const byte*)s.c_str(), s.length());
			encoder.Put(digest, sizeof(digest));
			encoder.MessageEnd();
		}
		else if (hash_type == "sha1") {
			byte digest[SHA1::DIGESTSIZE];
			SHA1().CalculateDigest(digest, (const byte*)s.c_str(), s.size());
			encoder.Put(digest, sizeof(digest));
			encoder.MessageEnd();
		}
		else if (hash_type == "sha256") {
			byte digest[SHA256::DIGESTSIZE];
			SHA256().CalculateDigest(digest, (const byte*)s.c_str(), s.size());
			encoder.Put(digest, sizeof(digest));
			encoder.MessageEnd();
		}

		//std::cout << "current hash summ is = " << hash_sum << std::endl;

		return hash_sum; // hash-sum (base 16) to compare with that given in controle_data.txt 
		
	}
};

int main() {
	std::string paths, file_path, dir_path;
	std::cout << "print full file path and path to directory: ";
	std::getline(std::cin, paths); // read both paths from console
	GetControleData GCD;
	std::stack<std::string> stk = GCD.get_paths(paths); // getting paths to controle-data-file and directory from cin
	dir_path = stk.top(); stk.pop(); dir_path.append("\\");// access and remove strings from stack, preparing dir_path for further manipulations
	file_path = stk.top(); stk.pop(); // now stack is empty

	std::vector<std::vector<std::string>> lines_sep;
	lines_sep = GCD.prep_controle_data(); // preparing controle data
	
	CheckFileIntegrity CFI;
	for (int i = 0; i < lines_sep.size(); i++) {
		std::string file_name = dir_path + lines_sep[i][0]; // [0] element is a file name that is to be checked, we concatenate it to dir_path
		//std::cout << "file_name = " << file_name << '\n';
		const char* file_name_cstr = file_name.c_str(); // converting to c_str because prep_for_encoding requires const char*

		if (CFI.prep_for_encoding(file_name_cstr) == "NOT FOUND") {
			std::cout << lines_sep[i][0] << "\tNOT FOUND" << '\n';
		}
		else {
			std::string hash_sum = CFI.get_hash_sum(lines_sep[i][1]); // getting hash summ of hash type, hash type taken from [1] element
			if (lines_sep[i][2] == hash_sum) { std::cout << lines_sep[i][0] << "\tOK" << '\n'; }// compare resulting hash summ with controle hash summ
			else {std::cout << lines_sep[i][0] << "\tFAIL" << '\n';}
		}
	}
		
	system("pause");
	return 0;
}
