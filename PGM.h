/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   PGM.h
 * Author: anonymous
 *
 * Created on 20 June 2020, 09:16
 */

#ifndef PGM_H
#define PGM_H

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <iostream>
# include<myGraph.h>
#include<queries.h>
#include<set>
#include <random>
#include<boost/functional/hash.hpp>
#include <csvReader.h>
using namespace std;

class PGM 
{
    
    
     
  public:
    Query query;
    myGraph skeleton;//declare the skeleton and nonSkeleton graphs
    myGraph pulledOutSkeleton; //To make cyclic joins acyclic
    string lastNode;
    double mr=0;
    int alg=0;
    struct timing{
        double readAndMakeFreqs;
        double inference;
        vector<pair<int,double>> sample;
        double writingCSV;
    }timing_seconds;
    
    PGM (Query *q) 
    {
        query=*q;
        cout<<"Query "<<query.name<<" is being processed."<<endl;
    }
    vector<string> pulledOutAttsconditions; // for pulled out cyclic version
    unordered_map_sequence<vector<unsigned int>, unsigned long int> pulledOutRejectionRates;
    unordered_map<unsigned int, unordered_map<unsigned int, unsigned long int>> pulledOutConditionalRejectionRate;
    vector<pair<unsigned int,unsigned long int>> lastCDF;
    template <typename T>
    set<T> getUnion(const std::set<T>& a, const std::set<T>& b);
    void buildGraph(int alg);
    void inferDP();
    void sample();
    void sampleWitPulledOut();
    void multiplyMaps (unordered_map<unsigned int,unsigned long int> &m1, unordered_map<unsigned int,unsigned long int> &m2,unordered_map<unsigned int,unsigned long int> &output); // Is there a better way???????????????
    void writeFileCSV(std::string fname, vector<unordered_map<string,unsigned int>> &m, vector<string> header);

};

template <typename T>
set<T> PGM::getUnion(const std::set<T>& a, const std::set<T>& b)
    {
      std::set<T> result = a;
      result.insert(b.begin(), b.end());
      return result;
    }

//finds the difference between two vectors
vector<string> vec_substract(vector<string> v1, vector<string> v2)
{
    sort(v1.begin(),v1.end());
    sort(v2.begin(),v2.end());
    vector<string> v3;
    set_difference(v1.begin(),v1.end(),v2.begin(),v2.end(),back_inserter(v3));
    return v3;
}
vector<string> vec_union(vector<string> v1,vector<string> v2)
{
    vector<string> v3;

    sort(v1.begin(), v1.end());
    sort(v2.begin(), v2.end());

    set_union(v1.begin(),v1.end(),
                          v2.begin(),v2.end(),
                          back_inserter(v3));
    return v3;
}


