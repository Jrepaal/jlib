#define _DEFAULT_SOURCE
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
//#include <string.h>

/* TODO:                                            -----
-Columns                                            -----
    -name                                           DONE!
    -filetype                                       DONE!
    -rwx (global? owner?)
    -formatting (random \n after .steam items)      DONE! fixed itself?

-If no args, pwd and check that                     DONE!
-get d_name, concatinate if too long (15 chars?)    DONE!
-sudo & error handling for that
*/



char * chkarg(const char * argp) // need to check if flag too
{
    if (argp == NULL)
    {
        char * cd = getcwd(cd, 0);
        //printf("%s", cd);
        return cd;
    } 
    else
    {
        return argp;
    }
    
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
        printf("%-15s %s %s\n", "Name", "|", "Type");
        printf("----------------+----------\n");
    }

    DIR *dir; // create directory stream dir, open


    dir = opendir(arg); // CHANGE THIS!!!


    struct dirent *de;

    char * name; // strings name and type 
    char * type;

    while((de = readdir(dir)) != NULL)
    {
        name = checkstr(de->d_name); // by-reference? don't want to print (de->) directly
        type = checktype(de->d_type);
        printf("%-15s %s %-10s\n", name, "|", type);
    }
    free(name);

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