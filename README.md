# SERENADE

This repo provides the core implementation of the sequential simulator of SERENADE, which is a parallel randomized algorithm for crossbar scheduling in input-queued switches. More details regarding this algorithm can be found in our paper.

## How to Use This Implementation 

We assume that you are using UBUNTU 18.04, although the codes should be able to run on other OSes.

### Install Dependencies

+ [boost c++ library](https://www.boost.org/): 
    ```bash 
    $ sudo apt-get install libboost-dev-all
    ```
+ [Google test (optional)](https://github.com/google/googletest): only required if you want to run the tests. Note that if the turn on google test, i.e., set `USE_GTest` in [CMakeLists.txt](./CMakeLists.txt) as `ON`, then Google test will be downloaded automatically.

+ [Armadillo C++ (optional)](http://arma.sourceforge.net/): only required if you want to run the tests. Armadillo is used to generate random matrix for some tests.

+ [Google Benchmark (optional)](https://github.com/google/benchmark): only required if you want to run benchmarking tests. It is used to benchmark the performance of Boost.Accumulators.

+ [GMP library (optional)](https://gmplib.org/): only required if you want to run the knowledge discovery procedure until it some vertex discovers some vertex twice for a large N. In this case, we need integers with extreme high precisions, thus we use `mpz_int` provided by GMP library. To enable it, you need to define a macro in [serenade.hpp](./serenade.hpp) as follows:
    ```C++ 
    #define TEST_EXTREME_LARGE_CASES
    ```
 
 ### Compile and Run
 
 ```bash 
 $ git clone https://github.com/long-gong/SERENADE.git
 $ mkdir build && cd build
 $ cmake ../SERENADE
 $ make
 $ make test
 ```