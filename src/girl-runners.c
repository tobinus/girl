#include "girl-runners.h"

GirlRunnersInfo *girl_runners_new (GirlRunnersInfo * head,
				   gchar *id,
				   gchar *name,
				   gchar *date,
				   gchar *time,
				   gchar *file) {}
void girl_runners_free(GirlRunnersInfo * info) {}
void girl_runners_mutex_init(GirlRunnersInfo *info, GMutex runner) {}
void girl_runners_mutex_lock(GirlRunnersInfo *info, GMutex runner) {}
void girl_runners_mutex_unlock(GirlRunnersInfo *info, GMutex mutex) {}
void girl_runners_mutex_exit(GirlRunnersInfo *info, GMutex mutex) {}

#if 0
void girl_helper_run(char *command)
{
  GMutex mutex;
  int recording_lock;
  girl_runner_mutex_init(program,mutex);
  if (recording_lock == TRUE) {
    girl_runner_mutex_lock(program,mutex);
    recording_lock = TRUE;
  }
  if (recording_lock == FALSE) {
    girl_runner_mutex_unlock(program,mutex);
    recording_lock = FALSE;
    girl_runner_mutex_exit(program,mutex);
  }
  return (0);
}
#endif 
