#include "ToolsMain.h"

DEITY_NODE* DeityNodeCreate() {
	DEITY_NODE* node = (DEITY_NODE*)DeityAlloc(sizeof(DEITY_NODE));

	return node;
}

DEITY_LIST* DeityListCreate(int initialSize = 1) {
	if (initialSize <= 0) {
		return NULL;
	}

	DEITY_LIST* list = (DEITY_LIST*)DeityAlloc(sizeof(DEITY_LIST));

	list->listSize = initialSize;
	list->listElements = (DEITY_NODE**)DeityAlloc(sizeof(DEITY_NODE*) * initialSize);

	list->lastIndex = -1;

	return list;
}

DEITY_MAP* DeityMapCreate(int hashSize) {
	if (hashSize <= 0) {
		return NULL;
	} else {
		DEITY_MAP* map = (DEITY_MAP*)DeityAlloc(sizeof(DEITY_MAP));
		map->hashSize = hashSize;

		map->hashElements = (DEITY_LIST**)DeityAlloc(sizeof(DEITY_LIST*) * hashSize);
		return map;
	}
}

int DeityNodeGetElement(DEITY_NODE* node) {
	if (IsBadReadPtr(node, sizeof(DEITY_NODE))) {
		return NULL;
	} else {
		return node->element;
	}
}

int DeityNodeGetHash(DEITY_NODE* node) {
	if (IsBadReadPtr(node, sizeof(DEITY_NODE))) {
		return NULL;
	} else {
		return node->hash;
	}
}

DEITY_NODE* DeityNodeGetNextNode(DEITY_NODE* node) {
	if (IsBadReadPtr(node, sizeof(DEITY_NODE))) {
		return NULL;
	} else {
		return node->nextNode;
	}
}

DEITY_NODE* DeityNodeGetPrevNode(DEITY_NODE* node) {
	if (IsBadReadPtr(node, sizeof(DEITY_NODE))) {
		return NULL;
	} else {
		return node->prevNode;
	}
}

void DeityNodeSetElement(DEITY_NODE* node, int element) {
	if (IsBadReadPtr(node, sizeof(DEITY_NODE))) {
		return;
	} else {
		node->element = element;
	}
}

void DeityNodeSetHash(DEITY_NODE* node, int hash) {
	if (IsBadReadPtr(node, sizeof(DEITY_NODE))) {
		return;
	} else {
		node->hash = hash;
	}
}

void DeityNodeSetNextNode(DEITY_NODE* node, DEITY_NODE* nextNode) {
	if (IsBadReadPtr(node, sizeof(DEITY_NODE)) || IsBadReadPtr(nextNode, sizeof(DEITY_NODE))) {
		return;
	} else {
		node->nextNode = nextNode;
		nextNode->prevNode = node;
	}
}

void DeityNodeSetPrevNode(DEITY_NODE* node, DEITY_NODE* prevNode) {
	if (IsBadReadPtr(node, sizeof(DEITY_NODE)) || IsBadReadPtr(prevNode, sizeof(DEITY_NODE))) {
		return;
	} else {
		node->prevNode = prevNode;
		prevNode->nextNode = node;
	}
}

void DeityListAdd(DEITY_LIST* list, DEITY_NODE* node) {
	if (IsBadReadPtr(list, sizeof(DEITY_LIST))) {
		return;
	}

	// to do: add memory checking
	if (list->lastIndex == -1) {
		list->listElements[0] = node;
		DeityNodeSetNextNode(node, NULL);
		DeityNodeSetPrevNode(node, NULL);
	} else {
		if (list->listSize == (list->lastIndex + 1)) {
			// list needs to grow
			int newSize = list->listSize * 2;
			DEITY_NODE** tempElements = (DEITY_NODE**)DeityAlloc(sizeof(DEITY_NODE*) * newSize);

			// copy elements
			CopyMemory(tempElements, list->listElements, sizeof(DEITY_NODE*) * list->listSize);

			// tidy up
			DeityFree(list->listElements);
			list->listElements = tempElements;
			list->listSize = newSize;
		}

		list->listElements[list->lastIndex + 1] = node;
		DeityNodeSetNextNode(list->listElements[list->lastIndex], node);
	}
}

void DeityListInsert(DEITY_LIST* list, DEITY_NODE* node, int index) {
	if (IsBadReadPtr(list, sizeof(DEITY_LIST))) {
		return;
	}

	// to do: add memory checking
	if (index > list->lastIndex || index < 0) {
		// invalid insert location
		return;
	} else {
		if (list->listSize == (list->lastIndex + 1)) {
			// list needs to grow
			int newSize = list->listSize * 2;
			DEITY_NODE** tempElements = (DEITY_NODE**)DeityAlloc(sizeof(DEITY_NODE*) * newSize);

			// copy elements before
			CopyMemory(tempElements, list->listElements, sizeof(DEITY_NODE*) * (index));
			
			// copy elements after, leaving space for new element
			CopyMemory(tempElements + index + 1, list->listElements + index, sizeof(DEITY_NODE*) * (list->lastIndex - index + 1));

			// tidy up
			DeityFree(list->listElements);
			list->listElements = tempElements;
			list->listSize = newSize;
		}

		list->listElements[index] = node;

		if (index != 0) {
			DeityNodeSetNextNode(list->listElements[index - 1], node);
		}

		DeityNodeSetNextNode(node, list->listElements[index + 1]);
	}
}

