#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define LINESIZE 32
#define OUTSIZE 80

#define MAXBUFFER 512

int main(int argc, char** argv)
{
  int   startFreq(atoi(argv[1]));
  int   stopFreq (atoi(argv[2]));
  int   bins     (atoi(argv[3]));
  int   anaStart (atoi(argv[4]));
  int   anaStop  (atoi(argv[5]));
  float treshold (atof(argv[6]));

  double freqStep(stopFreq-startFreq);
  freqStep/=bins;

  int    idxStart(0);
  int    idxStop (0);
  double freq(startFreq);

  for (unsigned int i=0; (i<bins) && (0==idxStop); ++i)
  {
    if ((0==idxStart) && (freq>=anaStart)) idxStart=i;
    if (freq>=anaStop) idxStop=i;
    freq=freq+freqStep;
  }

  int use(idxStop-idxStart+1);

  float* history[MAXBUFFER];
  for (unsigned int i=0; i<MAXBUFFER; ++i)
  {
    history[i]=new float[use];
    for (unsigned int j=0; j<use; ++j) history[i][j]=0;
  }
  unsigned int historyPtr(0);

  float* data     (new float[bins]);
  float* diffs    (new float[bins]);
  char   inBuffer[LINESIZE];
  memset(inBuffer, 0, LINESIZE);

  float      average(0);
  float      maximum(0);
  bool       hasData(false);
  float      avgDiff(0);
  float      maxDiff(0);
  int        maxIdx(0);
  time_t     tnow;
  struct tm* ttime;
  bool       detection;
  int        metCounter(0);
  char       event[OUTSIZE];
  memset(event, 0, OUTSIZE);

  const char* MET  = "*METEOR*";
  const char* NONE = "";

  while (!feof(stdin))
  {
    // read one sample, calculate average and find maximum
    average=0;
    hasData=false;

    for (unsigned int i=0; i<bins; ++i)
    {
      fgets(inBuffer, LINESIZE, stdin);
      data[i]=atof(inBuffer);

      if ((i>=idxStart) && (i<idxStop))
      {
        if (hasData)
        {
          if (maximum<data[i]) maximum=data[i];
        }
        else
        {
          maximum=data[i];
          hasData=true;
        }

        average+=data[i];
      }
    }

    // calculate average
    average/=use;

    // average difference
    avgDiff=0;

    for (unsigned int i=idxStart; i<idxStop; ++i)
    {
      diffs[i]=sqrt(abs(average-data[i]));
      avgDiff+=diffs[i];
    }
    avgDiff/=use;

    // max difference
    maxDiff=0;
    maxIdx =0;
    for (unsigned int i=idxStart; i<idxStop; ++i)
    {
      if (maxDiff<diffs[i])
      {
        maxDiff=diffs[i];
        maxIdx=i;
      }
    }

    // output
    unsigned int freqAtMax(freqStep*maxIdx);
    freqAtMax+=startFreq;

    tnow =time(0);
    ttime=localtime(&tnow);

    sprintf(event,
            "%04d-%02d-%02d-%02d-%02d-%02d",
            ttime->tm_year+1900, ttime->tm_mon+1, ttime->tm_mday,
            ttime->tm_hour,      ttime->tm_min,   ttime->tm_sec);

    printf("%s | ", event);

    detection=(maxDiff>avgDiff*treshold);

    printf("A:%7.2f M:%7.2f AD:%7.2f MD:%7.2f (%3d %9d) | %s\n",
           average, maximum, avgDiff, maxDiff, maxIdx, freqAtMax,
           (detection ? MET : NONE));

    if (detection)
    {
      // meteor
      ++historyPtr;
    }

    if (!detection)
    {
      if (historyPtr>2)
      {
        // save all data from history buffer
        FILE* fout(fopen(event, "w"));
        fprintf(fout, "%d\n", use);
        for (unsigned int i=0; i<historyPtr; ++i)
          for (unsigned int j=0; j<use; ++j)
            fprintf(fout, "%f\n", history[i][j]);
        fclose(fout);
      }
      historyPtr=0;
    }

    // copy current data to history buffer
    unsigned int target(0);
    for (unsigned int i=idxStart; i<idxStop; ++i) history[historyPtr][target++]=data[i];
  }

  return 0;
}
