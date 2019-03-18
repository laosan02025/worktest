#include <stdio.h>

typedef struct _singlechain{

	int data;
	struct _singlechain *next;

}SINGLECHAIN;

typedef struct _doublechain{
	int data;
	struct DOUBLECHAIN *l;
	struct DOUBLECHAIN *r;
	
}DOUBLECHAIN;

SINGLECHAIN* initsinglechain(int cnt){
	int i = 0;	
	SINGLECHAIN *tmp = malloc(sizeof(SINGLECHAIN));
	SINGLECHAIN *HEAD = tmp;
	for(i = 0 ;i< cnt ;i++){
		tmp->next = malloc(sizeof(SINGLECHAIN));
		tmp->data = i;
		tmp = tmp->next;
 
	}
	tmp->next = NULL;
	tmp->data = cnt;
	tmp = HEAD;
	while(tmp!=NULL){
		printf("%d|",tmp->data);
		tmp = tmp ->next;
	}
	printf("\n-------------------\n");
	return HEAD;
}

SINGLECHAIN* inverse_single(SINGLECHAIN* HEAD){
	SINGLECHAIN* tmp = HEAD;
	SINGLECHAIN *p,*q;
	p = HEAD;
	q = p->next;
	p->next = NULL;
	tmp = q;
	while(q!=NULL){
		tmp = q->next;
		q->next = p;
		p = q;
		q = tmp;
	}
	HEAD = p;
	printf("overview single:");
	while(p!=NULL){
		printf("%d|",p->data);
		p = p->next;
	}
	printf("\n");
	return HEAD;

}
DOUBLECHAIN * initdoublechain(int cnt)
{
	int i = 0;
	DOUBLECHAIN *tmp = malloc(sizeof(DOUBLECHAIN));
	DOUBLECHAIN *HEAD = tmp;
	DOUBLECHAIN *PRE = NULL;
	DOUBLECHAIN *tail = NULL;
	tmp->l = NULL;
	for(i = 0;i<cnt;i++){
		tmp->data = i;
		tmp->r = malloc(sizeof(DOUBLECHAIN));
		tmp->l = PRE;
		PRE = tmp;
		tmp = tmp->r;		
	}
	tmp->r = NULL;
	tmp->data = 100;
	tmp->l = PRE;
	tmp = HEAD;
	i = 0;
	while(tmp!=NULL){
		printf("%d||",tmp->data);
		tail = tmp;
		if(tmp->l==NULL){
			printf("left is NULL:(%d)\n",i++);
		}
		tmp = tmp->r;
	}
	printf("\n------------\n");
	
	tmp = tail;
	while(tmp!=NULL){
		printf("%d||",tmp->data);		
		tmp = tmp->l;
	}
	printf("\n------------------\n");
	return HEAD;
}

DOUBLECHAIN *inverse_double(DOUBLECHAIN* HEAD)
{
	DOUBLECHAIN *p,*q,*tmpl,*tmpr,*tail;
	
	p = HEAD;
	q = p->r;
	tmpr = q;
	p->r = NULL;
	p->l = q;
	while(q!=NULL){
		tmpr = q->r;
		q->r = p;
		q->l = tmpr;
		p = q;
		q = tmpr;
	}
	HEAD = p;
	printf("\ninverse double print head:");
	while(p!=NULL){
		printf("%d||",p->data);
		tail = p;
		p = p->r;
	}
	printf("\n");
	printf("\ninverse double print tail:");
	p = tail ;
	while(p!=NULL){
		printf("%d||",p->data);
		p = p->l;
	}
	printf("\n");
	return HEAD;
}



void main(){
	SINGLECHAIN *HEAD_S = initsinglechain(100);
	DOUBLECHAIN *HEAD_D = initdoublechain(100);

	HEAD_S = inverse_single(HEAD_S);
	HEAD_D = inverse_double(HEAD_D);
}
