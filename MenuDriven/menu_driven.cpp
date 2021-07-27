#include <iostream>
#include <cstdlib>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;

void ContentsOfDirectory();
void CheckFile();
void ShowFilePermissions();
void ChangePermissions();

int main()
{
	int choice;
	
    while(1){
    
    	cout<<"----------------------------------INDEX------------------------------" <<endl;
    	cout<<"1. list the contents of a directory" <<endl;
    	cout<<"2. Check if file exits" <<endl;
    	cout<<"3. Show file perissions of given file name(with absolute path)" <<endl;
    	cout<<"4. Chnagess file permissions for a given file name(with absolute path)" <<endl;
    	cout<<"5. Exit" <<endl;
    	cout<<"---------------------------------------------------------------------" <<endl;
    	cout<<"-> Enter your Choice " <<endl <<"->";
    	cin>>choice;
    
    	switch (choice){
    	case 1:
    		ContentsOfDirectory();
    		break;
    	case 2:
    		CheckFile();
    		break;
    	case 3:
    		ShowFilePermissions();
    		break;
    	case 4:
    		ChangePermissions();
    		break;
	case 5:
		return 0;
    	}
    }
    
    return EXIT_SUCCESS;
}

void ContentsOfDirectory(){
	char path[1024];
	
	DIR *dirp;
	struct dirent *dp;
	
	cout<<"-> Enter the directory path name: " <<endl <<"->";	
	cin>>path;
	
	cout<<endl;
	if((dirp = opendir(path)) == NULL){
		perror("Could not open file");
		return;
	}
	
	
	cout<<"--------------------Contents of the File --------------------" <<endl;
	while((dp = readdir(dirp) ) != NULL ){
		cout<<dp->d_name <<endl;
	}
	cout<<"-------------------------------------------------------------" <<endl;
	closedir(dirp);
	
	return;
}

void CheckFile(){
	char path[1024];
	cout<<"-> Enter the directory path name to check file exits of not(absolute path): " <<endl <<"->";	
	
	cin>>path;
	
	int rval;

 	rval = access (path, F_OK);
 	if (rval == 0) {
 		cout<<"----------------------------------------------------------------" <<endl;
  		cout<<"->" <<path <<": exists" <<endl;
 	
 	}else {
  		if (errno == ENOENT) 
   			cout<<"-> " <<path <<": does not exist" <<endl;
  		else if (errno == EACCES) ;
	}
	
	
	/*another way by using open directory
	  
	  DIR *dirp;
	  struct dirent *dp;
	  cout<<endl;
	if((dirp = opendir(path)) == NULL){
		perror(path);
		return;
	}
	cout<<path  <<" --->file are exists" <<endl;
	closedir(dirp);
	  */
	
	return;
}

/*
  for the show the opermissions of the given file i have used the
  system() call for the get permissions of file
  also i have implemented from the access command but that command 
  only shows the permissions for the cureent user 
*/
void ShowFilePermissions(){
	char path[1024] = {'\0'};
	char permissions[9] = {'\0'};
	
	cout<<"-> Enter the directory path name to check permissions(absolute path): " <<endl <<"->";	
	
	cin>>path;
	
	char temp_cmd[7] = "ls -l ";
	
	char* cmd = strcat(temp_cmd, path); //strcat command used for the concatinate the c style string
	
	/*check system command*/
	if (system(NULL)){
       	//cout << "Command processor exists";
    	}
    	else
       	cout << "Command processor doesn't exists";
       	
        
       /*the popen() function. It execuates a command, like system(), but directs 
       the output into a new file. A pointer to the stream is returned.*/
	FILE *lsofFile_p = popen(cmd, "r");

  	if (!lsofFile_p)
  	{
    		return ;
  	}
  	char buffer[1024];
  	
  	char *line_p = fgets(buffer, sizeof(buffer), lsofFile_p);
  		
  	char* permissionInShort = strtok(buffer, " "); //for get only permissions string from the buffer
  	/*close file*/
  	pclose(lsofFile_p); 
  	
  	cout<<endl  <<"---------------------permissions of given file--------------------" <<endl;
  	
  	cout<<permissionInShort <<" (in short)" <<endl;
       /*another way------------------------------------------------------------------------  */	
	
	system(cmd);
	cout<<"------------------------------------------------------------------" <<endl;
	
/*
	***below method displays only the current usertype permissions(u or g or p) ->
	
	// Check read access. 
 	rval = access (path, R_OK); //read permission
 	if (rval == 0){
  		//printf ("%s is readable\n", path);
  		permissions[i] = 'r';
  		i++;
  	}	
 	else{
  		//printf ("%s is not readable (access denied)\n", path);
		permissions[i] = '-';
		i++;
	}
	
 	// Check write access. 
 	rval = access (path, W_OK); //write permission
 	if (rval == 0){
  		//printf ("%s is writable\n", path);
  		permissions[i] = 'w';
  		i++;
  	}
 	else if (errno == EACCES){
  		//printf ("%s is not writable (access denied)\n", path);
  		permissions[i] = '-';
		i++;
	}
 	else if (errno == EROFS){
  		//printf ("%s is not writable (read-only filesystem)\n", path);
  		permissions[i] = '-';
		i++;
	}
	
	rval = access (path, X_OK);  //execuate permission
 	if (rval == 0){
  		//printf ("%s is writable\n", path);
  		permissions[i] = 'x';
  		i++;
  	}else{
  		permissions[i] = '-';
  		i++;
  	}
 	
  	cout<<"---------------------permissions of file--------------------" <<endl;
	cout<<permissions <<endl;
	cout<<"-----------------------------------------------------------" <<endl;
*/	
	return;
}

