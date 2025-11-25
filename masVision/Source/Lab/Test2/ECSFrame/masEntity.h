#pragma once

#include <stdint.h>

typedef uint64_t masEntity;

#define MAS_COMP(c) {#c, sizeof(c)}

typedef struct masComponentDesc
{
    const char* Name;
    size_t Size;
};

#define MAS_DECL_COMP_LIST(Name, ...)\
    const masComponentDesc Name[] = { __VA_ARGS__ }

#define MAS_COMP_LIST(CompList) CompList, (sizeof(CompList)/sizeof(CompList[0]))

// inside this function would query the archtype and find the one with the proper combination
//  if not create a new archtype with these combination and move entity from current to target archtype
//  with adjusting entity handle internal data like archtype and chunk within the archtype 
void masEntity_AddComponents   (masEntity Entity, const masComponentDesc* CompList, uint32_t Count);
void masEntity_RemoveComponents(masEntity Entity, const masComponentDesc* CompList, uint32_t Count);
bool masEntity_HasComponents   (masEntity Entity, const masComponentDesc* CompList, uint32_t Count);
void masEntity_Attach(masEntity Parent, masEntity Child);
void masEntity_Detach(masEntity Entity);


///////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////
typedef struct test 
{
} masPosition, masRotationEuler, masScale, masWorldMatrix, masSceneNode, masVelocity, masRigidBody, masPlayerTag, masMagicFire, masExoticWeapon;

void useage_test()
{
    masEntity Ent = 1000;

    MAS_DECL_COMP_LIST(PersistentCompList,
        MAS_COMP(masPosition),
        MAS_COMP(masRotationEuler),
        MAS_COMP(masScale),
        MAS_COMP(masWorldMatrix),
        MAS_COMP(masSceneNode));
    masEntity_AddComponents(Ent, MAS_COMP_LIST(PersistentCompList));

    MAS_DECL_COMP_LIST(EntRemoveCompList,
        MAS_COMP(masRigidBody),
        MAS_COMP(masPlayerTag));
    masEntity_RemoveComponents(Ent, MAS_COMP_LIST(EntRemoveCompList));

    MAS_DECL_COMP_LIST(EntFindCompList,
        MAS_COMP(masMagicFire),
        MAS_COMP(masExoticWeapon));
    if (masEntity_HasComponents(Ent, MAS_COMP_LIST(EntFindCompList)))
    {
        // do something
    }
    else
    {
        // report ent has not components of these types
    }
}