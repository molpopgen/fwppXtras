#include <iostream>
#include <fwdpp/diploid.hh>

#include <Sequence/SimData.hpp>

#include <Sequence/SimDataIO.hpp> //for writing & reading SimData objects in binary format
#include <Sequence/FST.hpp>
#include <numeric>
#include <cmath>
#include <functional>
#include <cassert>
#include <iomanip>
#include <fstream>
#include <fwdpp/sugar/infsites.hpp>
#include <fwdpp/sugar/serialization.hpp>
//the type of mutation
using mtype = KTfwd::mutation;
#define METAPOP_SIM
#include <common_ind.hpp>

using poptype = metapop_serialized_t;
using mlist = poptype::mlist_t;
using gtype = poptype::gamete_t;
using glist = poptype::glist_t;

using namespace std;
using namespace KTfwd;
using namespace Sequence;


//simplistic model for 500 demes, equally likely to migrate to/from anywhere.
size_t migpop(const size_t & source_pop, gsl_rng * r, const double & mig_prob)
{
  //if a migrant
  if( gsl_rng_uniform(r) <= mig_prob )
    {
      //get a new parental population
      unsigned ppop = gsl_rng_uniform_int(r,3);
      while(ppop==source_pop)
	{
	  ppop = gsl_rng_uniform_int(r,3);
	}
      return ppop;
    }
  return source_pop;
}

SimData merge( const std::vector<std::pair<double,std::string> > & sample1,
			 const std::vector<std::pair<double,std::string> > & sample2 ,
			 const unsigned & nsam);



int main( int argc, char ** argv )
{
  //migrate and seed are only arguments
  const double migrate = atof(argv[1]);
  const unsigned seed = atoi(argv[2]);
  GSLrng r(seed);
  //3 demes with N=5000
  std::vector<unsigned> Ns(3,5000);
  std::vector<double> fs(3,0.);
  poptype pop(&Ns[0],Ns.size());

  //"respectable" mutation rate
  const double theta = 100;
  //no crossing over
  const double littler = 0.;

  //mut. rate per gamete
  const double mu_neutral = theta/(4.*10*500.);
  //no selection
  std::vector<std::function<double (glist::const_iterator,
				    glist::const_iterator)> > vbf(3,
								  std::bind(KTfwd::no_selection(),std::placeholders::_1,std::placeholders::_2));


  //recombination map is uniform[0,1)
  std::function<double(void)> recmap = std::bind(gsl_rng_uniform,r);

  //Let's evolve it for 10*sum(Ns) generations.
  for( unsigned generation = 0 ; generation < 5000 ; ++generation )
    {
      //if(generation%500==0.)std::cerr<<generation<<'\n';
      std::vector<double> wbars = sample_diploid(r,
						 &pop.gametes,
						 &pop.diploids,
						 &pop.mutations,
						 &Ns[0],
						 mu_neutral,
						 std::bind(KTfwd::infsites(),r,&pop.mut_lookup,
							   mu_neutral,0,[&r](){return gsl_rng_uniform(r);},[](){return 0.;},[](){return 1.;}),
						 std::bind(KTfwd::genetics101(),std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,
							     littler,
							     r,
							     recmap),
						 std::bind(insert_at_end<mtype,mlist>,std::placeholders::_1,std::placeholders::_2),
						 std::bind(insert_at_end<gtype,glist>,std::placeholders::_1,std::placeholders::_2),
						 vbf,
						 //Only remove extinct mutations
						 std::bind(mutation_remover(),std::placeholders::_1,0),
						 std::bind(migpop,std::placeholders::_1,r,migrate),
						 &fs[0]);
      //4*N b/c it needs to be fixed in the metapopulation
      remove_lost(&pop.mutations);
    }
  auto x = KTfwd::ms_sample(r,&pop.diploids[0],10,true);
  for( const auto & __x : x )
    {
      std::cout << __x.first << ' ' << __x.second << '\n';
    }
}

