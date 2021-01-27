/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   queries.h
 * Author: anonymous
 *
 * Created on 18 June 2020, 08:35
 */

#ifndef QUERIES_H
#define QUERIES_H
#include <iostream>
#include <string>
#include <unordered_map>
#include<vector>
#include<set>

#include "csvReader.h"
typedef struct // Q3 of tpch
{
    //The default query is Q3 as an example to help declaring other queries
    
    vector<unsigned int> sample_sizes={ 1000}; // timing points in generating a sample. the last number is the actual sample size
//    static const string query="SELECT c_custkey, o_orderkey, l_linenumber FROM customer, orders, lineitem WHERE c_custkey = o_custkey AND l_orderkey = o_orderkey;";
    //tables and addresses
    unordered_map<string,string> tables={{"customer","/home/newdata/tpch10x/customer.csv"},{"orders","/home/newdata/tpch10x/orders.csv"},{"lineitem","/home/newdata/tpch10x/lineitem.csv"}};
    //tables and aliases. Currently, you should give aliases to the tables
    unordered_map<string,vector<string>> tableAliases={{"customer",{"c"}},{"orders", {"o"}},{"lineitem", {"l"}}};
    //tables and primary keys
    unordered_map<string,string> PKs={{"c", "custkey"},{"o", "orderkey"},{"l", ""}};
    //tables and projections. all the attributes that should be in join result
    unordered_map<string,set <string>> discreteProjections={{"c",{"custkey"}},{"o",{"orderkey"}},{"l",{"linenumber"}}};
    //table pairs and the join attribute between them. If composite, declare two different rows
    vector <vector<string>> predicates = {{"c","o", "custkey"},{"l","o","orderkey"}};
    //elimination order of the skeleton. If it is empty, the elimination algorithm will be used
    unordered_map<string,unsigned int> eliminationOrder={{"orderkey",0},{"custkey",1}};
    //use att aliases only for duplicated variables
    unordered_map<string,unordered_map <string,string>> attAliases={};  //per table, per att
    //output sample address
    string sampleOutAdd="samples/Q3/small/Q3_sample.csv";
    string statisticsAdd="samples/Q3/small/Q3_statist.txt";
    string name="Q3";
    unordered_map<string,int> pulledoutAtt={}; //to make cyclic joins acyclic. this also shows the elimination order of the pulled out variables
    pair<string,string> pulledoutEdge;// for algorithm 1, removing the edge to make the query acyclic
    unordered_map<string,int> sizes={};// to keep the sizes of the tables if available. if you have the sizes of the tables, insert below to make reading faster with buffer
}Query;

#endif /* QUERIES_H */
