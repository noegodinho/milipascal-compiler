#ifndef _GENERATOR_
#define _GENERATOR_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "functions.h"
#include "semantic.h"

#define SIZE_INT 3
#define SIZE_DOUBLE 6

typedef struct save_strings save_strings;
struct save_strings{
	save_strings *next;
	char *value;
	int id;
};

typedef struct Save_variables Save_variables;
struct Save_variables{
	char *func_name;
	char *name;
	char *type;
	char *registry;
	Save_variables *next;
};

int index_string_name = 1;
int index_variable_name = 1;
save_strings *string_list = NULL;
SymbolTableLine *globalTableLine;
Save_variables *store_variables = NULL;

/*Functions*/
void generateProgram(Program* program, SymbolTableHeader *symbolTableHeader);
void declarePrint();
void printAllStrings(Program *program);
char *formatString(char *str);
void insert_into_list_string(char *value);
void printString(char *name);
int typeFunctionGlobalTable(char *type);
void printGlobalVariable(SymbolTableLine *symbolTableLine);
void createHeaderFunction(char* name, Program *program, SymbolTableHeader *symbolTableHeader);
SymbolTableHeader *findTableFunction(char* name, SymbolTableHeader *symbolTableHeader);
void printHeaderFunction(char *type, char *name);
SymbolTableLine *findParamFunction(SymbolTableLine *symbolTableLine);
void findVarFunction(SymbolTableLine *symbolTableLine);
void printVarFunction(char *name, char *type);
Program *findFunction(Program *program, char *name);
void printReturnFunction(char *type, char *name);
char *varTypeTable(char *type);
int findId(char *value);
Program *findMain(Program *program);
void searchInMain(Program *program, SymbolTableLine *symbolTableLine, SymbolTableHeader *symbolTableHeader);
void searchWriteLn(Program *program, SymbolTableHeader *symbolTableHeader);
void printWriteLn(int size, char *id, char *value);
void printWriteLnId(int size, char *id, char *value);
char *rightAssignFunction(Program *program, SymbolTableLine *symbolTableLine);
int hasTerminalSymbol(char* type);
SymbolTableLine *findGlobalLine(char *name);
void save_variables_function(char* func_name, char* name, char* type, char* registry);
Save_variables *find_variables_function(char* func_name, char* name);


void generateProgram(Program* program, SymbolTableHeader *symbolTableHeader){
	Program *aux;
	SymbolTableLine *temp = symbolTableHeader->symbolTableLine;
	globalTableLine = temp;

	declarePrint();
	printAllStrings(program);

	while(symbolTableHeader->symbolTableLine != NULL){
		if(typeFunctionGlobalTable(symbolTableHeader->symbolTableLine->type)){
			createHeaderFunction(symbolTableHeader->symbolTableLine->name, program, symbolTableHeader);			
		}

		else{
			printGlobalVariable(symbolTableHeader->symbolTableLine);
		}

		symbolTableHeader->symbolTableLine = symbolTableHeader->symbolTableLine->next;
	}

	printHeaderFunction("i32", "main");
	printf(") {\n");
	aux = findMain(program);
	searchInMain(aux, temp, NULL);
	printReturnFunction("i32", "0");
}

void declarePrint(){
	printf("; Default Declarations\n");
	printf("@.strNewLine = private unnamed_addr constant [2 x i8] c\"\\0A\\00\"\n");
	printf("@.strInt = private unnamed_addr constant [3 x i8] c\"%%d\\00\"\n");
	printf("@.strDouble = private unnamed_addr constant [6 x i8] c\"%%.12E\\00\"\n");
	printf("@.str = private unnamed_addr constant [3 x i8] c\"%%s\\00\"\n");
	printf("@.strTrue = private unnamed_addr constant [5 x i8] c\"TRUE\\00\"\n");
	printf("@.strFalse = private unnamed_addr constant [6 x i8] c\"FALSE\\00\"\n");
	printf("\n");

	printf("; Print Declaration\n");
	printf("declare i32 @printf(i8*, ...)\n");
	printf("\n");
}

void printAllStrings(Program *program){
	if(program != NULL){
		if(strcmp(program->type, "String") == 0){
			program->value = formatString(program->value);
			insert_into_list_string(program->value);
			printString(program->value);
		}

		printAllStrings(program->son);
		printAllStrings(program->brother);
	}
}

