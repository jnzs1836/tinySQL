#include "index_manager.h"

#include<iostream>
#include<vector>
#include<algorithm>
void *memory = malloc(INDEX_BLOCK_SIZE);
bool exist = false;
std::string filename = "tmp";
namespace IndexManager {

	IndexManager::IndexManager() {
		if (!exist) {
			std::cout << "Creating Indexing";
			currentMemory = memory;
			MetaData metaData;
			metaData.rootPosition = 0;
			metaData.num = 0;
			for (int i = 0; i < MAX_NODE_NUM; i++) {
				metaData.nodeMemoryTable.used[i] = false;
			}
			memcpy(currentMemory, &metaData, sizeof(MetaData));
			exist = true;

		}
		else {
			currentMemory = memory;
		}
		
	}

	IndexManager::~IndexManager() {
		if (!bPlusTreeChar) {
			delete bPlusTreeChar;
		}
		if (!bPlusTreeFloat) {
			delete bPlusTreeFloat;
		}
		if (!bPlusTreeInt) {
			delete bPlusTreeInt;
		}
	
	}



	void IndexManager::setName(std::string name, std::string attr) {
		
		currentIndexName = name;
		currentType = INT;
		bPlusTreeType = INT;
		attrName = attr;
		
		writeIndexFromMemory(filename, memory, INDEX_BLOCK_SIZE);
		filename = name;
		readIndexToMemory(filename, memory, INDEX_BLOCK_SIZE);
		
		switch (currentType) {
		case INT:
			bPlusTreeInt = new BPlusTree<BPlusNodeInt, IntKey>(currentMemory);
			break;
		case CHAR:
			bPlusTreeChar = new BPlusTree<BPlusNodeChar, CharKey>(currentMemory);
			break;
		case FLOAT:
			bPlusTreeFloat = new BPlusTree<BPlusNodeFloat, FloatKey>(currentMemory);
			break;
		}

	}

	int IndexManager::createIndex(std::string name, rua type) {
		std::ifstream fin;
		std::string prefix = "./";
		std::string endfix = ".index";
		fin.open(prefix + name + endfix, std::ifstream::binary);
		if (!fin) {
			saveIndexFileCatalog(name, name + ".index");
			bPlusTreeType = type;
			currentType = type;
			switch (type) {
			case INT:
				bPlusTreeInt = new BPlusTree<BPlusNodeInt, IntKey>(currentMemory);
				break;
			case CHAR:
				bPlusTreeChar = new BPlusTree<BPlusNodeChar, CharKey>(currentMemory);
				break;
			case FLOAT:
				bPlusTreeFloat = new BPlusTree<BPlusNodeFloat, FloatKey>(currentMemory);
				break;
			}
			MetaData metaData;
			metaData.rootPosition = 0;
			metaData.num = 0;
			for (int i = 0; i < MAX_NODE_NUM; i++) {
				metaData.nodeMemoryTable.used[i] = false;
			}
			void *tmp = malloc(INDEX_BLOCK_SIZE);
			memcpy(tmp, &metaData, sizeof(MetaData));
			writeIndexFromMemory(name, tmp, INDEX_BLOCK_SIZE);
			return 1;
		}
		else {
			return 0;
		}
		
		
		
	}
	int IndexManager::dropIndex(std::string name) {
		if (filename == name) {
			filename = "tmp";
		}
		std::string prefix = "./";
		std::string endfix = ".index";
		std::string tmp = prefix + name + endfix;
		char fileChar[255];
		strcpy(fileChar, tmp.c_str());
		int get = remove(fileChar);
		if (get < 0) {
			return 0;
		}
		else {
			return 1;
		}

	}

	int IndexManager::Insert(int key, addressType data) {
		IntKey keyStructureInt;
		keyStructureInt.key = key;
		keyStructureInt.data = data;
		return bPlusTreeInt->insert(keyStructureInt);
		/* switch(currentType){
		case INT:
		BPlusTree<BPlusNodeInt,IntKey> bPlusTree = BPlusTree<BPlusNodeInt,IntKey>(currentMemory);
		IntKey keyStructure;
		break;
		case CHAR:
		BPlusTree<BPlusNodeChar,CharKey> bPlusTree = BPlusTree<BPlusNodeChar,CharKey>(currentMemory);
		CharKey keyStructure;
		break;
		case FLOAT:
		BPlusTree<BPlusNodeFloat,FloatKey> bPlusTree = BPlusTree<BPlusNodeFloat,FloatKey>(currentMemory);
		FloatKey keyStructure;
		break;
		}*/


	}

	int IndexManager::Insert(std::string key, addressType data) {
		CharKey keyStructureChar;
		keyStructureChar.key = toMyCharType(key);
		keyStructureChar.data = data;
		return bPlusTreeChar->insert(keyStructureChar);
	}

	int IndexManager::Insert(float key, addressType data) {
		FloatKey keyStructureFloat;
		keyStructureFloat.key = key;
		keyStructureFloat.data = data;
		return bPlusTreeFloat->insert(keyStructureFloat);

	}
	int IndexManager::Delete(int key) {
		IntKey keyStructure;
		keyStructure.key = key;
		keyStructure.data = 0;
		return bPlusTreeInt->remove(keyStructure);
	}
	int IndexManager::Delete(float key) {
		FloatKey keyStructure;
		keyStructure.key = key;
		keyStructure.data = 0;
		return bPlusTreeFloat->remove(keyStructure);
	}
	int IndexManager::Delete(std::string key) {

		CharKey keyStructure;
		keyStructure.key = toMyCharType(key);
		keyStructure.data = 0;
		return bPlusTreeChar->remove(keyStructure);
		

	}

	std::vector<addressType> IndexManager::Search(int key, int searchType) {
		IntKey keyStructure;
		keyStructure.key = key;
		keyStructure.data = 0;
		keyStructure.type = searchType;
		return bPlusTreeInt->searchNodes(keyStructure);
	}
	std::vector<addressType> IndexManager::Search(float key, int searchType) {
		FloatKey keyStructure;
		keyStructure.key = key;
		keyStructure.data = 0;
		keyStructure.type = searchType;
		return bPlusTreeFloat->searchNodes(keyStructure);
	}

	std::vector<addressType> IndexManager::Search(std::string key, int searchType) {
		CharKey keyStructure;
		keyStructure.key = toMyCharType(key);
		keyStructure.data = 0;
		keyStructure.type = searchType;
		return bPlusTreeChar->searchNodes(keyStructure);
	}

	void IndexManager::saveIndexFileCatalog(std::string indexName, std::string indexFile) {
		;
	}

	int IndexManager::readIndexToMemory(std::string file, void* memory, int size) {
		std::ifstream fin;
		std::string prefix = "./";
		std::string endfix = ".index";
		fin.open(prefix+file+endfix, std::ifstream::binary);
		if (!fin) {
			return 0;
		}
		fin.read((char*)memory, size);
		fin.close();
		return 1;
	}

	int IndexManager::writeIndexFromMemory(std::string file, void* memory, int size) {
		std::string prefix = "./";
		std::ofstream fout;
		std::string endfix = ".index";
		fout.open(prefix+file+endfix, std::ofstream::binary);

		fout.write((char *)memory, size);
		if (!fout) {
			return 0;
		}
		fout.close();
		return 1;
	}

	MyCharType IndexManager::toMyCharType(std::string s) {
		MyCharType my;
		strcpy(my.data, s.c_str());
		return my;
	}

}
