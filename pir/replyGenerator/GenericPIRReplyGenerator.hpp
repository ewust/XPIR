/* Copyright (C) 2014 Carlos Aguilar Melchor, Joris Barrier, Marc-Olivier Killijian
 * This file is part of XPIR.
 *
 *  XPIR is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  XPIR is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XPIR.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DEF_ABSTRACTGENERATOR
#define DEF_ABSTRACTGENERATOR

#include <omp.h>
#include <string>

#include <boost/thread.hpp>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>
#include <boost/thread/recursive_mutex.hpp>

#include "pir/PIRParameters.hpp"
#include "pir/dbhandlers/DBHandler.hpp"
#include "crypto/CryptographicSystem.hpp"
#include "crypto/HomomorphicCrypto.hpp"
#include "pir/shared_queue.hpp"
#include "pir/GlobalConstant.hpp"

class imported_database_t {
  public:
  void* imported_database_ptr;
  uint64_t nbElements;
  uint64_t polysPerElement;
  uint64_t beforeImportElementBytesize;
  //uint64_t afterImportElementBytesize;
};


/**
 *  * Class storing the database (or a chunk of it) after pre-processing
 *   * Type returned by the funcion importData below
 *   **/
class imported_database : public imported_database_t
{
public:
    ~imported_database();
};



class GenericPIRReplyGenerator
{
  protected:


  PIRParameters emptyPIRParams;
	PIRParameters& pirParam;
	unsigned int maxChunkSize;
	DBHandler* dbhandler;	
    uint64_t current_query_index;
    uint64_t current_dim_index;

  public:
    HomomorphicCrypto *cryptoMethod;
	boost::mutex mutex;
  char** repliesArray = NULL;
  unsigned repliesAmount = 0;
  unsigned repliesIndex = 0;

    GenericPIRReplyGenerator();
    GenericPIRReplyGenerator(PIRParameters& param, DBHandler* db);
    virtual void setCryptoMethod(CryptographicSystem* cm)=0;
    virtual void setPirParams(PIRParameters&)=0;

    virtual void importData(uint64_t offset, uint64_t bytes_per_db_element)=0;

    virtual imported_database *importDatabase(uint64_t offset, uint64_t bytes_per_db_element)=0;

    virtual void initQueriesBuffer()=0;
    virtual imported_database_t generateReplyGeneric(bool keep_imported_data = false)=0;
    virtual void generateReplyGenericFromData(const imported_database_t database)=0;
    virtual void generateReplyGenericFromData(const imported_database_t *database)=0;
    virtual double generateReplySimulation(const PIRParameters& pir_params, uint64_t plaintext_nbr)=0;
		virtual unsigned long computeReplySizeInChunks(unsigned long int)=0;
		virtual void pushQuery(char* rawQuery, unsigned int size, int dim, int nbr)=0;
        void pushQuery(char *rawQuery);
    virtual ~GenericPIRReplyGenerator(){};
    
    virtual double precomputationSimulation(const PIRParameters& pir_params, uint64_t plaintext_nbr) { return 0.0;}
    void freeResult();
    virtual void freeQueries()=0;
};

#endif