char *formatString(char *str){
	char *temp;

	temp = (char *)malloc(sizeof(char) * strlen(str) - 2);
	strncpy(temp, str + 1, strlen(str) - 2);

	return temp;
}

void insert_into_list_string(char *value){
	save_strings *new, *aux;

	if(string_list == NULL){
		string_list = (save_strings *)calloc(1, sizeof(save_strings));
		string_list->value = value;
		string_list->id = index_string_name;
	}

	else{
		aux = string_list;
		new = (save_strings *)calloc(1, sizeof(save_strings));

		while(aux->next != NULL){
			aux = aux->next;
		}

		new->value = value;
		new->id = index_string_name;
		aux->next = new;
	}
}

void printString(char *name){
	printf("@.str.%d = private unnamed_addr constant [%d x i8] c\"%s\\00\"\n", index_string_name++, (int)strlen(name), name);
}

int typeFunctionGlobalTable(char *type){
	return (strcmp(type, "_function_") == 0) ? 1 : 0;
}

void printGlobalVariable(SymbolTableLine *symbolTableLine){
	printf("@%s = common global %s 0\n", symbolTableLine->name, varTypeTable(symbolTableLine->type));
}

void createHeaderFunction(char *name, Program *program, SymbolTableHeader *symbolTableHeader){
	SymbolTableHeader *table;
	SymbolTableLine *temp;
	Program *aux;

	table = findTableFunction(name, symbolTableHeader->next);
	
	printHeaderFunction(varTypeTable(table->symbolTableLine->type), table->symbolTableLine->name);
	
	temp = findParamFunction(table->symbolTableLine->next);	
	findVarFunction(temp);

	aux = findFunction(program, table->symbolTableLine->name);
	searchInMain(aux, table->symbolTableLine, table);
	
	printReturnFunction(varTypeTable(table->symbolTableLine->type), "0");
}

SymbolTableHeader *findTableFunction(char *name, SymbolTableHeader *symbolTableHeader){
	SymbolTableLine *line;

	while(symbolTableHeader != NULL){	
		line = symbolTableHeader->symbolTableLine;

		if(strcmp(to_lower_case(name), line->name) == 0){
			return symbolTableHeader;
		}

		symbolTableHeader = symbolTableHeader->next;
	}
	
	return NULL;
}

void printHeaderFunction(char *type, char *name){
	printf("\ndefine %s @%s(", type, name);
}

SymbolTableLine *findParamFunction(SymbolTableLine *symbolTableLine){
	if(symbolTableLine == NULL){
		printf(") {\n");
		return symbolTableLine;
	}

	while(1){
		printf("%s %%%s", varTypeTable(symbolTableLine->type), symbolTableLine->name);

		symbolTableLine = symbolTableLine->next;

		if(symbolTableLine != NULL && symbolTableLine->flag != NULL){
			printf(", ");
		}

		else{
			printf(") {\n");
			break;
		}
	}

	return symbolTableLine;
}

void findVarFunction(SymbolTableLine *symbolTableLine){
	while(symbolTableLine != NULL){
		printVarFunction(symbolTableLine->name, varTypeTable(symbolTableLine->type));

		symbolTableLine = symbolTableLine->next;
	}
}

void printVarFunction(char *name, char *type){
	printf("  %%%s = alloca %s\n", name, type);
}

Program *findFunction(Program *program, char *name){
	Program *aux = program;

	/* goes to the son of FUNCPART, to find the function name */
	aux = aux->brother->brother->son;

	while(strcmp(aux->son->value, name) != 0){
		aux = aux->brother;
	}

	return findMain(aux->son);
	//return NULL;
}

void printReturnFunction(char *type, char *name){
	printf("  ret %s %s\n}\n", type, name);
}

char *varTypeTable(char *type){
	if(strcmp(type, "_integer_") == 0){
		return "i32";
	}

	if(strcmp(type, "_boolean_") == 0){
		return "i1";
	}

	return "double";
}

int findId(char *value){
	save_strings *aux = string_list;

	while(strcmp(value, aux->value) != 0){
		aux = aux->next;
	}

	return aux->id;
}

