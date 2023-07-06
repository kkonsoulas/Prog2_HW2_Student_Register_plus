#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NAME_LEN 64

typedef struct dl_list{
	struct entry *nxt;
	struct entry *prv;
} dl_list;

typedef struct course{
    unsigned short course_code;
    struct course *nxt;
}course;

typedef struct entry{
	long unsigned int aem;
	char name[NAME_LEN];
	short unsigned int failed_exams;
    course *head;
    dl_list link;
}entry;

typedef struct matrix{
	entry **entries_ptr;
	unsigned int size;
	unsigned int used_size;
	unsigned int sorted_size;
}matrix;

typedef struct hashtable_matrix{
entry **hashtable;
unsigned int core_size;
unsigned int size;
unsigned int members;
double load_factor;
unsigned int largest_bucket;
}hashtable_matrix;

void dl_list_init(dl_list *link){
	link->nxt = NULL;
    link->prv = NULL;
}

void entry_init(entry *entry){
	int i ;

	entry->aem = 0;
	entry->failed_exams = 0; 
	for (i=0 ;i<NAME_LEN ;i++){
	    entry->name[i] = '\0';
	}
    entry->head = NULL;
    dl_list_init(&(entry->link));
}

void course_init(course *course_ptr){
    course_ptr->course_code = 0;
    course_ptr->nxt = NULL;
}

void hashtable_matrix_init(hashtable_matrix *hashtable_matrix_ptr){
    hashtable_matrix_ptr->hashtable=NULL;
    hashtable_matrix_ptr->core_size=0;
    hashtable_matrix_ptr->size=0;
    hashtable_matrix_ptr->members=0;
    hashtable_matrix_ptr->load_factor=0;
	hashtable_matrix_ptr->largest_bucket=0;
}

course* course_create(){
    course *temp_course_ptr;
    temp_course_ptr = (course*) malloc(sizeof(course));
    if(temp_course_ptr != NULL){
        course_init(temp_course_ptr);
        return(temp_course_ptr);
    }
    else{
        printf("Course Create Error\n");
        return(NULL);
    }
}

void course_list_clear_core(course **course_ptr){
    
    if((*course_ptr)->nxt != NULL){
        course_list_clear_core(&((*course_ptr)->nxt));
    }
    free(*course_ptr);
    return;
}

void course_list_clear(course **head){
    if(*head == NULL){
        return;
    }
    course_list_clear_core(head);
    return;
}

void entry_delete(entry **entry_ptr){//modified for course list!
    
    course_list_clear(&((*entry_ptr)->head));
    //(*entry_ptr)->head = NULL;

    free(*entry_ptr);
	*entry_ptr = NULL;
}

void entry_create_n_link(const int start,const int finish,entry **entries_ptr){
	int i;
	entry *temp_entry_holder;
	for (i=start ;i<finish ;i++){
        temp_entry_holder = (entry *)malloc(sizeof(entry));

        if(temp_entry_holder != NULL){ //getting sure malloc gave us the memory
            entries_ptr[i] = temp_entry_holder;
        }
        else{
            printf("Error\n");
            return;
        }
        entry_init(entries_ptr[i]);

    }
}

entry* create_sentinel(){
	entry* new_sentinel;

	new_sentinel = malloc(sizeof(entry));
	entry_init(new_sentinel);
	return(new_sentinel);
}

void hashtable_sentinel_plant(entry **hashtable ,const unsigned int START ,const unsigned int FINISH){
	int i;
	
	for(i=START ;i<FINISH ;i++){
		hashtable[i]= create_sentinel();
	}
}

void hashtable_sentinel_unplant(entry **hashtable ,const unsigned int START ,const unsigned int FINISH){
	int i;
	
	for(i=START ;i<FINISH ;i++){
		free(hashtable[i]);
		hashtable[i] = NULL;
	}
}

