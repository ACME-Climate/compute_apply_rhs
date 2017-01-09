#include "data_structures.hpp"
#include "compute_and_apply_rhs.hpp"

#include <iostream>
#include <cstring>
#include <sys/time.h>

namespace Homme
{
int num_elems = 10;
}

bool is_unsigned_int(const char* str)
{
  const size_t len = strlen (str);
  for (size_t i = 0; i < len; ++i) {
    if (! isdigit (str[i])) {
      return false;
    }
  }
  return true;
}

int main (int argc, char** argv)
{
  using namespace Homme;

  if (argc > 1) {
    int iarg = 1;
    while (iarg<argc)
    {
      if (strncmp(argv[iarg],"--tinman-num-elems=",18) == 0)
      {
        char* number =  strchr(argv[iarg],'=')+1;
        if (!is_unsigned_int(number))
        {
          std::cerr << "Expecting an unsigned integer after '--tinman-num-elems='.\n";
          std::exit(1);
        }

        num_elems = std::atoi(number);

        ++iarg;
        continue;
      }

      if (strncmp(argv[iarg],"--tinman-help",13) == 0)
      {
        std::cout << "+---------------------------------------------------------------+\n"
                  << "|                 TinMan command line arguments                 |\n"
                  << "+---------------------------------------------------------------+\n"
                  << "|  --tinman-num-elems  : the number of elements (default=10)    |\n"
                  << "|  --tinman-help       : prints this message                    |\n"
                  << "+---------------------------------------------------------------+\n";

        std::exit(0);
      }

      ++iarg;
    }
  }

  if (num_elems < 1) {
    std::cerr << "Invalid number of elements: " << num_elems << std::endl;
    std::exit(1);
  }

  struct timeval start, end;

  TestData data;

  std::cout << " --- Initializing data...\n";
  data.init_data();

  print_results_2norm (data);

  std::cout << " --- Performing computations...\n";
  gettimeofday(&start, NULL);
  compute_and_apply_rhs(data);
  gettimeofday(&end, NULL);
  double delta = ((end.tv_sec  - start.tv_sec) * 1000000u +
                   end.tv_usec - start.tv_usec) / 1.e6;
  std::cout << "   ---> compute_and_apply_rhs execution time: " << delta << " seconds.\n";

  print_results_2norm (data);

  std::cout << " --- Dumping results to file...\n";
  dump_results_to_file (data);

  std::cout << " --- Cleaning up data...\n";
  data.cleanup_data ();

  return 0;
}