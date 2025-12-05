///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "mas_mmap.h"
#include "mas_ecs_memory.h"
#include "mas_ecs_components.h"


///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
#define MAS_PTR_OFFSET(type, ptr, offset) (type*)(((uint8_t*)ptr) + (offset))


///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
union mas_tag
{
    char parts[8];
    struct
    {
        uint64_t value;
    };
};

struct mas_component
{
    uint64_t crc64;
    uint64_t name_hash;
    uint32_t unique_id;
    uint32_t size;
    uint32_t name_len;
    char     name[64];
};

struct mas_header
{
    uint64_t tag;
    uint32_t comp_count;
    uint32_t unique_id_gen;
    mas_component components[];
};


///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
static mas_mmap_t  gfile = { };
static mas_header *ghdr      = NULL;


///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
static mas_component* mas_internal_find_comp(const char* comp_name)
{
    if(!comp_name)
        return NULL;

    for(int32_t i = 0; i < ghdr->comp_count; ++i)
    {
        mas_component* comp = &ghdr->components[i];
        if(strcmp(comp->name, comp_name) == 0)
            return comp;   
    }

    return NULL;
}

static uint64_t mas_internal_calc_crc64(const void* data, size_t len)
{
    const uint64_t CRC64_POLY = 0x42F0E1EBA9EA3693ULL;
    uint64_t       crc        = 0;

    const uint8_t *p = (const uint8_t *)data;
    while (len--) {
        crc ^= (uint64_t)(*p++) << 56;   /* bring byte into top 8 bits */
        for (int i = 0; i < 8; ++i) {
            if (crc & 0x8000000000000000ULL)
                crc = (crc << 1) ^ CRC64_POLY;
            else
                crc <<= 1;
        }
    }
    return crc;
}

static uint64_t mas_internal_hash_name(const char* data, size_t len)
{
    const uint64_t FNV64_OFFSET_BASIS = 0xCBF29CE484222325ULL;
    const uint64_t FNV64_PRIME        = 0x100000001B3ULL;

    uint64_t hash = FNV64_OFFSET_BASIS;
    const uint8_t *p = (const uint8_t *)data;
    while (len--) {
        hash ^= (uint64_t)(*p++);
        hash *= FNV64_PRIME;
    }
    return hash;
}

static bool mas_internal_register_comp(const char* name, size_t size)
{
    if(!name || size == 0)
        return false;
    
    // check name len
    size_t name_len = strlen(name);
    if(name_len >= 64)
        return false;

    // setup component data
    mas_component comp = { };
    memcpy(comp.name, name, name_len);
    comp.unique_id = ghdr->unique_id_gen;
    comp.size      = size;
    comp.name_len  = name_len;
    comp.crc64     = 0;
    comp.name_hash = mas_internal_hash_name(comp.name, comp.name_len);

    // write component data to file
    uint64_t write_offset = sizeof(mas_header) + (ghdr->comp_count * sizeof(mas_component));
    size_t   byte         = mas_mmap_write(&gfile, write_offset, &comp, sizeof(mas_component));
    if(byte != sizeof(mas_component))
    {
        // log error
        printf("ERROR[ REGISTER_COMPONENT ]: component %s\n", name);
        return false;
    }

    // calculate crc of the component block
    // NOTE: dont include crc variable when calculating
    mas_component* comp_ptr = (mas_component*)mas_mmap_read(&gfile, write_offset, sizeof(mas_component));
    if(!comp_ptr)
    {
        // log error
        return false;
    }
    const void *comp_block      = MAS_PTR_OFFSET(const void, comp_ptr, sizeof(uint64_t));
    size_t      comp_block_size = sizeof(mas_component) - sizeof(uint64_t);
    comp_ptr->crc64 = mas_internal_calc_crc64(comp_block, comp_block_size);

    // update file hdr
    ghdr->comp_count++;
    ghdr->unique_id_gen++;

    return true;
}