void clear_core(matrix *matrix_ptr,hashtable_matrix *hashtable_info_ptr){
	unsigned int i,old_size;
	entry **temp_hashtable;

	//entries_ptr_table clear
    for(i=0 ;i<matrix_ptr->used_size ;i++){
        entry_delete(&(matrix_ptr->entries_ptr[i]));
    }
	matrix_ptr->entries_ptr = realloc(matrix_ptr->entries_ptr,0);
    matrix_ptr->size = 0;
    matrix_ptr->used_size = 0;
    matrix_ptr->sorted_size = 0;
	//

	//Hashtable resizing and sentinel pointers NULLing 
	old_size = hashtable_info_ptr->size;
	hashtable_sentinel_unplant(hashtable_info_ptr->hashtable ,hashtable_info_ptr->core_size ,old_size);

	hashtable_info_ptr->size = hashtable_info_ptr->core_size;
	temp_hashtable = (entry**) realloc(hashtable_info_ptr->hashtable ,hashtable_info_ptr->size*sizeof(entry*));
	if(hashtable_info_ptr == NULL){
		printf("Failed to clear hashtable\n");
		return;
	}
	hashtable_info_ptr->hashtable = temp_hashtable;
	for(i=0 ;i<hashtable_info_ptr->size ;i++){

		hashtable_info_ptr->hashtable[i]->link.nxt = NULL;
	}
	hashtable_info_ptr->members=0;
	hashtable_info_ptr->load_factor=0;
	hashtable_info_ptr->largest_bucket=0;
	//

	return;
}

void clear(matrix *matrix_ptr,hashtable_matrix *hashtable_info_ptr){
   clear_core(matrix_ptr ,hashtable_info_ptr);
    printf("\nC-OK\n");
}

void entry_address_swap(entry **entry1 ,entry **entry2){//The use is obvius
    entry *temp_entry;

    temp_entry = *entry1;
    *entry1 = *entry2;
    *entry2 = temp_entry;
}

entry* find_binary_core(const matrix *matrix_ptr,const long unsigned int AEM,unsigned int *comparisons_ptr){
	int comparisons = 0;
	int start=0;
	int end = matrix_ptr->used_size-1;
	int middle = (start+end)/2;
	//printf("end is %d ,start is %d\n",end,start);//for testing

	//Binary search
	while(start <=end){
		if(matrix_ptr->entries_ptr[middle]->aem == AEM){
			comparisons+=1;
			if(comparisons_ptr !=NULL){
				*comparisons_ptr = comparisons;
			}
			
			return(matrix_ptr->entries_ptr[middle]);
		}
		else if(matrix_ptr->entries_ptr[middle]->aem < AEM){
			start = middle + 1;
			middle = (start + end)/2;
			comparisons+=2;
		}
		else{// >AEM
			end = middle - 1;
			middle = (start + end)/2;
			comparisons+=2;
		}	
	}
	//

	if(comparisons_ptr !=NULL){//if NULL means we don't need them
		*comparisons_ptr = comparisons;	//The comparison takes into account comparisons between AEMS
	}
	
	return(NULL);
}

entry* find_linear_core(const matrix *matrix_ptr,const long unsigned int AEM,unsigned int *comparisons_ptr){
    int i,comparisons = 1;

    for(i=0 ;(i<matrix_ptr->used_size) && !(matrix_ptr->entries_ptr[i]->aem ==AEM) ;i++ ,comparisons +=1){	
		//empty 
	}

	if(i == matrix_ptr->used_size){
		comparisons--;
	}

	if(comparisons_ptr != NULL){
		*comparisons_ptr =comparisons;
	}

    if(i == matrix_ptr->used_size){
		return(NULL);
	}
	
	return(matrix_ptr->entries_ptr[i]);
}

entry* find_core(const matrix *matrix_ptr,const long unsigned int AEM,unsigned int *comparisons_ptr){
    if(matrix_ptr->sorted_size == matrix_ptr->used_size){
		return(find_binary_core(matrix_ptr,AEM,comparisons_ptr));
	}
	else{
		return(find_linear_core(matrix_ptr,AEM,comparisons_ptr));
	}
}

