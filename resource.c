/*
 * resource.c
 *
 *  Created on: 11/apr/2013
 *      Author: user
 */

#include "ourhdr.h"

struct timeb g_TStart, g_TStop;
struct rusage *g_RStart = NULL, *g_RStop = NULL;
short countingStarted = FALSE, countingStopped = FALSE;

void startCounting() {
  countingStarted = TRUE;

  ftime(&g_TStart);

  if (g_RStart == NULL)
    if ((g_RStart = malloc(sizeof(struct rusage))) == NULL)
      return;

  if (getrusage(RUSAGE_SELF, g_RStart) < 0)
    err_ret("Can't get starting values of resource usage");
}

void stopCounting() {
  if (countingStarted == TRUE) {
    countingStopped = TRUE;

    ftime(&g_TStop);

    if (g_RStop == NULL)
      if ((g_RStop = malloc(sizeof(struct rusage))) == NULL)
        return;
    if (getrusage(RUSAGE_SELF, g_RStop) < 0)
      err_ret("Can't get final values of resource usage");
  }
}

void printResourceUsage() {
  if (countingStopped == FALSE)
    stopCounting();

  fprintf(stderr, "\nTotal Elapsed Time: %.3f\n",
          TimeDiffmSec(g_TStart, g_TStop));
  fprintf(stderr, "Total Sys Time    : %.3f\n",
          TimeDiffuSec(g_RStart->ru_stime, g_RStop->ru_stime));
  fprintf(stderr, "Total User Time   : %.3f\n",
          TimeDiffuSec(g_RStart->ru_utime, g_RStop->ru_utime));
  fprintf(stderr, "Blocking I/O oper.: %ld (in) %ld (out)\n",
          g_RStop->ru_inblock - g_RStart->ru_inblock,
          g_RStop->ru_oublock - g_RStart->ru_oublock);

  fprintf(stderr, "Max resident size : %ld\n", g_RStop->ru_maxrss); /*  */
  fprintf(stderr, "Integral shared memory size: %ld\n",
          g_RStop->ru_ixrss); /* */
  fprintf(stderr, "Integral unshared data size: %ld\n",
          g_RStop->ru_idrss); /* */
  fprintf(stderr, "Integral unshared stack size: %ld\n",
          g_RStop->ru_isrss); /*  */
  fprintf(stderr, "Page reclaims (soft page faults): %ld\n",
          g_RStop->ru_minflt); /*  */
  fprintf(stderr, "Page faults (hard page faults): %ld\n",
          g_RStop->ru_majflt);                                      /* ) */
  fprintf(stderr, "# Swap operations: %ld\n\n", g_RStop->ru_nswap); /* swaps */
}

double getRealTime() {
  if (countingStarted == FALSE || countingStopped == FALSE)
    return 0.;
  else
    return TimeDiffmSec(g_TStart, g_TStop);
}

double getSysTime() {
  if (countingStarted == FALSE || countingStopped == FALSE || g_RStop == NULL ||
      g_RStart == NULL)
    return 0.;
  else
    return TimeDiffuSec(g_RStart->ru_stime, g_RStop->ru_stime);
}

double getUserTime() {
  if (countingStarted == FALSE || countingStopped == FALSE || g_RStop == NULL ||
      g_RStart == NULL)
    return 0.;
  else
    return TimeDiffuSec(g_RStart->ru_utime, g_RStop->ru_utime);
}
double TimeDiffmSec(struct timeb Start, struct timeb Stop) {
  double diff;

  if (Stop.millitm < Start.millitm) {
    Stop.millitm += 1000;
    Stop.time -= 1;
  }
  diff =
      (double)(Stop.time - Start.time) + (Stop.millitm - Start.millitm) / 1000.;
  return diff;
}

double TimeDiffuSec(struct timeval Start, struct timeval Stop) {
  if (Stop.tv_usec < Start.tv_usec) {
    Stop.tv_usec += 1000000;
    Stop.tv_sec -= 1;
  }
  return (double)(Stop.tv_sec - Start.tv_sec +
                  (Stop.tv_usec - Start.tv_usec) / 1000000.);
}
