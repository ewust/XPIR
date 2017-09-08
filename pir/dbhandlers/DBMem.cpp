// ewust
#include "DBMem.hpp"

/************************************************/
/* Default constructor : no splitting           */
/*  -> 1 input file -> 1 output stream          */
/************************************************/
DBMem::DBMem(uint64_t nbStreams, uint64_t streamBytesize, bool silent):
random_engine(0), // Fixed seed of 0
random_distribution()
{
	maxFileBytesize=streamBytesize;
	nbFiles = 0;
#ifdef SEND_CATALOG
  for (unsigned int i = 0 ; i < nbStreams ; i++) {
	std::string fileName= std::to_string(i);
    file_list.push_back( fileName );
	}
#endif
  nbFiles = nbStreams;
  data.reserve(nbFiles * maxFileBytesize);
  if(!silent)
  {
	  std::cout << "DBMem: The size of the database is " << maxFileBytesize*nbFiles << " bytes" << std::endl;
	  std::cout << "DBMem: The number of elements in the catalog is " << nbFiles << std::endl;
  }
}

DBMem::~DBMem() {}

// ???
std::string DBMem::getCatalog(const bool typeOfCatalog) {
    std::cout << "getCatalog called, dunno what to do" << std::endl;
	std::string buf;
	if(typeOfCatalog) {
		// Start with the number of elements in the catalog
		buf = std::to_string((unsigned int)0)+ "\n";	
		buf += std::to_string(getNbStream())+ "\n";		
		// Then for each file contactenate (with newlines) filename and filesize
		for (auto f : file_list)
		{
			buf += f + "\n" + std::to_string(maxFileBytesize) + "\n";
		}
		return buf;
	} 
	// else we want a compact representation, i.e. nbFiles / fileSize
	buf = std::to_string((unsigned int)1)+ "\n";	
	buf += std::to_string(getNbStream())+ "\n";
	buf += std::to_string(maxFileBytesize)+ "\n";
	return buf;
}

//uint64_t DBMem::getDBSizeBits() {
//	return maxFileBytesize*nbFiles*8;
//}
uint64_t DBMem::getNbStream() {
	return  nbFiles;
}
uint64_t DBMem::getmaxFileBytesize() {
	return maxFileBytesize;
}

// Really the XPIR code should be using istream so we don't have to do this
// but we can just hack it...don't try to call any ifstream functions on it!
std::ifstream* DBMem::openStream(uint64_t streamNb, uint64_t requested_offset) {

    uint64_t *fake_ifs = (uint64_t*)malloc(sizeof(uint64_t));
    *fake_ifs = streamNb*maxFileBytesize + requested_offset;
	return (std::ifstream*)fake_ifs;
}

uint64_t DBMem::readStream(std::ifstream* s, char * buf, uint64_t size) {

    uint64_t *fake_ifs = (uint64_t*)s;
    memcpy(buf, data.data()+*fake_ifs, size);
    *fake_ifs += size;
    return size;
}

uint64_t DBMem::addFile(uint64_t streamNb, uint8_t *buf, uint64_t size) {
    if (size != maxFileBytesize) {
        std::cout << "Warning: writing " << size << " less than " <<
                maxFileBytesize << " bytes to index " << streamNb << std::endl;
    }

    uint64_t i;
    for (i=0; i<size; i++) {
        data.insert(data.begin() + streamNb*maxFileBytesize + i, (const uint8_t)buf[i]);
    }
    //data.insert(data.begin() + streamNb*maxFileBytesize, size, (const uint8_t*)buf);
}

void DBMem::closeStream(std::ifstream* s) {
    uint64_t *fake_ifs = (uint64_t*)s;
    free(fake_ifs);
}

void DBMem::readAggregatedStream(uint64_t streamNb, uint64_t alpha, uint64_t offset, uint64_t bytes_per_file, char* rawBits){
    //readStream(NULL, NULL, 0);
	uint64_t fileByteSize = std::min(bytes_per_file, maxFileBytesize-offset);
    uint64_t startStream = streamNb*alpha;
    uint64_t endStream = std::min(streamNb*alpha + alpha - 1, getNbStream() - 1);
    uint64_t paddingStreams = std::max((long long)((streamNb)*alpha+alpha) - (long long)getNbStream(), (long long)0);

    //memcpy(rawBits, data.data()+??, 
    //memset(rawBits, 0xaa, fileByteSize*(endStream-startStream + 1));

	for (int i=startStream; i <= endStream; i++)
	{
	    std::ifstream *stream = openStream(i, offset);

	    // Just read the file (plus padding for that file)
	    readStream(stream, rawBits + (i % alpha) * fileByteSize, fileByteSize);

		closeStream(stream);
	}

    if(paddingStreams !=0)
    {
	    bzero(rawBits + (endStream % alpha) * fileByteSize, fileByteSize*paddingStreams);
    }
}