void course_reg(matrix *matrix_ptr ,const unsigned long AEM ,const unsigned short COURSE_CODE){//course_list_add
    entry *entry_ptr;
    course *curr, *course_ptr;

    //Entry locate
    entry_ptr = find_core(matrix_ptr,AEM,NULL);

    if(entry_ptr == NULL){
        printf("\nG-NOK %ld\n",AEM);
        return;
    }
    //if exists...

    //COURSE CREATION AND LINK TO LIST NEXT!//
    //THE LOGIC ABOUT THE COURSE INSERTION IS curr->course_code < COURSE_CODE < (nxt->course_code) || (nxt==NULL)

    //Special occasions when head==NULL || head->course_code > COURSE CODE (no previous ptr to link it)
    if(entry_ptr->head == NULL){
        entry_ptr->head = course_create();
        entry_ptr->head->course_code = COURSE_CODE;
        printf("\nG-OK %ld %hd\n",entry_ptr->aem,entry_ptr->head->course_code);
        return;
    }
    else if(entry_ptr->head->course_code > COURSE_CODE){
        course_ptr = course_create();
        course_ptr->course_code = COURSE_CODE;
        course_ptr->nxt = entry_ptr->head;
        entry_ptr->head = course_ptr;
        printf("\nG-OK %ld %hd\n",entry_ptr->aem ,entry_ptr->head->course_code);
        return;
    }
    //

    //Searching for the right position to put it
    for(curr = entry_ptr->head ;curr->course_code < COURSE_CODE ;curr = curr->nxt){
        if(curr->nxt == NULL || curr->nxt->course_code > COURSE_CODE){
            course_ptr = course_create();
            course_ptr->course_code = COURSE_CODE;
            course_ptr->nxt = curr->nxt;
            curr->nxt = course_ptr;
            printf("\nG-OK %ld %hd\n",entry_ptr->aem ,curr->nxt->course_code);
            return;
        }
    }
    //

    printf("\nG-NOK %hd\n",COURSE_CODE);//COURSE EXISTS
    return;
}

void list_courses(matrix *matrix_ptr ,const unsigned long AEM){
    entry *entry_ptr;
    course *curr;

    //Entry locate
    entry_ptr = find_core(matrix_ptr,AEM,NULL);

    if(entry_ptr == NULL){
        printf("\nL-NOK %ld\n",AEM);
        return;
    }
    //if exists..

    //Printing Courses
    printf("\nL-OK %s\n",entry_ptr->name);
    for(curr = entry_ptr->head ;curr != NULL ;curr = curr->nxt){
        printf("%hd\n",curr->course_code);
    }
    return;
    //
}

course** course_isreg_core(matrix *matrix_ptr,const unsigned long AEM ,const unsigned short COURSE_CODE,char *exist_ptr){
    entry *entry_ptr;
    course *curr;
    
    //Entry locate
    entry_ptr = find_core(matrix_ptr,AEM,NULL);
    if(exist_ptr != NULL){
        if(entry_ptr == NULL){
        *exist_ptr = 0;
        return(NULL);
        }
        else{
            *exist_ptr = 1;
        }
    }
    //

    //THE LOGIC IS:
    //WE NEED TO RETURN THE ADDRESS OF THE PTR THAT POINTS TO THE COURSE WE SEARCH
    //That is overwhelming now but it will be usefull later

    //Special occasions (see the main idea comments below to understand)
    if(entry_ptr->head == NULL){
        return(NULL);
    }
    else if(entry_ptr->head->course_code == COURSE_CODE){
        return(&(entry_ptr->head));
    }
    //

    //The list is in order => Stop if we get to it or pass it //MAIN IDEA
    for(curr=entry_ptr->head ;curr->nxt != NULL && curr->nxt->course_code < COURSE_CODE ;curr=curr->nxt){
        //empty//
    }

    if(curr->nxt != NULL && curr->nxt->course_code == COURSE_CODE){
        return(&(curr->nxt)); //The only way to return the ptr's address ,we cannot do &curr
    }
    else{
        return(NULL);
    }
    //

}

void course_unreg(matrix *matrix_ptr ,const unsigned long AEM ,const unsigned short COURSE_CODE){
    course **curr_ptr_address,*curr_course_ptr;
    char exist;

    curr_ptr_address = course_isreg_core(matrix_ptr,AEM,COURSE_CODE,&exist);

    if(curr_ptr_address == NULL && exist == 0){
        printf("\nU-NOK %ld\n",AEM);
        return;
    }
    else if(curr_ptr_address == NULL && exist == 1){
        printf("\nU-NOK %hd\n",COURSE_CODE);
        return;
    }

    //*curr_ptr_adress)->course_code == COURSE_CODE //TRUE
	//The unreg process
    curr_course_ptr = *curr_ptr_address;
    *curr_ptr_address = curr_course_ptr->nxt;
    free(curr_course_ptr);
    printf("\nU-OK %ld %hd\n",AEM,COURSE_CODE);
    return;
	//
}

