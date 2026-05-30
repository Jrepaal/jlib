#define _DEFAULT_SOURCE
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
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
-dynamic column sizing 
    -find min size (column name)
    -find longest name, size based on longest
-flags for 
    -hidden files, default to .files hidden
    -size in bytes, kB, mB, gB
*/



char * chkarg(const char * argp) // need to check if flag too
{
    char * cd[PATH_MAX];
    if (argp == NULL)
    {
        *cd = getcwd(*cd, sizeof(cd)); 
        //printf("%s", cd);
        return *cd;
    } 
    else
    {
        return argp;
    }
    
} 

void mode_to_string(mode_t mode, char *str) {

    // 2. Owner Permissions
    str[0] = (mode & S_IRUSR) ? 'r' : '-';
    str[1] = (mode & S_IWUSR) ? 'w' : '-';
    str[2] = (mode & S_IXUSR) ? 'x' : '-';
    str[3] = ' ';
    // 3. Group Permissions
    str[4] = (mode & S_IRGRP) ? 'r' : '-';
    str[5] = (mode & S_IWGRP) ? 'w' : '-';
    str[6] = (mode & S_IXGRP) ? 'x' : '-';
    str[7] = ' ';
    // 4. Other Permissions
    str[8] = (mode & S_IROTH) ? 'r' : '-';
    str[9] = (mode & S_IWOTH) ? 'w' : '-';
    str[10] = (mode & S_IXOTH) ? 'x' : '-';
    str[11] = ' ';
    // Handle Special Execution Bits (e.g., setuid, setgid, sticky)
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

char * checkstr(char * name) 
{

    char *str = malloc(16);
    for (int i = 0; i < 15; i++) // put name into str for 15 chars, char * needs \0 end
    {
        str[i] = name[i];        // need to handle long names more gracefully, append elipses? resizing?
    }
    str[15] = '\0'; 

    return str;
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

    unsigned int * usr, * grp, * gbl = malloc(sizeof(unsigned int));
            
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
        printf("%-15s %s %-10s %s %-10s\n", "Name", "|", "Type", "|", "Perms (u/g/o)");
        printf("----------------+------------+---------\n");
    }

    DIR *dir; // create directory stream dir, open


    dir = opendir(arg); // CHANGE THIS!!!


    struct dirent *de;
    char * name; // strings name, type, perms
    char * type;
    char * perms;

    while((de = readdir(dir)) != NULL)
    {
        name = checkstr(de->d_name); // by-reference? don't want to print (de->) directly
        type = checktype(de->d_type);
        perms = checkperms(arg, de);
        printf("%-15s %s %-10s %s %-10s\n", name, "|", type, "|", perms);
    }
    free(name);
    free(perms);

    int exit_state = closedir(dir);
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