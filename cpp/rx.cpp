#include <iostream>
#include <libbladeRF.h>

using namespace std;
//g++ rx.cpp -lbladeRF -L/usr/local/lib



int main() 
{
	struct bladerf *dev = (bladerf *)malloc(10000*sizeof(double));
	const char *dev_id = (const char*)malloc(1000*sizeof(double));
	bladerf_open(&dev, dev_id);
	
	bladerf_close(dev);
	return 0;
}