void course_isreg(matrix *matrix_ptr,const unsigned long AEM ,const unsigned short COURSE_CODE){//COMMENTED
    char exist;
    course **course_ptr_address;

    course_ptr_address = course_isreg_core(matrix_ptr,AEM,COURSE_CODE,&exist);

    if(course_ptr_address == NULL && exist == 0){
        printf("\nI-NOK %ld\n",AEM);
    }
    else if(course_ptr_address == NULL && exist == 1){
        printf("\nNO\n");
    }
    else{
        printf("\nYES\n");
    }

}

unsigned long hash(char *str){//VANA'S CODE!
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;

    return hash;
}

unsigned long my_hash_foo(char *str,const unsigned int SIZE){
    return(hash(str)%SIZE);
}

void ht_prev_insert(hashtable_matrix *hashtable_info_ptr ,entry *second_member_ptr ,entry *new_member_ptr ,unsigned long pos){
    entry *first_member_ptr = second_member_ptr->link.prv;

    //We link the entries such that first <-> new_member <-> second

    //first <-> new_member
    first_member_ptr->link.nxt =new_member_ptr;
    new_member_ptr->link.prv = first_member_ptr;
    // 
    
    //new_member <-> second
    new_member_ptr->link.nxt = second_member_ptr;
    second_member_ptr->link.prv = new_member_ptr;
    //

    return;
}

unsigned int bucket_size_calc(hashtable_matrix *hashtable_info_ptr,const unsigned long POS){
	unsigned int bucket_size = -1;//cause we have the sentinel that we don't count
	entry *curr = hashtable_info_ptr->hashtable[POS];

	for( ;curr != NULL ;curr = curr->link.nxt ,bucket_size++);
	return(bucket_size);

}

unsigned int largest_bucket_calc(hashtable_matrix *hashtable_info_ptr){
	unsigned int pos ,bucket ,largest_bucket;

	largest_bucket = 0; 
	for(pos=0 ;pos<hashtable_info_ptr->size ;pos++){
		bucket = bucket_size_calc(hashtable_info_ptr,pos);
		if(bucket>largest_bucket){
			largest_bucket = bucket;
		}
	}
	return(largest_bucket);

}

void add_hashtable_member(hashtable_matrix *hashtable_info_ptr ,entry *new_member_ptr){//I should put some comments there
    unsigned long pos;
    entry *curr;
	unsigned int bucket_size;
    pos = my_hash_foo(new_member_ptr->name,hashtable_info_ptr->size);
    

    for(curr = hashtable_info_ptr->hashtable[pos] ;curr->link.nxt != NULL && strcmp(curr->name ,new_member_ptr->name) < 0 ;curr = curr->link.nxt){//Stop if found curr->name > ,or = ,or curr->nxt == NULL
        //empty//
    }

    if(curr->link.nxt == NULL && strcmp(curr->name ,new_member_ptr->name) < 0){
        curr->link.nxt = new_member_ptr;
        new_member_ptr->link.prv = curr;
		
		//Hashtable_info_recalculating
        hashtable_info_ptr->members++;
		hashtable_info_ptr->load_factor = (double) hashtable_info_ptr->members / hashtable_info_ptr->size;
		bucket_size = bucket_size_calc(hashtable_info_ptr,pos);
		if(bucket_size > hashtable_info_ptr->largest_bucket){
			hashtable_info_ptr->largest_bucket = bucket_size;
		}
		//

        return;
    }

    //We search in the part of the list where curr->name = new_member_ptr->name
    for( ;curr->link.nxt!=NULL && strcmp(curr->name ,new_member_ptr->name) == 0 && curr->aem < new_member_ptr->aem ;curr = curr->link.nxt){
        //empty
    }

    if(curr->link.nxt == NULL && strcmp(curr->name,new_member_ptr->name) == 0){
        if(curr->aem < new_member_ptr->aem){
            curr->link.nxt = new_member_ptr;
            new_member_ptr->link.prv = curr;
        }
        else{
            ht_prev_insert(hashtable_info_ptr,curr,new_member_ptr,pos);
        }
    }
    else{
		ht_prev_insert(hashtable_info_ptr,curr,new_member_ptr,pos);
    }
    
	//Hashtable_info_recalculating
	hashtable_info_ptr->members++;
	hashtable_info_ptr->load_factor = (double) hashtable_info_ptr->members / hashtable_info_ptr->size;
	bucket_size = bucket_size_calc(hashtable_info_ptr,pos);
	if(bucket_size > hashtable_info_ptr->largest_bucket){
		hashtable_info_ptr->largest_bucket = bucket_size;
	}
	//

    return;
}

