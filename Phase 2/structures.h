#ifndef _STRUCTURES_
#define _STRUCTURES_

typedef struct Stat Stat;
typedef struct IdStruct IdStruct;
struct IdStruct{
	IdStruct* next;
	char* id;
};

typedef struct VarDecl{
	char* first_id;
	char* last_id;
	IdStruct* id;
}VarDecl;

typedef struct VarPart VarPart;
struct VarPart{
	VarPart* next;
	VarDecl* varDecl;
};

typedef struct ProgBlock ProgBlock;
/* Program */
typedef struct Program{
	char* id;
	ProgBlock* progBlock;
}Program;

typedef struct FuncPart FuncPart;
typedef struct FormalParams{
	char* first_id;
	IdStruct* idStruct;
	char* last_id;
}FormalParams;

typedef struct FormalParamList FormalParamList;
struct FormalParamList{
	FormalParams* formalParams;
	FormalParamList* next;
};

typedef struct FuncHeading{
	char* first_id;
	FormalParamList* next;
	char* last_id;
}FuncHeading;

typedef struct FuncDeclaration{
	Stat* stat;
	VarPart* varPart;
	
	union{
		char *id;
		FuncHeading* funcHeading;
	}funcDeclarationUnion;
}FuncDeclaration;

struct FuncPart{
	FuncPart* next;
	FuncDeclaration *funcDeclaration;
};

struct ProgBlock{
	VarPart* varPart;
	FuncPart* funcPart;
	Stat* stat;
};

typedef struct StatList StatList;
struct StatList{
	Stat *stat;
	StatList *next;
};

typedef struct Expr{
	
}Expr;

typedef struct WriteInPList{
	
}WriteInPList;

struct Stat{
	Expr *expr;
	Stat *next;
	WriteInPList * writeInPList;
	union{
		char *id;
		StatList *statList;
	}StatUnion;
};

#endif
