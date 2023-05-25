#include "cd.h"
#include "basic.h"

static char pwd[BUFFSIZE] = { [0]='P', [1]='W', [2]='D', [3]='='};
static char oldPwd[BUFFSIZE] = {[0]='O', [1]='L', [2]='D', [3]='P', [4]='W', [5]='D', [6]='='};

char* returnUpPath(const char* currentPath) 
{
    size_t numOfLastSlash = 0, lengthOfCurrentPath = countLengthOfString(currentPath) + 1, indexOfLastSlash = 0;

    if(strcmp("", currentPath) == 0)
        return NULL;
    
    if(currentPath[0] != '/') {
        printf("The input is either a relative path or an invalid input!\n");
        fflush(stdout);
        return NULL;
    }
    if(currentPath[1] == '\0') {
        char* pathToReturn = malloc(2);
        pathToReturn[0] = '/';
        pathToReturn[1] = '\0';
        return pathToReturn;
    }

    for(size_t i = 0; currentPath[i] != '\0'; ++i) {
        if(currentPath[i] == '/')
            ++numOfLastSlash;
    }

    while(numOfLastSlash != 0) {
        if(indexOfLastSlash >= lengthOfCurrentPath) {
            printf("ERROR: indexOfLastSlash went over bound!\n");
            return NULL;
        }
        if(currentPath[indexOfLastSlash] == '/') {
            --numOfLastSlash;
            ++indexOfLastSlash;
            continue;
        }
        ++indexOfLastSlash;
    }
    if(indexOfLastSlash == 1) {
        char* newPathToReturn = malloc(indexOfLastSlash+1);
        initializeCharBuffer(newPathToReturn, indexOfLastSlash+1);
        strncpy(newPathToReturn, currentPath, indexOfLastSlash);
        return newPathToReturn;
    }
    else {
        char* newPathToReturn = malloc(indexOfLastSlash);
        initializeCharBuffer(newPathToReturn, indexOfLastSlash);
        strncpy(newPathToReturn, currentPath, indexOfLastSlash-1);
        return newPathToReturn;
    }

}

char* findPath_ColonSepDirectories(char* colonSepDirectories, const char* path)
{
    size_t numOfColons = 0, length_ColonsepDirectories = countLengthOfString(colonSepDirectories) + 1;


    if(colonSepDirectories == NULL || path == NULL)
        return NULL;

    for(size_t i = 0; i < length_ColonsepDirectories; ++i) {
        if(colonSepDirectories[i] == ':')
            ++numOfColons;
    }

    if(numOfColons == 0) {
        if(strcmp(colonSepDirectories, path) == 0) {
            char* pathCopyToReturn = malloc(length_ColonsepDirectories);
            initializeCharBuffer(pathCopyToReturn, length_ColonsepDirectories);
            strncpy(pathCopyToReturn, colonSepDirectories, length_ColonsepDirectories);            
            return pathCopyToReturn;
        }
        else
            return NULL;
    }

    ++numOfColons; //The number of directories seperated by colon

    char** pointerToDirectories = malloc((numOfColons + 1) * (sizeof(char*)));
    for(size_t i = 0; i < (numOfColons+1); ++i)
        pointerToDirectories[i] = NULL;


    size_t* lengthOfEachDirectory = malloc(numOfColons*sizeof(size_t));
    for(size_t i = 0; i < numOfColons; ++i)
        lengthOfEachDirectory[i] = 0;


    //Finding the length of each directory seperated by colon
    size_t index_lengthOfEachDirectory = 0;
    for(size_t i = 0; i < length_ColonsepDirectories; ++i) {
        size_t lengthOfDirectory = 0;

        while(colonSepDirectories[i] != ':' && colonSepDirectories[i] != '\0') {
            ++lengthOfDirectory;
            ++i;
        }
        if(index_lengthOfEachDirectory < numOfColons) {
            lengthOfEachDirectory[index_lengthOfEachDirectory] = lengthOfDirectory;
            ++index_lengthOfEachDirectory;
        }
    }

    char* directories = malloc(length_ColonsepDirectories*sizeof(char));
    initializeCharBuffer(directories, length_ColonsepDirectories);

    index_lengthOfEachDirectory = 0;
    for(size_t i = 0; i < length_ColonsepDirectories; ++i) {
        if(index_lengthOfEachDirectory < numOfColons)
            pointerToDirectories[index_lengthOfEachDirectory] = &directories[i];

        for(size_t j = 0; j < lengthOfEachDirectory[index_lengthOfEachDirectory]; ++j) {
            directories[i] = colonSepDirectories[i];
            ++i;
        }
        ++index_lengthOfEachDirectory;
        directories[i] = '\0';
    }
    free(lengthOfEachDirectory);
    
    for(size_t i = 0; pointerToDirectories[i] != NULL; ++i) {
        if(strcmp(pointerToDirectories[i], path) == 0) {
            size_t lengthOfPathCopyToReturn = countLengthOfString(pointerToDirectories[i]);
            char* pathCopyToReturn = malloc(lengthOfPathCopyToReturn + 1);
            initializeCharBuffer(pathCopyToReturn, lengthOfPathCopyToReturn+1);
            strncpy(pathCopyToReturn, pointerToDirectories[i], lengthOfPathCopyToReturn);
            free(directories);
            free(pointerToDirectories);
            return pathCopyToReturn;
        }
    }

    free(directories);
    free(pointerToDirectories);
    return NULL;
}