void remove_hashtable_member(hashtable_matrix *hashtable_info_ptr ,entry *removable_member){
    entry *prev = removable_member->link.prv;
    unsigned long pos = my_hash_foo(removable_member->name,hashtable_info_ptr->size);
	unsigned int bucket_size;

	//prev <-> removable_member <-> nxt

	//prev-> nxt
	prev->link.nxt = removable_member->link.nxt;
	removable_member->link.prv = NULL;
	//

	//prev <-nxt
	if(removable_member->link.nxt == NULL){
		//empty//
	}
	else{
		removable_member->link.nxt->link.prv = prev;
		removable_member->link.nxt = NULL;
	}
	//

	//Hashtable_info_recalculating (when an entry is removed though)
	hashtable_info_ptr->members--;
	hashtable_info_ptr->load_factor = (double) hashtable_info_ptr->members / hashtable_info_ptr->size;
	bucket_size = bucket_size_calc(hashtable_info_ptr,pos);
	if(bucket_size == hashtable_info_ptr->largest_bucket-1){
		if(bucket_size != 1 || hashtable_info_ptr->members == 1){
			hashtable_info_ptr->largest_bucket = bucket_size;
		}
		else{
			hashtable_info_ptr->largest_bucket = largest_bucket_calc(hashtable_info_ptr);
		}
		
	}
	//
}

void rehash_hashtable(hashtable_matrix *hashtable_info_ptr,matrix *matrix_ptr,const unsigned int OLD_SIZE){
	entry **temp_hashtable;
	unsigned int i;
	char trigger=0;

	//Hashtable resizing
	if(hashtable_info_ptr->size < OLD_SIZE){
		hashtable_sentinel_unplant(hashtable_info_ptr->hashtable ,hashtable_info_ptr->size ,OLD_SIZE);
		trigger=1;
	}

	temp_hashtable = (entry**) realloc(hashtable_info_ptr->hashtable ,hashtable_info_ptr->size*sizeof(entry*));
	if(temp_hashtable == NULL){
		printf("Not enough space for new hashtable\n");
		return;
	}
	hashtable_info_ptr->hashtable = temp_hashtable;

	if(!trigger){
		hashtable_sentinel_plant(hashtable_info_ptr->hashtable ,OLD_SIZE ,hashtable_info_ptr->size);
	}
	//

	//NULLing the nxt of the sentinels
	if(!trigger){
		for(i=0 ;i<OLD_SIZE ;i++){
			hashtable_info_ptr->hashtable[i]->link.nxt = NULL;
		}
	}
	else{
		for(i=0 ;i<hashtable_info_ptr->size ;i++){
			hashtable_info_ptr->hashtable[i]->link.nxt = NULL;
		}
	}
	//

	//Re_add entries
	hashtable_info_ptr->members = 0;
	hashtable_info_ptr->largest_bucket = 0;
	for(i=0 ;i<matrix_ptr->used_size ;i++){
		dl_list_init(&(matrix_ptr->entries_ptr[i]->link));
		add_hashtable_member(hashtable_info_ptr ,matrix_ptr->entries_ptr[i]);
	}
	//
	return;
}

