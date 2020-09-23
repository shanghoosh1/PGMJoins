#ifndef CSVREADER_H
#define CSVREADER_H

#include <unordered_map>
#include <boost/functional/hash.hpp>
#include<string>
#include<vector>
#include<map>
#include <chrono>
using namespace std::chrono;
using namespace std;

#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <string>
#include <algorithm>
#include <sstream>


template < typename SEQUENCE > struct seq_hash
{
	std::size_t operator() (const SEQUENCE& seq) const
	{
		std::size_t hash = 0;
		boost::hash_range(hash, seq.begin(), seq.end());
		return hash;
	}
};

template < typename SEQUENCE, typename T >
using unordered_map_sequence = std::unordered_map< SEQUENCE, T, seq_hash<SEQUENCE> >;




class AttributeFrequencies
{
public:
	std::vector<std::unordered_map<unsigned int, unordered_map<unsigned int, unsigned long int>>> ConditionalFreq;
	std::unordered_map<string, std::unordered_map<unsigned int, unsigned long int>> SingleAttributesFreq;
	unordered_map_sequence<vector<unsigned int>, unsigned int> GroupAttributesFreq;
	unordered_map_sequence<std::vector<unsigned int>, vector<vector<unsigned int>> > GroupAttributesFreq2;
	unordered_map<unsigned int, vector<vector<unsigned int>> > GroupAttributesFreq1;
	unordered_map_sequence<vector<unsigned int>, unsigned int> pulledOutEdgeFreq;
        unsigned long int dataSize;
	std::map<std::string, double> timings;

	AttributeFrequencies(std::vector<string>& singlAttributes);
};


class CSVReader
{
	std::string fileName;
	std::string delimeter;
	std::vector<string> headers;
	std::vector<int> headerIndexes;
public:
	CSVReader(std::string filename, std::string delm = ",") :
		fileName(filename), delimeter(delm)
	{ }
	// Function to fetch data from a CSV File
	std::vector<string> GetHeaders(std::vector<string>& singlAttributes, std::vector<std::pair<string, string>>& pairAttributes, vector<string>& First, vector<string>& Second);
	std::vector<int> GetHeadersIndexes(const std::string& s, char delimiter, vector<string>& headers);
	std::vector<unsigned int> split(const std::string& s, char delimiter);
	AttributeFrequencies getData(std::vector<string>& singlAttributes, std::vector<std::pair<string, string>>& pairAttributes, vector<string>& First, vector<string>& Second);
};


template < typename T>
std::pair<bool, int > findInVector(const std::vector<T>& vecOfElements, const T& element)
{
	std::pair<bool, int > result;
	// Find given element in vector
	auto it = std::find(vecOfElements.begin(), vecOfElements.end(), element);
	if (it != vecOfElements.end())
	{
		result.second = distance(vecOfElements.begin(), it);
		result.first = true;
	}
	else
	{
		result.first = false;
		result.second = -1;
	}
	return result;
}


void
TransferGroupAtt(std::vector<string>& First, std::vector<string>& Second, unordered_map_sequence<vector<unsigned int>, unsigned int>& GroupAttributesFreq, unordered_map_sequence<std::vector<unsigned int>, vector<vector<unsigned int>> >& GroupAttributesFreq2, unordered_map<unsigned int, vector<vector<unsigned int>> >& GroupAttributesFreq1)
{

	unsigned int sum = 0;
	for (const auto& pair : GroupAttributesFreq)
	{
		if (First.size() == 1) {
			vector<unsigned int> key;
			key = pair.first;
			int new_key = key[0];
			vector<unsigned int> value;
			for (int i = 0; i < Second.size(); i++) { unsigned int val; val = key[i + 1]; value.push_back(val); }

			sum += pair.second;
			value.push_back(sum);

			GroupAttributesFreq1[new_key].push_back(value);
		}
		else {
			vector<unsigned int> key;
			vector<unsigned int> value;
			for (int i = 0; i < First.size(); i++) {
				string val;
				//val = to_string(pair.first[i + 1]);
				key.push_back(pair.first[i]);
			}
			for (int i = 0; i < Second.size(); i++) {
				unsigned int val;
				val = pair.first[First.size() + i];
				value.push_back(val);
			}
			sum += pair.second;
			value.push_back(sum);

			GroupAttributesFreq2[key].push_back(value);
		}

	}
}
void
FindGroupAttributes(std::vector<string>& concat, unordered_map_sequence<vector<unsigned int>, unsigned int>& GroupAttributesFreq, std::vector<string>& headers, std::vector<unsigned int>& values) {
	// finding frequencies for Group attributes


	vector<unsigned int> key_vector;

	for (string att : concat) {
		std::pair<bool, unsigned int> result = findInVector<string>(headers, att);
		key_vector.push_back(values[result.second]);
	}
	try {
		GroupAttributesFreq[key_vector] = GroupAttributesFreq[key_vector] + 1;
	}
	catch (...) {
		GroupAttributesFreq[key_vector] = 1;
	}

}


