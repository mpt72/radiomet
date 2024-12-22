#include <stdio.h>
#include <string.h>

#define LINELENGTH 128
#define DATEPART 13

int main(int argc, char** argv)
{
  char line[LINELENGTH];
  memset(line, 0, LINELENGTH);

  // 2024-01-01-01
  // 1234567890123 -> 13

  char filename[DATEPART+1];
  memset(filename, 0, DATEPART+1);

  FILE* fout(0);

  while (!feof(stdin))
  {
    fgets(line, LINELENGTH, stdin);
    printf("%s", line);

    if (strncmp(filename, line, DATEPART))
    {
      if (fout) fclose(fout);

      strncpy(filename, line, DATEPART);
      fout=fopen(filename, "w");
    }

    fprintf(fout, "%s", line);
  }

  if (fout) fclose(fout);

  return 0;
}