void ChangePermissions(){
	char path[1024] = {'\0'};
	char t_path[1024] = {'\0'};
	cout<<"-> Enter the directory path name to change permissions(absolute path): " <<endl <<"->";	
	
	cin>>path;
	cout<<endl <<"-----------------------" <<endl;
	char temp_cmd[7] = "ls -l ";
	
	strcpy(t_path,path); //copying path because string are changing beacause of strcat
	
	char* cmd = strcat(temp_cmd, path); //strcat command used for the concatinate the string
	
	 
       /*the popen() function. It execuates a command, like system(), but directs 
       the output into a new file. A pointer to the stream is returned.*/
	FILE *lsofFile_p = popen(cmd, "r");

  	if (!lsofFile_p)
  	{
    		return ;
  	}
  	char buffer[1024];
  	
  	char *line_p = fgets(buffer, sizeof(buffer), lsofFile_p);
  		
  	char* permissions = strtok(buffer, " "); //for get only permissions string from the buffer
  	/*close file*/
  	pclose(lsofFile_p); 
  	
  	cout<<"Permissions of given file -> " ;
  	cout<<permissions <<endl;
  	
 /*-------------------Modify Permissions by using chmod()------------------------*/
 	cout<<"---------------------------------------------------------------" <<endl;
  	char userType;
  	nxt:
  	cout<<"Enter the user type to modify perissions (u,g,o):" <<endl;

  	cin>>userType;
	if(userType != 'u' && userType != 'g' && userType != 'o' ){
		cout<<"please enter valid type" <<endl;
		goto nxt;
	}  	
  	char yesNo;
  	int permissionType ,ret;
  	if(userType == 'u'){
  		next1:
  		cout<<"select the index" <<endl;
  		cout<<"[1]read(r)" <<endl;
  		cout<<"[2]write(w)" <<endl;
  		cout<<"[3]execuate(x)" <<endl;
  		cin>>permissionType;
  		if(permissionType != 1 && permissionType != 2 && permissionType != 3 ){
			cout<<"please enter valid index" <<endl <<"->";
			goto next1;	
		}  
		
		
		if(permissionType == 1)
		{
			ret = chmod(t_path, S_IRUSR);
			if(ret == 0){
				cout<<"permission Changed Successfully(00400 read by owner)" <<endl;
				
			}
			else if(ret == -1){
				cout<<"permission can not Changed" <<endl;
			}
		}
		else if(permissionType == 2)
		{
			ret = chmod(t_path, S_IWUSR);
			if(ret == 0){
				cout<<"permission Changed Successfully(00200 write by owner)" <<endl;
				
			}
			else if(ret == -1){
				cout<<"permission can not Changed" <<endl;
			}
		}
		else if(permissionType == 3)
		{
			ret = chmod(t_path, S_IXUSR);
			if(ret == 0){
				cout<<"permission Changed Successfully(00100 execute/search by owner)" <<endl;
				
			}
			else if(ret == -1){
				cout<<"permission can not Changed" <<endl;
			}
		}
		
  	}
  	else if(userType == 'g')
  	{	next2:
  		cout<<"select the index" <<endl;
  		cout<<"[1]read(r)" <<endl;
  		cout<<"[2]write(w)" <<endl;
  		cout<<"[3]execuate(x)" <<endl<<"->";
  		cin>>permissionType;
  		if(permissionType != 1 && permissionType != 2 && permissionType != 3 ){
			cout<<"please enter valid index..." <<endl;
			goto next2;
		}  
		if(permissionType == 1)
		{
			ret = chmod(t_path, S_IRGRP);
			if(ret == 0){
				cout<<"permission Changed Successfully(00040 read by group)" <<endl;
				
			}
			else if(ret == -1){
				cout<<"permission can not Changed" <<endl;
			}
		}
		else if(permissionType == 2)
		{	
			ret = chmod(t_path, S_IWGRP);
			if(ret == 0){
				cout<<"permission Changed Successfully(00020 write by group)" <<endl;
				
			}
			else if(ret == -1){
				cout<<"permission can not Changed" <<endl;
			}
		}
		else if(permissionType == 3)
		{ 
			ret = chmod(t_path, S_IXGRP);
			if(ret == 0){
				cout<<"permission Changed Successfully(00010 execute/search by group)" <<endl;
				
			}
			else if(ret == -1){
				cout<<"permission can not Changed" <<endl;
			}
		}
		
  	}
  	else if(userType == 'u')
	{	next3:
		cout<<"select the index" <<endl;
  		cout<<"[1] read(r)" <<endl;
  		cout<<"[2] write(w)" <<endl;
  		cout<<"[3] execuate(x)" <<endl <<"->";
		cin>>permissionType;
		if(permissionType != 1 && permissionType != 2 && permissionType != 3 ){
			cout<<"please enter valid index" <<endl;
			goto next3;
		}  
		
		if(permissionType == 1)
		{
			ret = chmod(t_path, S_IROTH);
			if(ret == 0){
				cout<<"permission Changed Successfully(00004 read by others)" <<endl;
				
			}
			else if(ret == -1){
				cout<<"permission can not Changed" <<endl;
			}
		}
		else if(permissionType == 2)
		{
			ret = chmod(t_path, S_IWOTH);
			if(ret == 0){
				cout<<"permission Changed Successfully(00002 write by others)" <<endl;
				
			}
			else if(ret == -1){
				cout<<"permission can not Changed" <<endl;
			}
		}
		else if(permissionType == 3)
		{
			ret = chmod(t_path, S_IXOTH);
			if(ret == 0){
				cout<<"permission Changed Successfully(00001 execute/search by others)" <<endl;
				
			}
			else if(ret == -1){
				cout<<"permission can not Changed" <<endl;
			}
		}
		
	}

      return;
}

