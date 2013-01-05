#include <iostream>
#include "Cluster.h"
#include "utils.h"
#include "numerics.h"
#include "RandomNumberGenerator.h"

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>

using namespace std;

int main(int argc, char** argv) {
  cout << "Begin:: test_cluster" << endl;
  RandomNumberGenerator rng;

  // set some test sizing parameters
  int max_value = 20;
  int num_rows = 3;
  int num_cols = 3;

  // creat the objects
  Cluster<double> cd(num_cols);
  vector<Suffstats<double> > sd_v;
  for(int row_idx=0; row_idx<num_rows; row_idx++) {
    Suffstats<double> sd;
    sd_v.push_back(sd);
  }

  // generate random data;
  vector<vector<double> > rows;
  for(int row_idx=0; row_idx<num_rows; row_idx++) {
    vector<double> row_data;
    for(int col_idx=0; col_idx<num_cols; col_idx++) {
      double random_value = (rng.nexti(max_value) + 1) * rng.next();
      row_data.push_back(random_value);
    }
    rows.push_back(row_data);
  }

  // poplute the objects
  cout << "Populating objects" << endl;
  for(int row_idx=0; row_idx<num_rows; row_idx++) {
    vector<double> row_data = rows[row_idx];
    for(int col_idx=0; col_idx<num_cols; col_idx++) {
      double random_value = rows[row_idx][col_idx];
      sd_v[col_idx].insert_el(random_value);
    }
    cd.insert_row(row_data, row_idx);
  }

  // test score equivalence
  vector<double> score_v;
  double sum_scores = 0;
  for(int col_idx=0; col_idx<num_cols; col_idx++) {
    double suff_score = sd_v[col_idx].get_score();
    score_v.push_back(suff_score);
    sum_scores += suff_score;
  }
  cout << "vector of separate suffstats scores after population: ";
  cout << score_v << endl;
  cout << "sum separate scores: " << sum_scores << endl;
  cout << "Cluster score with same data: " << cd.get_score() << endl;
  cout << endl;
  //
  assert(is_almost(sum_scores, cd.get_score(), 1E-10));

  // depopulate the objects
  cout << "De-populating objects" << endl;
  for(int row_idx=0; row_idx<num_rows; row_idx++) {
    vector<double> row_data = rows[row_idx];
    for(int col_idx=0; col_idx<num_cols; col_idx++) {
      double random_value = rows[row_idx][col_idx];
      sd_v[col_idx].remove_el(random_value);
    }
    cd.remove_row(row_data, row_idx);
  }

  // test score equivalence
  score_v.clear();
  sum_scores = 0;
  for(int col_idx=0; col_idx<num_cols; col_idx++) {
    double suff_score = sd_v[col_idx].get_score();
    score_v.push_back(suff_score);
    sum_scores += suff_score;
  }
  cout << "vector of separate suffstats scores after depopulation: ";
  cout << score_v << endl;
  cout << "sum separate scores: " << sum_scores << endl;
  cout << "Cluster score with same data: " << cd.get_score() << endl;
  cout << endl;
  //
  assert(is_almost(sum_scores, cd.get_score(), 1E-10));


  // double sum_sum_score_deltas;
  // boost::numeric::ublas::matrix<double> Data;
  // LoadData("SynData2.csv", Data);
  // // std::cout << Data << std::endl;

  // cd = Cluster<double>(5); //hard code # columns
  // std::cout << std::endl << "Init cluster" << std::endl;
  // std::cout << cd << std::endl;

  // sum_sum_score_deltas = 0;
  // for(int i=0; i < 4; i++) {
  //   std::vector<double> V;
  //   for(int j=0;j < Data.size2(); j++) {
  //     V.push_back(Data(i,j));
  //   }
  //   sum_sum_score_deltas += cd.insert_row(V, i);
  // }
  // std::cout << std::endl << "modified cluster" << std::endl;
  // std::cout << cd << std::endl;
  // std::cout << "sum_sum_score_deltas: " << sum_sum_score_deltas << std::endl;
  // std::cout << std::endl << "logps" << std::endl;
  // std::cout << cd.calc_logps() << std::endl;;
  // std::cout << std::endl << "sum logp" << std::endl;
  // std::cout << cd.calc_sum_logp() << std::endl;;
  // //
  // sum_sum_score_deltas = 0;
  // for(int i=4; i < 8; i++) {
  //   std::vector<double> V;
  //   for(int j=0;j < Data.size2(); j++) {
  //     V.push_back(Data(i,j));
  //   }
  //   sum_sum_score_deltas += cd.insert_row(V, i);
  // }
  // std::cout << std::endl << "modified cluster" << std::endl;
  // std::cout << cd << std::endl;
  // std::cout << "sum_sum_score_deltas: " << sum_sum_score_deltas << std::endl;
  // std::cout << std::endl << "logps" << std::endl;
  // std::cout << cd.calc_logps() << std::endl;;
  // std::cout << std::endl << "sum logp" << std::endl;
  // std::cout << cd.calc_sum_logp() << std::endl;;
  // //
  // int i = 8;
  // std::vector<double> V;
  // for(int j=0;j<Data.size2(); j++) {
  //   V.push_back(Data(i,j));
  // }
  // double vector_logp = cd.get_vector_logp(V);
  // std::cout << "add vector with vector_logp" << std::endl;
  // std::cout << vector_logp << std::endl;
  // std::cout << "calc_data_logp() is result" << std::endl;
  // std::cout << cd.calc_data_logp(V) << std::endl;

  // cd.insert_row(V, i);
  // std::cout << std::endl << "modified cluster" << std::endl;
  // std::cout << cd << std::endl;
  // std::cout << "remove vector" << std::endl;
  // cd.remove_row(V, i);
  // std::cout << std::endl << "modified cluster" << std::endl;
  // std::cout << cd << std::endl;
  // //
  // std::cout << "calculate logps from scratch" << std::endl;
  // std::cout << std::endl << "logps" << std::endl;
  // std::cout << cd.calc_logps() << std::endl;;
  // std::cout << std::endl << "sum logp" << std::endl;
  // std::cout << cd.calc_sum_logp() << std::endl;;
  
  // print_defaults();

  // std::cout << "show use of underlying numerics functions" << std::endl;
  // std::cout << "calc_continuous_logp(0, 1, 2, 2, 0)" << std::endl;
  // std::cout << numerics::calc_continuous_logp(0, 1, 2, 2, 0) << std::endl;
  // std::cout << "calc_cluster_crp_logp(10, 100, 10)" << std::endl;
  // std::cout << numerics::calc_cluster_crp_logp(10, 100, 10) << std::endl;

  cout << "Stop:: test_cluster" << endl;
}