Program *findMain(Program *program){
	Program *aux = program;

	while(strcmp(aux->type, "VarPart") != 0){
		aux = aux->brother;
	}

	aux = aux->brother;

	if(strcmp(aux->type, "FuncPart") == 0){
		aux = aux->brother;
	}

	return aux;
}

void searchInMain(Program *program, SymbolTableLine *symbolTableLine, SymbolTableHeader *symbolTableHeader){
	SymbolTableLine *line = symbolTableLine;
	char *aux = (char*) malloc(sizeof(char)*40);

	if(program != NULL){
		if(strcmp(program->type, "WriteLn") == 0){
			searchWriteLn(program->son, symbolTableHeader);
		}

		if(strcmp(program->type, "Assign")==0){
			char *assign_value;

			assign_value = rightAssignFunction(program->son->brother, line);

			line = findGlobalLine(program->son->value);
			strcpy(aux,"@");

			if(line==NULL){
				line = symbolTableLine;
				while(strcmp(program->son->value,line->name)!=0 && line!=NULL){
					line = line->next;
				}
				strcpy(aux,"%");
			}

			if(symbolTableHeader==NULL)
				save_variables_function("main", program->son->value, varTypeTable(line->type), assign_value);
			else
				save_variables_function(symbolTableLine->name, program->son->value, varTypeTable(line->type), assign_value);			

			strcat(aux,program->son->value);
			printf("  store %s %s, %s* %s\n",varTypeTable(line->type),assign_value,varTypeTable(line->type),aux);
		}

		searchInMain(program->son, symbolTableLine, symbolTableHeader);
		searchInMain(program->brother, symbolTableLine, symbolTableHeader);
	}
}

void searchWriteLn(Program *program, SymbolTableHeader *symbolTableHeader){
	char temp[17];
	Save_variables *aux;
	
	while(program != NULL){
		if(strcmp(program->type, "IntLit") == 0){
			printWriteLn(SIZE_INT, "@.strInt", program->value);
		}

		else if(strcmp(program->type, "RealLit") == 0){
			printWriteLn(SIZE_DOUBLE, "@.strDouble", program->value);
		}

		else if(strcmp(program->type, "String") == 0){
			sprintf(temp, "@.str.%d", findId(program->value));
			printWriteLn((int)strlen(program->value), temp, "0");
		}

		else if(strcmp(program->type, "Id") == 0){



			if(symbolTableHeader==NULL)
				aux = find_variables_function("main", program->value);
			else
				aux = find_variables_function(symbolTableHeader->symbolTableLine->name, program->value);

			if(strcmp(aux->type, "i32")==0)
				printWriteLnId(SIZE_INT, "@.strInt", aux->registry);
			else if(strcmp(aux->type, "i1")==0){
				if(strcmp(aux->name, "true")==0)
					printWriteLnId(5, "@.strTrue", aux->registry);
				else
					printWriteLnId(6, "@.strFalse", aux->registry);
			}

			else
				printWriteLnId(SIZE_DOUBLE, "@.strDouble", aux->registry);
		}

		program = program->brother;
	}

	printf("  %%%d = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([2 x i8]* @.strNewLine, i32 0, i32 0))\n", index_variable_name++);
}

void printWriteLn(int size, char *id, char *value){
	printf("  %%%d = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([%d x i8]* %s, i32 0, i32 %s))\n", index_variable_name++, size, id, value);
}

void printWriteLnId(int size, char *id, char *value){
	printf("  %%%d = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([%d x i8]* %s, i32 0, i32 0), i32 %s)\n", index_variable_name++, size, id, value);
}