void
FindPairAttributes(std::vector<std::pair<string, string>>& pairAttributes, std::vector<std::unordered_map<unsigned int, unordered_map<unsigned int, unsigned long int>>>& ConditionalFreq, std::vector<string>& headers, std::vector<unsigned int>& values) {
	//finding conditional attributes
	int cnt = -1;
	for (std::pair<string, string> pair : pairAttributes) {
		cnt += 1;
		std::pair<bool, unsigned int> p1 = findInVector<string>(headers, pair.first);
		std::pair<bool, unsigned int> p2 = findInVector<string>(headers, pair.second);
		if (p1.first && p2.first)
		{
			unsigned int x_val, y_val;
			x_val = values[p1.second];
			y_val = values[p2.second];
			if (ConditionalFreq.size() > 0) {
				try {
					ConditionalFreq[cnt][x_val][y_val] = ConditionalFreq[cnt][x_val][y_val] + 1;
				}
				catch (...) {
					ConditionalFreq[cnt][x_val][y_val] = 1;
				}
			}
			else {
				std::unordered_map<unsigned int, unordered_map<unsigned int, unsigned long int>> temp;
				temp[x_val][y_val] = 1;
				ConditionalFreq.push_back(temp);
			}

		}
		else
			throw;
	}


}

void
FindSingleAttributes(std::vector<string>& singlAttributes, std::unordered_map<string, std::unordered_map<unsigned int, unsigned long int>>& SingleAttributesFreq, std::vector<string>& headers, std::vector<unsigned int>& values) {
	// finding frequencies for single attributes 
	for (string att : singlAttributes) {
		std::pair<bool, unsigned int> result = findInVector<string>(headers, att);
		if (result.first)
		{
			unsigned int val;
			val = values[result.second];

			try {

				SingleAttributesFreq[att][val] = SingleAttributesFreq[att][val] + 1;
			}
			catch (...) {

				SingleAttributesFreq[att][val] = 1;
			}

		}
		else
			throw;
	}


}
string trim(const string& str)
{
	size_t first = str.find_first_not_of(' ');
	if (string::npos == first)
	{
		return str;
	}
	size_t last = str.find_last_not_of(' ');
	return str.substr(first, (last - first + 1));
}
//
//int main() {
//
//
//	CSVReader reader("C:\\Users\\meghdad\\Downloads\\orders.csv");
//	// get the data from csv file
//	bool header = false;
//	std::vector<string> singlattr = { };
//	std::pair<string, string> p;
//	std::vector<string> first = { "orderkey", "custkey" };
//	std::vector<string> second = { "custkey" };
//	std::vector<string> pulled = { "orderkey", "custkey" };
//
//	std::vector<std::pair<string, string>> pairattributes = { p };
//	AttributeFrequencies datalist = reader.getData(singlattr, pairattributes, pulled,first, second);
//
//	/*
//	std::map<vector<int>, vector<int>> maps;
//	std::vector<int> a = { 1,2 ,3};
//	std::vector<int> b = { 7,8 };
//	std::vector<int> c = { 4,5};
//	std::vector<int> d = { 6,7 };
//
//	maps[a] = c;
//
//	maps[b] = d;
//	*/
//
//	return 0;
//}

AttributeFrequencies::AttributeFrequencies(std::vector<string>& singlAttributes)
{
	for (string att : singlAttributes)
	{

		unordered_map<unsigned int, unsigned long int> x;
		SingleAttributesFreq[att] = x;
	}

}

std::vector<unsigned int> CSVReader::split(const std::string& s, char delimiter)
{
	std::vector<unsigned int> tokens;

	std::string token;
	std::istringstream tokenStream(s);
	while (std::getline(tokenStream, token, delimiter))
	{
		unsigned int value = 0;

		try {
			value = stoi(token);
		}
		catch (std::exception& e) {};
		tokens.push_back(value);

	}
	return tokens;
}

std::vector<int> CSVReader::GetHeadersIndexes(const std::string& ss, char delimiter, vector<string>& headers) {

	std::vector<string> headerline;
	std::string token;
	std::istringstream tokenStream(ss);
	while (std::getline(tokenStream, token, delimiter))
	{

		headerline.push_back(trim(token));

	}

	vector<int> headerindexes;
	for (string s : headers)
	{
		std::pair<bool, unsigned int> result = findInVector<string>(headerline, s);
		if (result.first)
		{
			headerindexes.push_back(result.second);
		}
	}

	return headerindexes;

}



