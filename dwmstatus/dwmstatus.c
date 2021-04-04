/*
 * Copy me if you can.
 * by 20h
 */

#define _BSD_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <strings.h>
#include <sys/time.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <X11/Xlib.h>

char *tzargentina = "America/Buenos_Aires";
char *tzutc = "UTC";
char *tzberlin = "Europe/Berlin";
char *American = "America/New_York";

static Display *dpy;

char *
smprintf(char *fmt, ...)
{
	va_list fmtargs;
	char *ret;
	int len;

	va_start(fmtargs, fmt);
	len = vsnprintf(NULL, 0, fmt, fmtargs);
	va_end(fmtargs);

	ret = malloc(++len);
	if (ret == NULL) {
		perror("malloc");
		exit(1);
	}

	va_start(fmtargs, fmt);
	vsnprintf(ret, len, fmt, fmtargs);
	va_end(fmtargs);

	return ret;
}

void
settz(char *tzname)
{
	setenv("TZ", tzname, 1);
}

char *
mktimes(char *fmt, char *tzname)
{
	char buf[129];
	time_t tim;
	struct tm *timtm;

	settz(tzname);
	tim = time(NULL);
	timtm = localtime(&tim);
	if (timtm == NULL)
		return smprintf("");

	if (!strftime(buf, sizeof(buf)-1, fmt, timtm)) {
		fprintf(stderr, "strftime == 0\n");
		return smprintf("");
	}
	strftime(buf, 129, "%I:%M %h %d", timtm); 

	return smprintf("%s", buf);
}

void
setstatus(char *str)
{
	XStoreName(dpy, DefaultRootWindow(dpy), str);
	XSync(dpy, False);
}

char *
loadavg(void)
{
	double avgs[3];

	if (getloadavg(avgs, 3) < 0)
		return smprintf("");

	return smprintf("%.2f %.2f %.2f", avgs[0], avgs[1], avgs[2]);
}

char *
readfile(char *base, char *file)
{
	char *path, line[513];
	FILE *fd;

	memset(line, 0, sizeof(line));

	path = smprintf("%s/%s", base, file);
	fd = fopen(path, "r");
	free(path);
	if (fd == NULL)
		return NULL;

	if (fgets(line, sizeof(line)-1, fd) == NULL)
		return NULL;
	fclose(fd);

	return smprintf("%s", line);
}

char *
getbattery(char *base)
{
	
	char *co, status;
	int descap, remcap;
	

	descap = -1;
	remcap = -1;

	// special case for mouse that doesn't have percentage indicators
	if (!strncmp("/sys/class/power_supply/hidpp_battery_0", base, 40)) {
		co = readfile(base, "capacity_level");
		 co[strlen(co)-1]=0;

		return smprintf("%s", co);
	}

	co = readfile(base, "present");
	if (co == NULL)
		return smprintf("");
	if (co[0] != '1') {
		free(co);
		return smprintf("not present");
	}
	free(co);

	co = readfile(base, "charge_full_design");
	if (co == NULL) {
		co = readfile(base, "energy_full_design");
		if (co == NULL)
			return smprintf("");
	}
	sscanf(co, "%d", &descap);
	free(co);

	co = readfile(base, "charge_now");
	if (co == NULL) {
		co = readfile(base, "energy_now");
		if (co == NULL)
			return smprintf("");
	}
	sscanf(co, "%d", &remcap);
	free(co);

	co = readfile(base, "status");
	if (!strncmp(co, "Discharging", 11)) {
		status = '-';
	} else if(!strncmp(co, "Charging", 8)) {
		status = '+';
	} else {
		status = '?';
	}

	if (remcap < 0 || descap < 0)
		return smprintf("invalid");

	return smprintf("%.0f%%%c", ((float)remcap / (float)descap) * 100, status);
}

char *
gettemperature(char *base, char *sensor)
{
	char *co;

	co = readfile(base, sensor);
	if (co == NULL)
		return smprintf("");
	return smprintf("%02.0f°C", atof(co) / 1000);
}

float getram(){
    int total, free, buffers, cached;
    FILE *f;

    f = fopen("/proc/meminfo", "r");

    if(f == NULL){
        perror("fopen");
        exit(1);
    }

    fscanf(f, "%*s %d %*s" // mem total
              "%*s %d %*s" // mem free
              "%*s %*d %*s" // mem available
              "%*s %d %*s" // buffers
              "%*s %d", //cached
              &total, &free, &buffers, &cached);
    fclose(f);

    return (float)(total-free-buffers-cached)/total * 100;
}

struct cpuusage{
    long int total, used;
};

struct cpuusage getcpu(){
    long int user, nice, system, idle, iowait, irq, softirq;
    struct cpuusage usage;

    FILE *f;
    f = fopen("/proc/stat", "r");

    if(f == NULL){
        perror("fopen");
        exit(1);
    }

    fscanf(f, "%*s %ld %ld %ld %ld %ld %ld %ld", &user, &nice, &system, &idle, &iowait, &irq, &softirq);

    usage.used = user + nice + system + irq +softirq;
    usage.total = user + nice + system + idle + iowait + irq +softirq;

    fclose(f);

    return usage;
}



int
main(void)
{
	char *status;
	char *avgs;
	char *bat;
	char *tmar;
	char *mouseBat;
	// char *temp;

	struct cpuusage cpu_i_usage = getcpu();
    struct cpuusage cpu_f_usage;

    double cpu_used, cpu_total;

	

	if (!(dpy = XOpenDisplay(NULL))) {
		fprintf(stderr, "dwmstatus: cannot open display.\n");
		return 1;
	}

	for (;;sleep(10)) {
		cpu_f_usage = getcpu();
		cpu_used = cpu_f_usage.used - cpu_i_usage.used;
        cpu_total = cpu_f_usage.total - cpu_i_usage.total;

		avgs = loadavg();
		bat = getbattery("/sys/class/power_supply/BAT0");
		mouseBat = getbattery("/sys/class/power_supply/hidpp_battery_0");
		tmar = mktimes("%H:%M", American);
		// temp = gettemperature("/sys/class/hwmon/hwmon0/device", "");
		status = smprintf(" R: %0.f%% | C: %.0f%% | B: %s | MB: %s | %s",
				 getram(), cpu_used/cpu_total*100, bat, mouseBat, tmar);
		setstatus(status);

		free(avgs);
		free(bat);
		free(tmar);
		free(status);
		cpu_i_usage = cpu_f_usage;

	}

	XCloseDisplay(dpy);

	return 0;
}
