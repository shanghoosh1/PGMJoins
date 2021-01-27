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
#include<myGraph.h>

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
        std::vector<std::vector<unsigned int>> data;
	std::vector<string> GetHeaders(std::vector<string>& singlAttributes, std::vector<std::pair<string, string>>& pairAttributes, vector<string>& First, vector<string>& Second);
	std::vector<int> GetHeadersIndexes(const std::string& s, char delimiter, vector<string>& headers);
	std::vector<unsigned int> split(const std::string& s, char delimiter);
	void findRawData_withBuffer(std::vector<string>& singlAttributes, std::vector<std::pair<string, string>>& pairAttributes, vector<string>& First, vector<string>& Second, int size);
        void findRawData(std::vector<string>& singlAttributes, std::vector<std::pair<string, string>>& pairAttributes, vector<string>& First, vector<string>& Second);
        void calSingleFreq(string att,unordered_map<unsigned int,unsigned long int> &pot);
        void calPairFreq(pair<string,string> atts,unordered_map<unsigned int, unordered_map<unsigned int, unsigned long int>> &pot);
        void calNon_Sk_Freq(vector<string>& First, vector<string>& Second, unordered_map_sequence<std::vector<unsigned int>, vector<vector<unsigned int>> > & vec_Key,unordered_map<unsigned int, vector<vector<unsigned int>>> &single_key);
        void calNon_Sk_Freq2(vector<string>& First, vector<string>& Second, unordered_map_sequence<std::vector<unsigned int>, vector<vector<unsigned int>> > & vec_Key,unordered_map<unsigned int, vector<vector<unsigned int>>> &single_key);
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

