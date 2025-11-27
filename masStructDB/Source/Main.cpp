#include "masECS.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////
typedef union masVec2
{
	float xy[2];
	struct
	{
		float x, y;
	};
} masPoint;

typedef union masVec3
{
	float xyz[3];
	struct
	{
		float x, y, z;
	};
} masPosition, masScale, masRotation, masVelociy;

typedef union masVec4
{
	float xyzw[4];
	struct
	{
		float x, y, z, w;
	};
} masQuaternion;

typedef struct masMatrix
{
	float m16[16];
};

typedef struct masPlayer
{
	uint8_t ControllerID;
};


///////////////////////////////////////////////////////////////////////////////////////////////////////
bool RegisterStructs();
void CreatePlayer();


///////////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, const char** argv)
{
	if (!masECS_Init())
		return -1;

	if (!RegisterStructs())
		return -1;

	CreatePlayer();

	while (1)
	{
		masECS_Update();
	}

	masECS_DeInit();
}



///////////////////////////////////////////////////////////////////////////////////////////////////////
bool RegisterStructs()
{
	// STRUCTS REGISTERATION
	MAS_REGISTER_STRUCT(masPlayer,
		MAS_STRUCT_FIELD(uint8_t, ControllerID));

	MAS_REGISTER_STRUCT(masMatrix,
		MAS_STRUCT_FIELD(float, m16));

	MAS_REGISTER_STRUCT(masVec4,
		MAS_STRUCT_FIELD(float, x),
		MAS_STRUCT_FIELD(float, y),
		MAS_STRUCT_FIELD(float, z),
		MAS_STRUCT_FIELD(float, w));

	MAS_REGISTER_STRUCT(masVec3,
		MAS_STRUCT_FIELD(float, x),
		MAS_STRUCT_FIELD(float, y),
		MAS_STRUCT_FIELD(float, z));

	MAS_REGISTER_STRUCT(masVec2,
		MAS_STRUCT_FIELD(float, x),
		MAS_STRUCT_FIELD(float, y));


	// ALIASES REGISTERATION
	MAS_REGISTER_STRUCT_ALIAS(masVec4,
		MAS_STRUCT_ALIAS(masQuaternion));

	MAS_REGISTER_STRUCT_ALIAS(masVec3,
		MAS_STRUCT_ALIAS(masPosition),
		MAS_STRUCT_ALIAS(masScale),
		MAS_STRUCT_ALIAS(masRotation),
		MAS_STRUCT_ALIAS(masVelociy));

	MAS_REGISTER_STRUCT_ALIAS(masVec2,
		MAS_STRUCT_ALIAS(masPoint));

	if (!masStructDB_Save())
		return false;

	return true;
}

void CreatePlayer()
{
#if 0
	MAS_DECL_STRUCT_LIST(PlayerTemplate,
		MAS_STRUCT(masPosition),
		MAS_STRUCT(masRotation),
		MAS_STRUCT(masScale),
		MAS_STRUCT(masMatrix),
		MAS_STRUCT(masPlayer));

	uint64_t ent_node = mas_object();
	mas_object_add_structs(ent_node, MAS_STRUCT_LIST(PlayerTemplate));
#endif
}