short int isNESFile (char* header){
	short int state = 0;
	if (header[0]=='N' && header[1]=='E' && header[2]=='S' && header[3]==26){
		state = 1;
	}
	return state;
}