void CSVReader::findRawData_withBuffer(std::vector<string>& singlAttributes, std::vector<std::pair<string, string>>& pairAttributes, vector<string>& First, vector<string>& Second, int size)
{
//    std::vector<std::vector<unsigned int>> data;
    data.resize(size);
    std::ifstream file(fileName);
    if (!file.good())
    {
        cout<<"file not found."<<fileName<<endl;
     // If file is not there
     exit(1);
    }
    std::string line = "";

    // Iterate through each line and split the content using delimeter
    string tmpS;int tmpI;
    headers = GetHeaders(singlAttributes, pairAttributes, First, Second);
    getline(file, line);
    headerIndexes = GetHeadersIndexes(line, '|', headers); 
    // sort pair wise header and indices to scan faster
    for (int h = 0; h < headerIndexes.size()-1;h++)       
        for (int j = 0; j < headerIndexes.size()-h-1; j++)  
            if (headerIndexes[j] > headerIndexes[j+1])  
            {
                tmpS=headers[j];
                headers[j]=headers[j+1];
                headers[j+1]=tmpS;
                tmpI=headerIndexes[j];
                headerIndexes[j]=headerIndexes[j+1];
                headerIndexes[j+1]=tmpI;
            }

    std::vector<string> row;
    row.resize(headers.size());
    vector<unsigned int> casted;
    casted.resize(row.size());
    int rowNum=0;
    while (getline(file, line))
    {
        
            int headerCnt = 0;
            int lineCnt=0;

            std::string::const_iterator start = line.begin();
            std::string::const_iterator end = line.end();
            std::string::const_iterator next = std::find(start, end, '|');

            while (next != end) {
                if(lineCnt==headerIndexes[headerCnt])
                {
                    row[headerCnt++] = std::string(start, next);
                }
                start = next + 1;
                next = std::find(start, end, '|');
                lineCnt++;
            }

            for (int i = 0; i < row.size(); i++) 
                 casted[i] = stoi(row[i]); 
            data[rowNum++]=casted;
    }

    file.close();
}
void CSVReader::findRawData(std::vector<string>& singlAttributes, std::vector<std::pair<string, string>>& pairAttributes, vector<string>& First, vector<string>& Second)
{
    std::ifstream file(fileName);
    if (!file.good())
    {
        cout<<"file not found."<<fileName<<endl;
     // If file is not there
     exit(1);
    }
    std::string line = "";

    // Iterate through each line and split the content using delimeter
    string tmpS;int tmpI;
    headers = GetHeaders(singlAttributes, pairAttributes, First, Second);
    getline(file, line);
    headerIndexes = GetHeadersIndexes(line, '|', headers); 
    // sort pair wise header and indices to scan faster
    for (int h = 0; h < headerIndexes.size()-1;h++)       
        for (int j = 0; j < headerIndexes.size()-h-1; j++)  
            if (headerIndexes[j] > headerIndexes[j+1])  
            {
                tmpS=headers[j];
                headers[j]=headers[j+1];
                headers[j+1]=tmpS;
                tmpI=headerIndexes[j];
                headerIndexes[j]=headerIndexes[j+1];
                headerIndexes[j+1]=tmpI;
            }

    std::vector<string> row;
    row.resize(headers.size());
    vector<unsigned int> casted;
    casted.resize(row.size());
    int lastInx=headerIndexes[headerIndexes.size()-1];
    while (getline(file, line))
    {
        
            int headerCnt = 0;
            int lineCnt=0;

            std::string::const_iterator start = line.begin();
            std::string::const_iterator end = line.end();
            std::string::const_iterator next = std::find(start, end, '|');

            while (next != end) {
                if(lineCnt==headerIndexes[headerCnt])
                    row[headerCnt++] = std::string(start, next);
                
                if(lastInx==lineCnt)
                        break;
                start = next + 1;
                next = std::find(start, end, '|');
                lineCnt++;
            }

            if (lineCnt==headerIndexes[headerCnt])//last attribute
                row[headerCnt++] = std::string(start, next);

            for (int i = 0; i < row.size(); i++) 
                 casted[i] = stoi(row[i]); 
            data.push_back(casted);
    }

    file.close();
}
void CSVReader::calSingleFreq(string att,unordered_map<unsigned int,unsigned long int> &pot)
{
    
    std::pair<bool, unsigned int> attInx = findInVector<string>(headers, att);
    if (attInx.first)
    {
        unsigned int key;
        for (auto &row : data)
        {   
                key = row[attInx.second];
                pot[key] = pot[key] + 1;
        }
    }
    else
    {
        cout<<"the attribute "<<att<<"does not exist!"<<endl;
        exit(1);
    }

}
void CSVReader::calPairFreq(pair<string,string> atts,unordered_map<unsigned int, unordered_map<unsigned int, unsigned long int>> &pot)
{
    std::pair<bool, unsigned int> p1 = findInVector<string>(headers, atts.first);
    std::pair<bool, unsigned int> p2 = findInVector<string>(headers, atts.second);
    unsigned int x_val, y_val;
    if (p1.first && p2.first)
    {
        for (auto & row : data)
        {       
            x_val = row[p1.second];
            y_val = row[p2.second];
            pot[x_val][y_val] = pot[x_val][y_val] + 1;
        }
    }
    else
    {
        cout<<"Check attributes "<<atts.first <<" and "<<atts.second << endl;
        exit(1);
    }
}
void CSVReader::calNon_Sk_Freq2(vector<string>& First, vector<string>& Second, unordered_map_sequence<std::vector<unsigned int>, vector<vector<unsigned int>> > & vec_Key,unordered_map<unsigned int, vector<vector<unsigned int>>> &single_key)
{

    unordered_map_sequence<vector<unsigned int>, unordered_map_sequence<vector<unsigned int>, unsigned int>  >  vec_vec;
    unordered_map<unsigned int, unordered_map_sequence<vector<unsigned int>, unsigned int> > sing_vec;
    unordered_map_sequence<vector<unsigned int>, unordered_map<unsigned int, unsigned int>  >  vec_sing;
    unordered_map<unsigned int, unordered_map<unsigned int, unsigned int> > sing_sing;
 
    vector<unsigned int> firstInx;
    vector<unsigned int> secondInx;
    for (string att : First) 
    {
        std::pair<bool, unsigned int> attInxx = findInVector<string>(headers, att);
        if(attInxx.first)
            firstInx.push_back(attInxx.second);
    }
    for (string att : Second) 
    {
        std::pair<bool, unsigned int> attInxx = findInVector<string>(headers, att);
        if(attInxx.first)
            secondInx.push_back(attInxx.second);
    }
    
    unsigned int key;
    unsigned int value;
    if (First.size() == 1 && Second.size()==1) // insert to sing_sing
    {
        for (auto &row : data)
        {       
            key=row[firstInx[0]];
            value=row[secondInx[0]];
            sing_sing[key][value]=sing_sing[key][value]+1;
        }
    }
    else if (First.size() > 1 && Second.size()==1)
    {
        unsigned int value;
        for (auto &row : data)
        {       
            vector<unsigned int> key;
            value=row[secondInx[0]];
            for (auto attID : firstInx) 
                key.push_back(row[attID]);
            
            vec_sing[key][value]=vec_sing[key][value]+1;
        }
    }
    else if (First.size() == 1 && Second.size()>1)
    {
        unsigned int key;
        for (auto &row : data)
        {       
            vector<unsigned int> value;
            key=row[firstInx[0]];
            for (auto attID : secondInx) 
                value.push_back(row[attID]);
            
            sing_vec[key][value]=sing_vec[key][value]+1;
        }
    }
    else if (First.size() > 1 && Second.size()>1)
    {
        
        for (auto &row : data)
        {       
            vector<unsigned int> key;
            vector<unsigned int> value;
            for (auto attID : secondInx) 
                value.push_back(row[attID]);
            for (auto attID : firstInx) 
                key.push_back(row[attID]);
            vec_vec[key][value]=vec_vec[key][value]+1;
        }
    }
    //now make the final conditional non-skeleton potential functions  (vectorized and accumulated to ease the sampling)
    // the second map should be converted to vector and make the accumulative distribution
    if (First.size() == 1) 
    {
        if (sing_sing.size()>0)
        {
            unsigned int key;
            
            for (auto &key1: sing_sing)
            {
                key=key1.first;
                vector< vector<unsigned int>> value(key1.second.size());
                int i=0;
                int sum=0;
                for (auto &key2:key1.second)
                {
                    value[i].push_back( key2.first);
                    sum+=key2.second;
                    value[i++].push_back(sum);
//                    cout<<i<<endl;
                    
                }
                single_key[key]=value;
            }
        }
        else if (sing_vec.size()>1)
        {
            unsigned int key;
            
            for (auto &key1: sing_vec)
            {
                
                key=key1.first;
                vector< vector<unsigned int>> value(key1.second.size());
                int i=0;
                int sum=0;
                for (auto &key2:key1.second)
                {
                    for (auto &vvv:key2.first)
                        value[i].push_back(vvv);
                    sum+=key2.second;
                    value[i++].push_back(sum);
//                    cout<<i<<endl;
                    
                }
                single_key[key]=value;
            }
        }
        
    }
    else if (First.size() > 1)
    {
       if (vec_sing.size()>0)
        {

            for (auto &key1: vec_sing)
            {
                vector<unsigned int> key=key1.first;
                vector< vector<unsigned int>> value(key1.second.size());
                int i=0;
                int sum=0;
                for (auto &key2:key1.second)
                {
                    value[i].push_back( key2.first);
                    sum+=key2.second;
                    value[i++].push_back(sum);
//                    cout<<i<<endl;
                    
                }
                vec_Key[key]=value;
            }
        }
       else if (vec_vec.size()>1)
        {
            for (auto &key1: vec_vec)
            {
                vector<unsigned int> key=key1.first;
                vector< vector<unsigned int>> value(key1.second.size());
                int i=0;
                int sum=0;
                for (auto &key2:key1.second)
                {
                    for (auto &vvv:key2.first)
                        value[i].push_back(vvv);
                   
                    sum+=key2.second;
                    value[i++].push_back(sum);
//                    cout<<i<<endl;
                    
                }
                vec_Key[key]=value;
            }
        }
    }

}
void CSVReader::calNon_Sk_Freq(vector<string>& First, vector<string>& Second, unordered_map_sequence<std::vector<unsigned int>, vector<vector<unsigned int>> > & vec_Key,unordered_map<unsigned int, vector<vector<unsigned int>>> &single_key)
{
    vector<string> concated;
    concated.insert(concated.end(), First.begin(), First.end());
    concated.insert(concated.end(), Second.begin(), Second.end());
    unordered_map_sequence<vector<unsigned int>, unsigned int> GroupFreq; //to keep the freqs of all attributes together
    vector<unsigned int> attInx;
    for (string att : concated) 
    {
        std::pair<bool, unsigned int> attInxx = findInVector<string>(headers, att);
        if(attInxx.first)
            attInx.push_back(attInxx.second);
    }
    
    
    for (auto &row : data)
    {       
        vector<unsigned int> key_vector;
        for (unsigned int attID : attInx) 
            key_vector.push_back(row[attID]);

        GroupFreq[key_vector] = GroupFreq[key_vector] + 1;

    }
    
    //now make the conditional non-skeleton potential functions (Accumulated)

//    vector<unsigned int> all;
//    vector<unsigned int> value;
    
    if (First.size() == 1) 
    {
        for (const auto& pair : GroupFreq)
        {
           
            int new_key = pair.first[0];
            vector<unsigned int> value;
            
            for (int i = 0; i < Second.size(); i++) 
                value.push_back(pair.first[i + 1]); 
            
            value.push_back(pair.second);
            single_key[new_key].push_back(value);
        }
    }
    else if (First.size() > 1)
    {
        vector<unsigned int> newKey;
        newKey.resize(First.size());
        vector<unsigned int> value;
        value.resize(Second.size()+1);
        for (const auto& pair : GroupFreq)
        {
//            all = pair.first;
//            
            for (int i = 0; i < First.size(); i++) 
                newKey[i]=(pair.first[i]);
        
            for (int i = 0; i < Second.size(); i++) 
                value[i]=pair.first[i + First.size()]; 
//
            vec_Key[newKey].push_back(value);
        }
    }
//    Make the frequencies of Y accumulative per distinct key X values
    
    if (First.size() == 1) 
    {
        int s_size=Second.size();
        for (auto &pair1:single_key)
        {
            unsigned int  sum=0;
            for (auto &pair2:pair1.second)
            {
                sum+= pair2[s_size];
                pair2[s_size]=sum;
            }
        }
    }
    else if (First.size() > 1)
    {
        int s_size=Second.size();
        for (auto &pair1:vec_Key)
        {
            unsigned int  sum=0;
            for (auto &pair2:pair1.second)
            {
                sum+= pair2[s_size];
                pair2[s_size]=sum;
            }
        }
    }
}
#endif /* CSVREADER_H */