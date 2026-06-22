#ifndef OBJ_DIAG_H
#define OBJ_DIAG_H

#include <stddef.h>

#define OBJ_MAGIC 0x4F424A54UL

typedef unsigned int ObjTypeID;

#define TYPEID_SHAPE          1
#define TYPEID_CIRCLE         2
#define TYPEID_RECTANGLE      3
#define TYPEID_SHAPECONTAINER 4

typedef struct ObjDiagInfo ObjDiagInfo;

struct ObjDiagInfo {
    unsigned int magic;
    ObjTypeID type_id;
    const char* type_name;
    void* obj_ptr;
    ObjDiagInfo* parent;
    ObjDiagInfo** children;
    size_t child_count;
    size_t child_capacity;
    size_t create_order;
    size_t destroy_order;
    int is_destroyed;
    const char* create_file;
    int create_line;
    void** vtable;
    size_t vtable_size;
    const char** method_names;
};

void ObjDiag_Init(void);
void ObjDiag_Shutdown(void);

ObjDiagInfo* ObjDiag_RegisterObject(void* obj_ptr,
                                    ObjTypeID type_id,
                                    const char* type_name,
                                    const char* file,
                                    int line,
                                    void** vtable,
                                    size_t vtable_size,
                                    const char** method_names);

void ObjDiag_SetParent(ObjDiagInfo* child, ObjDiagInfo* parent);
void ObjDiag_UnregisterObject(ObjDiagInfo* info);
void ObjDiag_RecordDestroy(ObjDiagInfo* info);

int ObjDiag_CheckType(const ObjDiagInfo* info, ObjTypeID expected_type);

void ObjDiag_PrintDiagnosticPage(void);
void ObjDiag_PrintObjectHierarchy(const ObjDiagInfo* info, int indent);
void ObjDiag_PrintFunctionPointers(const ObjDiagInfo* info);
void ObjDiag_PrintReleaseOrder(void);
void ObjDiag_PrintLeaks(void);
int ObjDiag_ValidateReleaseOrder(void);
int ObjDiag_HasLeaks(void);

void ObjDiag_ReportTypeMismatch(const ObjDiagInfo* info,
                                ObjTypeID expected_type,
                                const char* context);

const char* ObjDiag_GetTypeName(ObjTypeID type_id);

#define DIAG_REGISTER(obj, type_id, type_name, vt, vt_size, mnames) \
    ObjDiag_RegisterObject((obj), (type_id), (type_name), __FILE__, __LINE__, \
                           (void**)(vt), (vt_size), (mnames))

#define DIAG_SET_PARENT(child_info, parent_info) \
    ObjDiag_SetParent((child_info), (parent_info))

#define DIAG_UNREGISTER(info) \
    ObjDiag_UnregisterObject((info))

#define DIAG_RECORD_DESTROY(info) \
    ObjDiag_RecordDestroy((info))

#define DIAG_CHECK_TYPE(info, expected_type, context) \
    do { \
        if (!ObjDiag_CheckType((info), (expected_type))) { \
            ObjDiag_ReportTypeMismatch((info), (expected_type), (context)); \
        } \
    } while (0)

#endif
