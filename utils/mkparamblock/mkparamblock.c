/* just some basic, very unfancy code for now */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#define USE_BLOB_TOGETHER_WITH_LIBC
#include <blob/param_block.h>
#include <blob/serial.h>

struct config_keyword {
	char keyword[40];
	int (*handler)(char *line, FILE *fp);
};

int convert_baud(char *arg)
{
	serial_baud_t retval;
	
	if(strcmp(arg, "1200") == 0) {
		retval = baud_1200;
	} else if(strcmp(arg, "1k2") == 0) {
		retval = baud_1200;
	} else if(strcmp(arg, "9600") == 0) {
		retval = baud_9600;
	} else if(strcmp(arg, "9k6") == 0) {
		retval = baud_9600;
	} else if(strcmp(arg, "19200") == 0) {
		retval = baud_19200;
	} else if(strcmp(arg, "19k2") == 0) {
		retval = baud_19200;
	} else if(strcmp(arg, "38400") == 0) {
		retval = baud_38400;
	} else if(strcmp(arg, "38k4") == 0) {
		retval = baud_38400;
	} else if(strcmp(arg, "57600") == 0) {
		retval = baud_57600;
	} else if(strcmp(arg, "57k6") == 0) {
		retval = baud_57600;
	} else if(strcmp(arg, "115200") == 0) {
		retval = baud_115200;
	} else if(strcmp(arg, "115k2") == 0) {
		retval = baud_115200;
	} else if(strcmp(arg, "230400") == 0) {
		retval = baud_230400;
	} else if(strcmp(arg, "230k4") == 0) {
		retval = baud_230400;
	} else return -1;
	
	return (int) retval;
}


int read_yn(char *line)
{
	char *arg;
	if (!(arg = strtok(line, " \t"))) return -1;
	if (!strcasecmp("yes", arg) || !strcmp("1", arg) || !strcasecmp("true", arg))
		return 1;
	else if (!strcasecmp("no", arg) || !strcmp("0", arg) || !strcasecmp("false", arg))
		return 0;
	
	return -1;
}


int tack_ramdisk(char *line, FILE *fp)
{
	struct ptag tag;
	tag.hdr.ptag = PTAG_RAMDISK;
	tag.hdr.size = ptag_size(ptag_ramdisk);
	tag.hdr.conf_mask = 0;
	tag.hdr.conf = 0;

	if ((tag.u.ramdisk.flags = read_yn(line)) == -1) return -1;

	fwrite(&tag, ptag_size(ptag_ramdisk), 4, fp);
	return 0;
}


int tack_bootdelay(char *line, FILE *fp)
{
	struct ptag tag;
	tag.hdr.ptag = PTAG_BOOTDELAY;
	tag.hdr.size = ptag_size(ptag_bootdelay);
	tag.hdr.conf_mask = 0;
	tag.hdr.conf = 0;

	if ((tag.u.bootdelay.delay = strtoul(line, 0, 0)) <= 0) return -1;

	fwrite(&tag, ptag_size(ptag_bootdelay), 4, fp);
	return 0;
}


int tack_cmdline(char *line, FILE *fp)
{
	struct ptag tag;
	int zero = 0;
	tag.hdr.ptag = PTAG_CMDLINE;
	tag.hdr.size = (sizeof(tag.hdr) + strlen(line) + 1 + 4) >> 2;
	tag.hdr.conf_mask = 0;
	tag.hdr.conf = 0;

	fwrite(&tag, sizeof(tag.hdr), 1, fp);
	fwrite(line, strlen(line) + 1, 1, fp);
	fwrite(&zero, 1, (4 - (strlen(line) + 1)) % 4, fp);
	return 0;
}


int tack_baud(char *line, FILE *fp)
{
	struct ptag tag;
	char *arg;
	u32 terminal, download;
	fpos_t bookmark;
	
	tag.hdr.ptag = PTAG_BAUD;
	tag.hdr.size = ptag_size(ptag_baud);
	tag.hdr.conf_mask = 0;
	tag.hdr.conf = 0;

	if (!(arg = strtok(line, "\t =,"))) return -1;
	if ((terminal = convert_baud(arg)) < 0) return -1;

	if (!(arg = strtok(NULL, "\t ,"))) return -1;
	if ((download = convert_baud(arg)) < 0) return -1;
	tag.u.baud.download = download;
	
	fgetpos(fp, &bookmark);
	fseek(fp, (u32) &(tag.u.core.terminal) - (u32) &tag, SEEK_SET);
	fwrite(&terminal, 4, 1, fp);
	fsetpos(fp, &bookmark);	

	fwrite(&tag, ptag_size(ptag_baud), 4, fp);
	return 0;
}


void tack_core(FILE *fp)
{
	struct ptag tag;
	tag.hdr.ptag = PTAG_CORE;
	tag.hdr.size = ptag_size(ptag_core);
	tag.hdr.conf_mask = 0;
	tag.hdr.conf = 0;

	tag.u.core.terminal = baud_9600;
	
	fwrite(&tag, ptag_size(ptag_core), 4, fp);
}


void tack_none(FILE *fp)
{
	struct ptag tag;
	tag.hdr.ptag = PTAG_NONE;
	tag.hdr.size = 0;
	tag.hdr.conf_mask = 0;
	tag.hdr.conf = 0;
	
	fwrite(&tag, sizeof(tag.hdr), 1, fp);
}


static struct config_keyword keywords[] = {
	/* keyword	handler */
	{"ramdisk",	tack_ramdisk},
	{"bootdelay",	tack_bootdelay},
	{"cmdline",	tack_cmdline},
	{"baud",	tack_baud},
};


int main(int argc, char *argv[])
{
	FILE *in, *out;
	char buffer[256], *token, *line;
	int i, line_num = 0;

	if (argc < 2) {
		printf("Usage: mkparamblock <configfile> <outfile>\n\n");
		return 0;
	}

	if (!(in = fopen(argv[1], "r"))) {
		perror("could not open config file");
		return 0;
	}
	
	if (!(out = fopen(argv[2], "w"))) {
		perror("could not open output file");
		return 0;
	}
	
	tack_core(out);
	
	while (fgets(buffer, 256, in)) {

		line_num++;
		/* clear trailing newline */
		if (strchr(buffer, '\n')) *(strchr(buffer, '\n')) = '\0';
		/* clear trailing comments */
		if (strchr(buffer, '#')) *(strchr(buffer, '#')) = '\0';
		/* find token */
		token = buffer + strspn(buffer, " \t");
		if (*token == '\0') continue;
		/* find line */
		line = token + strcspn(token, " \t=");
		if (*line == '\0') continue;
		*line = '\0';
		line++;
		line = line + strspn(line, " \t=");
		if (*line == '\0') continue;

		for (i = 0; strlen(keywords[i].keyword); i++)
			if (!strcasecmp(token, keywords[i].keyword))
				if (keywords[i].handler(line, out) < 0) 
					printf("Error %s args on line %d\n", token, line_num);
				
	}

	tack_none(out);

	fclose(in);
	fclose(out);
	
	return 0;
}			
			
			