void PGM::buildGraph (int alg1) 
{
    alg=alg1;
    const clock_t begin_time2 = clock();
    
    //build alias reverse map
    unordered_map<string , string> tableAliasRev;
    for(const auto &pair1 : query.tableAliases) 
    {
        for (auto const &als:pair1.second)
            tableAliasRev[als]=pair1.first;
    }
    //get the graph nodes from predicates per table
    unordered_map<string,set <string>> predicates;
    vector< vector<string> >::iterator row;
    vector<string>::iterator col;
    for (row = query.predicates.begin(); row != query.predicates.end(); row++) 
    {
        col = row->begin();
        col++;
        col++;
        for (; col != row->end(); col++) // for more than one predicate variable between tables
        {
            //the predicate should be added to the att list of the both tables
            predicates[row[0][0]].insert(*col);
            predicates[row[0][1]].insert(*col);

        }
    }
    
    //insert the nodes into the skeleton graph
    for(const auto &pair1 : predicates) //per table 
        for(const auto &pair2 : pair1.second) 
            if((alg==2 && query.pulledoutAtt.find(pair2)==query.pulledoutAtt.end()) || alg!=2)
                skeleton.addNode(pair2);

        

    //The nodes in the same tables should be connected with an edge
    //all the skeleton nodes in the same table should be fully connected graph
    for(const auto &pair1 : query.tables) //per table 
    {
        vector<string> aliases=query.tableAliases[pair1.first];
        for (auto const &table: aliases)
        {
            set<string> attPredicatesS=predicates[table];
            vector<string> attPredicates(attPredicatesS.begin(), attPredicatesS.end());
            if(attPredicates.size()>2)
            {
                cout<<"more than 2 atts from the same table, not checked yet.";
                exit (1);
            }
            if (attPredicates.size()>1)
                
                for (unsigned int i=0;i<attPredicates.size();i++)
                    for (unsigned int j=i+1;j<attPredicates.size();j++)
                    {
                        
                        if(alg==0)
                        {
                            skeleton.addedge(attPredicates[i],attPredicates[j]);
                        }
                        else if(alg==1)
                        {
                            if(!((query.pulledoutEdge.first==attPredicates[i]&&query.pulledoutEdge.second==attPredicates[j])||(query.pulledoutEdge.first==attPredicates[j]&&query.pulledoutEdge.second==attPredicates[i]))) // if the is edge is not in pulled out edges
                                skeleton.addedge(attPredicates[i],attPredicates[j]);
                                 
                        }
                        else if (alg==2)
                        {
                            if(query.pulledoutAtt.find(attPredicates[i])==query.pulledoutAtt.end() && query.pulledoutAtt.find(attPredicates[j])==query.pulledoutAtt.end() )                      
                                skeleton.addedge(attPredicates[i],attPredicates[j]);
                            else
                            {
                                if (pulledOutSkeleton.nodes.find(attPredicates[i])==pulledOutSkeleton.nodes.end())
                                    pulledOutSkeleton.addNode(attPredicates[i]);
                                if (pulledOutSkeleton.nodes.find(attPredicates[j])==pulledOutSkeleton.nodes.end())
                                    pulledOutSkeleton.addNode(attPredicates[j]);
                                pulledOutSkeleton.addedge(attPredicates[i],attPredicates[j]);
                            }
                        }
                        
                    }
        }
    }

    //find the elimination order to make the conditional frequencies
    // if it is empty the next line will find the elimination order with the min add-in edge algorithm
    unordered_map<string,unsigned int> eliminationOrder;
    if(query.eliminationOrder.size()>0)
        eliminationOrder= skeleton.setEliminationOrder(query.eliminationOrder);
    else
        eliminationOrder= skeleton.findEliminationOrder();
    
    if(alg==2)
        pulledOutSkeleton.setConditionedAtts(query.pulledoutAtt);
    
    // per table calculate the frequencies
    for(const auto &pair1 : query.tables) //per table 
    {
        //currently for any alias we make different freq tables and read the data from disk. 
        //It can be done more intillegently!!
        for (const auto table:query.tableAliases[pair1.first]) // per alias
        {
            set<string> attPredicatesS=predicates[table];
            set<string> attProjectionsS=query.discreteProjections[table];
            
            vector<string> attPredicates(attPredicatesS.begin(), attPredicatesS.end());
            vector<string> attProjectionsAls(attProjectionsS.begin(), attProjectionsS.end());

            //find pair attributes to calculate the frequencies
            vector<pair<string,string>> pairAtts;// main names of attributes to send it to the csv reader
            vector<string> singleAtts;// main names of attributes to send it to the csv reader
            vector<pair<string,string>> pairAttsAls; // aliases to put in the graph
            vector<string> singleAttsAls;// aliases to put in the graph
    
            if (attPredicates.size()>1 ) // pair attributes in the same table
            {
                if(attPredicates.size()>2)
                {
                    cout<< "more than two predicate variables from the same table. not checked yet.";
                    exit (1);
                }
                

                for (unsigned int i=0;i<attPredicates.size();i++)
                {
                    for (unsigned int j=i+1;j<attPredicates.size();j++)
                    {
                        pair<string,string> vec;
                        pair<string,string> vecAls;
                        string att1, att2;
                        if(query.attAliases[table].find(attPredicates[i])!=query.attAliases[table].end()) // if the att has an alias
                            att1=query.attAliases[table][attPredicates[i]];
                        else
                            att1=attPredicates[i];
                        if(query.attAliases[table].find(attPredicates[j])!=query.attAliases[table].end())
                            att2=query.attAliases[table][attPredicates[j]];
                        else
                            att2=attPredicates[j];
                        
                        if(alg==0)
                        {
                            auto e=skeleton.getedge(attPredicates[i],attPredicates[j]);
                            if (e->conditionedOn==attPredicates[j]) // to determine where the condition should be placed
                            {
                                vec.first=att2;
                                vec.second=att1;
                                vecAls.first=attPredicates[j];
                                vecAls.second=attPredicates[i];
                            }
                            else
                            {
                                vec.first=att1;
                                vec.second=att2;
                                vecAls.first=attPredicates[i];
                                vecAls.second=attPredicates[j];
                            }
                        }
                        else if(alg==2)
                        {
                            //if none of the atts is one of the pulled out atts
                            if(query.pulledoutAtt.find(attPredicates[i])==query.pulledoutAtt.end() && query.pulledoutAtt.find(attPredicates[j])==query.pulledoutAtt.end() )                      
                            {
                                auto e=skeleton.getedge(attPredicates[i],attPredicates[j]);
                                if (e->conditionedOn==attPredicates[j]) // to determine where the condition should be placed
                                {
                                    vec.first=att2;
                                    vec.second=att1;
                                    vecAls.first=attPredicates[j];
                                    vecAls.second=attPredicates[i];
                                }
                                else
                                {
                                    vec.first=att1;
                                    vec.second=att2;
                                    vecAls.first=attPredicates[i];
                                    vecAls.second=attPredicates[j];
                                }
                            }
                            else
                            {
                                auto e=pulledOutSkeleton.getedge(attPredicates[i],attPredicates[j]);
                                if (e->conditionedOn==attPredicates[i]) // to determine where the condition should be placed
                                {
                                    vec.first=att1;
                                    vec.second=att2;
                                    vecAls.first=attPredicates[i];
                                    vecAls.second=attPredicates[j];
                                }
                                else if (e->conditionedOn==attPredicates[j])
                                {
                                    vec.first=att2;
                                    vec.second=att1;
                                    vecAls.first=attPredicates[j];
                                    vecAls.second=attPredicates[i];
                                }
                            }
                        }
                        else if(alg==1)
                        {
                            if(query.pulledoutEdge.first==attPredicates[i] && query.pulledoutEdge.second==attPredicates[j]  )
                            {
                                // the edge is the pulled out edge
                                vec.first=att1;
                                vec.second=att2;
                                vecAls.first=attPredicates[i];
                                vecAls.second=attPredicates[j];
                            }
                            else if(query.pulledoutEdge.first==attPredicates[j] && query.pulledoutEdge.second==attPredicates[i])
                            {
                                // the edge is the pulled out edge
                                vec.first=att2;
                                vec.second=att1;
                                vecAls.first=attPredicates[j];
                                vecAls.second=attPredicates[i];
                            }
                            else
                            {
                                auto e=skeleton.getedge(attPredicates[i],attPredicates[j]);
                                if (e->conditionedOn==attPredicates[j]) // to determine where the condition should be placed
                                {
                                    vec.first=att2;
                                    vec.second=att1;
                                    vecAls.first=attPredicates[j];
                                    vecAls.second=attPredicates[i];
                                }
                                else
                                {
                                    vec.first=att1;
                                    vec.second=att2;
                                    vecAls.first=attPredicates[i];
                                    vecAls.second=attPredicates[j];
                                }
                            }
                        }

                        pairAtts.push_back(vec);
                        pairAttsAls.push_back(vecAls);
   
                    }
                }
            }
            else // single attributes
            {
                if(query.attAliases[table].find(attPredicates[0])!=query.attAliases[table].end())
                    singleAtts.push_back(query.attAliases[table][attPredicates[0]]);
                else
                    singleAtts.push_back(attPredicates[0]);
                singleAttsAls.push_back(attPredicates[0]);

            }
            
            //for non-skeleton attributes we need only a model from skeleton attributes to non-skeleton attribtes
            // find the dependent and independent variables for the non-skeleton attributes
            auto non_skeleton_keys= singleAtts; // for reading from the csv
            auto non_skeleton_keysAls= singleAttsAls;// to store the freqs in the graph
            //if singleAtts is empty, the pairs exist
            for (int i=0;i<pairAtts.size();i++)
            {
                vector<string> vec={pairAtts[i].first,pairAtts[i].second}; 
                vector<string> vecAls={pairAttsAls[i].first,pairAttsAls[i].second}; 
                non_skeleton_keys= vec_union(non_skeleton_keys,vec);
                 non_skeleton_keysAls= vec_union(non_skeleton_keysAls,vecAls);
            }
            auto non_skeleton_key2Als=vec_substract(attProjectionsAls,non_skeleton_keysAls);
            vector <string> attProjections;
            for (auto &att : attProjectionsAls)
                if(query.attAliases[table].find(att)!=query.attAliases[table].end())
                    attProjections.push_back(query.attAliases[table][att]);
                else
                    attProjections.push_back(att);
            auto non_skeleton_key2=vec_substract(attProjections,non_skeleton_keys);
            
            string data=query.tables[tableAliasRev[table]];
            
            CSVReader reader(data);

            auto dataList = reader.getData(singleAtts,pairAtts, non_skeleton_keys,non_skeleton_key2);
            //Read the csv file for the currect table and calculate the single, pairs, and non-skeleton attributes and make the freq tables
//            auto out = getvalues(tableAliasRev[table],singleAtts,pairAtts, non_skeleton_keys,non_skeleton_key2);
            
            // we have at most a single attribute per table, if it exists then we do not have pair attributes in the table
            //push it  into the skeleton
            unordered_map<unsigned int,unsigned long int> tmpPot;
            for (unsigned int i=0;i<singleAttsAls.size();i++)
            {
                string att=singleAttsAls[i];
                auto &sing=dataList.SingleAttributesFreq[singleAtts[i]];
//                auto &pot=skeleton.nodes[att].pot;
                if(alg==0 || (alg==2 && query.pulledoutAtt.find(att)==query.pulledoutAtt.end()) || alg==1)
                {
                    if (skeleton.nodes[att].pot.size()>0) // means we had already some pots comming from other tables, and they should be multiplied with the old ones 
                    {
                        for(auto &row: skeleton.nodes[att].pot)
                        {
                            if(sing.find(row.first)!=sing.end())
                            {
                                tmpPot[row.first]=row.second*sing[row.first];
                            }
                        }
                        skeleton.nodes[att].pot=tmpPot;
                        if(tmpPot.size()==0)
                        {
                            cout<<"The join result size is zero"<<endl;
                            exit(1);
                        }
                    }
                    else
                       skeleton.nodes[att].pot=dataList.SingleAttributesFreq[singleAtts[i]];// copy by value, if pot is empty. means this is the first pot comming from other tables
                }
                else if(alg==2 && query.pulledoutAtt.find(att)!=query.pulledoutAtt.end())// it is a pulled out att
                {
                    if (pulledOutSkeleton.nodes[att].pot.size()>0) // means we had already some pots comming from other tables, and they should be multiplied with the old ones 
                    {
                        for(auto &row: pulledOutSkeleton.nodes[att].pot)
                        {
                            if(sing.find(row.first)!=sing.end())
                            {
                                tmpPot[row.first]=row.second*sing[row.first];
                            }
                        }
                        pulledOutSkeleton.nodes[att].pot=tmpPot;
                        if(tmpPot.size()==0)
                        {
                            cout<<"The join result size is zero"<<endl;
                            exit(1);
                        }
                    }
                    else
                       pulledOutSkeleton.nodes[att].pot=dataList.SingleAttributesFreq[singleAtts[i]];
                }
            }
            for (unsigned int i=0;i<pairAttsAls.size();i++)
            {
                pair<string,string> atts=pairAttsAls[i];
                if(alg==2 && (query.pulledoutAtt.find(atts.first)!=query.pulledoutAtt.end() || query.pulledoutAtt.find(atts.second)!=query.pulledoutAtt.end()))
                {
                    auto e=pulledOutSkeleton.getedge(atts.first,atts.second);
                    e->pot=dataList.ConditionalFreq[i]; // copy by value.
                }
                else if(alg==1 && (query.pulledoutEdge.first==atts.first && query.pulledoutEdge.second==atts.second || query.pulledoutEdge.first==atts.second && query.pulledoutEdge.second==atts.first ))
                {
                    //if alg==1 and the edge is the pulled out edge
                    pulledOutConditionalRejectionRate=dataList.ConditionalFreq[i];
                }
                else
                {
                    auto e=skeleton.getedge(atts.first,atts.second);
                    e->pot=dataList.ConditionalFreq[i]; // copy by value.
                }
            }

            //just one model needed to get non-skeleton attributes given skeleton attribute
            for (auto &att: non_skeleton_key2Als)
                att=table+"_"+att;
            if(non_skeleton_key2.size()>0)// if any non skeleton variables exist 
            {
                non_skeleton_node nn;
                nn.key1=non_skeleton_keysAls;
                nn.key2=non_skeleton_key2Als;
                if (non_skeleton_keys.size()>1)
                {
                    nn.vec_key=dataList.GroupAttributesFreq2; // a map with a vector as a key
                }
                else
                {
                    nn.single_key=dataList.GroupAttributesFreq1; // single key map
                }
                skeleton.non_skeleton_nodes[table]=nn;
            }
            
            cout<<"Time for reading and cal freqs of table "<<tableAliasRev[table]<<": "<<dataList.timings["total"]<<endl;
        }
    }
    
    
    
    timing_seconds.readAndMakeFreqs=float( clock () - begin_time2 ) /  CLOCKS_PER_SEC;
}

