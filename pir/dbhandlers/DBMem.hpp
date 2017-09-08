/* ewust */
#ifndef DEF_DBMEM
#define DEF_DBMEM

#include "DBHandler.hpp"
#include <cstring>
#include <random>
#include <vector>
#include <omp.h>

class DBMem : public DBHandler
{
private:
  std::vector <std::string> file_list; // the output file list
public:
  DBMem(uint64_t nbStreams, uint64_t streamBytesize, bool silent); 
  ~DBMem();
  
  std::string getCatalog(const bool typeOfCatalog);
  
  uint64_t getNbStream();
  uint64_t getmaxFileBytesize();
  
  std::ifstream* openStream(uint64_t streamNb, uint64_t requested_offset);
  uint64_t readStream(std::ifstream* s, char * buf, uint64_t size);
  void readAggregatedStream(uint64_t streamNb, uint64_t alpha, uint64_t offset, uint64_t bytes_per_file, char* rawBits);
  void closeStream(std::ifstream* s);
  uint64_t addFile(uint64_t streamNb, uint8_t *buf, uint64_t size);
  
private:
    std::mt19937_64 random_engine; // Fixed seed of 0
    std::uniform_int_distribution<> random_distribution;

    std::vector<uint8_t> data;
  
    uint64_t maxFileBytesize;
    uint64_t nbFiles;
};

#endif //DEF_DBMEM
