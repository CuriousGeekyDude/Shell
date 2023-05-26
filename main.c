#include "basic.c"


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

int cd(int argc, char* argv[])
{
    if(argv[1] == NULL) {
        char* newPWD = getenv("HOME");
        if(newPWD == NULL)
            return -1;

        if(chdir(newPWD) == -1)
            errExit("chdir() in cd() in NULL case");

        char* newOLDPWD = getenv("PWD");
        if(newOLDPWD == NULL)
            return -2;
        
        if(setenv("PWD", newPWD, 1) == -1)
            errExit("setenv() in cd() in NULL case");
        if(setenv("OLDPWD", newOLDPWD, 1) == -1)
            errExit("setenv() in cd() in NULL case");

        return 0;
    }

    if(strcmp(argv[1], "..") == 0) {
        char* newOLDPWD = getenv("PWD");
        if(newOLDPWD == NULL)
            return -3;
        char* newPWD = returnUpPath(newOLDPWD);
        if(newPWD == NULL) {
            printf("PWD variable was changed during the program and so cannot be used anymore\n");
            fflush(stdout);
            free(newPWD);
            return -4;
        }
        if(chdir(newPWD) == -1) {
            free(newPWD);
            errExit("chdir() in cd() in .. case");
        }

        if(setenv("PWD", newPWD, 1) == -1)
            errExit("setenv() for PWD in cd() in case ..");

        if(setenv("OLDPWD", newOLDPWD, 1) == -1)
            errExit("setenv() for OLDPWD in cd() in case ..");
        
        free(newPWD);
        return 0;
    }

    if(strcmp(argv[1], "-") == 0) {

        char* newPWD = getenv("OLDPWD");
        if(newPWD == NULL)
            return -5;

        if(chdir(newPWD) == -1) 
            errExit("chdir() in cd() - case");

        char* newOLDPWD = getenv("PWD");
        if(newOLDPWD == NULL)
            return -6;

        printf("%s\n", newPWD);
        fflush(stdout);
        
        if(setenv("PWD", newPWD, 1) == -1)
            errExit("setenv() for PWD in cd() in - case");
        
        if(setenv("OLDPWD", newOLDPWD, 1) == -1)
            errExit("setenv() for OLDPWD in cd() in - case");
        
        return 0;
    }
    

    //The relative path and absolute path cases are combined in one step
    char* newOLDPWD = getenv("PWD");
    if(newOLDPWD == NULL)
        return -7;
            
    if(chdir(argv[1]) == -1) 
        errExit("chdir() in cd() in relative/absolute path case");
                
    char* newPWD = getcwd(NULL, BUFFSIZE);
    if(newPWD == NULL)
        errExit("getcwd() in cd()");
        
    if(setenv("PWD", newPWD, 1) == -1) {
        free(newPWD);
        errExit("setenv() for PWD in cd() in relative/absolute path case");
    }
    if(setenv("OLDPWD", newOLDPWD, 1) == -1) {
        free(newPWD);
        errExit("setenv() for OLDPWD in cd() in relative/absolute path case");
    }
    printf("%s\n", newPWD);
    fflush(stdout);
    free(newPWD);
    return 0;
}








int main(void)
{
    
    while(1) {

        readInput();
        if(strcmp(argv[0],"\n") == 0)
            continue;
        if(strcmp(argv[0], "cd") == 0)
            cd(argc, argv);
    }

    
}