void PGM::inferDP () 
{
    cout<<"\n"<<"started the inference"<<endl;
    /*
     * 
     * 
     * 
     *  the order of (if-for) statements can be changed to optimize the code
     
     
     */
    const clock_t begin_time = clock(); 
    vector <string> alreadyEliminated; // to keep the nodes that have been eliminated so far
    unordered_map<unsigned int, string> revEliminationOrder;//reverse map of the elimination order. It is not the reverse order :)
    for (auto const pair: skeleton.eliminationOrder)
    {
        revEliminationOrder[pair.second]=pair.first;
    }
    cout<<"\nWith elimination order: "<<endl;
    for (int i=0;i<revEliminationOrder.size();i++)
        cout<<i<<":"<<revEliminationOrder[i]<<" , ";
    cout<<"\n"<<endl;cout.flush();

    unsigned int elSize=revEliminationOrder.size();
    for (unsigned int el_num=0;el_num<elSize;el_num++)
    {
        string currentEl=revEliminationOrder[el_num];
        // get the neighbors of the current node minus alreadyEl
        auto neighbors=skeleton.getNextNeigh(currentEl,alreadyEliminated);
        // gets the number of neighbors of current node minus alreadyEl
        auto neighCount=skeleton.getNextNeighNum(currentEl,alreadyEliminated);
        //if the neigh count is one then the current join is a chain or acyclic join
        if(neighCount==1 && el_num<elSize-1)
        {
            string currentsNeigh=neighbors[0];
            /* if the current node is a leaf with no pot  from other tables and other nodes
             we just sum over the pairs and make the CDFs only
            iterates over the map, removes from pot and inserts to finalMarginal concurrently to avoid growing memory usage. 
            this may make the DP step slower
            maybe this will cause a problem with multi threading*/
            auto e=skeleton.getedge(currentEl,currentsNeigh);

            // the sum product algorithm on chain and cyclic joins
            // while doing sum product algorithm, calculate the CDFs as well, and 
            // concurrently, remove from the conditional e.pot
            auto &phi1=skeleton.nodes[currentEl].pot;
            auto &phi2=skeleton.nodes[currentsNeigh].pot;
            unordered_map<unsigned int, unsigned long int> tmpPot;
            //if there is a pot already in the nextEL node
            unsigned long int tmp=0;

            if(phi1.size()>0 && phi2.size()>0)
            {
                for (auto pair1 = e->pot.cbegin(); pair1 != e->pot.cend() ; /* no increment */)
                {
                    unsigned long int sum=0; // gives the accumulative density + sumProduct value
                    if(phi2.find(pair1->first)!=phi2.end())
                    {
                        for (auto  const &pair2 : pair1->second)// loop over the second level of pot map
                        {
                            if(phi1.find(pair2.first)!= phi1.end())
                            {
                                tmp=phi1[pair2.first] * pair2.second; // CDF
                                if (tmp!=0)
                                {
                                    sum+= tmp; // CDF
                                    e->finalMarginal[pair1->first].push_back({pair2.first,sum});
                                }
                            }
                        }
                        if(sum>0)
                            tmpPot[pair1->first]=sum*phi2[pair1->first]; 
                    }
//                            e->pot.erase(pair1++);  // at the same time we remove from e.pot  
                    pair1++;
                }
                skeleton.nodes[currentsNeigh].pot=tmpPot;
            }
            else if (phi1.size()>0 && phi2.size()==0)
            {
                for (auto pair1 = e->pot.cbegin(); pair1 != e->pot.cend() ; /* no increment */)
                {
                    unsigned long int sum=0; // gives the accumulative density + sumProduct value
                    for (auto  const &pair2 : pair1->second)// loop over the second level of pot map
                    {
                        if(phi1.find(pair2.first)!= phi1.end())
                        {
                            tmp=phi1[pair2.first] * pair2.second; // CDF
                            if (tmp!=0)
                            {
                                sum+= tmp; // CDF
                                e->finalMarginal[pair1->first].push_back({pair2.first,sum});
                            }
                        }
                    }
                    if(sum>0)
                        phi2[pair1->first]=sum;  
//                            e->pot.erase(pair1++);  // at the same time we remove from e.pot  
                    pair1++;
                }
            }
            else if (phi1.size()==0 && phi2.size()>0)
            {
                for (auto pair1 = e->pot.cbegin(); pair1 != e->pot.cend() ; /* no increment */)
                {
                    unsigned long int sum=0; // gives the accumulative density + sumProduct value
                    if(phi2.find(pair1->first)!=phi2.end())
                    {
                        for (auto  const &pair2 : pair1->second)// loop over the second level of pot map
                        {
                            sum+=pair2.second; // CDF
                            e->finalMarginal[pair1->first].push_back({pair2.first,sum});
                        }
                        if(sum!=0)
                            tmpPot[pair1->first]=sum*phi2[pair1->first]; 
//                            e->pot.erase(pair1++);  // at the same time we remove from e.pot  
                    }

                    pair1++;
                }
                skeleton.nodes[currentsNeigh].pot=tmpPot;
            }
            else if (phi1.size()==0 && phi2.size()==0)
            {
                for (auto pair1 = e->pot.cbegin(); pair1 != e->pot.cend() ; /* no increment */)
                {
                    unsigned long int sum=0; // gives the accumulative density + sumProduct value
                    for (auto  const &pair2 : pair1->second)// loop over the second level of pot map
                    {
                        sum+=pair2.second; // CDF
                        e->finalMarginal[pair1->first].push_back({pair2.first,sum});
                    }
                    if(sum!=0)
                        phi2[pair1->first]=sum; // this is the first pot in the currentsNeigh node
//                            e->pot.erase(pair1++);  // at the same time we remove from e.pot  
                    pair1++;
                }
            }


            if(skeleton.nodes[currentsNeigh].pot.size()==0)
            {
                cout<<"The join-result size is zero"<<endl;
                exit(1);
            }
//                    clear the current node pots and current edge pot
//                    skeleton.getNode(currentEl)->pot.clear();
//                    e->pot.clear();
        }
        // if there are more than an un-eliminated node in neighborhood, the join is cyclic
        if(neighCount>1 && el_num<elSize-1)
        {
            if(neighCount>2)
            {
                cout<<"more than two atts from the same table in the predicates, not checked yet.";
                exit (1);
            }

            // In triangulated graph we always have at least two neighbors 
            // the following code is only for the case if we have only two neighbors
            //find the source and target edges
            bool inx=false; 
            if (skeleton.getedge(currentEl,neighbors[1])->conditionedOn==neighbors[1])
                    inx=true;
            auto e1=skeleton.getedge(currentEl,neighbors[(int)inx]); //edge from others to current. the edge that its source is not the current node
            auto e2=skeleton.getedge(currentEl,neighbors[(int)(!inx)]);//edge from current to others with XOR between 0 and 1
            auto e3=skeleton.getedge(neighbors[0],neighbors[1]); //edge from others to others
            auto &currentNodePot=skeleton.nodes[currentEl].pot;
            // if-statements were pulled out from for-statements because of efficiency issues
            // if the edge in front exists, after the elimination, the results ahould be multiplied with the in-front edge
            unsigned long int mul=0;
            unordered_map<unsigned int, unordered_map<unsigned int,unsigned long int> > tmpE3_pot;

            if (e3->conditionedOn==neighbors[(int) inx])
            {
                if(currentNodePot.size()>0) // if the pot has some values from other tables or variables
                    for (auto pair_a = e1->pot.cbegin(); pair_a != e1->pot.cend() ; /* no increment */)//iterate on the first edge
                    {
                        for(auto &c: pair_a->second) // the variable is being removed
                        {
                            for(auto &b: e2->pot[c.first])//iterate over the second edge values given first edge values
                            {
                                mul=c.second*b.second*currentNodePot[c.first];
                                if(mul>0)
                                {
                                    tmpE3_pot[pair_a->first][b.first]+= mul; //sum: there is no edge already, so there is no join here
                                    e1->finalMarginalCyclic[{pair_a->first, b.first}].push_back({c.first,mul}); 
                                }
                            }
                        }
//                                    e1->pot.erase(pair_a++);
                        pair_a++;
                    }

                else// if the current pot is empty

                    for (auto pair_a = e1->pot.cbegin(); pair_a != e1->pot.cend() ; /* no increment */)//iterate on the first edge
                    {
                        for(auto &c: pair_a->second)// the variable is being removed
                        {
                            for(auto &b: e2->pot[c.first])//iterate over the second edge values given first edge values
                            {
                                mul=c.second*b.second;
                                if(mul>0)
                                {
                                    tmpE3_pot[pair_a->first][b.first]+= mul; //sum: there is no edge already, so there is no join here
                                    e1->finalMarginalCyclic[{pair_a->first, b.first}].push_back({c.first,mul}); 
                                }
                            }
                        }
//                                    e1->pot.erase(pair_a++);
                        pair_a++;
                    }

            }
            else// for caring the source of output edge
            {
                if(currentNodePot.size()>0) // if the pot has some values from other tables or variables
                    for (auto pair_a = e1->pot.cbegin(); pair_a != e1->pot.cend() ; /* no increment */)//iterate on the first edge
                    {
                        for(auto &c: pair_a->second)// the variable is being removed
                        {
                            for(auto &b: e2->pot[c.first])//iterate over the second edge values given first edge values
                            {
                                mul=c.second*b.second*currentNodePot[c.first];
                                if(mul>0)
                                {
                                    tmpE3_pot[b.first][pair_a->first]+= mul; //sum: there is no edge already, so there is no join here
                                    e1->finalMarginalCyclic[{pair_a->first, b.first}].push_back({c.first,mul});  
                                }
                            }
                        }
//                                    e1->pot.erase(pair_a++);
                        pair_a++;
                    }

                else// if the current pot is empty

                    for (auto pair_a = e1->pot.cbegin(); pair_a != e1->pot.cend() ; /* no increment */)//iterate on the first edge
                    {
                        for(auto &c: pair_a->second)// the variable is being removed
                        {
                            for(auto &b: e2->pot[c.first])//iterate over the second edge values given first edge values
                            {
                                mul=c.second*b.second;
                                if(mul>0)
                                {
                                    tmpE3_pot[b.first][pair_a->first]+= mul; //sum: there is no edge already, so there is no join here
                                    e1->finalMarginalCyclic[{pair_a->first, b.first}].push_back({c.first,mul}); 
                                }                                       
                            }
                        }
//                                    e1->pot.erase(pair_a++);
                        pair_a++;
                    }
            }


            //clear the current node pots and current edges pots
//                    skeleton.nodes[currentEl].pot.clear();
//                    e2->pot.clear();
//                    e1->pot.clear();


            // this multiplication should be outside of the calculating tmpE3
            // otherwise the logic is wrong>
            if(e3->activated==true)
            {
                for (auto pair1 = e3->pot.cbegin(); pair1 != e3->pot.cend() ; /* no increment */)
                {                           

                    multiplyMaps(e3->pot[pair1->first],tmpE3_pot[pair1->first],e3->pot[pair1->first]);//zero values are removed inside 
//
//                            if (e3->pot[pair1->first].size()==0)
//                                e3->pot.erase(pair1++);
//                            else
                        pair1++;

                }
            }
            else
            {
                e3->activated=true;
                e3->pot=tmpE3_pot;
            }

            if(e3->pot.size()==0)
            {
                cout<<"The join-result size is zero"<<endl;
                exit(1);
            }


            //calculate the cdf based on e1 and e2 frequencies and save it in e1
            //in any cases, e1 contains the final cdfs
            unsigned long int sum=0;
            for(auto &pair1:e1->finalMarginalCyclic) //calculate the CDFs
            {
                sum=0;
                for (auto &pair2: pair1.second)
                {
                    pair2.second+=sum;
                    sum=pair2.second;
                }
            }
//                    cout<<"sdf";
        }

        // for the last node
        if (el_num==elSize-1)   
        {
            //nothing needed for now. The pot at the last node is the final marginal
        }

        alreadyEliminated.push_back(currentEl);
        cout<<currentEl<< " eliminated in "<<float( clock () - begin_time ) /  CLOCKS_PER_SEC<<"  (Accumulative time)"<<endl;
    }
    lastNode=alreadyEliminated[alreadyEliminated.size()-1];
    
    cout <<"\n"<<"Time until the skeleton Inference without considering pulledout: "<<float( clock () - begin_time ) /  CLOCKS_PER_SEC <<"  (Accumulative time)"<<endl;
    
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////pulled out part
    
    if(alg==2 && query.pulledoutAtt.size()==1)
    {
        string currentEl;
        for (auto att:query.pulledoutAtt)
            currentEl=att.first;
        auto neighbors=pulledOutSkeleton.getNeighbors(currentEl);
        bool inx=false; 
        if (pulledOutSkeleton.getedge(currentEl,neighbors[1])->conditionedOn==neighbors[1])
                inx=true;
        auto e1=pulledOutSkeleton.getedge(currentEl,neighbors[(int)inx]); 
        auto e2=pulledOutSkeleton.getedge(currentEl,neighbors[(int)(!inx)]);
        pulledOutAttsconditions.push_back(neighbors[(int)inx]);
        pulledOutAttsconditions.push_back(neighbors[(int)(!inx)]);
        auto &currentNodePot=pulledOutSkeleton.nodes[currentEl].pot;

                                     
        unsigned long int mul=0;
        if(currentNodePot.size()>0) // if the pot has some values from other tables or variables
            for (auto pair_a = e1->pot.cbegin(); pair_a != e1->pot.cend() ; /* no increment */)//iterate on the first edge
            {
                for(auto &c: pair_a->second)// the variable is being removed
                {
                    for(auto &b: e2->pot[c.first])//iterate over the second edge values given first edge values
                    {
                        mul=c.second*b.second*currentNodePot[c.first];
                        if(mul>0)
                        {
                            
                            e1->finalMarginalCyclic[{pair_a->first, b.first}].push_back({c.first,mul});  
                        }
                    }
                }
    //                                    e1->pot.erase(pair_a++);
                pair_a++;
            }

        else// if the current pot is empty
        {
            for (auto pair_a = e1->pot.cbegin(); pair_a != e1->pot.cend() ; /* no increment */)//iterate on the first edge
            {
                for(auto &c: pair_a->second)// the variable is being removed
                {
                    for(auto &b: e2->pot[c.first])//iterate over the second edge values given first edge values
                    {
                        mul=c.second*b.second;
                        if(mul>0)
                        {

                            e1->finalMarginalCyclic[{pair_a->first, b.first}].push_back({c.first,mul}); 
                        }                                       
                    }
                }
    //                                    e1->pot.erase(pair_a++);
                pair_a++;
            }
        }
        unsigned long int sum=0;
        unsigned long int max=0;
        for(auto &pair1:e1->finalMarginalCyclic) //calculate the CDFs
        {
            sum=0;
            for (auto &pair2: pair1.second)
            {
                if (pair2.second>max)
                    max=pair2.second;
                
                pair2.second+=sum;
                sum=pair2.second;
            }
            pulledOutRejectionRates[pair1.first]=sum;
        }
        
        pulledOutSkeleton.Mr=max; 
    }
    else if(alg==2 && query.pulledoutAtt.size()>1)
    {
        cout<<"More than one pulled-out attribute, not implemented yet"<<endl;
        exit(1);
    }
    
    
    unsigned long int sum=0;
    for( auto &pair: skeleton.nodes[lastNode].pot) {
        sum+=pair.second;
        lastCDF.push_back( {pair.first, sum});
//        cout<<sum<<endl;
    }
    
    cout<<"Join size for the skeleton is: "<<sum<<endl;
    if(alg==1)
    {
        
        for(auto &x1:pulledOutConditionalRejectionRate)
            for(auto &x2:x1.second)
                if (x2.second>mr)
                    mr=x2.second;
    }
    
    
    timing_seconds.inference=float( clock () - begin_time ) /  CLOCKS_PER_SEC;
    cout <<"\n"<<"Time for full inference : "<<float( clock () - begin_time ) /  CLOCKS_PER_SEC <<"  (Accumulative time)"<<"\n\n";
  
}

