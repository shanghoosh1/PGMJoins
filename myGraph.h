
/* 
 * File:   myGraph.h
 * Author: u1880698
 *
 * Created on 16 June 2020, 15:44
 */

#ifndef MYGRAPH_H
#define MYGRAPH_H
#include <iostream>
#include <unordered_map>
#include <memory> 
#include <algorithm>
#include <vector>
#include<boost/functional/hash.hpp>
#include <sampler.h>
#include<csvReader.h>

using namespace std;

//// to have an unordered map with a vector as key
//template < typename SEQUENCE > struct seq_hash
//{
//	std::size_t operator() (const SEQUENCE& seq) const
//	{
//		std::size_t hash = 0;
//		boost::hash_range(hash, seq.begin(), seq.end());
//		return hash;
//	}
//};
//template < typename SEQUENCE, typename T >
//using unordered_map_sequence = std::unordered_map< SEQUENCE, T, seq_hash<SEQUENCE> >; // this the data type for vector map

typedef struct // defines a node type for skeleton variables
{
    unordered_map<unsigned int,unsigned long int> pot;
//    unordered_map<string, unordered_map<unsigned int,unsigned long int> > phi;
//    unordered_map<unsigned int,unsigned long int> finalnodeMarginal;
    string name;
}node;

typedef struct // defines a node type for non skeleton vaiables
{ 
    unordered_map_sequence<vector<unsigned int>, vector<vector<unsigned int>>> vec_key;
    unordered_map<unsigned int, vector<vector<unsigned int>>> single_key;
    vector<string> key1;
    vector<string> key2;
}non_skeleton_node;

typedef struct // defines an edge type
{
    unordered_map<unsigned int, unordered_map<unsigned int,unsigned long int> > pot;
    unordered_map<unsigned int, vector<pair<unsigned int,unsigned long int> >> finalMarginal; // contains conditional cumulative distributions for non-cyclic
    unordered_map_sequence<vector<unsigned int>, vector<pair<unsigned int,unsigned long int>>> finalMarginalCyclic; // for cyclic joins
    bool is_PK=false;
    bool activated=true;
    string conditionedOn="";
}edge;

class myGraph
{

    public:
        
        unordered_map<string, node > nodes; // contains the skeleton nodes as structs
        unordered_map<string, unordered_map<string,edge> > edges; // contains the edges as structs for the skeleton nodes
        unordered_map<string, non_skeleton_node > non_skeleton_nodes; // contains non-skeleton nodes with no edges. a node per table
        unordered_map<string,unsigned int> eliminationOrder; // shows the elimination order of the nodes in DP
        double Mr; // for the query with pulled out atts
      
        myGraph()
        {
            srand(time(0));
        }
        //adds a node
        node addNode(string name) // adds a node if it does not exist
        {
            
            if (nodes.find(name)!= nodes.end())
            {
//                cout<<"node already exists\n";
//                exit (1);
            }
            else
            {
                node newNode;
                newNode.name=name;
                nodes.insert(make_pair( name, newNode ));
                return nodes[name];
            }
                         
        }
        //adds an edge
        edge addedge(string n1, string n2) // returns 1 if one of the nodes does not exist, otherwise adds an edge and returns 0.
        {
            edge e1;
            if (nodes.find(n1)== nodes.end() )
            {
                cout<< "first node does not exist\n";
                exit (1);
            }
            else if (nodes.find(n2)== nodes.end() )
            {
                cout<< "second node does not exist\n";
                exit (1);
            }
            //if edge already exists
            else if((edges.find(n1)!= edges.end() && edges[n1].find(n2)!= edges[n1].end()) || (edges.find(n2)!= edges.end() && edges[n2].find(n1)!= edges[n2].end()))
            {
                cout<<"edge already exists, nothing been applied\n";
//                exit (1);
            }
                
            else{
                edges[n1][n2]=e1;
                return edges[n1][n2];
            }
            

        }
        
        //returns all the neighbors of the node
        vector<string> getNeighbors(string n)
        {
            vector <string> neigh;
            if (nodes.find(n)==nodes.end())
            {
                cout<<"node does not exist.";
                exit (1);
            }
            for(const auto &pair1 : edges) 
            {

               for(const auto &pair2 : pair1.second) 
               {

                   if (pair1.first== n )
                       neigh.push_back(pair2.first);
                   else if (pair2.first==n)
                       neigh.push_back(pair1.first);
               }
            }
            return neigh;
        }
        
        //returns all the neighbors of the node with higher elimination order
        vector<string> getNeighborsWithHigherElOrder(string n)
        {
            vector <string> neigh;
            if (nodes.find(n)==nodes.end())
            {
                cout<<"node does not exist.";
                exit (1);
            }
            for(const auto &pair1 : edges) 
            {

               for(const auto &pair2 : pair1.second) 
               {
                   
                   if (pair1.first== n && eliminationOrder[n]< eliminationOrder[pair2.first])
                       neigh.push_back(pair2.first);
                   else if (pair2.first==n && eliminationOrder[n]< eliminationOrder[pair1.first])
                       neigh.push_back(pair1.first);
               }
            }
            return neigh;
        }
        