///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
bool mas_ecs_components_init()
{
    mas_mmap_ret_t ret = mas_mmap(&gfile, "components.masCompDB");
    if(ret == mas_mmap_ret_error)
        return false;

    mas_tag tag = {"COMPSDB"};
    mas_header* hdr = (mas_header*)mas_mmap_read(&gfile, 0, sizeof(mas_header));
    if(!hdr)
        return false;

    if(ret == mas_mmap_ret_created)
    {
        hdr->tag           = tag.value;
        hdr->comp_count    = 0;
        hdr->unique_id_gen = 1;
    }

    if(hdr->tag != tag.value)
    {
        mas_unmmap(&gfile);
        return false;
    }

    ghdr = hdr;
    return true;
}

void mas_ecs_components_deinit()
{
    mas_unmmap(&gfile);
    ghdr = NULL;
}

void mas_ecs_components_register(const char* name, size_t size)
{
    if(!mas_mmap_is_valid(&gfile))
        return;

    mas_component* comp = mas_internal_find_comp(name);
    if(!comp)
        mas_internal_register_comp(name, size);
}

mas_component_query* mas_ecs_components_query(const char** comp_name_list, uint32_t count)
{
    if(!comp_name_list || count == 0)
        return NULL;

    uint32_t             query_size = sizeof(mas_component_query) + (sizeof(mas_component_query_desc) * count);
    mas_component_query *query      = MAS_ECS_MEMORY_FRAME_MALLOC(mas_component_query, query_size);
    if(!query)
        return NULL;
    query->comps   = MAS_PTR_OFFSET(mas_component_query_desc, query, sizeof(mas_component_query));
    query->count = 0;

    size_t component_offset = 0;
    for(int32_t i = 0; i < count; ++i)
    {
        mas_component* comp = mas_internal_find_comp(comp_name_list[i]);
        if(!comp)
        {
            // query is allocated on frame memory that got reset at the end of each frame
            printf("ERROR: [ QUERY_COMPONENTS ] -> %s\n", comp_name_list[i]);
            return NULL;
        }

        mas_component_query_desc* comp_desc = &query->comps[i];
        comp_desc->offset    = component_offset;
        comp_desc->size      = comp->size;
        comp_desc->unique_id = comp->unique_id;
        comp_desc->name_hash = comp->name_hash;

        query->count++;

        component_offset += comp->size;
    }

    return query;
}

// FOR DEBUG
void mas_ecs_comonents_print()
{
    if(!mas_mmap_is_valid(&gfile) || !ghdr)
    {
        printf("ERROR: [ COMPONENT_DB_NOT_OPENED ]\n");
        return;
    }

    printf("REGISTERED_COMPONENTS:\n");
    printf("    - TAG:      %llu\n",  ghdr->tag);
    printf("    - COUNT:    %u\n",    ghdr->comp_count);
    printf("    - GUID_GEN: %u\n",    ghdr->unique_id_gen);
    printf("    - COMPONENTS[%u]:\n", ghdr->comp_count);
    
    for(int32_t i = 0; i < ghdr->comp_count; ++i)
    {
        printf("\n");

        mas_component* comp = &ghdr->components[i];

        const void *comp_block      = MAS_PTR_OFFSET(const void*, comp, sizeof(uint64_t));
        uint32_t    comp_block_size = sizeof(mas_component) - sizeof(uint64_t);
        uint64_t    recheck_crc64   = mas_internal_calc_crc64(comp_block, comp_block_size);

        printf("        [ COMPONENT ]:\n");
        printf("            - CHECK_CRC: %llu\n", recheck_crc64);
        printf("            - CRC:       %llu\n", comp->crc64);
        printf("            - NAME_HASH: %llu\n", comp->name_hash);
        printf("            - UNIQUE_ID: %u\n",   comp->unique_id);
        printf("            - SIZE:      %u\n",   comp->size);
        printf("            - NAME:      %s\n",   comp->name);
    }
}