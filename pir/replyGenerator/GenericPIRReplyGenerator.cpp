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

#include "GenericPIRReplyGenerator.hpp"

imported_database::~imported_database(){
    for (unsigned int i = 0 ; i < nbElements ; i++){
        // uh...
        //free(((lwe_in_data *)imported_database_ptr)[i].p[0]);
        //free(((lwe_in_data *)imported_database_ptr)[i].p);
    }
    free(imported_database_ptr);
}


GenericPIRReplyGenerator::GenericPIRReplyGenerator():
  pirParam(emptyPIRParams),
  repliesArray(NULL),
  repliesAmount(0),
  repliesIndex(0)
{
  pirParam.d = 0;
  pirParam.alpha = 0;
  for (int i = 0 ; i < MAX_REC_LVL; i++) pirParam.n[i] = 0;
  mutex.lock();
}

GenericPIRReplyGenerator::GenericPIRReplyGenerator(PIRParameters& param, DBHandler *db):
  pirParam(param),
  dbhandler(db),
  repliesArray(NULL),
  repliesAmount(0),
  repliesIndex(0)
{
  mutex.lock();
}

/*
void GenericPIRReplyGenerator::setPirParams(PIRParameters& param)
{
  pirParam = param;
}
*/

void GenericPIRReplyGenerator::pushQuery(char *rawQuery)
{
    pushQuery(rawQuery, cryptoMethod->getPublicParameters().getCiphertextBitsize()/8, current_dim_index, current_query_index);
    current_query_index++;
    if (current_query_index >= pirParam.n[current_dim_index])
    {
        current_query_index = 0;
        current_dim_index++;
    }
    if (current_dim_index >= pirParam.d)
    {
        std::cout << "PIRReplyGenerator: Finished importing query (this message should appear only once)" << std::endl;
    }
}

void GenericPIRReplyGenerator::freeResult()
{
  for(unsigned i=0 ; i < repliesAmount; i++)
  {
    if(repliesArray[i]!=NULL) delete[] repliesArray[i];
    repliesArray[i] = NULL;
  }
  free(repliesArray);
  repliesArray=NULL;
}