DEITY_NODE* DeityListGetHeadNode(DEITY_LIST* list) {
	// to do: add memory checking
	return list->headElement;
}

DEITY_NODE* DeityListGetTailNode(DEITY_LIST* list) {
	// to do: add memory checking
	return list->tailElement;
}

DEITY_NODE* DeityListGetAt(DEITY_LIST* list, int index) {
	if (IsBadReadPtr(list, sizeof(DEITY_LIST))) {
		return NULL;
	}

	// to do: add memory checking
	if (index > list->lastIndex || index < 0) {
		// invalid index
		return NULL;
	} else {
		return list->listElements[index];
	}
}

void DeityListRemoveAt(DEITY_LIST* list, int index) {
	if (IsBadReadPtr(list, sizeof(DEITY_LIST))) {
		return;
	}

	// to do: add memory checking
	if (index > list->lastIndex || index < 0) {
		// invalid index
		return;
	} else {
		DEITY_NODE** tempElements = (DEITY_NODE**)DeityAlloc(sizeof(DEITY_NODE*) * list->listSize);
		
		// copy elements before index
		CopyMemory(tempElements, list->listElements, sizeof(DEITY_NODE*) * (index));

		// copy elements after index
		CopyMemory(tempElements + index, list->listElements + index + 1, sizeof(DEITY_NODE*) * (list->listSize - index - 1));

		// tidy up
		DeityFree(list->listElements);
		list->listElements = tempElements;
		list->lastIndex--;
	}
}

int DeityListLastIndex(DEITY_LIST* list) {
	// to do: add memory checking
	return list->lastIndex;
}

void DeityListClear(DEITY_LIST* list) {
	if (IsBadReadPtr(list, sizeof(DEITY_LIST))) {
		return;
	}

	// to do: add memory checking
	DeityFree(list->listElements);
}

void DeityMapPut(DEITY_MAP* map, DEITY_NODE* node) {
	if (IsBadReadPtr(map, sizeof(DEITY_MAP))) {
		return;
	}

	// to do: add memory checking
	int slot = DeityNodeGetHash(node) % map->hashSize;

	// check if we've created a slot
	if (map->hashElements[slot] == NULL) {
		map->hashElements[slot] = DeityListCreate();
	}

	// add to slot
	DeityListAdd(map->hashElements[slot], node);
}

DEITY_NODE* DeityMapGet(DEITY_MAP* map, int hash) {
	// to do: add memory checking
	int slot = hash % map->hashSize;

	DEITY_LIST* list = map->hashElements[slot];

	if (list == NULL) {
		// slot not created, we don't have element with this hash
		return NULL;
	} else {
		DEITY_NODE* node = DeityListGetHeadNode(list);

		while (node != NULL && DeityNodeGetHash(node) != hash) {
			node = DeityNodeGetNextNode(node);
		}

		// NULL or real node at this point
		return node;
	}
}

void DeityMapRemove(DEITY_MAP* map, int hash) {
	// to do: add memory checking
	int slot = hash * map->hashSize;

	DEITY_LIST* list = map->hashElements[slot];

	if (list == NULL) {
		// don't have element at this hash
		return;
	} else {
		int index = 0;
		DEITY_NODE* node = DeityListGetAt(list, index);

		while (node != NULL && DeityNodeGetHash(node) != hash) {
			index++;
			node = DeityListGetAt(list, index);
		}

		if (node != NULL) {
			// found node, remove it
			DeityListRemoveAt(list, index);
		}

		if (DeityListLastIndex(list) < 0) {
			// no elements left in slot, clean up
			DeityListClear(list);
			DeityFree(list);
		}
	}
}

void DeityMapRehash(DEITY_MAP* map, int hashSize) {
	// to do: implement
}

void DeityMapClear(DEITY_MAP* map) {
	// to do: add memory checking
	int slot = 0;
	DEITY_LIST* list = map->hashElements[slot];

	while (slot < map->hashSize) {
		if (list != NULL) {
			DeityListClear(list);
			DeityFree(list);
			map->hashElements[slot] = NULL;
		}

		slot++;
	}
}


void DeityDisplayError(LPSTR message) {
	LPVOID lpMsgBuf;
	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
	);
	// Process any inserts in lpMsgBuf.
	// ...
	// Display the string.
	MessageBox( NULL, (LPCTSTR)lpMsgBuf, message, MB_TOPMOST | MB_OK | MB_ICONINFORMATION );
	// Free the buffer.
	LocalFree( lpMsgBuf );
}