void add(matrix *matrix_ptr ,entry temp_entry,const unsigned int SIZE_FLUCTUATION ,hashtable_matrix *hashtable_info_ptr){
	entry **temp_entries_ptr;
	int i,j;
	char trigger= '1';

	//Finding the aem(if it exists or not)
	if(matrix_ptr->sorted_size == matrix_ptr->used_size){
		if(!find_binary_core(matrix_ptr,temp_entry.aem,NULL)){
			trigger= '0';
		}
	}
	else{
		if(!find_linear_core(matrix_ptr,temp_entry.aem,NULL)){
			trigger= '0';
		}
	}
	//

	//if it exists don't add
	if(trigger == '1'){
		printf("\nA-NOK %ld, %d %d\n",temp_entry.aem ,matrix_ptr->used_size ,matrix_ptr->size);
		return;
	}
	//else...continue with the adding process

	//entries_ptr array enlarger(if needed)
	if(matrix_ptr->size == matrix_ptr->used_size){
		
		matrix_ptr->size +=SIZE_FLUCTUATION;
		temp_entries_ptr = (entry**) realloc(matrix_ptr->entries_ptr,(matrix_ptr->size)*sizeof(entry*));
		if(temp_entries_ptr == NULL){
			return;
		}
		matrix_ptr->entries_ptr = temp_entries_ptr;
	}
	//
		
	//Entry space creating(puts it after the last used ptr)
	entry_create_n_link(matrix_ptr->used_size,matrix_ptr->used_size+1,matrix_ptr->entries_ptr);
	i = matrix_ptr->used_size;
	matrix_ptr->used_size++;
	//


	//The actual adding process	
	matrix_ptr->entries_ptr[i]->aem =temp_entry.aem;
	matrix_ptr->entries_ptr[i]->failed_exams = temp_entry.failed_exams;
	for(j=0 ;j<NAME_LEN && temp_entry.name[j] !='\0' ;j++){
		if(temp_entry.name[j]>='a' && temp_entry.name[j]<='z'){
        	temp_entry.name[j] -=32;
    	}
	}
	strcpy(matrix_ptr->entries_ptr[i]->name ,temp_entry.name);

	//////Hashtable addon
    add_hashtable_member(hashtable_info_ptr,matrix_ptr->entries_ptr[i]);
	if(hashtable_info_ptr->load_factor >= 4){
		hashtable_info_ptr->size*=2;
		rehash_hashtable(hashtable_info_ptr,matrix_ptr,(hashtable_info_ptr->size/2) );
	}
	//////

	printf("\nA-OK %ld, %d %d\n",matrix_ptr->entries_ptr[i]->aem ,matrix_ptr->used_size ,matrix_ptr->size);
	return;
	//
	
}


void find(const matrix *matrix_ptr,const long unsigned int AEM){
	entry *temp_entry;
	unsigned int comparisons;
	//find_core
	if(matrix_ptr->sorted_size == matrix_ptr->used_size){
		temp_entry = find_binary_core(matrix_ptr,AEM,&comparisons);
	}
	else{
		temp_entry = find_linear_core(matrix_ptr,AEM,&comparisons);
	}
	//

	fprintf(stderr,"\n$%d\n",comparisons);//print comparisons between AEMS
	if(temp_entry != NULL){
		printf("\nF-OK %s %hd\n",temp_entry->name,temp_entry->failed_exams);
	}
	else{
		printf("\nF-NOK %ld\n",AEM);
	}

}

void sort(matrix *matrix_ptr){
	int i,j;
	long long int comparisons=0;

	//inserion sort
	for(i=matrix_ptr->sorted_size ;i<matrix_ptr->used_size ;i++,matrix_ptr->sorted_size++){
		//finding the element that is <= element that we want to insert
		for(j=i-1 ;j>=0 ;j--){
			comparisons++;
			if(matrix_ptr->entries_ptr[j]->aem <= matrix_ptr->entries_ptr[i]->aem){
				break;
			}
		}
		//

		//We swap in a manner that the elemet gets inserted and the other elements remain sorted				
		for(j=j+1 ;j<i ;j++){
			entry_address_swap(&(matrix_ptr->entries_ptr[j]),&(matrix_ptr->entries_ptr[i]));
		}
		//
	}
	//
	fprintf(stderr,"\n$%lld\n",comparisons);
	printf("\nS-OK\n");
}

void mod(matrix *matrix_ptr ,const long unsigned int AEM ,const short unsigned int FAILED_EXAMS){
	entry *temp_entry;
	//Finding if the entry with that AEM exists
	if(matrix_ptr->sorted_size == matrix_ptr->used_size){
		temp_entry = find_binary_core(matrix_ptr,AEM,NULL);
	}
	else{
		temp_entry = find_linear_core(matrix_ptr,AEM,NULL);
	}
	//

	if(temp_entry != NULL){
		temp_entry->failed_exams = FAILED_EXAMS;//The actual moding process (if entry found)
		printf("\nM-OK %ld\n",temp_entry->aem);
	}
	else{
		printf("\nM-NOK %ld\n",AEM);
	}
}

