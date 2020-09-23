/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   sampler.h
 * Author: u1880698
 *
 * Created on 20 June 2020, 08:13
 */

#ifndef SAMPLER_H
#define SAMPLER_H

// C++ implementation of the approach 
#include <bits/stdc++.h> 
#include <random>
#include <chrono>
#include <stdlib.h>
#include <time.h>
using namespace std; 


double random_zero_to_one(){
    /*
    std::mt19937_64 rng;
    // initialize the random number generator with time-dependent seed
    uint64_t timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::seed_seq ss{uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed>>32)};
    rng.seed(ss);
    // initialize a uniform distribution between 0 and 1
    std::uniform_real_distribution<double> unif(0, 1);
    // ready to generate random numbers
    const int nSimulations = 10;
    double currentRandomNumber = unif(rng);
    return currentRandomNumber;
	/**/
    
    return ((double) rand() / (RAND_MAX)) ;
}

unsigned long int random_range(unsigned long int n1, unsigned long int n2)
{
    
    return n1 + ( std::rand() % ( n2 - n1 + 1 ) );
}
unsigned int random_range1(unsigned  int n1, unsigned  int n2)
{
    
    return n1 + ( std::rand() % ( n2 - n1 + 1 ) );
}
// returns the index
int random_sample(double distribution[],unsigned int size){
        //check if the array adds up to 1
    double sum = 1.0;
	/*
    for(int i=0; i<size; i++){
        sum += distribution[i];
    }
	/**/
    if(sum == 1.0){//valid
        // will generate [0,1)
        double rnum = random_zero_to_one();
        
        for(int i=0; i< size; i++){
            if (rnum < distribution[i]) return i;
            else{
                rnum -= distribution[i];
            }
        }
    }else{//didn't get a valid distribution
		//return -1 indicate that input not valid
        return(-1);
    }

}

vector<int> random_sample_n(vector <int> population, int n)
{
    // sample n values with shuffeling without weights
    
//    std::vector <int> samplingIndex;
//   for (int i = 0; i < 1000000; ++i) { samplingIndex.push_back(i); } 
//   std::srand(50); 
//   std::random_shuffle(samplingIndex.begin(), samplingIndex.end());
}

unsigned int binary_CDF_sample(vector<pair<unsigned int,unsigned long int>> &values)
{
        unsigned int last=values.size()-1;
        unsigned int midPoint;
        unsigned int first=0;
        unsigned long int number= random_range(0,values[last].second);

        while(first<last)
        {
                midPoint = (first+last)/2;
                if(number > values[midPoint].second)
                        first = midPoint + 1;
                else
                        last = midPoint;
        }
        return values[first].first;
}
vector<unsigned int> binary_CDF_sample_NonSkeleton_multiple_output(vector<vector<unsigned int>> &values, int varSize=1)
{
    
        unsigned int last=values.size()-1;
        unsigned int midPoint;
        unsigned int first=0;
        unsigned int number= random_range1(0,values[last][varSize-1]);

        while(first<last)
        {
                midPoint = (first+last)/2;
                if(number > values[midPoint][varSize-1])
                        first = midPoint + 1;
                else
                        last = midPoint;
        }
        return values[first];
}
unsigned int binary_CDF_sample_NonSkeleton_single_output(vector<vector<unsigned int>> &values)
{
    
        unsigned int last=values.size()-1;
        unsigned int midPoint;
        unsigned int first=0;
        unsigned int number= random_range1(0,values[last][1]);

        while(first<last)
        {
                midPoint = (first+last)/2;
                if(number > values[midPoint][1])
                        first = midPoint + 1;
                else
                        last = midPoint;
        }
        return values[first][0];
}


#endif /* SAMPLER_H */

