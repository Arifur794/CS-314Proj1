/*
 *********************************************
 *  314 Principles of Programming Languages  *
 *  Spring 2017                              *
 *  Author: Ulrich Kremer  and Arifur Rahman *
 *  Student Version                          *
 *********************************************
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "InstrUtils.h"
#include "Utils.h"


void optimize(Instruction *head);
Instruction *getRegOrig(Instruction *instr, int reg);
Instruction *getMemOrig
(Instruction *instr, int reg, int offset);
void origMark(Instruction *instr);

int main()
{
	Instruction *head;

	head = ReadInstructionList(stdin);
	if (!head) {
		WARNING("No instructions\n");
		exit(EXIT_FAILURE);
	}

	optimize(head);

	if (head) 
		PrintInstructionList(stdout, head);
	
	return EXIT_SUCCESS;
}


void optimize(Instruction *head) {
	Instruction *node, *temp;
	
	node = head;
	while (node != NULL) {
		
		if (node->opcode == OUTPUTAI) {  //OUTPUTAI marked
			
			origMark(node);
		}
		node = node->next;
	}
	
	node = head;
	while (node != NULL) {
		if (node->critical) {
			node = node->next;
		} else {

			if (node->prev) {
				node->prev->next = node->next;
			}
			
			if (node->next) {
				node->next->prev = node->prev;
			}
			
			temp = node->next;
			free(node);
			node = temp;
		}
	}
}

Instruction *getRegOrig(Instruction *instr, int reg) {
	Instruction *node = instr;
	
	while (node != NULL) {
		switch (node->opcode) {
			case LOADI:
				if (node->field2 == reg) 
					return node;
				break;
			case LOADAI:
			case ADD:
			case SUB:
			case MUL:
			case DIV:
				if (node->field3 == reg)
					return node;
				break;
			default:
				break;
		}
		
		node = node->prev;
	}
	
	return NULL;
}


Instruction *getMemOrig
(Instruction *instr, int reg, int offset) {
	Instruction *node = instr->prev;
	
	while (node != NULL) {
		if (node->opcode == STOREAI) {
			if (node->field2 == reg && node->field3 == offset) {
				return node;
			}
		}
		node = node->prev;
	}
	
	return NULL;
}


void origMark(Instruction *instr) {
	Instruction *node1 = NULL, *node2 = NULL;
	
	if (instr->critical) {
		
		return;
	}
	
	instr->critical = 1;
	
	switch (instr->opcode) {
		case STOREAI:
			node1 = getRegOrig
		(instr, instr->field1);
			node2 = getRegOrig
		(instr, instr->field2);
			break;
		case OUTPUTAI:
		case LOADAI:
			node1 = getRegOrig
		(instr, instr->field1);
			node2 = getMemOrig
			(instr, instr->field1, instr->field2);
			break;
		case ADD:
		case SUB:
		case MUL:
		case DIV:
			node1 = getRegOrig
		(instr, instr->field1);
			node2 = getRegOrig
		(instr, instr->field2);
			break;
		case LOADI:  
			break;
	}
	
	if (node1) {
		origMark(node1);
	}
	
	if (node2) {
		origMark(node2);
	}
}