void my_remove(matrix *matrix_ptr,const long unsigned int AEM,const unsigned int SIZE_FLUCTUATION,hashtable_matrix *hashtable_info_ptr){
	int i;
	entry *temp_entry; 
	entry **temp_entries_ptr;

	//AEM's address finder
	if(matrix_ptr->sorted_size == matrix_ptr->used_size){
		temp_entry = find_binary_core(matrix_ptr,AEM,NULL);
	}
	else{
		temp_entry = find_linear_core(matrix_ptr,AEM,NULL);
	}
	//

	//Remove if the entry with that AEM exists
	if(temp_entry != NULL){

		//Entries_ptr Size Reducer
		if( (matrix_ptr->size - matrix_ptr->used_size) >= SIZE_FLUCTUATION ){
			
			matrix_ptr->size -=SIZE_FLUCTUATION;
			temp_entries_ptr = (entry**) realloc(matrix_ptr->entries_ptr,matrix_ptr->size*sizeof(entry*));

			if(temp_entries_ptr != NULL){
				matrix_ptr->entries_ptr = temp_entries_ptr;
			}
			else{
				return;
			}
		}
		//
		
		for(i=0 ;matrix_ptr->entries_ptr[i] != temp_entry ;i++);//finding the entry that has that aem in correlation with our entries_ptr array
		
		remove_hashtable_member(hashtable_info_ptr,matrix_ptr->entries_ptr[i]);//HASHTABLE ADDON

		
		entry_delete(&(matrix_ptr->entries_ptr[i])); //delete entry and make pointer pointing NULL

		for( ;i<matrix_ptr->used_size-1 ;i++){
			entry_address_swap(&(matrix_ptr->entries_ptr[i]),&(matrix_ptr->entries_ptr[i+1]));
		}
		matrix_ptr->used_size--;
		
		if(matrix_ptr->sorted_size>0){
			matrix_ptr->sorted_size--;
		}

		//Rehashing (if needed)
		if(hashtable_info_ptr->load_factor == 1 && hashtable_info_ptr->size > hashtable_info_ptr->core_size){
			hashtable_info_ptr->size/=2;
			rehash_hashtable(hashtable_info_ptr ,matrix_ptr ,(hashtable_info_ptr->size*2) );
		}
		//

		printf("\nR-OK %ld, %d %d\n",AEM,matrix_ptr->used_size,matrix_ptr->size);
	}
	else{
		printf("\nR-NOK %ld, %d %d\n",AEM,matrix_ptr->used_size,matrix_ptr->size);
	}
	//
}

void find_by_name(hashtable_matrix *hashtable_info_ptr,char *NAME){
	unsigned long pos;
	entry *curr;
	int j;
	unsigned long int comparisons=1;

	//NAME CAPITALISER
	for(j=0 ;j<NAME_LEN && NAME[j] !='\0' ;j++){
		if(NAME[j]>='a' && NAME[j]<='z'){
        	NAME[j] -=32;
    	}
	}
	//

	pos = my_hash_foo(NAME,hashtable_info_ptr->size);

	for(curr=hashtable_info_ptr->hashtable[pos] ;curr != NULL && strcmp(curr->name,NAME)<0 ;curr =curr->link.nxt,comparisons++);

	if(curr == NULL){
		comparisons--;
		printf("\nN-NOK %s\n",NAME);
	}
	else if(strcmp(curr->name,NAME)>0){
		comparisons++;
		printf("\nN-NOK %s\n",NAME);
	}
	else{
		printf("\nN-OK %s\n",curr->name);
		for( ;curr!=NULL && strcmp(curr->name,NAME) == 0 ;curr = curr->link.nxt){
			printf("%ld %d\n",curr->aem,curr->failed_exams);
		}

	}
	
	fprintf(stderr,"\n$%ld\n",comparisons);//print comparisons between NAMES
	return;
}

void print2(hashtable_matrix *hashtable_info_ptr){
	entry *curr;
	int pos;

	printf("\n#\n");
	printf("%d %d %.2lf %d\n",hashtable_info_ptr->size,hashtable_info_ptr->members,hashtable_info_ptr->load_factor,hashtable_info_ptr->largest_bucket);

	for(pos=0 ;pos<hashtable_info_ptr->size ;pos++){
		printf("%d %d",pos,bucket_size_calc(hashtable_info_ptr,pos));

		for(curr=hashtable_info_ptr->hashtable[pos]->link.nxt ;curr != NULL ;curr=curr->link.nxt){
			printf(" [%ld %s %d]",curr->aem,curr->name,curr->failed_exams);
		}
		printf("\n\n");
	}
	return;
}