int updatePWD_OLDPWD(const char* newOLDPWD,const char* newPWD)
{
    size_t index = 0;
    for(;newOLDPWD[index] != '\0'; ++index)
        oldPwd[7+index] = newOLDPWD[index];
    oldPwd[7+index] = '\0';
    
    index = 0;
    for(; newPWD[index] != '\0'; ++index)
        pwd[4+index] = newPWD[index];
    pwd[4+index] = '\0';
    
    if(putenv(pwd) != 0)
        errExit("putenv");
    if(putenv(oldPwd) != 0)
        errExit("putenv");
    return 0;
}

int cd(int argc, char* argv[])
{
    if(argv[1] == NULL) {
        char* valueOfEnvVariable = getenv("HOME");
        if(valueOfEnvVariable == NULL)
            return -1;

        char* valueOfPWD = getenv("PWD");
        if(valueOfPWD == NULL)
            return -1;
        
        printf("%s\n", valueOfEnvVariable);
        fflush(stdout);
        updatePWD_OLDPWD(valueOfPWD, valueOfEnvVariable);
        if(chdir(valueOfEnvVariable) == -1)
            errExit("chdir() in cd() in NULL case");
        return 0;
    }

    if(strcmp(argv[1], "..") == 0) {
        char* pwd = getenv("PWD");
        char* newPWD = returnUpPath(pwd);
        updatePWD_OLDPWD(pwd, newPWD);
        if(chdir(newPWD) == -1)
            errExit("chdir() in cd() in .. case");
        free(newPWD);
        return 0;
    }

    if(strcmp(argv[1], "-") == 0) {
        char* newOLDPWD = getenv("PWD");
        char* newPWD = getenv("OLDPWD");
        printf("%s\n", newPWD);
        fflush(stdout);
        size_t lengthOfnewPWD = countLengthOfString(newPWD) + 1;

        char* PWD = malloc(lengthOfnewPWD);
        initializeCharBuffer(PWD, lengthOfnewPWD);

        strncpy(PWD, newPWD, lengthOfnewPWD-1);
        
        size_t index = 0;
        for(;newOLDPWD[index] != '\0'; ++index)
            oldPwd[7+index] = newOLDPWD[index];
        oldPwd[7+index] = '\0';
    
        index = 0;
        for(; PWD[index] != '\0'; ++index)
            pwd[4+index] = PWD[index];
        pwd[4+index] = '\0';
    
        if(putenv(pwd) != 0)
            errExit("putenv");
        if(putenv(oldPwd) != 0)
            errExit("putenv");

        if(chdir(newPWD) == -1) {
            free(PWD);
            errExit("chdir() in cd() - case");
        }

        free(PWD);
        return 0;
    }
    if(argv[1][0] == '/') {
        char* newOLDPWD = getenv("PWD");
        if(chdir(argv[1]) == -1)
            errExit("chdir() in cd() / case");
        updatePWD_OLDPWD(newOLDPWD, argv[1]);
        return 0;
    }

    if(argv[1][0] != '/') {
        char* cdPath = getenv("CDPATH");
        if(cdPath == NULL) {
        label:
            if(chdir(argv[1]) == -1)
                errExit("ERROR(1):chdir() in cd()");
            char* newPWD = getcwd(NULL, BUFFSIZE);
            if(newPWD == NULL)
                errExit("ERROR(2):getcwd() in cd()");
            char* newOLDPWD = getenv("PWD");
            updatePWD_OLDPWD(newOLDPWD, newPWD);
            free(newPWD);
            return 0;
        }

        else {
            char* newPWD = findPath_ColonSepDirectories(cdPath, argv[1]);
            if(newPWD == NULL) {
                goto label;
            }
            else {
                if(chdir(newPWD) == -1) {
                    free(newPWD);
                    errExit("ERROR(3):chdir() in cd()");
                }
                char* newOLDPWD = getenv("PWD");
                char* new_PWD = getcwd(NULL, BUFFSIZE);
                updatePWD_OLDPWD(newOLDPWD, new_PWD);
                printf("%s\n", new_PWD);
                free(new_PWD);
                free(newPWD);
                return 0;
            }

        }

    }
    
    printf("feri: cd: Incorrect format of cd command. Consult man pages.\n");
    fflush(stdout);
    return -1;
}