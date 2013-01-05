#include <iostream>
#include "Cluster.h"
#include "utils.h"
#include "numerics.h"
#include "View.h"
#include "RandomNumberGenerator.h"

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>

typedef boost::numeric::ublas::matrix<double> matrixD;
using namespace std;
typedef vector<Cluster<double>*> vectorCp;
typedef set<Cluster<double>*> setCp;
typedef map<int, Cluster<double>*> mapICp;
typedef setCp::iterator setCp_it;
typedef mapICp::iterator mapICp_it;
typedef vector<int>::iterator vectorI_it;


vector<double> extract_row(matrixD data, int row_idx) {
  vector<double> row;
  for(int j=0;j < data.size2(); j++) {
    row.push_back(data(row_idx, j));
  }
  return row;
}

void print_cluster_memberships(View& v) {
  cout << "Cluster memberships" << endl;
  setCp_it it = v.clusters.begin();
  for(; it!=v.clusters.end(); it++) {
    Cluster<double> &cd = **it;
    cout << cd.get_global_row_indices() << endl;
  }
  cout << "num clusters: " << v.get_num_clusters() << endl;

}

void insert_and_print(View& v, map<int, vector<double> > data_map,
		      int cluster_idx, int row_idx) {
  vector<double> row = data_map[row_idx];
  Cluster<double>& cluster = v.get_cluster(cluster_idx);
  v.insert_row(row, cluster, row_idx);
  cout << "v.insert_row(" << row << ", " << cluster_idx << ", " \
	    << row_idx << ")" << endl;
  cout << "v.get_score(): " << v.get_score() << endl;
}

void remove_all_data(View &v, map<int, vector<double> > data_map) {
  vector<int> rows_in_view;
  for(mapICp_it it=v.cluster_lookup.begin(); it!=v.cluster_lookup.end(); it++) {
    rows_in_view.push_back(it->first);
  }
  for(vectorI_it it=rows_in_view.begin(); it!=rows_in_view.end(); it++) {
    int idx_to_remove = *it;
    vector<double> row = data_map[idx_to_remove];
    v.remove_row(row, idx_to_remove);
  }
  cout << "removed all data" << endl;
  v.print();
  //
  for(setCp_it it=v.clusters.begin(); it!=v.clusters.end(); it++) {
    v.remove_if_empty(**it);
  }
  assert(v.get_num_vectors()==0);
  assert(v.get_num_clusters()==0);
  cout << "removed empty clusters" << endl; 
  v.print();
}