/*Project1 print
void print(const matrix *matrix_ptr){
	int i ;
	printf("\n#\n");

	for(i=0 ;i<matrix_ptr->used_size ;i++){
		printf("%ld %s %hd\n",matrix_ptr->entries_ptr[i]->aem ,matrix_ptr->entries_ptr[i]->name ,matrix_ptr->entries_ptr[i]->failed_exams);
	}
	//printf("Size: %d Used: %d Sorted: %d\n",matrix_ptr->size,matrix_ptr->used_size,matrix_ptr->sorted_size); //only for testing purpuses
	
}*/

void quit(matrix **matrix_ptr,hashtable_matrix *hashtable_info_ptr){
	clear_core(*matrix_ptr ,hashtable_info_ptr);
	hashtable_sentinel_unplant(hashtable_info_ptr->hashtable ,0,hashtable_info_ptr->core_size);
	free(hashtable_info_ptr->hashtable);
	free(*matrix_ptr);
}

int main(int argc,char*argv[]){
	entry **temp_entries_ptr;
	matrix *matrix_ptr ,*temp_matrix_ptr;
    entry temp_entry;
	char choice;
	unsigned int size_fluctuation;
    course temp_course;
    hashtable_matrix hashtable_info;
    
	hashtable_matrix_init(&hashtable_info);
	entry_init(&temp_entry);

	//That's the argument part/programm initiation
	if(argc < 4){
        printf("No arguments\n");
		return(0);
    }

	size_fluctuation = atoi(argv[2]);

	temp_matrix_ptr = (matrix*) calloc(1,sizeof(matrix));
	temp_entries_ptr = (entry**) calloc(atoi(argv[1]),sizeof(entry*));
	if(temp_matrix_ptr == NULL || temp_entries_ptr == NULL){
		printf("There is not enough memory for temp matrix ptr and temp entries ptr \n");
		return(0);
	}
	matrix_ptr = temp_matrix_ptr;
	matrix_ptr->entries_ptr = temp_entries_ptr;
	
	matrix_ptr->size = atoi(argv[1]);

    hashtable_info.core_size = atoi(argv[3]);
    hashtable_info.size = hashtable_info.core_size;
    temp_entries_ptr = (entry**) calloc(hashtable_info.core_size,sizeof(entry*));
    hashtable_info.hashtable = temp_entries_ptr;
	hashtable_sentinel_plant(hashtable_info.hashtable,0,hashtable_info.size);
	//
	
	//Menu
  	do{
		
		scanf(" %c",&choice);
		switch (choice)
		{
		case 'a':
			scanf("%ld %s %hd",&temp_entry.aem ,temp_entry.name ,&temp_entry.failed_exams);
			add(matrix_ptr,temp_entry,size_fluctuation,&hashtable_info);
			break;
		case 'r':
			scanf("%ld",&temp_entry.aem);
			my_remove(matrix_ptr,temp_entry.aem,size_fluctuation,&hashtable_info);
			break;
		case 'm':
			scanf("%ld %hd",&temp_entry.aem,&temp_entry.failed_exams);
			mod(matrix_ptr,temp_entry.aem,temp_entry.failed_exams);
			break;
		case 's':
			sort(matrix_ptr);
			break;
		case 'f':
			scanf("%ld",&temp_entry.aem);
			find(matrix_ptr,temp_entry.aem);
			break;
		case'p':
			//print(matrix_ptr);
			print2(&hashtable_info);
			break;
		case'c':
            clear(matrix_ptr,&hashtable_info);
			break;
        case 'g':
            scanf("%ld %hd",&temp_entry.aem,&temp_course.course_code);
            course_reg(matrix_ptr,temp_entry.aem,temp_course.course_code);
            break;
        case 'u':
            scanf("%ld %hd",&temp_entry.aem,&temp_course.course_code);
            course_unreg(matrix_ptr,temp_entry.aem,temp_course.course_code);
            break;
        case 'i':
            scanf("%ld %hd",&temp_entry.aem,&temp_course.course_code);
            course_isreg(matrix_ptr,temp_entry.aem,temp_course.course_code);
            break;
        case 'l':
            scanf("%ld",&temp_entry.aem);
            list_courses(matrix_ptr,temp_entry.aem);
            break;
		case 'n':
			scanf(" %s",temp_entry.name);
			find_by_name(&hashtable_info,temp_entry.name);
			break;
		case'q':
			quit(&matrix_ptr,&hashtable_info);
			return(0);

		}

    }while(1);
	//

	return(0);
}