std::vector<string> CSVReader::GetHeaders(std::vector<string>& singlAttributes, std::vector<std::pair<string, string>>& pairAttributes, vector<string>& First, vector<string>& Second) {

	vector<string> headers;
	for (string att : singlAttributes) {
		if (std::find(headers.begin(), headers.end(), att) == headers.end()) {
			// someName not in name, add it
			headers.push_back(att);
		}
	}
	for (string att : First) {
		if (std::find(headers.begin(), headers.end(), att) == headers.end()) {
			// someName not in name, add it
			headers.push_back(att);
		}
	}

	for (string att : Second) {
		if (std::find(headers.begin(), headers.end(), att) == headers.end()) {
			// someName not in name, add it
			headers.push_back(att);
		}
	}

	for (std::pair<string, string> p : pairAttributes) {
		if (std::find(headers.begin(), headers.end(), p.first) == headers.end()) {
			// someName not in name, add it
			headers.push_back(p.first);
		}
		if (std::find(headers.begin(), headers.end(), p.second) == headers.end()) {
			// someName not in name, add it
			headers.push_back(p.second);
		}
	}
        
//        for (string att : pulledout) {
//		if (std::find(headers.begin(), headers.end(), att) == headers.end()) {
//			// someName not in name, add it
//			headers.push_back(att);
//		}
//	}

	return headers;
}

AttributeFrequencies CSVReader::getData(std::vector<string>& singlAttributes, std::vector<std::pair<string, string>>& pairAttributes, vector<string>& First, vector<string>& Second)
{

	auto start_total = high_resolution_clock::now();
	vector<string> concat;
	concat.reserve(First.size() + Second.size()); // preallocate memory
	concat.insert(concat.end(), First.begin(), First.end());
	concat.insert(concat.end(), Second.begin(), Second.end());

	AttributeFrequencies F(singlAttributes);

	std::ifstream file(fileName);
  
          if (!file.good())
          {
              cout<<"file not found."<<fileName<<endl;
           // If file is not there
           exit(1);
          }
	std::string line = "";

	// Iterate through each line and split the content using delimeter
	auto start = high_resolution_clock::now();


	headers = GetHeaders(singlAttributes, pairAttributes, First, Second);
	getline(file, line);
	headerIndexes = GetHeadersIndexes(line, '|', headers);

	std::vector<std::vector<unsigned int>> data;

	std::vector<string> results;
	results.resize(headers.size());
	vector<unsigned int> casted;
	casted.resize(results.size());
	while (getline(file, line))
	{

		int cnt = 0;

		std::string::const_iterator start = line.begin();
		std::string::const_iterator end = line.end();
		std::string::const_iterator next = std::find(start, end, '|');

		while (next != end) {

			std::pair<bool, unsigned int> r = findInVector<int>(headerIndexes, cnt);
			if (r.first)
			{
				results[r.second] = std::string(start, next);
			}
			start = next + 1;
			next = std::find(start, end, '|');
			cnt += 1;
		}
		std::pair<bool, unsigned int> r = findInVector<int>(headerIndexes, cnt);
		if (r.first)
		{
			results[r.second] = std::string(start, next);
		}

		
                for (int i = 0; i < results.size(); i++) { casted[i] = stoi(results[i]); }
		data.push_back(casted);
                
                
	}

	file.close();
	auto end = high_resolution_clock::now();
	auto duration = duration_cast<seconds>(end - start);
	//	std::cout << "\n";
	//	std::cout << duration.count();
	//	std::cout << "\n";

	F.timings["loading"] = duration.count();

	auto start1 = high_resolution_clock::now();
	for (std::vector<unsigned int> row : data)
        {
            if (Second.size() > 0)
                FindGroupAttributes(concat, F.GroupAttributesFreq, headers, row);
            if (singlAttributes.size()>0)
                FindSingleAttributes(singlAttributes, F.SingleAttributesFreq, headers, row);
            if(pairAttributes.size()>0)
                FindPairAttributes(pairAttributes, F.ConditionalFreq, headers, row);
//            if(pulledOut.size()>0)
//                FindGroupAttributes(pulledOut, F.pulledOutEdgeFreq, headers, row);
        
        }
        
	auto end1 = high_resolution_clock::now();
	auto duration1 = duration_cast<seconds>(end1 - start1);
	//	std::cout << "\n";
	//	std::cout << duration1.count();
	//	std::cout << "\n";
	F.timings["frequency_calculation"] = duration1.count();

	if (Second.size() > 0)
	{

		TransferGroupAtt(First, Second, F.GroupAttributesFreq, F.GroupAttributesFreq2, F.GroupAttributesFreq1);
	}
        F.dataSize=data.size();
        
	F.GroupAttributesFreq.clear();
	auto end_total = high_resolution_clock::now();
	auto total_time = duration_cast<seconds>(end_total - start_total);
	F.timings["total"] = total_time.count();
	return F;
};
#endif /* CSVREADER_H */