char *rightAssignFunction(Program *program, SymbolTableLine *symbolTableLine){
	Program *temp, *temp2;
	SymbolTableLine *line = symbolTableLine;
	char *aux = (char*) malloc(sizeof(char)*40);
	char *var1 = (char*) malloc(sizeof(char)*10);
	char *var2 = (char*) malloc(sizeof(char)*10);
	int need_return = 0;


	if(strcmp(program->type, "Add")==0){
		temp = program;
		temp2 = temp->son;
		while(hasTerminalSymbol(temp->son->type)!=0){
			need_return=1;
			temp=temp->son;
			var1 = rightAssignFunction(temp, line);
		}

		temp = temp2;

		while(hasTerminalSymbol(temp->type)!=0){
			temp = temp->brother;
		}

		while(temp!=NULL){
			line = findGlobalLine(temp->value);
			strcpy(aux,"@");

			if(line==NULL){
				line = symbolTableLine;
				while(strcmp(temp->value,line->name)!=0 && line!=NULL){
					line = line->next;
				}
				strcpy(aux,"%");
			}
				
			strcat(aux, temp->value);

			if(strcmp(var1,"")==0)
				sprintf(var1, "%d",index_variable_name);
			else
				sprintf(var2, "%d",index_variable_name);

			printf("  %%%d = load %s* %s\n", index_variable_name++, varTypeTable(line->type), aux);
			temp=temp->brother;
		}

		printf("  %%%d = add nsw %s %%%s, %%%s\n", index_variable_name++, varTypeTable(line->type), var1, var2);
		if(need_return==0)
			sprintf(aux,"%d", index_variable_name-1);
		else
			sprintf(aux,"%%%d", index_variable_name-1);
		return aux;
	}

	else if(strcmp(program->type, "Sub")==0){
		temp = program;
		temp2 = temp->son;
		while(hasTerminalSymbol(temp->son->type)!=0){
			need_return = 1;
			temp=temp->son;
			var1 = rightAssignFunction(temp, line);
		}

		temp = temp2;

		while(hasTerminalSymbol(temp->type)!=0){
			temp = temp->brother;
		}

		while(temp!=NULL){
			line = findGlobalLine(temp->value);
			strcpy(aux,"@");

			if(line==NULL){
				line = symbolTableLine;
				while(strcmp(temp->value,line->name)!=0 && line!=NULL){
					line = line->next;
				}
				strcpy(aux,"%");
			}
			strcat(aux, temp->value);

			if(strcmp(var1,"")==0)
				sprintf(var1, "%d",index_variable_name);
			else
				sprintf(var2, "%d",index_variable_name);

			printf("  %%%d = load %s* %s\n", index_variable_name++, varTypeTable(line->type), aux);
			temp=temp->brother;
		}

		printf("  %%%d = sub nsw %s %%%s, %%%s\n", index_variable_name++, varTypeTable(line->type), var1, var2);
		if(need_return==0)
			sprintf(aux,"%d", index_variable_name-1);
		else
			sprintf(aux,"%%%d", index_variable_name-1);
		return aux;

	}

	else if(strcmp(program->type, "IntLit")==0){ 
		return program->value;
	}

	else{
		strcpy(aux,"%");
		strcat(aux,program->value);
		return aux;
	}

	return NULL;
}

int hasTerminalSymbol(char* type){
	if(strcmp(type, "IntLit") == 0){
		return 0;
	}

	else if(strcmp(type, "RealLit") == 0){
		return 0;
	}

	else if(strcmp(type, "Id") == 0){
		return 0;
	}

	return 1;
}

SymbolTableLine *findGlobalLine(char *name){
	SymbolTableLine *temp = globalTableLine;

	while(temp!=NULL){
		if(strcmp(name, temp->name)==0)
			return temp;
		temp=temp->next;
	}

	return NULL;
}

void save_variables_function(char* func_name, char* name, char* type, char* registry){
	Save_variables *temp, *aux = store_variables;

	if(store_variables==NULL){
		store_variables = (Save_variables*)calloc(1,sizeof(Save_variables));
		store_variables->func_name = func_name;
		store_variables->name = name;
		store_variables->type = type;
		store_variables->registry = registry;
		return;
	}

	temp = (Save_variables*)calloc(1,sizeof(Save_variables));

	while(aux->next!=NULL){
		if(strcmp(aux->name,name)==0 && strcmp(aux->func_name,func_name)==0){
			aux->registry = registry;
			return;
		}

		aux = aux->next;
	}

	temp->func_name = func_name;
	temp->name = name;
	temp->type = type;
	temp->registry = registry;
	aux->next = temp;
}

Save_variables *find_variables_function(char* func_name, char* name){
	Save_variables *aux = store_variables;
	while(aux!=NULL){
		if(strcmp(aux->func_name,func_name)==0 && strcmp(aux->name,name)==0)
			return aux;

		aux = aux->next;
	}

	return NULL;
}

#endif