        // checks if the edge exists between two nodes
        bool  ifEdge(string n1, string n2)
        {
            if((edges.find(n1)!= edges.end() && edges[n1].find(n2)!= edges[n1].end()) || (edges.find(n2)!= edges.end() && edges[n2].find(n1)!= edges[n2].end()))
                return true;
            else
                return false;
        }
        
        // checks if the node exists
        bool ifNode(string n)
        {
            if (nodes.find(n)== nodes.end() )
                return false;
            else
                return true;
        }
        //returns the edge. if not exist, returns an empty edge. 
        edge*  getedge(string n1, string n2)
        {
            if (edges.find(n1) != edges.end() && edges[n1].find(n2) != edges[n1].end()) 
		return &edges[n1][n2];

            else if (edges.find(n2) != edges.end() && edges[n2].find(n1) != edges[n2].end())
                return &edges[n2][n1];

            else{
                std::cout << "Edge not found";
                exit (1);
//                edge emptyE;
//                return &emptyE;
            }

        }
//        //get the edge for pulledOut version
//        edge*  getedgeBasedOnCondition(string n1)
//        {
//            
//            for (auto const &n : nodes)
//            {
//                if (edges.find(n1) != edges.end() && edges[n1].find(n.first) != edges[n1].end() && edges[n1][n.first].conditionedOn==n.first)
//                    return &edges[n1][n.first];
//                if (edges.find(n.first) != edges.end() && edges[n.first].find(n1) != edges[n.first].end() && edges[n.first][n1].conditionedOn==n.first)
//                    return &edges[n.first][n1];
//            }
//            
//
//        }
        //returns the node. if not exist, returns an empty node.
        node* getNode(string n)
        {
            if (nodes.find(n)== nodes.end())
            {
                cout<<"node does not exist\n";
                exit (1);
//                node n;
//                return &n;
            }
            else
                return &nodes[n];
        }

        unordered_map<string,unsigned int> getEliminationOrder()
        {
            return eliminationOrder;
        }
        //for the pulled out atts
        void setConditionedAtts(unordered_map<string,int> elor)
        {
            for(auto &pair: elor)
            {
                auto neighbors=getNeighbors(pair.first);
                int c=neighbors.size();
                if (c==1)
                {
                    auto e=getedge(pair.first,neighbors[0]);
                    e->conditionedOn=neighbors[0]; //mostly used in tree and chain joins. 
                }
                else if(c==2)
                {                      
                    
                    auto e=getedge(pair.first,neighbors[0]);
                    e->conditionedOn=pair.first;
                    auto e1=getedge(pair.first,neighbors[1]);
                    e1->conditionedOn=neighbors[1];

                }
                else if(c>2)
                {
                    cout<< "more than two attributes in a table. not checked yet!";
                    exit (1) ;
                }
            }
        }
        //finds the elimination order for the graph with triangulation.
        //newly added egdes in triangulation are set to deactivated for further considerations.
        //any optimization can be done here including join order optimization with heuristics.
        unordered_map<string,unsigned int> findEliminationOrder()
        {
            srand(time(0));
            
            unordered_map<string,unsigned int> eo; // final elimination order
            vector<string> alreadyEliminated;
            for (unsigned int i=0;i<nodes.size();i++)// n node should be chosen
            {
                unordered_map<unsigned int,vector<string>> neigh_counts;// keep the nodes in the map of counts. Num of fill-in adges per nodeZz
                string currentOne=""; // we eliminate this each time 
                for(const auto nod: nodes)// check all attributes in terms of adding new edges
                {
                    if(find(alreadyEliminated.begin(), alreadyEliminated.end(), nod.first) != alreadyEliminated.end())
                        continue; //already has been eliminated
                    auto neighs=getNeighbors(nod.first);
                    auto sub_neighs=vec_substract(neighs,alreadyEliminated);// should not consider already eliminated nodes
                    // check the number of needed add-in edges
                    unsigned int siz=0;
                    for (unsigned int i=0;i<sub_neighs.size();i++)
                        for (unsigned int j=i+1;j<sub_neighs.size();j++)
                            if (!ifEdge(sub_neighs[i],sub_neighs[j]))
                                siz++;

                    neigh_counts[siz].push_back(nod.first);
                }

                // sample one node from nodes with the smallest neigh counts
                for(unsigned int k=0; k < nodes.size(); k++)
                {
                    if (neigh_counts.find(k)!=neigh_counts.end())
                    {
                        unsigned int rnd= random_range(0,neigh_counts[k].size()-1);
                        currentOne=neigh_counts[k][rnd];
//                            cout<<currentOne<<" selected \n";
                        if(k>0) // query is cyclic and should be triangulated
                        {
                            // add new edges if not exist among all the neighbors of the currently removed node
                            auto neighbors=getNeighbors(currentOne);
                            auto sub_neighs=vec_substract(neighbors,alreadyEliminated);// should not consider already eliminated nodes
                            //make the all neighbors fully connected with deactivated add-in edges
                            for (unsigned int i=0;i<sub_neighs.size();i++)
                                for (unsigned int j=i+1;j<sub_neighs.size();j++)
                                {
                                    if (!ifEdge(sub_neighs[i],sub_neighs[j]))
                                    {
                                        addedge(sub_neighs[i],sub_neighs[j]);
                                        auto e=getedge(sub_neighs[i],sub_neighs[j]);
                                        e->activated=false; // means that it has been added with triangulation
                                    }
                                }

                        }
                        break;
                    }
                }
                if(i!=nodes.size()-1)
                {
                    alreadyEliminated.push_back(currentOne);
                    eo[currentOne]=i;
                }
                else// the last node is inserted
                {
                    eo[neigh_counts[0][0]]=i;
//                        cout<<neigh_counts[0][0]<<" is selected \n";
                }
            }
            eliminationOrder=eo;
            // determine the source of the edges
            for (auto const& el:nodes)
            {
                auto neighbors=getNeighborsWithHigherElOrder(el.first);
                int c=neighbors.size();
                if (c==1)
                {
                    auto e=getedge(el.first,neighbors[0]);
                    e->conditionedOn=neighbors[0]; //mostly used in tree and chain joins. 
                }
                else if(c==2)
                {                      
                    //QY happens here
                    //randomly choose a->b and b->c where el.first is b   
                    ///  Primary keys should be considered for optimization
                    //this should be checked for new queries
                    auto e=getedge(el.first,neighbors[0]);
                    e->conditionedOn=el.first;
                    auto e1=getedge(el.first,neighbors[1]);
                    e1->conditionedOn=neighbors[1];

                }
                else if(c>2)
                {
                    cout<< "more than two attributes in a table. not checked yet!";
                    exit (1) ;
                }
            }
            return eliminationOrder;
        }
        unsigned int getNextNeighNum(string n, vector<string> alreadyEl)// gives the number of neighbors of n substract with alreadyEl
        {
            unsigned int siz;
            auto neighs=getNeighbors(n);
            auto sub_neighs=vec_substract(neighs,alreadyEl);
            siz=sub_neighs.size();
            return siz;
        }
        vector< string> getNextNeigh(string n, vector<string> alreadyEl)// gives the neighbors of n substract with alreadyEl
        {
 
            auto neighs=getNeighbors(n);
            auto sub_neighs=vec_substract(neighs,alreadyEl);
            return sub_neighs;
        }
        
        
        vector<string> vec_intersection(vector<string> v1,vector<string> v2)
        {
            vector<string> v3;

            sort(v1.begin(), v1.end());
            sort(v2.begin(), v2.end());

            set_intersection(v1.begin(),v1.end(),
                                  v2.begin(),v2.end(),
                                  back_inserter(v3));
            return v3;
        }
        
