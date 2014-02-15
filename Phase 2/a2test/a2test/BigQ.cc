#include "BigQ.h"

void BigQ::quicksort(vector<Record> &rb, int left, int right,OrderMaker &sortorder){  

	int i = left;
	int j = right;

	ComparisonEngine compare;

   Record pivot = rb.at((left+right)/2);
  
   // partition  
   while (i <= j) {  
       while (compare.Compare(&rb.at(i),&pivot,&sortorder)<0)
           i++;  
  
       while (compare.Compare(&rb.at(j),&pivot,&sortorder)>0)
           j--;  
  
       if (i <= j) {  
           Record tmp = rb.at(i);  
           rb.at(i) = rb.at(j);  
           rb.at(j) = tmp;  
  
           i++;  
           j--;  
   }  
       }  
  
   // recursion  ?
   if (left < j)  
       quicksort(rb, left, j,sortorder);  
  
   if (i < right)  
       quicksort(rb, i, right,sortorder);  
}  



void* BigQ::TPMMS_Phase1(void* arg){
	/*
	Pipe *input;
		OrderMaker *sort_order;
		int run_length;
		Record *temporary = &temp;
		Page *buf = buffer;
		*/
	
	//Deprecated: replaced by DBFile
	//int page_Index = -1;//same as below indexing starts at 1
	//int pagelen =0;
	
	
	vector <Record> record_Buffer = vector<Record>();//delete record buffer from header

	args_phase1_struct *args;
	args = (args_phase1_struct *)arg;

	*(args->num_runs) = -1;//goes from 0 to n,set to one as the array size is n, else set array size to n+1 to use indexing from 1
	
	//Create and open new file 'file.run_no'

	DBFile *new_file = new DBFile();
	*(args->num_runs)++;
	(args->run_buffer)->push_back(new_file);//file for run1
	char actual_path[20];

	
	sprintf(actual_path,"%s.%d","run",*(args->num_runs));//(args->file_path) removed
	args->run_buffer->at(*(args->num_runs))->Create(actual_path,heap,NULL);

	cout << "Created run file "<<*(args->num_runs)<<"\n";
	int result =1;
	//***check resets of indexes
	while(result!=0){ // till input pipe is empty
	
				
		//args->(*recordBuffer)[pagelen++] = args->temporary;
	
	        result = args->input->Remove(args->temporary); // till input pipe is empty
		//append record temporary to page at pageindex
		if((int)args->run_buffer->at(*(args->num_runs))->GetLength()<*(args->run_length)&&result!=0){

			args->run_buffer->at(*(args->num_runs))->Add(*(args->temporary));

		}

		else{//if file is full !!CHANGE PAGE LIMIT IN DBFILE to runlength
			
		        cout<<"Pages read "<<(int)args->run_buffer->at(*(args->num_runs))->GetLength();
	                cout << "Run No:  "<<*(args->num_runs)<<"\n";
			/*Deprecated:
			//args->(*buf)[page_Index]->append(args->temporary) == 0){//if page is full
			//run length exceed sort runs and write out run file

			//!!We should not use page or file toBinary methods!!
			//the file is made binary by file.close . When page is full create a new file and append the pages to that file.After the run length exceeds
			//just close the file and open a new file of the next run no.
			
			//add page to file buffer at page_index
			
			//args->(*run_buffer)[num_runs]->AddPage(args->(*buf)[page_Index],page_Index);//getlength doesnt work use page index
			
			//create new run file and empty page buffer	
			//if(++page_Index>=args->run_length){//increment if run length is exceeded */
			

			
			args->run_buffer->at(*(args->num_runs))->Close();
			
			args->run_buffer->at(*(args->num_runs))->Open(actual_path);
			
			args->run_buffer->at(*(args->num_runs))->MoveFirst();
			
			cout << "Reached 1\n";
			
			int z = 0;

			Record temp;

			while(args->run_buffer->at(*(args->num_runs))->GetNext(temp) != 0){//empty out file into vector
				
			//	Record *nu = new Record();
			//	nu->Consume(&temp);
				record_Buffer.push_back(temp);
				z++;
			}	
			cout << "read "<<z<<" records\n";
			cout << "Reached 2\n";
			cout << "Record Buffer size: "<< record_Buffer.size()<<"\n";
			
			BigQ::quicksort(record_Buffer,0,record_Buffer.size(),*(args->sort_order));	//Sort runs vector
				

			cout << "Reached 3\n";
			
			args->run_buffer->at(*(args->num_runs))->MoveFirst();
			
			for(int i=0;i<record_Buffer.size();i++){//empty record buffer into dbfile
			
				*(args->temporary) = record_Buffer[i];//check if this copies
				args->run_buffer->at(*(args->num_runs))->Add(*(args->temporary));//check for references and pointers DOES THIS PERFORM DEEP COPY??
			}	
				//close dbfile
			record_Buffer.clear();


			cout << "Reached 4\n";
			args->run_buffer->at(*(args->num_runs))->Close();

				//new File
			
			cout << "Reached 5\n";
			
                        DBFile *new_file = new DBFile();
	                *(args->num_runs)++;
			args->run_buffer->push_back(new_file); //create new run file

			sprintf(actual_path,"%s.%d","run",*(args->num_runs));//set path as "file_path.num_run"
			args->run_buffer->at(*(args->num_runs))->Create(actual_path,heap,NULL);//??concatenate run no
				
				//empty page buffer ?? do we need to? can we overwrite?
			cout << "Reached 6\n";
				/*for(i=0;i<args->run_length;i++)
					args->(*buf)[i]->EmptyItOut();
				
				page_Index=1;
				args->(*buf)[page_Index]->append(args->temporary);
				//code for writing out the run and empting the page buffer
				//USE FILE OBJECT??
			//} // page index out of bound

			page_Index = 1;//reset page index*/
			
		}
		
		//  if page is full, increment page index
		//  if page index is out of bound, sort() page[]
		// write out as page[] run as binary
		// empty put page[]
		
	}
	cout << "Closing last file";	
	args->run_buffer->at(*(args->num_runs))->Close();
	 
}



BigQ :: BigQ (Pipe &in, Pipe &out, OrderMaker &sortorder, int runlen) {
	
	this->no_runs = 0;
	
	
	//buffer = new Page[runlen];//set to runlen +1 to use indxing starting from 1
	//runBuff = new Record[pageLength];//how many records per page?
	runs = new vector<DBFile*>();
//	*f_name = 'run';

	args_phase1.num_runs = &no_runs;
	args_phase1.temporary = &temp;
	args_phase1.run_buffer = runs;
	args_phase1.file_path = "run";
	args_phase1.input = &in;
	args_phase1.sort_order = &sortorder;
	args_phase1.run_length = &runlen;
	
	pthread_create (&worker, NULL, &BigQ::TPMMS_Phase1 , (void *)&args_phase1);

	pthread_join(worker,NULL);
	
	// read data from in pipe sort them into runlen pages
	
    	// construct priority queue over sorted runs and dump sorted data 
 	// into the out pipe

    	// finally shut down the out pipe
	out.ShutDown ();
}




BigQ::~BigQ () {
	//delete buffer;
	//delete thread
}
