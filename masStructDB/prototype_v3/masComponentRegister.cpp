#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "masComponentRegister.h"
#include "io/mas_mmap.h"

union masTag
{
	size_t Value;
	struct
	{
		char Bytes[sizeof(size_t)];
	};
};

struct masComponent
{
	size_t UniqueID;
	size_t Size;
	char   Name[64];
};

struct masRegister
{
	size_t       Tag;
	size_t       UniqueIDGen;
	size_t       Count;
	masComponent Comps[];
};


static mas_mmap_t   GFile     = { };
static masRegister *GRegister = NULL;


bool masComponentRegister_Init()
{
	mas_mmap_ret_t ret = mas_mmap(&GFile, "Comps.masReg");
	if (ret == mas_mmap_ret_error)
		return false;

	const char* TagValue = "masReg07";
	masTag Tag = {};
	memcpy(Tag.Bytes, TagValue, strlen(TagValue));

	if (ret == mas_mmap_ret_created)
	{
		masRegister Register = { };
		Register.Tag         = Tag.Value;
		Register.UniqueIDGen = 1;
		Register.Count       = 0;

		size_t Bytes = mas_mmap_write(&GFile, 0, &Register, sizeof(masRegister));
		if (Bytes != sizeof(masRegister))
		{
			mas_unmmap(&GFile);
			return false;
		}
	}
	else if(ret == mas_mmap_ret_opened)
	{
		GRegister = (masRegister*)mas_mmap_read(&GFile, 0, sizeof(masRegister));
		if (GRegister->Tag != Tag.Value)
		{
			mas_unmmap(&GFile);
			return false;
		}
	}

	GRegister = (masRegister*)mas_mmap_read(&GFile, 0, sizeof(masRegister));
	return true;
}

void masComponentRegister_DeInit()
{
	mas_unmmap(&GFile);
	GRegister = NULL;
}

void masComponentRegister_Add(const char* name, uint64_t size)
{
	if (!GRegister)
		return;

	// check component is not in the file
	for (size_t c = 0; c < GRegister->Count; ++c)
	{
		masComponent* Comp = &GRegister->Comps[c];
		if (strcmp(name, Comp->Name) == 0)
		{
			printf("[ ERROR ]: Component is already in the file[ %s ]\n", name);
			return;
		}
	}

	masComponent Comp = { };
	Comp.Size     = size;
	Comp.UniqueID = GRegister->UniqueIDGen++;
	memcpy(Comp.Name, name, strlen(name));

	size_t WriteOffset = sizeof(masRegister) + (sizeof(masComponent) * GRegister->Count);
	size_t Bytes       = mas_mmap_write(&GFile, WriteOffset, &Comp, sizeof(masComponent));
	if (Bytes != sizeof(masComponent))
	{
		printf("[ ERROR ]: Writing component to file [ %s ]\n", name);
		return;
	}

	GRegister->Count++;

}