        unordered_map<string,unsigned int> setEliminationOrder(unordered_map<string,unsigned int> elor)
        {
            eliminationOrder=elor;
            vector<string> alreadyEliminated;
            unordered_map<unsigned int,string> revEl;
            for (auto &currentOne : eliminationOrder)// n node should be chosen
            {
                revEl[currentOne.second]=currentOne.first;
            }
            for (int i=0;i<revEl.size();i++)// n node should be chosen
            {
                string currentOne=revEl[i];
                alreadyEliminated.push_back(currentOne);
                auto neighs=getNeighbors(currentOne);
                auto sub_neighs=vec_substract(neighs,alreadyEliminated);// should not consider already eliminated nodes
                for (unsigned int i=0;i<sub_neighs.size();i++)
                    for (unsigned int j=i+1;j<sub_neighs.size();j++)
                        if (!ifEdge(sub_neighs[i],sub_neighs[j]))
                        {
                            addedge(sub_neighs[i],sub_neighs[j]);
                            auto e=getedge(sub_neighs[i],sub_neighs[j]);
                            e->activated=false; // means that it has been added with triangulation
                        }

            }


            // determine the source of the edges
            for (auto const& el:nodes)
            {
                auto neighbors=getNeighborsWithHigherElOrder(el.first);
                int c=neighbors.size();
                if (c==1)
                {
                    auto e=getedge(el.first,neighbors[0]);
                    e->conditionedOn=neighbors[0]; //mostly used in tree and chain joins. 
                }
                else if(c==2)
                {                      
                    //QY happens here
                    //randomly choose a->b and b->c where el.first is b   
                    ///  Primary keys should be considered for optimization
                    //this should be checked for new queries
                    auto e=getedge(el.first,neighbors[1]);
                    e->conditionedOn=el.first;
                    auto e1=getedge(el.first,neighbors[0]);
                    e1->conditionedOn=neighbors[0];
//                    cout<<neighbors[0]<<"->"<<el.first<<"->"<<neighbors[1]<<endl;
                }
//                else if(c>2)
//                {
//                    cout<< "more than two attributes in a table. not checked yet!";
//                    exit (1) ;
//                }

            }
           
            return eliminationOrder;
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
        
};


#endif /* MYGRAPH_H */