void PGM::sample () /// generate the sample uniformly and independently
{
//    const clock_t begin_time = clock(); 
    //get the CDF vector of the map in the last node
    unsigned int skeletonVarNum=skeleton.nodes.size();

    unordered_map<unsigned int, string> revEliminationOrder;//reverse of the elimination order map
    for (auto const pair: skeleton.eliminationOrder)
    {
        revEliminationOrder[pair.second]=pair.first;
    }
    vector<string> skeleton_variables(skeletonVarNum);
    unsigned int jj=0;
    for (int j=skeletonVarNum-1;j>=0;j--)
        skeleton_variables[jj++]=revEliminationOrder[j];

    vector<unordered_map<string,unsigned int>> sample(query.sample_sizes[0]); // each row of the array is a dictionary
//    timing_seconds.sample.push_back({0.0,float( clock () - begin_time ) /  CLOCKS_PER_SEC});
//    cout <<"time to allocate memory for the whole sample: sample size 0: "<<"\t is: " <<float( clock () - begin_time ) /  CLOCKS_PER_SEC <<"\n";
    
//    unsigned int sample_size0=0;
//    unsigned int sample_sizek=0;

//    for( unsigned int k=0;k<query.sample_sizes.size();k++)// per timing point 
//    {
        vector<string> alreadyAdded={};
//        sample_sizek= query.sample_sizes[0];
        //generate the first attribute columnwise(the leftmost attribute)
        for(unsigned int i=0;i<query.sample_sizes[0];i++)
            sample[i][skeleton_variables[0]]=binary_CDF_sample(lastCDF);

        alreadyAdded.push_back(skeleton_variables[0]);
        // generate other attributes columnwise
        for (unsigned int j=1;j<skeletonVarNum;j++)
        {
            auto alreadyAdded_neighbors=skeleton.vec_intersection(skeleton.getNeighbors(skeleton_variables[j]),alreadyAdded);
            if (alreadyAdded_neighbors.size()==1)
            {
                auto x=skeleton.getedge(alreadyAdded_neighbors[0],skeleton_variables[j]);
                for(unsigned int i=0;i<query.sample_sizes[0];i++) // per row of the sample
                {
                    //if(x->finalMarginal[sample[i][alreadyAdded_neighbors[0]]].size()>0)
                        sample[i][skeleton_variables[j]]=binary_CDF_sample(x->finalMarginal[sample[i][alreadyAdded_neighbors[0]]]); // generate a random value for the attribute with order j in elimination
                   // else
                       // cout<<sample[i][alreadyAdded_neighbors[0]]<<endl;
                }
                alreadyAdded.push_back(skeleton_variables[j]);
            }
            else if(alreadyAdded_neighbors.size()==2)
            {
                
                auto x=skeleton.getedge(alreadyAdded_neighbors[0],skeleton_variables[j]);
                if (x->conditionedOn==skeleton_variables[j]) // to find the first and second DVs already added
                {
                    x=skeleton.getedge(alreadyAdded_neighbors[1],skeleton_variables[j]);
                    for(unsigned int i=0;i<query.sample_sizes[0];i++) // per row of the sample
                    {
                        auto s=x->finalMarginalCyclic[{sample[i][alreadyAdded_neighbors[1]], sample[i][alreadyAdded_neighbors[0]]}];
        
                        sample[i][skeleton_variables[j]]=binary_CDF_sample(s);
                    }
                }
                else
                {
                    for(unsigned int i=0;i<query.sample_sizes[0];i++) // per row of the sample
                    {
                        auto s=x->finalMarginalCyclic[{sample[i][alreadyAdded_neighbors[0]], sample[i][alreadyAdded_neighbors[1]]}];
                        
                        sample[i][skeleton_variables[j]]=binary_CDF_sample(s);
                    }
                    
                }
                alreadyAdded.push_back(skeleton_variables[j]);
            }
            else
            {
                cout<<"more than two attributes already added. not chacked yet.";
                exit(1);
            }

        }

        //generate the non_skeleton variables
        vector<string> allNonSkeletonAtts={}; // just for writing the sample to the csv file
        for(auto &nod:skeleton.non_skeleton_nodes) //per table
        {
            if (nod.second.key1.size()>1 && nod.second.key2.size()>1)// none of them is single
            {
                auto key1=nod.second.key1;
                auto key2=nod.second.key2;
                int sizee=key2.size();
                auto &vec_key=nod.second.vec_key;
                for(unsigned int i=0;i<query.sample_sizes[0];i++) // per row of the sample
                {
                    vector<unsigned int> key11;
                    for (auto const & att:key1)
                        key11.push_back(sample[i][att]);
                    
                    auto sam=binary_CDF_sample_NonSkeleton_multiple_output(vec_key[key11], sizee);
                    for (int ii=0;ii<sizee;ii++)
                        sample[i][key2[ii]]=sam[ii];
                }
            }
            else if (nod.second.key1.size()==1 && nod.second.key2.size()>1) // the second key is not single
            {
                auto key1=nod.second.key1[0];
                auto key2=nod.second.key2;
                int sizee=key2.size();
                auto &single_key=nod.second.single_key;
                for(unsigned int i=0;i<query.sample_sizes[0];i++) // per row of the sample
                {
                    auto sam=binary_CDF_sample_NonSkeleton_multiple_output(single_key[sample[i][key1]], sizee);
                    for (int ii=0;ii<sizee;ii++)
                        sample[i][key2[ii]]=sam[ii];
                }
            }
            else if (nod.second.key1.size()>1 && nod.second.key2.size()==1)//first key is not single
            {
                auto key1=nod.second.key1;
                auto key2=nod.second.key2[0];
                auto &vec_key=nod.second.vec_key;
                for(unsigned int i=0;i<query.sample_sizes[0];i++) // per row of the sample
                {
                    vector<unsigned int> key11;
                    for (auto const & att:key1)
                        key11.push_back(sample[i][att]);
                    
                    sample[i][key2]=binary_CDF_sample_NonSkeleton_single_output(vec_key[key11]);
                }
            }
            else if (nod.second.key1.size()==1 && nod.second.key2.size()==1) // both are single
            {
                auto key1=nod.second.key1[0];
                auto key2=nod.second.key2[0];
                auto &single_key=nod.second.single_key;
                for(unsigned int i=0;i<query.sample_sizes[0];i++) // per row of the sample
                    sample[i][key2]=binary_CDF_sample_NonSkeleton_single_output(single_key[sample[i][key1]]);
                   
            }
        }
//        timing_seconds.sample.push_back({query.sample_sizes[0],float( clock () - begin_time ) /  CLOCKS_PER_SEC});
//        cout <<"Sampling time with sample_size: "<<query.sample_sizes[0]<<"\t is: " <<float( clock () - begin_time ) /  CLOCKS_PER_SEC <<"\n";
////        cout.flush();
//        sample_size0=sample_sizek;
//    }
 
//    cout<<"\n"<<"Samples are like:"<<endl;    
//    for (unsigned int i=0;i<3;i++)
//    {
//        for (auto const &att: sample[i])
//            cout<<att.first<<":"<<att.second<<", ";
//        cout<<endl;
//    }
//    cout<<"\n"<<"Started writing the samples"<<endl;
//    vector<string> allNonSkeletonAtts; // just for writing the sample to the csv file
//    for(auto &nod:skeleton.non_skeleton_nodes) //per table
//    {
//        for (auto const& att: nod.second.key1)
//            allNonSkeletonAtts.push_back(att);
//        for (auto const& att: nod.second.key2)
//            allNonSkeletonAtts.push_back(att);
//    }
//    auto allVars=vec_union(skeleton_variables,allNonSkeletonAtts);
//    const clock_t begin_time2 = clock();
//    writeFileCSV(query.sampleOutAdd,sample,allVars);
//    timing_seconds.writingCSV=float( clock () - begin_time2 ) /  CLOCKS_PER_SEC;
//    cout <<"Time for writing the sample: "<<"\t is: " <<float( clock () - begin_time2 ) /  CLOCKS_PER_SEC <<"\n";
        
//

}
void PGM::writeFileCSV(std::string fname, vector<unordered_map<string,unsigned int>> &m, vector<string> header) 
    {
        ofstream myfile(fname);
        int vsize = m.size();
        for(auto const & att: header)
                myfile << att <<",";
        myfile<<endl;
        for (int n=0; n<vsize; n++)
        {
            for(auto const & att: header)
                myfile << m[n][att] <<",";
            myfile<< endl;
//            if(n%(int)(vsize/100)==0)
//                cout<<"+"<<flush;
        }
//        cout<<endl<<"\n";
        myfile.close();
    }