int main(int argc, char** argv) {
  cout << endl << "Hello World!" << endl;

  // load some data
  matrixD data;
  LoadData("Synthetic_data.csv", data);
  int num_cols = data.size2();
  double init_crp_alpha = 3;
  //
  map<int, vector<double> > data_map;
  for(int idx=0; idx<6; idx++) {
    vector<double> row = extract_row(data, idx);
    data_map[idx] = row;
  }
  //
  map<int, int> where_to_push;
  where_to_push[0] = 0;
  where_to_push[1] = 1;
  where_to_push[2] = 0;
  where_to_push[3] = 1;
  where_to_push[4] = 0;
  where_to_push[5] = 1;
  //
  set<int> cluster_idx_set;
  for(map<int,int>::iterator it=where_to_push.begin(); it!=where_to_push.end(); it++) {
    int cluster_idx = it->second;
    cluster_idx_set.insert(cluster_idx);
  }

  // create the objects to test
  View v = View(num_cols, init_crp_alpha);
  //
  vectorCp cd_v;
  for(set<int>::iterator it=cluster_idx_set.begin(); it!=cluster_idx_set.end(); it++) {
    int cluster_idx = *it;
    cout << "inserting cluster idx: " << cluster_idx << endl;
    Cluster<double> *p_cd = new Cluster<double>(num_cols);
    cd_v.push_back(p_cd);
  }

  cout << "empty view print" << endl;
  v.print();
  cout << "empty view print" << endl;
  cout << endl;
  
  // populate the objects to test
  cout << endl << "populating objects" << endl;
  cout << "=================================" << endl;
  for(map<int,int>::iterator it=where_to_push.begin(); it!=where_to_push.end(); it++) {
    int row_idx = it->first;
    int cluster_idx = it->second;
    cout << "INSERTING ROW: " << row_idx << endl;
    insert_and_print(v, data_map, cluster_idx, row_idx);
    Cluster<double> *p_cd = cd_v[cluster_idx];
    double cluster_score_delta = (*p_cd).insert_row(data_map[row_idx], row_idx);
    cout << "cluster_score_delta: " << cluster_score_delta << endl;
    cout << "DONE INSERTING ROW: " << row_idx << endl;
  }
  cout << endl << "view after population" << endl;
  v.print();
  cout << "view after population" << endl;
  cout << "=================================" << endl;
  cout << endl;

  cout << endl << "separately created clusters after population" << endl;
  for(vectorCp::iterator it=cd_v.begin(); it!=cd_v.end(); it++) {
    cout << **it << endl;
  }
  cout << endl;

  // test
  vector<double> cluster_scores;
  setCp_it it = v.clusters.begin();
  double sum_scores;
  for(; it!=v.clusters.end(); it++) {
    double cluster_score = (*it)->get_score();
    cluster_scores.push_back(cluster_score);
    sum_scores += cluster_score;
  }
  vector<int> cluster_counts = v.get_cluster_counts();
  double crp_score = numerics::calc_crp_alpha_conditional(cluster_counts,
							  v.get_crp_alpha(),
							  -1, true);
  double crp_plus_data_score = crp_score + sum_scores;
  cout << "vector of cluster scores: " << cluster_scores << endl;
  cout << "sum cluster scores: " << sum_scores << endl;
  cout << "crp score: " << crp_score << endl;
  cout << "sum cluster scores and crp score: " << crp_plus_data_score << endl;
  cout << "view score: " << v.get_score() << endl;
  assert(is_almost(v.get_score(), crp_plus_data_score, 1E-10));

  vector<double> test_alphas;
  test_alphas.push_back(.3);
  test_alphas.push_back(3.);
  test_alphas.push_back(30.);
  cluster_counts = v.get_cluster_counts();  
  vector<double> test_alpha_scores;
  for(vector<double>::iterator it=test_alphas.begin(); it!=test_alphas.end(); it++) {
    double test_alpha_score = numerics::calc_crp_alpha_conditional(cluster_counts, *it, -1, true);
    test_alpha_scores.push_back(test_alpha_score);
  }
  cout << "test_alphas: " << test_alphas << endl;
  cout << "test_alpha_scores: " << test_alpha_scores << endl;
  double new_alpha = test_alphas[0];
  double crp_score_delta = v.set_alpha(new_alpha);
  cout << "new_alpha: " << new_alpha << ", new_alpha score: " << v.get_crp_score() << ", crp_score_delta: " << crp_score_delta << endl;
  new_alpha = test_alphas[1];
  crp_score_delta = v.set_alpha(new_alpha);
  cout << "new_alpha: " << new_alpha << ", new_alpha score: " << v.get_crp_score() << ", crp_score_delta: " << crp_score_delta << endl;


  print_cluster_memberships(v);
  int num_vectors = v.get_num_vectors();
  cout << "num_vectors: " << v.get_num_vectors() << endl;
  //
  cout << "====================" << endl;
  cout << "Sampling" << endl;

  // test transition_zs
  RandomNumberGenerator rng = RandomNumberGenerator();
  for(int iter=0; iter<100; iter++) {
    v.transition_zs(data_map);
    //if(iter % 10 == 0) {

    if(iter % 1 == 0) {
      print_cluster_memberships(v);
      cout << "Done iter: " << iter << endl;
      cout << endl;
    }
  }
  print_cluster_memberships(v);
  cout << "Done transition_zs" << endl;
  cout << endl;

  // empty object and verify empty
  remove_all_data(v, data_map);
  v.print();

  for(vectorCp::iterator it = cd_v.begin(); it!=cd_v.end(); it++) {
    delete (*it);
  }

  cout << endl << "Goodbye World!" << endl;
}