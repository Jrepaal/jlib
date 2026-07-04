#define _DEFAULT_SOURCE 1
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdbool.h>
//#include <string.h>

// This code is published under GPL 3.0, see LICENSE for details

/* TODO:                                            -----
-Columns                                            -----
    -name                                           DONE!
    -filetype                                       DONE!
    -rwx (global? owner?)                           DONE!
    -formatting (random \n after .steam items)      DONE! fixed itself?

-If no args, pwd and check that                     DONE!
-get d_name, concatinate if too long (15 chars?)    DONE!
-sudo & error handling for that
-dynamic column sizing -- should be in own library? DONE! holy fuck
    -find min size (15)                             DONE!
    -find longest name, size based on longest       DONE!
    -should just be verbose option?                 NO!
-flags for  // READ UP ON POSIX COMPLIANCE
    - -a
        -hidden files, default to . files hidden
    - -i (info)
        -size in bytes, kB, mB, gB
*/



 struct entries {
    char * name;
    char * type;
    char * perms;

};

void print(int length, struct entries * entry, int count)
{
    printf("%s %-.*s %-s %s %-s %s\n", "Name", (length-4), "                                  ", "|", "Type", "|", "Perms (u/g/o)"); // change this so everything lines up
    printf("%-.*s%s", (length+2), "------------------------------------------------------------", "+------+---------------\n");

    for (int i = 0; i < count; i++)
    {
        int strl = 0;
        char tmp = entry[i].name[strl];
        while(tmp != '\0')
        {
            strl++;
            tmp = entry[i].name[strl];
        }
        
        
        printf("%s %-.*s %s %s %s %s\n", entry[i].name, (length-strl), "                              ",  "|", entry[i].type, " |", entry[i].perms);
    }
}

char * chkarg(const char * arg) // need to check if flag too
{
    char * cd[PATH_MAX];
    if (arg == NULL)
    {
        *cd = getcwd(*cd, sizeof(cd)); 
        //printf("%s", cd);
        return *cd;
    } 
    else
    {
        return (char *)arg;
    }
    
} 

void mode_to_string(mode_t mode, char *str) {

    // Owner permissions
    str[0] = (mode & S_IRUSR) ? 'r' : '-';
    str[1] = (mode & S_IWUSR) ? 'w' : '-';
    str[2] = (mode & S_IXUSR) ? 'x' : '-';
    str[3] = ' ';
    // Group permissions
    str[4] = (mode & S_IRGRP) ? 'r' : '-';
    str[5] = (mode & S_IWGRP) ? 'w' : '-';
    str[6] = (mode & S_IXGRP) ? 'x' : '-';
    str[7] = ' ';
    // Other permissions
    str[8] = (mode & S_IROTH) ? 'r' : '-';
    str[9] = (mode & S_IWOTH) ? 'w' : '-';
    str[10] = (mode & S_IXOTH) ? 'x' : '-';
    str[11] = ' ';
    // Handle special execution bits (e.g., setuid, setgid, sticky)
    if (mode & S_ISUID) str[2] = (str[2] == 'x') ? 's' : 'S';
    if (mode & S_ISGID) str[6] = (str[6] == 'x') ? 's' : 'S';
    if (mode & S_ISVTX) str[9] = (str[9] == 'x') ? 't' : 'T';
    
    // Null-terminate the string
    str[12] = '\0';
}

char * checktype(int num) // This should be a header file? One for each system type?? Definied standards somewhere online??? 
{
    switch (num) // switch case to turn int de->d_type into char * 
    {
    case 1:
        return "Ppe";
    case 2:
        return "Chr";
    case 4:
        return "Dir";
    case 6:
        return "Blk";   
    case 8:
        return "Fle";
    case 10:
        return "Lnk";
    case 12:
        return "Sck";
    case 14:
        return "WHT"; // is this a union filesystem???????
    default:
        return "Unknown";
        break;
    }
}

int checkstr(char name[]) 
{
    int len = 0;
    while (name[len] != '\0')
    {
        len++;
    }

    return len;
}

char * checkperms (char * name, struct dirent * dir )
{
    char * tname = name;
    struct stat fileStat;
        // Get file stats
    if (stat(tname, &fileStat) == -1)
    {
        perror("stat");
        return NULL;
    }

    stat(tname, &fileStat); // Passes the valid address
    mode_t mode = fileStat.st_mode;

    //unsigned int * usr, * grp, * gbl = malloc(sizeof(unsigned int)); // see if this does nothing
            
    char * perms = malloc(13);
    mode_to_string(mode, perms);
    return perms;
}


int main(int argc, const char * argp[]) /* array of args, array of pointers pointing to args; [0] is program name */
{
    // printf("Opening Filestream...\n");

    char * arg = chkarg(argp[1]);

    if (arg == NULL)
    {
        printf("No Directory Found!\n");
        return -1;
    }
    else
    {
        // printf("Filestream Opened!\n");

    }


    DIR *dir; // create directory stream dir, open


    dir = opendir(arg); // CHANGE THIS!!!


    struct dirent *de;
    // strings type, perms, # of entries
    int count = 0; // # of entries in array, starting at 0 for indexing
    int len, maxlen = 0;
    int set = 0;

    while((de = readdir(dir)) != NULL)
    {
        count++;

    }

    int exit_state = closedir(dir);
    dir = opendir(arg);

    struct entries * entry = calloc(count, sizeof(struct entries));

    while((de = readdir(dir)) != NULL)
    {
        entry[set] = (struct entries){de->d_name, checktype(de->d_type), checkperms(arg, de)};
        //entry[set].perms = checkperms(arg, de);
       // entry[set].type = checktype(de->d_type); // fill out struct for each directory entry

        len = checkstr(de->d_name); // by-reference? don't want to print (de->) directly
        if (len > maxlen)
        {
            maxlen=len; // find max name length for sizing output
        }
        set++;
    }

    print(maxlen, entry, count);
        
    for (int i = 0; i < count; i++)
    {
        free(entry[i].perms); // free malloc from checkperms
    }
    free (entry);

    exit_state = closedir(dir);
    if(exit_state == 0)
    {
        //printf("Sucessful Exit!\n");
        return 0;
    }
    else if(exit_state == -1)
    {
        printf("Bad Exit! Memory Error!\n");
        return -1;
    }
}