void PGM::multiplyMaps (unordered_map<unsigned int,unsigned long int> &m1, unordered_map<unsigned int,unsigned long int> &m2,unordered_map<unsigned int,unsigned long int> &output) 
{
    unsigned long int x=0;
    unordered_map<unsigned int,unsigned long int> outp;
    
    if(m1.size()<m2.size())
        
        for (auto pair = m1.cbegin(); pair != m1.cend() /* not hoisted */; /* no increment */)
        {
            x=pair->second * m2[pair->first];
            
            if (x!=0)
                outp[pair->first]=x;
            
            pair++;
        }
    
    else
        
        for (auto pair = m2.cbegin(); pair != m2.cend() /* not hoisted */; /* no increment */)
        {
            x=m1[pair->first]*pair->second;
            
            if (x!=0)
                outp[pair->first]=x;
            
            pair++;
        }
    output=outp;
//    if(m1.size()<m2.size())
//        
//        for (auto pair = m1.cbegin(); pair != m1.cend() /* not hoisted */; /* no increment */)
//        {
//            x=pair->second * m2[pair->first];
//            
//            if (x!=0)
//            {
//                m1[pair->first]=x;  
//                pair++;
//            }
//            else
//                m1.erase(pair++);
//        }
//    
//    else
//        
//        for (auto pair = m2.cbegin(); pair != m2.cend() /* not hoisted */; /* no increment */)
//        {
//            x=m1[pair->first]*pair->second;
//            
//            if (x!=0)
//            {
//                m1[pair->first]=x;  
//                pair++;
//            }
//            else
//                m1.erase(pair->first);
//        }
////    output=outp;
}
void PGM::sampleWitPulledOut () /// generate the sample uniformly and independently
{
//    const clock_t begin_time = clock(); 
    
    unordered_map<unsigned int, string> revEliminationOrder;//reverse of the elimination order map
    for (auto const pair: skeleton.eliminationOrder)
    {
        revEliminationOrder[pair.second]=pair.first;
    }
    int sample_size=query.sample_sizes[0];
    vector<unordered_map<string,unsigned int>> sample(sample_size); // each row of the array is a dictionary
//    timing_seconds.sample.push_back({0.0,float( clock () - begin_time ) /  CLOCKS_PER_SEC});
//    cout <<"time to allocate memory for the whole sample: sample size 0: "<<"\t is: " <<float( clock () - begin_time ) /  CLOCKS_PER_SEC <<"\n";
//    vector<pair<unsigned int,unsigned long int>> lastCDF;
//    unsigned long int sum=0;
//    for( auto &pair: skeleton.nodes[lastNode].pot) {
//        sum+=pair.second;
//        lastCDF.push_back( {pair.first, sum});
//    }
//    cout<<"Join size for the acyclic skeleton is: "<<sum<<endl;
    int skeletonVarNum=skeleton.nodes.size();
    int maxOrder=skeletonVarNum-1;
    vector <edge*> edgesPointers(skeletonVarNum);
    vector<string> prev_att_list(skeletonVarNum);
    for (int i=0;i<maxOrder;i++)
    {
        
        auto x=skeleton.getNeighborsWithHigherElOrder(revEliminationOrder[i]);
        if(x.size()!=1)
        {
            cout<<"The query is a chain join, but you treat it as cyclic. Efficiency warning!!"<<endl;
            exit(1);
        }
        edgesPointers[i]=skeleton.getedge(revEliminationOrder[i],x[0]);
        prev_att_list[i]=x[0];
    }
    if(alg==2)
    {
        string pulledOut;
        for(auto att:query.pulledoutAtt)
            pulledOut=att.first;
        auto e=pulledOutSkeleton.getedge(pulledOut,pulledOutAttsconditions[0]);
        unsigned int count=0;
        double rnd;
        double hr;

        while(count<sample_size)
        {
            //to generate the variable with last order
            sample[count][revEliminationOrder[maxOrder]]=binary_CDF_sample(lastCDF);
            for (int j=maxOrder-1;j>=0;j--)
                sample[count][revEliminationOrder[j]]=binary_CDF_sample(edgesPointers[j]->finalMarginal[sample[count][prev_att_list[j]]]); // generate a random value for the attribute with order j in elimination

            if(pulledOutRejectionRates.find({sample[count][pulledOutAttsconditions[0]],sample[count][pulledOutAttsconditions[1]]})!=pulledOutRejectionRates.end())
            {
                hr=pulledOutRejectionRates[{sample[count][pulledOutAttsconditions[0]],sample[count][pulledOutAttsconditions[1]]}];
                rnd= (double) rand() / (RAND_MAX);
                if(rnd<(hr/pulledOutSkeleton.Mr))
                {
                    sample[count][pulledOut]=binary_CDF_sample(e->finalMarginalCyclic[{sample[count][pulledOutAttsconditions[0]],sample[count][pulledOutAttsconditions[1]]}]);
                    count++; // the sample accepted
                }
            }
        }
    }
    else if(alg==1)
    {
//        double mr=0;
//        for(auto &x1:pulledOutConditionalRejectionRate)
//            for(auto &x2:x1.second)
//                if (x2.second>mr)
//                    mr=x2.second;
        double hr;
        double rnd;
        
        int count=0;
        while(count<sample_size)
        {
            //to generate the variable with last order
            sample[count][revEliminationOrder[maxOrder]]=binary_CDF_sample(lastCDF);
            for (int j=maxOrder-1;j>=0;j--)
                sample[count][revEliminationOrder[j]]=binary_CDF_sample(edgesPointers[j]->finalMarginal[sample[count][prev_att_list[j]]]); // generate a random value for the attribute with order j in elimination

            if(pulledOutConditionalRejectionRate.find(sample[count][query.pulledoutEdge.first])!=pulledOutConditionalRejectionRate.end())
            {
                // if the sample is valid considering the pulled out edge
                auto &x=pulledOutConditionalRejectionRate[sample[count][query.pulledoutEdge.first]];
                if(x.find(sample[count][query.pulledoutEdge.second])!=x.end())
                {
                    hr=x[sample[count][query.pulledoutEdge.second]];
                    
                    rnd= (double) rand() / (RAND_MAX);
                    if(rnd<hr/mr)
                    {
                        count++;
//                        if(count%10000==0)
//                            cout <<float( clock () - begin_time ) /  CLOCKS_PER_SEC<<"  (Accumulative time)" <<"\n";
                    }
                }
            }
        }
    }

    //generate the non_skeleton variables
//    vector<string> allNonSkeletonAtts={}; // just for writing the sample to the csv file
    for(auto &nod:skeleton.non_skeleton_nodes) //per table
    {
        if (nod.second.key1.size()>1 && nod.second.key2.size()>1)// none of them is single
        {
            auto key1=nod.second.key1;
            auto key2=nod.second.key2;
            int sizee=key2.size();
            auto &vec_key=nod.second.vec_key;
            for(unsigned int i=0;i<sample_size;i++) // per row of the sample
            {
                vector<unsigned int> key11;
                for (auto const & att:key1)
                    key11.push_back(sample[i][att]);

                auto sam=binary_CDF_sample_NonSkeleton_multiple_output(vec_key[key11], sizee);
                for (int ii=0;ii<sizee;ii++)
                    sample[i][key2[ii]]=sam[ii];
            }
        }
        else if (nod.second.key1.size()==1 && nod.second.key2.size()>1) // the second key is not single
        {
            auto key1=nod.second.key1[0];
            auto key2=nod.second.key2;
            int sizee=key2.size();
            auto &single_key=nod.second.single_key;
            for(unsigned int i=0;i<sample_size;i++) // per row of the sample
            {
                auto sam=binary_CDF_sample_NonSkeleton_multiple_output(single_key[sample[i][key1]], sizee);
                for (int ii=0;ii<sizee;ii++)
                    sample[i][key2[ii]]=sam[ii];
            }
        }
        else if (nod.second.key1.size()>1 && nod.second.key2.size()==1)//first key is not single
        {
            auto key1=nod.second.key1;
            auto key2=nod.second.key2[0];
            auto &vec_key=nod.second.vec_key;
            for(unsigned int i=0;i<sample_size;i++) // per row of the sample
            {
                vector<unsigned int> key11;
                for (auto const & att:key1)
                    key11.push_back(sample[i][att]);

                sample[i][key2]=binary_CDF_sample_NonSkeleton_single_output(vec_key[key11]);
            }
        }
        else if (nod.second.key1.size()==1 && nod.second.key2.size()==1) // both are single
        {
            auto key1=nod.second.key1[0];
            auto key2=nod.second.key2[0];
            auto &single_key=nod.second.single_key;
            for(unsigned int i=0;i<sample_size;i++) // per row of the sample
                sample[i][key2]=binary_CDF_sample_NonSkeleton_single_output(single_key[sample[i][key1]]);

        }   
    }
//    
//    timing_seconds.sample.push_back({sample_size,float( clock () - begin_time ) /  CLOCKS_PER_SEC});
//    cout <<"Sampling time with sample_size: "<<sample_size<<"\t is: " <<float( clock () - begin_time ) /  CLOCKS_PER_SEC<<"\n";
//    cout.flush();
//    cout<<"\n"<<"Samples are like:"<<endl;    
//    for (unsigned int i=0;i<3;i++)
//    {
//        for (auto const &att: sample[i])
//            cout<<att.first<<":"<<att.second<<", ";
//        cout<<endl;
//    }
//    cout<<"\n"<<"Started writing the samples"<<endl;
//    vector<string> allVars;
//    for(auto &sam:sample[0])
//        allVars.push_back(sam.first);
//    const clock_t begin_time2 = clock();
//    writeFileCSV(query.sampleOutAdd,sample,allVars);
//    timing_seconds.writingCSV=float( clock () - begin_time2 ) /  CLOCKS_PER_SEC;
//    cout <<"Time for writing the sample: "<<"\t is: " <<float( clock () - begin_time2 ) /  CLOCKS_PER_SEC <<"\n";
        
//

}
#endif /* PGM_H */

