
/* 
 * File:   main.cpp
 * Author: anonymous
 *
 * Created on 16 June 2020, 15:21
 */

#include <cstdlib>
#include <myGraph.h>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <queries.h>
#include <PGM.h>
#include<set>
#include<sampler.h>
#include <sys/resource.h>
#include <dirent.h>
#include <errno.h>
#include <algorithm>
#include<string>

using namespace std;


char* getCmdOption(char ** begin, char ** end, const std::string & option)
{
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}
bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
    return std::find(begin, end, option) != end;
}
int main(int argc, char** argv) {
    string fileAdd="/media/u1880698/2TB/data/Twitter/small/";
//    string fileAdd="/media/u1880698/2TB/data/TPCH/tpch1x/";
//    string fileAdd="/media/u1880698/2TB/data/exactJoin/tpch/small/";
//    string fileAdd="/media/u1880698/2TB/data/exactJoin/JOB/20Percent/";
//    string fileAdd="/media/u1880698/2TB/data/JOB/mainData/allTests/SF5/";
    string outAdd="/home/u1880698/NetBeansProjects/joinPGM/samples/Q3/1x/";
    if(cmdOptionExists(argv, argv+argc, "--help"))
    {
        cout<<"-input \t\t the address for the tables: Note, the tables should be in .csv format separating the attributes by '|' and should include the headers \n"<<endl;
        cout<<"-output \t the output address to store the running times and the generated sample\n"<<endl;
        cout<<"-query \t\t the query to run. The query should be already defined in main.cpp \n"<<endl;
        cout<<"-algorithm \t for the cyclic queries this should be 1, otherwise 0 \n"<<endl;
        cout<<"-sample \t the sample size to be generated \n"<<endl;
        cout<<"q3, qx and qyyy are the pre-defined queries for Q3, QX and QY on TPC-h"<<endl;
        cout<<"qf, qttt, qsss are the predefined queries for QF, QT and QS on twitter data"<<endl;
        cout<<"sh1 and sh333 are the predefined queries for QDS1 and QDS2 on TPC-DS"<<endl;
        cout<<"q16b is the predefined query for Q16b in JOB"<<endl;
        cout<<"qttt, qsss, qyyy and sh333 are the cyclic queries which the chosen algorithm for them should be 1"<<endl;
        exit(1);
    }
    if(cmdOptionExists(argv, argv+argc, "-input"))
        fileAdd = getCmdOption(argv, argv + argc, "-input");
    if(cmdOptionExists(argv, argv+argc, "-output"))
    {
        outAdd = getCmdOption(argv, argv + argc, "-output");
    
        DIR* dir = opendir(getCmdOption(argv, argv + argc, "-output"));
        if (!dir) 
        {
            cout<<"Output Directory does not exist"<<endl;
            exit(1);
        }
    }
    ////////////////////////////////////////////////////////////////// Query 3 //
    Query q3;
    q3.tables={{"customer",fileAdd+"customer.csv"},{"orders",fileAdd+"orders.csv"},{"lineitem",fileAdd+"lineitem.csv"}};
    q3.tableAliases= {{"customer",{"c"}},{"orders", {"o"}},{"lineitem", {"l"}}};
    q3.predicates={{"c","o", "custkey"},{"l","o","orderkey"}};
    q3.discreteProjections={{"c",{"custkey"}},{"o",{"orderkey"}},{"l",{"linenumber"}}};
    q3.eliminationOrder={{"orderkey",0},{"custkey",1}};
    q3.sample_sizes={ 1000};
    q3.sampleOutAdd=outAdd+"Q3_sample.csv";
    q3.statisticsAdd=outAdd+"Q3_statist.txt";
    q3.name="Q3";
    q3.attAliases={};

    ////////////////////////////////////////////////////////////////// Query X //
    Query qx;

    qx.tables={{"customer",fileAdd+"customer.csv"},{"orders",fileAdd+"orders.csv"},{"lineitem",fileAdd+"lineitem.csv"},{"nation",fileAdd+"nation.csv"},{"supplier",fileAdd+"supplier.csv"}};
    qx.tableAliases= {{"customer", {"c"}},{"orders", {"o"}},{"lineitem", {"l"}},{"nation",{"n"}},{ "supplier", {"s"}}};
    qx.predicates={{"c","o", "custkey"},{"l","o","orderkey"},{"n","s", "nationkey"},{"s","c","nationkey"}};
    qx.discreteProjections={{"c",{"custkey"}},{"o",{"orderkey"}},{"l",{"linenumber"}},{"n", {"nationkey"}},{"s", {"suppkey"}}};
    qx.eliminationOrder={{"orderkey",0},{"nationkey",2},{"custkey",1}};
    qx.sample_sizes={ 1000};
    qx.sampleOutAdd=outAdd+"QX_sample.csv";
    qx.statisticsAdd=outAdd+"QX_statist.txt";
    qx.name="QX";
    qx.attAliases={};
    
    ////////////////////////////////////////////////////////////////// Query Y //
//    Query qy;
//    qy.tables={{"customer",fileAdd+"customer.csv"},{"orders",fileAdd+"orders.csv"},{"lineitem",fileAdd+"lineitem.csv"},{"supplier",fileAdd+"supplier.csv"}};
//    qy.tableAliases= {{"customer", {"c1","c2"}},{"orders", {"o1","o2"}},{"lineitem", {"l1","l2"}},{ "supplier", {"s"}}};
//    //use att aliases only for duplicated variables
////    qy.attAliases= {{"c1",{{"custkey","custkey1"}}},{"custkey2","custkey"},{"orderkey1","orderkey"},{"orderkey2","orderkey"},{"linenumber1","linenumber"},{"linenumber2","linenumber"}};// only for duplicated atts   
//    qy.attAliases= {{"c1",{{"custkey1","custkey"}}},{"c2",{{"custkey2","custkey"}}},{"o1",{{"orderkey1","orderkey"},{"custkey1","custkey"}}},{"o2",{{"orderkey2","orderkey"},{"custkey2","custkey"}}},{"l1",{{"linenumber1","linenumber"},{"orderkey1","orderkey"}}},{"l2",{{"linenumber2","linenumber"},{"orderkey2","orderkey"}}}};
//    
//    //predicates should contain the aliases for tables and attributes if any 
//    qy.predicates={{"c1","o1", "custkey1"},{"l1","o1","orderkey1"},{"l1","l2", "partkey"},{"l2","o2", "orderkey2"},{"o2","c2","custkey2"},{"c1","s", "nationkey"},{"s","c2", "nationkey"}};
//    qy.discreteProjections={{"c1",{"custkey1"}},{"o1",{"orderkey1"}},{"l1",{"linenumber1"}},{"c2",{"custkey2"}},{"o2",{"orderkey2"}},{"l2",{"linenumber2"}},{"s", {"suppkey"}}};
//    //if the node names of the skeleton is not clear, just leave it empty
//    qy.eliminationOrder={{"orderkey1",2},{"orderkey2",3},{"custkey1",0},{"custkey2",1},{"partkey",4},{"nationkey",5}};
//    qy.sample_sizes={ 1000};
//    qy.sampleOutAdd=outAdd+"QY_sample.csv";
//    qy.statisticsAdd=outAdd+"QY_statist.txt";
//    qy.name="QY";

    ////////////////////////////////////////////////////////////////// Query SH1 on TPCDS //
    Query sh1;
    sh1.tables={{"web_sales",fileAdd+"web_sales.csv"},{"web_returns",fileAdd+"web_returns.csv"},{"store_sales",fileAdd+"store_sales.csv"},{"store_returns",fileAdd+"store_returns.csv"},{"catalog_sales",fileAdd+"catalog_sales.csv"},{"catalog_returns",fileAdd+"catalog_returns.csv"}};
    sh1.tableAliases= {{"web_sales", {"WS"}},{"web_returns", {"WR"}},{"store_sales", {"SS"}},{ "store_returns", {"SR"}},{"catalog_sales", {"CS"}},{ "catalog_returns", {"CR"}}};
    //use att aliases only for duplicated variables
    sh1.attAliases= {};// only for duplicated atts   
    //predicates should contain the aliases for tables and attributes if any 
    sh1.predicates={{"WS","WR", "web_page_sk"},{"CS","CR","catalog_page_sk"},{"SS","SR", "store_sk"},{"SS","CS", "promo_sk"},{"SS","WS","promo_sk"}};
    sh1.discreteProjections={{"WR",{"reason_sk"}},{"CR",{"reason_sk"}},{"SR",{"reason_sk"}},{"WS",{"promo_sk"}},{"SS",{"promo_sk"}},{"CS",{"promo_sk"}}};
    //if the node names of the skeleton is not clear, just leave it empty
    sh1.eliminationOrder={{"web_page_sk",0},{"catalog_page_sk",1},{"store_sk",2},{"promo_sk",3}};
    sh1.sample_sizes={ 1000};
    sh1.sampleOutAdd=outAdd+"SH1_sample.csv";
    sh1.statisticsAdd=outAdd+"SH1_statist.txt";
    sh1.name="SH1";
    
    ////////////////////////////////////////////////////////////////// Query SH2 2_cycles on TPCDS //
//    Query sh2;
//    sh2.tables={{"web_sales",fileAdd+"web_sales.csv"},{"web_returns",fileAdd+"web_returns.csv"},{"store_sales",fileAdd+"store_sales.csv"},{"store_returns",fileAdd+"store_returns.csv"},{"catalog_sales",fileAdd+"catalog_sales.csv"},{"catalog_returns",fileAdd+"catalog_returns.csv"}};
//    sh2.tableAliases= {{"web_sales", {"WS"}},{"web_returns", {"WR"}},{"store_sales", {"SS"}},{ "store_returns", {"SR"}},{"catalog_sales", {"CS"}},{ "catalog_returns", {"CR"}}};
//    //use att aliases only for duplicated variables
//    sh2.attAliases= {};// only for duplicated atts   
//    //predicates should contain the aliases for tables and attributes if any 
//    sh2.predicates={{"CR","WR", "reason_sk"},{"SR","WR", "reason_sk"},{"WS","WR", "web_page_sk"},{"CS","CR","catalog_page_sk"},{"SS","SR", "store_sk"},{"SS","CS", "promo_sk"},{"SS","WS","promo_sk"}};
//    sh2.discreteProjections={{"WR",{"reason_sk"}},{"CR",{"reason_sk"}},{"SR",{"reason_sk"}},{"WS",{"promo_sk"}},{"SS",{"promo_sk"}},{"CS",{"promo_sk"}}};
//    //if the node names of the skeleton is not clear, just leave it empty
//    sh2.eliminationOrder={{"web_page_sk",1},{"catalog_page_sk",0},{"store_sk",2},{"promo_sk",3},{"reason_sk",4}};
//    sh2.sample_sizes={ 1000};
//    sh2.sampleOutAdd=outAdd+"SH2_sample.csv";
//    sh2.statisticsAdd=outAdd+"SH2_statist.txt";
//    sh2.name="SH2";
//    
//     ////////////////////////////////////////////////////////////////// Query SH3 1_cycle on TPCDS //
//    Query sh3;
//    sh3.tables={{"web_sales",fileAdd+"web_sales.csv"},{"web_returns",fileAdd+"web_returns.csv"},{"catalog_sales",fileAdd+"catalog_sales.csv"},{"catalog_returns",fileAdd+"catalog_returns.csv"}};
//    sh3.tableAliases= {{"web_sales", {"WS"}},{"web_returns", {"WR"}},{"catalog_sales", {"CS"}},{ "catalog_returns", {"CR"}}};
//    //use att aliases only for duplicated varables
//    sh3.attAliases= {};// only for duplicated atts   
//    //predicates should contain the aliases for tables and attributes if any 
//    sh3.predicates={{"CR","WR", "reason_sk"},{"WS","WR", "web_page_sk"},{"CS","CR","catalog_page_sk"},{"CS","WS","promo_sk"}};
//    sh3.discreteProjections={{"WR",{"reason_sk"}},{"CR",{"reason_sk"}},{"WS",{"promo_sk"}},{"CS",{"promo_sk"}}};
//    //if the node names of the skeleton is not clear, just leave it empty
//    sh3.eliminationOrder={{"web_page_sk",1},{"catalog_page_sk",0},{"promo_sk",3},{"reason_sk",2}};
//    sh3.sample_sizes={ 1000};
//    sh3.sampleOutAdd=outAdd+"SH3_sample.csv";
//    sh3.statisticsAdd=outAdd+"SH3_statist.txt";
//    sh3.name="SH3";
    ////////////////////////////////////////////////////////////////// Query T on Twitter //
//    Query qt;
//    qt.tables={{"tpop",fileAdd+"t_pop.csv"},{"tall",fileAdd+"t_all.csv"}};
//    qt.tableAliases= {{"tpop", {"A"}},{"tall", {"B","C"}}};
//    qt.attAliases= {{"A",{{"a","dst"},{"c","src"}}},{"B",{{"a","src"},{"b","dst"}}},{"C",{{"b","src"},{"c","dst"}}} };
//    
//    //predicates should contain the aliases for tables and attributes if any 
//    qt.predicates={{"A","B", "a"},{"B","C","b"},{"C","A", "c"}};
//    qt.discreteProjections={{"A",{"a","c"}},{"B",{"b","a"}},{"C",{"c","b"}}};
//    //if the node names of the skeleton is not clear, just leave it empty
//    qt.eliminationOrder={{"a",1},{"b",2},{"c",0}};
//    qt.sample_sizes={ 1000};
//    qt.sampleOutAdd=outAdd+"QT_sample.csv";
//    qt.statisticsAdd=outAdd+"QT_statist.txt";
//    qt.name="QT";
//    ////////////////////////////////////////////////////////////////// Query S on Twitter //
//    Query qs;
//    qs.tables={{"tpop",fileAdd+"t_pop.csv"},{"tall",fileAdd+"t_all.csv"}};
//    qs.tableAliases= {{"tpop", {"A"}},{"tall", {"B","C","D"}}};
//    qs.attAliases= {{"A",{{"a","dst"},{"c","src"}}},{"B",{{"a","src"},{"b","dst"}}},{"C",{{"b","src"},{"d","dst"}}},{"D",{{"d","src"},{"c","dst"}}} };
//    
//    //predicates should contain the aliases for tables and attributes if any 
//    qs.predicates={{"A","B", "a"},{"B","C","b"},{"C","D", "d"},{"A","D", "c"}};
//    qs.discreteProjections={{"A",{"a","c"}},{"B",{"b","a"}},{"C",{"d","b"}},{"D",{"d","c"}}};
//    //if the node names of the skeleton is not clear, just leave it empty
//    qs.eliminationOrder={{"d",2},{"a",3},{"b",1},{"c",0}};
//    qs.sample_sizes={ 1000};
//    qs.sampleOutAdd=outAdd+"Qs_sample.csv";
//    qs.statisticsAdd=outAdd+"Qs_statist.txt";
//    qs.name="QS";
     ////////////////////////////////////////////////////////////////// Query F on Twitter //
 
    Query qf;
    qf.tables={{"tpop",fileAdd+"t_pop.csv"},{"tall",fileAdd+"t_all.csv"}};
    qf.tableAliases= {{"tpop", {"A","D"}},{"tall", {"B","C"}}};
    qf.attAliases= {{"A",{{"a","src"}}},{"B",{{"a","src"}}},{"C",{{"b","src"},{"a","dst"}}},{"D",{{"b","src"}}} };
    
    //predicates should contain the aliases for tables and attributes if any 
    qf.predicates={{"A","B", "a"},{"A","C","a"},{"C","D", "b"}};
    qf.discreteProjections={{"A",{"a","dst"}},{"B",{"a","dst"}},{"C",{"b","a"}},{"D",{"b","dst"}}};
    //if the node names of the skeleton is not clear, just leave it empty
    qf.eliminationOrder={{"a",1},{"b",0}};
    qf.sample_sizes={ 1000};
    qf.sampleOutAdd=outAdd+"QF_sample.csv";
    qf.statisticsAdd=outAdd+"QF_statist.txt";
    qf.name="QF";
    ////////////////////////////////////////////////////////////////// Query YY //
//    Query qyy ;
//    qyy.tables={{"customer",fileAdd+"customer.csv"},{"orders",fileAdd+"orders.csv"},{"lineitem",fileAdd+"lineitem.csv"},{"supplier",fileAdd+"supplier.csv"}};
//    qyy.tableAliases= {{"customer", {"c1","c2"}},{"orders", {"o1","o2"}},{"lineitem", {"l1","l2"}},{ "supplier", {"s"}}};
//    //use att aliases only for duplicated variables
////    qy.attAliases= {{"c1",{{"custkey","custkey1"}}},{"custkey2","custkey"},{"orderkey1","orderkey"},{"orderkey2","orderkey"},{"linenumber1","linenumber"},{"linenumber2","linenumber"}};// only for duplicated atts   
//    qyy.attAliases= {{"c1",{{"custkey1","custkey"}}},{"c2",{{"custkey2","custkey"}}},{"o1",{{"orderkey1","orderkey"},{"custkey1","custkey"}}},{"o2",{{"orderkey2","orderkey"},{"custkey2","custkey"}}},{"l1",{{"linenumber1","linenumber"},{"orderkey1","orderkey"}}},{"l2",{{"linenumber2","linenumber"},{"orderkey2","orderkey"}}}};
//    
//    //predicates should contain the aliases for tables and attributes if any 
//    qyy.predicates={{"c1","o1", "custkey1"},{"l1","o1","orderkey1"},{"l1","l2", "partkey"},{"l2","o2", "orderkey2"},{"o2","c2","custkey2"},{"c1","s", "nationkey"},{"s","c2", "nationkey"}};
//    qyy.discreteProjections={{"c1",{"custkey1"}},{"o1",{"orderkey1"}},{"l1",{"linenumber1"}},{"c2",{"custkey2"}},{"o2",{"orderkey2"}},{"l2",{"linenumber2"}},{"s", {"suppkey"}}};
//    //if the node names of the skeleton is not clear, just leave it empty
//    qyy.eliminationOrder={{"orderkey1",2},{"orderkey2",4},{"custkey1",1},{"nationkey",0},{"partkey",3}};// the elimination order should be an acyclic join if we use pulled out attr
//    qyy.sample_sizes={ 1000};
//    qyy.sampleOutAdd=outAdd+"QY_sample.csv";
//    qyy.statisticsAdd=outAdd+"QY_statist.txt";
//    qyy.name="QYY";
//    qyy.pulledoutAtt={{"custkey2",0}};  // the elimination order should be chain in this case
//    ////////////////////////////////////////////////////////////////// Query SH33 1_cycle on TPCDS //
//    Query sh33;
//    sh33.tables={{"web_sales",fileAdd+"web_sales.csv"},{"web_returns",fileAdd+"web_returns.csv"},{"catalog_sales",fileAdd+"catalog_sales.csv"},{"catalog_returns",fileAdd+"catalog_returns.csv"}};
//    sh33.tableAliases= {{"web_sales", {"WS"}},{"web_returns", {"WR"}},{"catalog_sales", {"CS"}},{ "catalog_returns", {"CR"}}};
//    //use att aliases only for duplicated varables
//    sh33.attAliases= {};// only for duplicated atts   
//    //predicates should contain the aliases for tables and attributes if any 
//    sh33.predicates={{"CR","WR", "reason_sk"},{"WS","WR", "web_page_sk"},{"CS","CR","catalog_page_sk"},{"CS","WS","promo_sk"}};
//    sh33.discreteProjections={{"WR",{"reason_sk"}},{"CR",{"reason_sk"}},{"WS",{"promo_sk"}},{"CS",{"promo_sk"}}}; //if the node names of the skeleton is not clear, just leave it empty
//    sh33.eliminationOrder={{"reason_sk",0},{"catalog_page_sk",2},{"promo_sk",1}};
//    sh33.sample_sizes={ 1000000};
//    sh33.sampleOutAdd=outAdd+"SH33_sample.csv";
//    sh33.statisticsAdd=outAdd+"SH33_statist.txt";
//    sh33.name="SH33";
//    sh33.pulledoutAtt={{"web_page_sk",0}};
//    ////////////////////////////////////////////////////////////////// Query T on Twitter //
//    Query qtt;
//    qtt.tables={{"tpop",fileAdd+"t_pop.csv"},{"tall",fileAdd+"t_all.csv"}};
//    qtt.tableAliases= {{"tpop", {"A"}},{"tall", {"B","C"}}};
//    qtt.attAliases= {{"A",{{"a","dst"},{"c","src"}}},{"B",{{"a","src"},{"b","dst"}}},{"C",{{"b","src"},{"c","dst"}}} };
//    
//    //predicates should contain the aliases for tables and attributes if any 
//    qtt.predicates={{"A","B", "a"},{"B","C","b"},{"C","A", "c"}};
//    qtt.discreteProjections={{"A",{"a","c"}},{"B",{"b","a"}},{"C",{"c","b"}}};
//    //if the node names of the skeleton is not clear, just leave it empty
//    qtt.eliminationOrder={{"b",1},{"a",0}};
//    qtt.sample_sizes={ 1000};
//    qtt.sampleOutAdd=outAdd+"QT_sample.csv";
//    qtt.statisticsAdd=outAdd+"QT_statist.txt";
//    qtt.name="QTT";
//    qtt.pulledoutAtt={{"c",0}};
//    ////////////////////////////////////////////////////////////////// Query S on Twitter //
//    Query qss;
//    qss.tables={{"tpop",fileAdd+"t_pop.csv"},{"tall",fileAdd+"t_all.csv"}};
//    qss.tableAliases= {{"tpop", {"A"}},{"tall", {"B","C","D"}}};
//    qss.attAliases= {{"A",{{"a","dst"},{"c","src"}}},{"B",{{"a","src"},{"b","dst"}}},{"C",{{"b","src"},{"d","dst"}}},{"D",{{"d","src"},{"c","dst"}}} };
//    
//    //predicates should contain the aliases for tables and attributes if any 
//    qss.predicates={{"A","B", "a"},{"B","C","b"},{"C","D", "d"},{"A","D", "c"}};
//    qss.discreteProjections={{"A",{"a","c"}},{"B",{"b","a"}},{"C",{"d","b"}},{"D",{"d","c"}}};
//    //if the node names of the skeleton is not clear, just leave it empty
//    qss.eliminationOrder={{"b",2},{"d",0},{"a",1}};
//    qss.sample_sizes={ 1000};
//    qss.sampleOutAdd=outAdd+"QSS_sample.csv";
//    qss.statisticsAdd=outAdd+"QSS_statist.txt";
//    qss.name="QSS";
//    qss.pulledoutAtt={{"c",0}};
    ////////////////////////////////////////////////////////////////// Query YYY edge eliminated to be acyclic //
    Query qyyy;
    qyyy.tables={{"orders",fileAdd+"orders.csv"},{"customer",fileAdd+"customer.csv"},{"lineitem",fileAdd+"lineitem.csv"},{"supplier",fileAdd+"supplier.csv"}};
    qyyy.tableAliases= {{"customer", {"c1","c2"}},{"orders", {"o1","o2"}},{"lineitem", {"l1","l2"}},{ "supplier", {"s"}}};
    //use att aliases only for duplicated variables
//    qy.attAliases= {{"c1",{{"custkey","custkey1"}}},{"custkey2","custkey"},{"orderkey1","orderkey"},{"orderkey2","orderkey"},{"linenumber1","linenumber"},{"linenumber2","linenumber"}};// only for duplicated atts   
    qyyy.attAliases= {{"c1",{{"custkey1","custkey"}}},{"c2",{{"custkey2","custkey"}}},{"o1",{{"orderkey1","orderkey"},{"custkey1","custkey"}}},{"o2",{{"orderkey2","orderkey"},{"custkey2","custkey"}}},{"l1",{{"linenumber1","linenumber"},{"orderkey1","orderkey"}}},{"l2",{{"linenumber2","linenumber"},{"orderkey2","orderkey"}}}};
    
    //predicates should contain the aliases for tables and attributes if any 
    qyyy.predicates={{"c1","o1", "custkey1"},{"l1","o1","orderkey1"},{"l1","l2", "partkey"},{"l2","o2", "orderkey2"},{"o2","c2","custkey2"},{"c1","s", "nationkey"},{"s","c2", "nationkey"}};
    qyyy.discreteProjections={{"c1",{"custkey1"}},{"o1",{"orderkey1"}},{"l1",{"linenumber1"}},{"c2",{"custkey2"}},{"o2",{"orderkey2"}},{"l2",{"linenumber2"}},{"s", {"suppkey"}}};
    //if the node names of the skeleton is not clear, just leave it empty
    qyyy.eliminationOrder={{"orderkey1",4},{"orderkey2",2},{"custkey1",5},{"custkey2",1},{"partkey",3},{"nationkey",0}};
//qyyy.eliminationOrder={{"orderkey1",1},{"orderkey2",3},{"custkey1",0},{"custkey2",4},{"partkey",2},{"nationkey",5}};
    qyyy.sample_sizes={ 1000};
    qyyy.sampleOutAdd=outAdd+"QY_sample.csv";
    qyyy.statisticsAdd=outAdd+"QY_statist.txt";
    qyyy.name="QYYY";
    qyyy.pulledoutEdge={"custkey1","nationkey"};
    // if you have the sizes of the tables, insert below to make reading faster with buffer
//    qyyy.sizes={{"customer",149999},{"orders",1499999},{"lineitem",6001214},{"supplier",9999}};
     ////////////////////////////////////////////////////////////////// Query SH3 1_cycle on TPCDS removed one edge //
    Query sh333;
    sh333.tables={{"web_sales",fileAdd+"web_sales.csv"},{"web_returns",fileAdd+"web_returns.csv"},{"catalog_sales",fileAdd+"catalog_sales.csv"},{"catalog_returns",fileAdd+"catalog_returns.csv"}};
    sh333.tableAliases= {{"web_sales", {"WS"}},{"web_returns", {"WR"}},{"catalog_sales", {"CS"}},{ "catalog_returns", {"CR"}}};
    //use att aliases only for duplicated varables
    sh333.attAliases= {};// only for duplicated atts   
    //predicates should contain the aliases for tables and attributes if any 
    sh333.predicates={{"CR","WR", "reason_sk"},{"WS","WR", "web_page_sk"},{"CS","CR","catalog_page_sk"},{"CS","WS","promo_sk"}};
    sh333.discreteProjections={{"WR",{"reason_sk"}},{"CR",{"reason_sk"}},{"WS",{"promo_sk"}},{"CS",{"promo_sk"}}};
    //if the node names of the skeleton is not clear, just leave it empty
    sh333.eliminationOrder={{"web_page_sk",1},{"catalog_page_sk",2},{"promo_sk",3},{"reason_sk",0}};
    sh333.sample_sizes={ 1000};
    sh333.sampleOutAdd=outAdd+"SH333_sample.csv";
    sh333.statisticsAdd=outAdd+"SH333_statist.txt";
    sh333.name="SH333";
    sh333.pulledoutEdge={"reason_sk","web_page_sk"};
    ////////////////////////////////////////////////////////////////// Query TTT on Twitter //
    Query qttt;
    qttt.tables={{"tpop",fileAdd+"t_pop.csv"},{"tall",fileAdd+"t_all.csv"}};
    qttt.tableAliases= {{"tpop", {"A"}},{"tall", {"B","C"}}};
    qttt.attAliases= {{"A",{{"a","dst"},{"c","src"}}},{"B",{{"a","src"},{"b","dst"}}},{"C",{{"b","src"},{"c","dst"}}} };
    
    //predicates should contain the aliases for tables and attributes if any 
    qttt.predicates={{"A","B", "a"},{"B","C","b"},{"C","A", "c"}};
    qttt.discreteProjections={{"A",{"a","c"}},{"B",{"b","a"}},{"C",{"c","b"}}};
    //if the node names of the skeleton is not clear, just leave it empty
    qttt.eliminationOrder={{"a",1},{"b",0},{"c",2}};
    qttt.sample_sizes={ 1000};
    qttt.sampleOutAdd=outAdd+"QTTT_sample.csv";
    qttt.statisticsAdd=outAdd+"QTT_statist.txt";
    qttt.name="QTTT";
    qttt.pulledoutEdge={"a","b"};
    
    
   ////////////////////////////////////////////////////////////////// Query SSS on Twitter //
    Query qsss;
    qsss.tables={{"tpop",fileAdd+"t_pop.csv"},{"tall",fileAdd+"t_all.csv"}};
    qsss.tableAliases= {{"tpop", {"A"}},{"tall", {"B","C","D"}}};
    qsss.attAliases= {{"A",{{"a","dst"},{"c","src"}}},{"B",{{"a","src"},{"b","dst"}}},{"C",{{"b","src"},{"d","dst"}}},{"D",{{"d","src"},{"c","dst"}}} };
    
    //predicates should contain the aliases for tables and attributes if any 
    qsss.predicates={{"A","B", "a"},{"B","C","b"},{"C","D", "d"},{"A","D", "c"}};
    qsss.discreteProjections={{"A",{"a","c"}},{"B",{"b","a"}},{"C",{"d","b"}},{"D",{"d","c"}}};
    //if the node names of the skeleton is not clear, just leave it empty
    qsss.eliminationOrder={{"d",3},{"a",0},{"b",1},{"c",2}};
    qsss.sample_sizes={ 1000};
    qsss.sampleOutAdd=outAdd+"QSS_sample.csv";
    qsss.statisticsAdd=outAdd+"QSS_statist.txt";
    qsss.name="QSSS";
    qsss.pulledoutEdge={"a","b"};
    
    //////////////////////////////////////////////////////////////////////////////Q16b of JOB
    
    Query q16b;
    q16b.tables={{"name",fileAdd+"name.csv"},{"aka_name",fileAdd+"aka_name.csv"},{"company_name",fileAdd+"company_name.csv"},{"cast_info",fileAdd+"cast_info.csv"},{"title",fileAdd+"title.csv"},{"movie_keyword",fileAdd+"movie_keyword.csv"},{"keyword",fileAdd+"keyword.csv"},{"movie_companies",fileAdd+"movie_companies.csv"}};
    q16b.tableAliases= {{"name", {"n"}},{"aka_name", {"an"}},{"cast_info", {"ci"}},{"company_name", {"cn"}},{"title", {"t"}},{"movie_keyword", {"mk"}},{"keyword", {"k"}},{"movie_companies", {"mc"}}};
    q16b.attAliases= {};
    
    //predicates should contain the aliases for tables and attributes if any 
    q16b.predicates={{"an","n", "person_id"},{"n","ci","person_id"},{"ci","t", "movie_id"},{"t","mk", "movie_id"},{"k","mk", "keyword_id"},{"t","mc", "movie_id"},{"mc","cn", "company_id"},{"an","ci", "person_id"},{"mc","ci", "movie_id"},{"mk","ci", "movie_id"},{"mc","mk", "movie_id"}};
    q16b.discreteProjections={};
    //if the node names of the skeleton is not clear, just leave it empty
    q16b.eliminationOrder={{"person_id",3},{"keyword_id",1},{"company_id",0},{"movie_id",2}};
    q16b.sample_sizes={ 1000};
    q16b.sampleOutAdd=outAdd+"Q16b_sample.csv";
    q16b.statisticsAdd=outAdd+"Q16b_statist.txt";
    q16b.name="Q16b";


    ///////////////////////////////////////////////////////////////////////////////////
    
    char * que="q3";
    if(cmdOptionExists(argv, argv+argc, "-query"))
        que = getCmdOption(argv, argv + argc, "-query");
    Query q;
    if(!strcmp(que, "q3")) 
        q= q3;
    else if(!strcmp(que, "qx"))
       q=qx;
//    else if(!strcmp(que, "qy"))
//        q=qy;
    else if(!strcmp(que, "sh1"))
       q=sh1;
//    else if(!strcmp(que, "sh2"))
//       q=sh2;
//    else if(!strcmp(que, "sh3"))
//        q=sh3;
//    else if(!strcmp(que, "qt"))
//        q=qt;
//    else if(!strcmp(que, "qs"))
//        q=qs;
    else if(!strcmp(que, "qf"))
        q=qf;
//    else if(!strcmp(que, "qyy"))
//        q=qyy;
//    else if(!strcmp(que, "sh33"))
//        q=sh33;
//    else if(!strcmp(que, "qtt"))
//        q=qtt;
//    else if(!strcmp(que, "qss"))
//        q=qss;
    else if(!strcmp(que, "qyyy"))
        q=qyyy;
    else if(!strcmp(que, "sh333"))
        q=sh333;
    else if(!strcmp(que, "qttt"))
        q=qttt;
    else if(!strcmp(que, "qsss"))
        q=qsss;
    else if(!strcmp(que, "q16b"))
        q=q16b;
    else
    {
        cout<<"The query has not been defined yet."<<endl;
        exit(1);
    }

    int alg=0; // to solve cycle problem// 0. main algorithm with triangulation. 1. removing an edge of the graph (splitting a table) 2. removing the attributes.
    if(cmdOptionExists(argv, argv+argc, "-algorithm"))
        alg = atoi(getCmdOption(argv, argv + argc, "-algorithm"));
    

    PGM pgm(&q);
    cout<<"====================================================================="<<"\n"<<endl;
    cout<<"Algorithm "<<alg<<" was selected."<<endl;
    
    cout<<"Started to build the query graph from scratch"<<endl;
    cout<<"---------------------------------------------------------------------"<<endl;
    pgm.buildGraph(alg);

    cout<<"\n"<<"Time for the building graph is:\t"<<pgm.timing_seconds.readAndMakeFreqs<<endl;
    cout<<"Building the graph finished"<<endl;cout.flush();
    
    pgm.inferDP();
    
    cout<<"Inference finished"<<"\n" << endl;cout.flush();
    cout<<"Started to generate the samples"<<endl;
    cout<<"---------------------------------------------------------------------"<<endl;
    
    if(cmdOptionExists(argv, argv+argc, "-sample"))
    {
        
        // warming up
        pgm.query.sample_sizes[0]=10000;
        
        if(alg==0)
            pgm.sample();
        else
            pgm.sampleWitPulledOut();
        
        //main run
        pgm.query.sample_sizes[0]= atoi(getCmdOption(argv, argv + argc, "-sample"));
        
        const clock_t begin_time = clock();
        
        if(alg==0)
            pgm.sample();
        else
            pgm.sampleWitPulledOut();
        
        
        pgm.timing_seconds.sample.push_back({pgm.query.sample_sizes[0],float( clock () - begin_time ) /  CLOCKS_PER_SEC});
        cout <<"Sampling time with sample_size: "<<pgm.query.sample_sizes[0]<<"\t is: " <<float( clock () - begin_time ) /  CLOCKS_PER_SEC <<"\n";
        struct rusage r_usage;
        getrusage(RUSAGE_SELF,&r_usage);
        std::cout<<"\nPeak Memory usage is: "<<r_usage.ru_maxrss/1024<<"MB"<<std::endl;


        // writing the statistics in a file
        ofstream myfile(pgm.query.statisticsAdd);
        myfile<<"Statistics for query "<<pgm.query.name<<", all numbers are in seconds\n"<<endl;
        myfile<<"Building the graph including file readings is:\t"<<pgm.timing_seconds.readAndMakeFreqs<<endl;
        myfile<<"Inference on the probabilistic graph model is:\t"<<pgm.timing_seconds.inference<<endl;
        myfile<<"Making samples with different sizes:\t"<<endl;
        for (auto ss:pgm.timing_seconds.sample)
            myfile<<"\t"<<"Sample size: "<<ss.first<<"\t Generation time: "<<ss.second<<endl;
        myfile<<"Writing the samples to the disk took:\t"<<pgm.timing_seconds.writingCSV<<endl;
        myfile<<"Peak memory usage for the process is: \t"<<r_usage.ru_maxrss/1024<<"MB"<<endl;
        myfile.close();
    }
    else
    {
        auto sam={1000,1000,10000,100000,1000000};
//        auto sam={1000};
        for(auto s:sam)
        {
            const clock_t begin_time = clock(); 
            pgm.query.sample_sizes[0]=s;
            if(alg==0)
                pgm.sample();
            else
                pgm.sampleWitPulledOut();
            pgm.timing_seconds.sample.push_back({s,float( clock () - begin_time ) /  CLOCKS_PER_SEC});
            cout <<"Sampling time with sample_size: "<<s<<"\t is: " <<float( clock () - begin_time ) /  CLOCKS_PER_SEC <<"\n";

            
            struct rusage r_usage;
            getrusage(RUSAGE_SELF,&r_usage);
            cout<<"\nPeak Memory usage is: "<<r_usage.ru_maxrss/1024<<"MB"<<endl;

            // writing the statistics in a file
            ofstream myfile(pgm.query.statisticsAdd+to_string(q.sample_sizes[0]));
            myfile<<"Statistics for query "<<pgm.query.name<<", all numbers are in seconds\n"<<endl;
            myfile<<"Building the graph including file readings is:\t"<<pgm.timing_seconds.readAndMakeFreqs<<endl;
            myfile<<"Inference on the probabilistic graph model is:\t"<<pgm.timing_seconds.inference<<endl;
            myfile<<"Making samples with different sizes:\t"<<endl;
            for (auto ss:pgm.timing_seconds.sample)
                myfile<<"\t"<<"Sample size: "<<ss.first<<"\t Generation time: "<<ss.second<<endl;
            myfile<<"Writing the samples to the disk took:\t"<<pgm.timing_seconds.writingCSV<<endl;
            myfile<<"Peak memory usage for the process is: \t"<<r_usage.ru_maxrss/1024<<"MB"<<endl;
            myfile.close();
        }
        
    }

    return 0;
}
