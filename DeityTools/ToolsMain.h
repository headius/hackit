// structs and functions for deity applications
#include <windows.h>

#define STRUCT_VERSION 1

#define DeityAlloc(y) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, y)
#define DeityFree(z) HeapFree(GetProcessHeap(), 0, z)

typedef struct _DEITY_WINDOW {
	HWND handle;
	LPSTR title[256];
	int version;
} DEITY_WINDOW;

typedef struct _DEITY_NODE {
	int element;
	int hash;
	_DEITY_NODE *prevNode;
	_DEITY_NODE *nextNode;
} DEITY_NODE;

typedef struct _DEITY_LIST {
	int listSize;
	int lastIndex;
	DEITY_NODE** listElements;
	DEITY_NODE* headElement;
	DEITY_NODE* tailElement;
} DEITY_LIST;

typedef struct _DEITY_MAP {
	int hashSize;
	DEITY_LIST** hashElements;
} DEITY_MAP;

DEITY_NODE* DeityNodeCreate();
DEITY_LIST* DeityListCreate(int initialSize);
DEITY_MAP* DeityMapCreate(int hashSize);

int DeityNodeGetElement(DEITY_NODE* node);
int DeityNodeGetHash(DEITY_NODE* node);
DEITY_NODE* DeityNodeGetNextNode(DEITY_NODE* node);
DEITY_NODE* DeityNodeGetPrevNode(DEITY_NODE* node);
void DeityNodeSetElement(DEITY_NODE* node, int element);
void DeityNodeSetHash(DEITY_NODE* node, int hash);
void DeityNodeSetNextNode(DEITY_NODE* node, DEITY_NODE* nextNode);
void DeityNodeSetPrevNode(DEITY_NODE* node, DEITY_NODE* prevNode);

void DeityListAdd(DEITY_LIST* list, DEITY_NODE* node);
void DeityListInsertBefore(DEITY_LIST* list, DEITY_NODE* node, int index);
DEITY_NODE* DeityListGetAt(DEITY_LIST* list, int index);
void DeityListRemoveAt(DEITY_LIST* list, int index);
DEITY_NODE* DeityListGetHeadNode(DEITY_LIST* list);
DEITY_NODE* DeityListGetTailNode(DEITY_LIST* list);
void DeityListClear(DEITY_LIST* list);

void DeityMapPut(DEITY_MAP* map, DEITY_NODE* node);
DEITY_NODE* DeityMapGet(DEITY_MAP* map, int hash);
void DeityMapRemove(DEITY_MAP* map, int hash);
void DeityMapRehash(DEITY_MAP* map, int hashSize);
void DeityMapClear(DEITY_MAP* map);

void DeityDisplayError(LPSTR message);