#include <stdio.h>
#include <string>
#include <unistd.h>

int main(int argc, char** argv)
{
  std::string  fifoname = std::string(argv[1]);
  unsigned int bins(atoi(argv[2]));

  float* data(new float[bins]);
  FILE*  fifofile(0);

  fifoname += std::string(".bin");
  
  while (true)
  {
    fifofile=fopen(fifoname.c_str(), "rb");

    fread(data, sizeof(float), bins, fifofile);
    for (unsigned int i=0; i<bins; ++i) printf("%f\n", data[i]);

    fclose(fifofile);
  }

  printf("\n");

  return 0;
}
