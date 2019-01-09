#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

#include <powertweak.h>
#include "dmi.h"


void DMI_tweak_destructor (struct tweak *tweak) 
{
	struct private_DMI_data *private;

	default_destructor(tweak);
	private = (struct private_DMI_data*)tweak->PrivateData;

	if (private != NULL) { 
		release_value(&private->value);
		free (tweak->PrivateData);
	}
}


static value_t DMI_get_value(struct tweak *tweak)
{
	struct private_DMI_data *private;
	value_t localvalue={0,NULL};

	assert(tweak!=NULL);
	private=(struct private_DMI_data*)tweak->PrivateData;
	assert(private!=NULL);

	copy_values(&localvalue,private->value);
	return localvalue;
}



struct tweak *alloc_DMI_tweak (int type)
{
	struct tweak *tweak;
	struct private_DMI_data *private;

	tweak = alloc_tweak(type);
	private = malloc (sizeof (struct private_DMI_data));
	if (private == NULL) {
		printf ("Out of memory\n");
		free (tweak);
		return (NULL);
	}
	memset(private,0,sizeof(struct private_DMI_data));
	tweak->PrivateData = (void *)private;
	tweak->Destroy = &DMI_tweak_destructor;
	tweak->GetValue = &DMI_get_value;

	return tweak;
}


int InitPlugin(int showinfo)
{
	unsigned char buf[20];
	int fd, rc=FALSE;
	long fp;

	if (showinfo == FALSE)
		return FALSE;

	fd=open("/dev/mem", O_RDONLY);
	fp=0xE0000L;
    if (fd==-1) {
		log_message("/dev/mem");
		return FALSE;
	}
	if (lseek(fd,fp,0)==-1) {
		log_message("seek");
		close(fd);
		return FALSE;
	}
	fp -= 16;

	/* Find DMI struct. */
	while( fp < 0xFFFFF) {
		fp+=16;
		if(read(fd, buf, 16)!=16)
			log_message("read");
		if(memcmp (buf, "_SM_", 4)==0) {/*SMBIOS Structure table entry point*/
			char biosinfo[]="BIOS Information";
			char information[]="Information";
			struct tweak *tweak=NULL;
			struct private_DMI_data *private;
			char version[10];
			
			snprintf(version, 9, "%i.%i", buf[6], buf[7]);
			DMI_STR_VALUE ("DMI Version", version,  biosinfo, information);
		}
		if(memcmp (buf, "_DMI_", 5)==0) {/**/
			u16 num=buf[13]<<8|buf[12];
			u16 len=buf[7]<<8|buf[6];
			u32 base=buf[11]<<24|buf[10]<<16|buf[9]<<8|buf[8];

			dmi_table (fd, base, len, num);
			rc = TRUE;
			break;
		}
	}
	close(fd);
	return (rc);
}
