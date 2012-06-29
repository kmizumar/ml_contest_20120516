#include <cstdio>
#include <cstdlib>
#include <cstddef>
#include <string>
#include <cstring>
#include <cmath>

#include <list>
#include <vector>

#define AE_USE_CPP

// ================================================================ alglib_impl
namespace alglib_impl {
/*
 * definitions
 */
#define AE_UNKNOWN 0
#define AE_MSVC 1
#define AE_GNUC 2
#define AE_SUNC 3
#define AE_INTEL 1
#define AE_SPARC 2
 
/*
 * automatically determine compiler
 */
#define AE_COMPILER AE_UNKNOWN
#ifdef __GNUC__
#undef AE_COMPILER
#define AE_COMPILER AE_GNUC
#endif

/*
 * if we work under C++ environment, define several conditions
 */
#ifdef AE_USE_CPP
#define AE_USE_CPP_BOOL
#define AE_USE_CPP_ERROR_HANDLING
#define AE_USE_CPP_SERIALIZATION
#endif

#if (AE_COMPILER==AE_GNUC) || (AE_COMPILER==AE_SUNC) || (AE_COMPILER==AE_UNKNOWN)
typedef int ae_int32_t;
#endif

#if (AE_COMPILER==AE_GNUC) || (AE_COMPILER==AE_SUNC) || (AE_COMPILER==AE_UNKNOWN)
typedef signed long long ae_int64_t;
#endif

#if !defined(AE_INT_T)
typedef ptrdiff_t ae_int_t;
#endif

/*
 * local definitions
 */
#define x_nb 16
#define AE_DATA_ALIGN 16
#define AE_PTR_ALIGN sizeof(void*)
#define DYN_BOTTOM ((void*)1)
#define DYN_FRAME  ((void*)2)
#define AE_LITTLE_ENDIAN 1
#define AE_BIG_ENDIAN 2
#define AE_MIXED_ENDIAN 3
#define AE_SER_ENTRY_LENGTH 11
#define AE_SER_ENTRIES_PER_ROW 5

#define AE_SM_DEFAULT 0
#define AE_SM_ALLOC 1
#define AE_SM_READY2S 2
#define AE_SM_TO_STRING 10
#define AE_SM_FROM_STRING 20
#define AE_SM_TO_CPPSTRING 11

#define ae_bool bool
#define ae_true true
#define ae_false false

typedef struct { double x, y; } ae_complex;

typedef enum
{
    ERR_OK = 0,
    ERR_OUT_OF_MEMORY = 1,
    ERR_XARRAY_TOO_LARGE = 2,
    ERR_ASSERTION_FAILED = 3
} ae_error_type;

typedef ae_int_t ae_datatype;

/*
 * other definitions
 */
enum { OWN_CALLER=1, OWN_AE=2 };
enum { ACT_UNCHANGED=1, ACT_SAME_LOCATION=2, ACT_NEW_LOCATION=3 };
enum { DT_BOOL=1, DT_INT=2, DT_REAL=3, DT_COMPLEX=4 };
enum { CPU_SSE2=1 };

typedef struct
{
    ae_int64_t     owner;
    ae_int64_t     last_action;
    char *ptr;
} x_string;

typedef struct
{
    ae_int64_t     cnt;
    ae_int64_t     datatype;
    ae_int64_t     owner;
    ae_int64_t     last_action;
    void *ptr;
} x_vector;

typedef struct
{
    ae_int64_t     rows;
    ae_int64_t     cols;
    ae_int64_t     stride;
    ae_int64_t     datatype;
    ae_int64_t     owner;
    ae_int64_t     last_action;
    void *ptr;
} x_matrix;

typedef struct ae_dyn_block
{
    struct ae_dyn_block * volatile p_next;
    /* void *deallocator; */
    void (*deallocator)(void*);
    void * volatile ptr;
} ae_dyn_block;

typedef struct ae_frame
{
    ae_dyn_block db_marker;
} ae_frame;

typedef struct
{
    ae_int_t endianness;
    double v_nan;
    double v_posinf;
    double v_neginf;
    
    ae_dyn_block * volatile p_top_block;
    ae_dyn_block last_block;
#ifndef AE_USE_CPP_ERROR_HANDLING
    jmp_buf * volatile break_jump;
#endif
    ae_error_type volatile last_error;
    const char* volatile error_msg;
} ae_state;

typedef struct
{
    ae_int_t mode;
    ae_int_t entries_needed;
    ae_int_t entries_saved;
    ae_int_t bytes_asked;
    ae_int_t bytes_written;

#ifdef AE_USE_CPP_SERIALIZATION
    std::string     *out_cppstr;
#endif
    char            *out_str;
    const char      *in_str;
} ae_serializer;

typedef void(*ae_deallocator)(void*);

typedef struct ae_vector
{
    ae_int_t cnt;
    ae_datatype datatype;
    ae_dyn_block data;
    union
    {
        void *p_ptr;
        ae_bool *p_bool;
        ae_int_t *p_int;
        double *p_double;
        ae_complex *p_complex;
    } ptr;
} ae_vector;

typedef struct ae_matrix
{
    ae_int_t rows;
    ae_int_t cols;
    ae_int_t stride;
    ae_datatype datatype;
    ae_dyn_block data;
    union
    {
        void *p_ptr;
        void **pp_void;
        ae_bool **pp_bool;
        ae_int_t **pp_int;
        double **pp_double;
        ae_complex **pp_complex;
    } ptr;
} ae_matrix;

ae_int_t ae_misalignment(const void *ptr, size_t alignment);
void* ae_align(void *ptr, size_t alignment);
void* aligned_malloc(size_t size, size_t alignment);
void  aligned_free(void *block);

void* ae_malloc(size_t size, ae_state *state);
void  ae_free(void *p);
ae_int_t ae_sizeof(ae_datatype datatype);

void ae_state_init(ae_state *state);
void ae_state_clear(ae_state *state);
#ifndef AE_USE_CPP_ERROR_HANDLING
void ae_state_set_break_jump(ae_state *state, jmp_buf *buf);
#endif
void ae_break(ae_state *state, ae_error_type error_type, const char *msg);

void ae_frame_make(ae_state *state, ae_frame *tmp);
void ae_frame_leave(ae_state *state);

void ae_db_attach(ae_dyn_block *block, ae_state *state);
ae_bool ae_db_malloc(ae_dyn_block *block, ae_int_t size, ae_state *state, ae_bool make_automatic);
ae_bool ae_db_realloc(ae_dyn_block *block, ae_int_t size, ae_state *state);
void ae_db_free(ae_dyn_block *block);
void ae_db_swap(ae_dyn_block *block1, ae_dyn_block *block2);

ae_bool ae_vector_init(ae_vector *dst, ae_int_t size, ae_datatype datatype, ae_state *state, ae_bool make_automatic);
ae_bool ae_vector_init_copy(ae_vector *dst, ae_vector *src, ae_state *state, ae_bool make_automatic);
void ae_vector_init_from_x(ae_vector *dst, x_vector *src, ae_state *state, ae_bool make_automatic);
ae_bool ae_vector_set_length(ae_vector *dst, ae_int_t newsize, ae_state *state);
void ae_vector_clear(ae_vector *dst);
void ae_swap_vectors(ae_vector *vec1, ae_vector *vec2);

ae_bool ae_matrix_init(ae_matrix *dst, ae_int_t rows, ae_int_t cols, ae_datatype datatype, ae_state *state, ae_bool make_automatic);
ae_bool ae_matrix_init_copy(ae_matrix *dst, ae_matrix *src, ae_state *state, ae_bool make_automatic);
void ae_matrix_init_from_x(ae_matrix *dst, x_matrix *src, ae_state *state, ae_bool make_automatic);
ae_bool ae_matrix_set_length(ae_matrix *dst, ae_int_t rows, ae_int_t cols, ae_state *state);
void ae_matrix_clear(ae_matrix *dst);
void ae_swap_matrices(ae_matrix *mat1, ae_matrix *mat2);

void ae_x_set_vector(x_vector *dst, ae_vector *src, ae_state *state);
void ae_x_set_matrix(x_matrix *dst, ae_matrix *src, ae_state *state);
void ae_x_attach_to_vector(x_vector *dst, ae_vector *src);
void ae_x_attach_to_matrix(x_matrix *dst, ae_matrix *src);

void x_vector_clear(x_vector *dst);

ae_bool x_is_symmetric(x_matrix *a);
ae_bool x_is_hermitian(x_matrix *a);
ae_bool x_force_symmetric(x_matrix *a);
ae_bool x_force_hermitian(x_matrix *a);
ae_bool ae_is_symmetric(ae_matrix *a);
ae_bool ae_is_hermitian(ae_matrix *a);
ae_bool ae_force_symmetric(ae_matrix *a);
ae_bool ae_force_hermitian(ae_matrix *a);

void ae_serializer_init(ae_serializer *serializer);
void ae_serializer_clear(ae_serializer *serializer);

void ae_serializer_alloc_start(ae_serializer *serializer);
void ae_serializer_alloc_entry(ae_serializer *serializer);
ae_int_t ae_serializer_get_alloc_size(ae_serializer *serializer);

#ifdef AE_USE_CPP_SERIALIZATION
void ae_serializer_sstart_str(ae_serializer *serializer, std::string *buf);
void ae_serializer_ustart_str(ae_serializer *serializer, const std::string *buf);
#endif
void ae_serializer_sstart_str(ae_serializer *serializer, char *buf);
void ae_serializer_ustart_str(ae_serializer *serializer, const char *buf);

void ae_serializer_serialize_bool(ae_serializer *serializer, ae_bool v, ae_state *state);
void ae_serializer_serialize_int(ae_serializer *serializer, ae_int_t v, ae_state *state);
void ae_serializer_serialize_double(ae_serializer *serializer, double v, ae_state *state);
void ae_serializer_unserialize_bool(ae_serializer *serializer, ae_bool *v, ae_state *state);
void ae_serializer_unserialize_int(ae_serializer *serializer, ae_int_t *v, ae_state *state);
void ae_serializer_unserialize_double(ae_serializer *serializer, double *v, ae_state *state);

void ae_serializer_stop(ae_serializer *serializer);

void ae_assert(ae_bool cond, const char *msg, ae_state *state);
ae_int_t ae_cpuid();

ae_bool ae_fp_eq(double v1, double v2);
ae_bool ae_fp_neq(double v1, double v2);
ae_bool ae_fp_less(double v1, double v2);
ae_bool ae_fp_less_eq(double v1, double v2);
ae_bool ae_fp_greater(double v1, double v2);
ae_bool ae_fp_greater_eq(double v1, double v2);

ae_bool ae_isfinite_stateless(double x, ae_int_t endianness);
ae_bool ae_isnan_stateless(double x,    ae_int_t endianness);
ae_bool ae_isinf_stateless(double x,    ae_int_t endianness);
ae_bool ae_isposinf_stateless(double x, ae_int_t endianness);
ae_bool ae_isneginf_stateless(double x, ae_int_t endianness);

ae_int_t ae_get_endianness();

ae_bool ae_isfinite(double x,ae_state *state);
ae_bool ae_isnan(double x,   ae_state *state);
ae_bool ae_isinf(double x,   ae_state *state);
ae_bool ae_isposinf(double x,ae_state *state);
ae_bool ae_isneginf(double x,ae_state *state);

double   ae_fabs(double x,   ae_state *state);
ae_int_t ae_iabs(ae_int_t x, ae_state *state);
double   ae_sqr(double x,    ae_state *state);
double   ae_sqrt(double x,   ae_state *state);

ae_int_t ae_sign(double x,   ae_state *state);
ae_int_t ae_round(double x,  ae_state *state);
ae_int_t ae_trunc(double x,  ae_state *state);
ae_int_t ae_ifloor(double x, ae_state *state);
ae_int_t ae_iceil(double x,  ae_state *state);

ae_int_t ae_maxint(ae_int_t m1, ae_int_t m2, ae_state *state);
ae_int_t ae_minint(ae_int_t m1, ae_int_t m2, ae_state *state);
double   ae_maxreal(double m1, double m2, ae_state *state);
double   ae_minreal(double m1, double m2, ae_state *state);
double   ae_randomreal(ae_state *state);
ae_int_t ae_randominteger(ae_int_t maxv, ae_state *state);

double   ae_sin(double x, ae_state *state);
double   ae_cos(double x, ae_state *state);
double   ae_tan(double x, ae_state *state);
double   ae_sinh(double x, ae_state *state);
double   ae_cosh(double x, ae_state *state);
double   ae_tanh(double x, ae_state *state);
double   ae_asin(double x, ae_state *state);
double   ae_acos(double x, ae_state *state);
double   ae_atan(double x, ae_state *state);
double   ae_atan2(double y, double x, ae_state *state);

double   ae_log(double x, ae_state *state);
double   ae_pow(double x, double y, ae_state *state);
double   ae_exp(double x, ae_state *state);

void ae_state_init(ae_state *state)
{
    ae_int32_t *vp;

    /*
     * p_next points to itself because:
     * * correct program should be able to detect end of the list
     *   by looking at the ptr field.
     * * NULL p_next may be used to distinguish automatic blocks
     *   (in the list) from non-automatic (not in the list)
     */
    state->last_block.p_next = &(state->last_block);
    state->last_block.deallocator = NULL;
    state->last_block.ptr = DYN_BOTTOM;
    state->p_top_block = &(state->last_block);
#ifndef AE_USE_CPP_ERROR_HANDLING
    state->break_jump = NULL;
#endif
    state->error_msg = "";
    
    /*
     * determine endianness and initialize precomputed IEEE special quantities.
     */
    state->endianness = ae_get_endianness();
    if( state->endianness==AE_LITTLE_ENDIAN )
    {
        vp = (ae_int32_t*)(&state->v_nan);
        vp[0] = 0;
        vp[1] = (ae_int32_t)0x7FF80000;
        vp = (ae_int32_t*)(&state->v_posinf);
        vp[0] = 0;
        vp[1] = (ae_int32_t)0x7FF00000;
        vp = (ae_int32_t*)(&state->v_neginf);
        vp[0] = 0;
        vp[1] = (ae_int32_t)0xFFF00000;
    }
    else if( state->endianness==AE_BIG_ENDIAN )
    {
        vp = (ae_int32_t*)(&state->v_nan);
        vp[1] = 0;
        vp[0] = (ae_int32_t)0x7FF80000;
        vp = (ae_int32_t*)(&state->v_posinf);
        vp[1] = 0;
        vp[0] = (ae_int32_t)0x7FF00000;
        vp = (ae_int32_t*)(&state->v_neginf);
        vp[1] = 0;
        vp[0] = (ae_int32_t)0xFFF00000;
    }
    else
        abort();
}

void ae_state_clear(ae_state *state)
{
    while( state->p_top_block->ptr!=DYN_BOTTOM )
        ae_frame_leave(state);
}

ae_bool ae_fp_eq(double v1, double v2)
{
    /* IEEE-strict floating point comparison */
    volatile double x = v1;
    volatile double y = v2;
    return x==y;
}

ae_bool ae_fp_neq(double v1, double v2)
{
    /* IEEE-strict floating point comparison */
    return !ae_fp_eq(v1,v2);
}

ae_bool ae_fp_less(double v1, double v2)
{
    /* IEEE-strict floating point comparison */
    volatile double x = v1;
    volatile double y = v2;
    return x<y;
}

ae_bool ae_fp_less_eq(double v1, double v2)
{
    /* IEEE-strict floating point comparison */
    volatile double x = v1;
    volatile double y = v2;
    return x<=y;
}

ae_bool ae_fp_greater(double v1, double v2)
{
    /* IEEE-strict floating point comparison */
    volatile double x = v1;
    volatile double y = v2;
    return x>y;
}

ae_bool ae_fp_greater_eq(double v1, double v2)
{
    /* IEEE-strict floating point comparison */
    volatile double x = v1;
    volatile double y = v2;
    return x>=y;
}

ae_bool ae_isfinite_stateless(double x, ae_int_t endianness)
{
    union _u
    {
        double a;
        ae_int32_t p[2];
    } u;
    ae_int32_t high;
    u.a = x;
    if( endianness==AE_LITTLE_ENDIAN )
        high = u.p[1];
    else
        high = u.p[0];
    return (high & (ae_int32_t)0x7FF00000)!=(ae_int32_t)0x7FF00000;
}

ae_bool ae_isnan_stateless(double x,    ae_int_t endianness)
{
    union _u
    {
        double a;
        ae_int32_t p[2];
    } u;
    ae_int32_t high, low;
    u.a = x;
    if( endianness==AE_LITTLE_ENDIAN )
    {
        high = u.p[1];
        low =  u.p[0];
    }
    else
    {
        high = u.p[0];
        low =  u.p[1];
    }
    return ((high &0x7FF00000)==0x7FF00000) && (((high &0x000FFFFF)!=0) || (low!=0));
}

ae_bool ae_isinf_stateless(double x,    ae_int_t endianness)
{
    union _u
    {
        double a;
        ae_int32_t p[2];
    } u;
    ae_int32_t high, low;
    u.a = x;
    if( endianness==AE_LITTLE_ENDIAN )
    {
        high = u.p[1];
        low  = u.p[0];
    }
    else
    {
        high = u.p[0];
        low  = u.p[1];
    }
    
    /* 31 least significant bits of high are compared */
    return ((high&0x7FFFFFFF)==0x7FF00000) && (low==0); 
}

ae_bool ae_isposinf_stateless(double x, ae_int_t endianness)
{
    union _u
    {
        double a;
        ae_int32_t p[2];
    } u;
    ae_int32_t high, low;
    u.a = x;
    if( endianness==AE_LITTLE_ENDIAN )
    {
        high = u.p[1];
        low  = u.p[0];
    }
    else
    {
        high = u.p[0];
        low  = u.p[1];
    }
    
    /* all 32 bits of high are compared */
    return (high==(ae_int32_t)0x7FF00000) && (low==0); 
}

ae_bool ae_isneginf_stateless(double x, ae_int_t endianness)
{
    union _u
    {
        double a;
        ae_int32_t p[2];
    } u;
    ae_int32_t high, low;
    u.a = x;
    if( endianness==AE_LITTLE_ENDIAN )
    {
        high = u.p[1];
        low  = u.p[0];
    }
    else
    {
        high = u.p[0];
        low  = u.p[1];
    }
    
    /* this code is a bit tricky to avoid comparison of high with 0xFFF00000, which may be unsafe with some buggy compilers */
    return ((high&0x7FFFFFFF)==0x7FF00000) && (high!=(ae_int32_t)0x7FF00000) && (low==0);
}

ae_int_t ae_get_endianness()
{
    union
    {
        double a;
        ae_int32_t p[2];
    } u;
    
    /*
     * determine endianness
     * two types are supported: big-endian and little-endian.
     * mixed-endian hardware is NOT supported.
     *
     * 1983 is used as magic number because its non-periodic double 
     * representation allow us to easily distinguish between upper 
     * and lower halfs and to detect mixed endian hardware.
     *
     */
    u.a = 1.0/1983.0; 
    if( u.p[1]==(ae_int32_t)0x3f408642 )
        return AE_LITTLE_ENDIAN;
    if( u.p[0]==(ae_int32_t)0x3f408642 )
        return AE_BIG_ENDIAN;
    return AE_MIXED_ENDIAN;
}

ae_bool ae_isfinite(double x,ae_state *state)
{
    return ae_isfinite_stateless(x, state->endianness);
}

ae_bool ae_isnan(double x,   ae_state *state)
{
    return ae_isnan_stateless(x, state->endianness);
}

ae_bool ae_isinf(double x,   ae_state *state)
{
    return ae_isinf_stateless(x, state->endianness);
}

ae_bool ae_isposinf(double x,ae_state *state)
{
    return ae_isposinf_stateless(x, state->endianness);
}

ae_bool ae_isneginf(double x,ae_state *state)
{
    return ae_isneginf_stateless(x, state->endianness);
}

double ae_fabs(double x,  ae_state *state)
{
    return fabs(x);
}

ae_int_t ae_iabs(ae_int_t x, ae_state *state)
{
    return x>=0 ? x : -x;
}

double ae_sqr(double x,  ae_state *state)
{
    return x*x;
}

double ae_sqrt(double x, ae_state *state)
{
    return sqrt(x);
}

ae_int_t ae_sign(double x, ae_state *state)
{
    if( x>0 ) return  1;
    if( x<0 ) return -1;
    return 0;
}

ae_int_t ae_round(double x, ae_state *state)
{
    return (ae_int_t)(ae_ifloor(x+0.5,state));
}

ae_int_t ae_trunc(double x, ae_state *state)
{
    return (ae_int_t)(x>0 ? ae_ifloor(x,state) : ae_iceil(x,state));
}

ae_int_t ae_ifloor(double x, ae_state *state)
{
    return (ae_int_t)(floor(x));
}

ae_int_t ae_iceil(double x,  ae_state *state)
{
    return (ae_int_t)(ceil(x));
}

ae_int_t ae_maxint(ae_int_t m1, ae_int_t m2, ae_state *state)
{
    return m1>m2 ? m1 : m2;
}

ae_int_t ae_minint(ae_int_t m1, ae_int_t m2, ae_state *state)
{
    return m1>m2 ? m2 : m1;
}

double ae_maxreal(double m1, double m2, ae_state *state)
{
    return m1>m2 ? m1 : m2;
}

double ae_minreal(double m1, double m2, ae_state *state)
{
    return m1>m2 ? m2 : m1;
}

ae_bool ae_vector_set_length(ae_vector *dst, ae_int_t newsize, ae_state *state)
{
    /* ensure that size is >=0
       two ways to exit: 1) through ae_assert, if we have non-NULL state, 2) by returning ae_false */
    if( state!=NULL )
        ae_assert(newsize>=0, "ae_vector_set_length(): negative size", state);
    if( newsize<0 )
        return ae_false;

    /* set length */
    if( dst->cnt==newsize )
        return ae_true;
    dst->cnt = newsize;
    if( !ae_db_realloc(&dst->data, newsize*ae_sizeof(dst->datatype), state) )
        return ae_false;
    dst->ptr.p_ptr = dst->data.ptr;
    return ae_true;
}

void ae_vector_clear(ae_vector *dst)
{
    dst->cnt = 0;
    ae_db_free(&dst->data);
    dst->ptr.p_ptr = 0;
}

void ae_assert(ae_bool cond, const char *msg, ae_state *state)
{
    if( !cond )
        ae_break(state, ERR_ASSERTION_FAILED, msg);
}

void ae_v_muld(double *vdst,  ae_int_t stride_dst, ae_int_t n, double alpha)
{
    ae_int_t i;
    if( stride_dst!=1 )
    {
        /*
         * general unoptimized case
         */
        for(i=0; i<n; i++, vdst+=stride_dst)
            *vdst *= alpha;
    }
    else
    {
        /*
         * highly optimized case
         */
        for(i=0; i<n; i++, vdst++)
            *vdst *= alpha;
    }
}

ae_int_t ae_v_len(ae_int_t a, ae_int_t b)
{
    return b-a+1;
}

ae_int_t ae_misalignment(const void *ptr, size_t alignment)
{
    union _u
    {
        const void *ptr;
        ae_int_t iptr;
    } u;
    u.ptr = ptr;
    return (ae_int_t)(u.iptr%alignment);
}

void* ae_align(void *ptr, size_t alignment)
{
    char *result = (char*)ptr;
    if( (result-(char*)0)%alignment!=0 )
        result += alignment - (result-(char*)0)%alignment;
    return result;
}

void ae_break(ae_state *state, ae_error_type error_type, const char *msg)
{
#ifndef AE_USE_CPP_ERROR_HANDLING
    if( state!=NULL )
    {
        ae_state_clear(state);
        state->last_error = error_type;
        state->error_msg = msg;
        if( state->break_jump!=NULL )
            longjmp(*(state->break_jump), 1);
        else
            abort();
    }
    else
        abort();
#else
    if( state!=NULL )
    {
        ae_state_clear(state);
        state->last_error = error_type;
        state->error_msg = msg;
    }
    throw error_type;
#endif
}

void* aligned_malloc(size_t size, size_t alignment)
{
    if( size==0 )
        return NULL;
    if( alignment<=1 )
    {
        /* no alignment, just call malloc */
        void *block;
        void **p; ;
        block = malloc(sizeof(void*)+size);
        if( block==NULL )
            return NULL;
        p = (void**)block;
        *p = block;
#ifdef AE_USE_ALLOC_COUNTER
        _alloc_counter++;
#endif
        return (void*)((char*)block+sizeof(void*));
    }
    else
    {
        /* align */
        void *block;
        char *result;
        block = malloc(alignment-1+sizeof(void*)+size);
        if( block==NULL )
            return NULL;
        result = (char*)block+sizeof(void*);
        /*if( (result-(char*)0)%alignment!=0 )
            result += alignment - (result-(char*)0)%alignment;*/
        result = (char*)ae_align(result, alignment);
        *((void**)(result-sizeof(void*))) = block;
#ifdef AE_USE_ALLOC_COUNTER
        _alloc_counter++;
#endif
        return result;
    }
}

void aligned_free(void *block)
{
    void *p;
    if( block==NULL )
        return;
    p = *((void**)((char*)block-sizeof(void*)));
    free(p);
#ifdef AE_USE_ALLOC_COUNTER
    _alloc_counter--;
#endif
}

void* ae_malloc(size_t size, ae_state *state)
{
    void *result;
    if( size==0 )
        return NULL;
    result = aligned_malloc(size,AE_DATA_ALIGN);
    if( result==NULL && state!=NULL)
    {
        char buf[256];
        sprintf(buf, "ae_malloc(): out of memory (attempted to allocate %llu bytes)", (unsigned long long)size);
        ae_break(state, ERR_OUT_OF_MEMORY, buf);
    }
    return result;
}

void ae_free(void *p)
{
    if( p!=NULL )
        aligned_free(p);
}

ae_int_t ae_sizeof(ae_datatype datatype)
{
    switch(datatype)
    {
        case DT_BOOL:       return (ae_int_t)sizeof(ae_bool);
        case DT_INT:        return (ae_int_t)sizeof(ae_int_t);
        case DT_REAL:       return (ae_int_t)sizeof(double);
        case DT_COMPLEX:    return 2*(ae_int_t)sizeof(double);
        default:            return 0;
    }
}

void ae_frame_make(ae_state *state, ae_frame *tmp)
{
    tmp->db_marker.p_next = state->p_top_block;
    tmp->db_marker.deallocator = NULL;
    tmp->db_marker.ptr = DYN_FRAME;
    state->p_top_block = &tmp->db_marker;
}

void ae_frame_leave(ae_state *state)
{
    while( state->p_top_block->ptr!=DYN_FRAME && state->p_top_block->ptr!=DYN_BOTTOM)
    {
        if( state->p_top_block->ptr!=NULL && state->p_top_block->deallocator!=NULL)
            ((ae_deallocator)(state->p_top_block->deallocator))(state->p_top_block->ptr);
        state->p_top_block = state->p_top_block->p_next;
    }
    state->p_top_block = state->p_top_block->p_next;
}

void ae_db_attach(ae_dyn_block *block, ae_state *state)
{
    block->p_next = state->p_top_block;
    state->p_top_block = block;
}

ae_bool ae_db_malloc(ae_dyn_block *block, ae_int_t size, ae_state *state, ae_bool make_automatic)
{
    /* ensure that size is >=0
       two ways to exit: 1) through ae_assert, if we have non-NULL state, 2) by returning ae_false */
    if( state!=NULL )
        ae_assert(size>=0, "ae_db_malloc(): negative size", state);
    if( size<0 )
        return ae_false;
    
    /* alloc */
    block->ptr = ae_malloc((size_t)size, state);
    if( block->ptr==NULL && size!=0 )
        return ae_false;
    if( make_automatic && state!=NULL )
        ae_db_attach(block, state);
    else
        block->p_next = NULL;
    block->deallocator = ae_free;
    return ae_true;
}

ae_bool ae_db_realloc(ae_dyn_block *block, ae_int_t size, ae_state *state)
{
    /* ensure that size is >=0
       two ways to exit: 1) through ae_assert, if we have non-NULL state, 2) by returning ae_false */
    if( state!=NULL )
        ae_assert(size>=0, "ae_db_realloc(): negative size", state);
    if( size<0 )
        return ae_false;
    
    /* realloc */
    if( block->ptr!=NULL )
        ((ae_deallocator)block->deallocator)(block->ptr);
    block->ptr = ae_malloc((size_t)size, state);
    if( block->ptr==NULL && size!=0 )
        return ae_false;
    block->deallocator = ae_free;
    return ae_true;
}

void ae_db_free(ae_dyn_block *block)
{
    if( block->ptr!=NULL )
        ((ae_deallocator)block->deallocator)(block->ptr);
    block->ptr = NULL;
    block->deallocator = ae_free;
}

ae_bool ae_vector_init(ae_vector *dst, ae_int_t size, ae_datatype datatype, ae_state *state, ae_bool make_automatic)
{
    /* ensure that size is >=0
       two ways to exit: 1) through ae_assert, if we have non-NULL state, 2) by returning ae_false */
    if( state!=NULL )
        ae_assert(size>=0, "ae_vector_init(): negative size", state);
    if( size<0 )
        return ae_false;

    /* init */
    dst->cnt = size;
    dst->datatype = datatype;
    if( !ae_db_malloc(&dst->data, size*ae_sizeof(datatype), state, make_automatic) )
        return ae_false;
    dst->ptr.p_ptr = dst->data.ptr;
    return ae_true;
}

ae_bool ae_vector_init_copy(ae_vector *dst, ae_vector *src, ae_state *state, ae_bool make_automatic)
{
    if( !ae_vector_init(dst, src->cnt, src->datatype, state, make_automatic) )
        return ae_false;
    if( src->cnt!=0 )
        memcpy(dst->ptr.p_ptr, src->ptr.p_ptr, (size_t)(src->cnt*ae_sizeof(src->datatype)));
    return ae_true;
}

static ae_int_t _ae_char2sixbits_tbl[] = {
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, 62, -1, -1,
     0,  1,  2,  3,  4,  5,  6,  7,
     8,  9, -1, -1, -1, -1, -1, -1,
    -1, 10, 11, 12, 13, 14, 15, 16,
    17, 18, 19, 20, 21, 22, 23, 24,
    25, 26, 27, 28, 29, 30, 31, 32,
    33, 34, 35, -1, -1, -1, -1, 63,
    -1, 36, 37, 38, 39, 40, 41, 42,
    43, 44, 45, 46, 47, 48, 49, 50,
    51, 52, 53, 54, 55, 56, 57, 58,
    59, 60, 61, -1, -1, -1, -1, -1 };
ae_int_t ae_char2sixbits(char c)
{
    return (c>=0 && c<127) ? _ae_char2sixbits_tbl[(unsigned) c] : -1;
}

void ae_foursixbits2threebytes(const ae_int_t *src, unsigned char *dst)
{
    dst[0] = (unsigned char)(     src[0] | ((src[1]&0x03)<<6));
    dst[1] = (unsigned char)((src[1]>>2) | ((src[2]&0x0F)<<4));
    dst[2] = (unsigned char)((src[2]>>4) | (src[3]<<2));
}

ae_int_t ae_str2int(const char *buf, ae_state *state, const char **pasttheend)
{
    const char *emsg = "ALGLIB: unable to read integer value from stream";
    ae_int_t sixbits[12];
    ae_int_t sixbitsread, i;
    union _u
    {
        ae_int_t ival;
        unsigned char bytes[9];
    } u;
    /* 
     * 1. skip leading spaces
     * 2. read and decode six-bit digits
     * 3. set trailing digits to zeros
     * 4. convert to little endian 64-bit integer representation
     * 5. convert to big endian representation, if needed
     */
    while( *buf==' ' || *buf=='\t' || *buf=='\n' || *buf=='\r' )
        buf++;
    sixbitsread = 0;
    while( *buf!=' ' && *buf!='\t' && *buf!='\n' && *buf!='\r' && *buf!=0 )
    {
        ae_int_t d;
        d = ae_char2sixbits(*buf);
        if( d<0 || sixbitsread>=AE_SER_ENTRY_LENGTH )
            ae_break(state, ERR_ASSERTION_FAILED, emsg);
        sixbits[sixbitsread] = d;
        sixbitsread++;
        buf++;
    }
    *pasttheend = buf;
    if( sixbitsread==0 )
        ae_break(state, ERR_ASSERTION_FAILED, emsg);
    for(i=sixbitsread; i<12; i++)
        sixbits[i] = 0;
    ae_foursixbits2threebytes(sixbits+0, u.bytes+0);
    ae_foursixbits2threebytes(sixbits+4, u.bytes+3);
    ae_foursixbits2threebytes(sixbits+8, u.bytes+6);
    if( state->endianness==AE_BIG_ENDIAN )
    {
        for(i=0; i<(int) sizeof(ae_int_t)/2; i++)
        {
            unsigned char tc;
            tc = u.bytes[i];
            u.bytes[i] = u.bytes[sizeof(ae_int_t)-1-i];
            u.bytes[sizeof(ae_int_t)-1-i] = tc;
        }
    }
    return u.ival;
}

double ae_str2double(const char *buf, ae_state *state, const char **pasttheend)
{
    const char *emsg = "ALGLIB: unable to read double value from stream";
    ae_int_t sixbits[12];
    ae_int_t sixbitsread, i;
    union _u
    {
        double dval;
        unsigned char bytes[9];
    } u;
    
    
     /* 
      * skip leading spaces
      */
    while( *buf==' ' || *buf=='\t' || *buf=='\n' || *buf=='\r' )
        buf++;
      
    /*
     * Handle special cases
     */
    if( *buf=='.' )
    {
        const char *s_nan =    ".nan_______";
        const char *s_posinf = ".posinf____";
        const char *s_neginf = ".neginf____";
        if( strncmp(buf, s_nan, strlen(s_nan))==0 )
        {
            *pasttheend = buf+strlen(s_nan);
            return state->v_nan;
        }
        if( strncmp(buf, s_posinf, strlen(s_posinf))==0 )
        {
            *pasttheend = buf+strlen(s_posinf);
            return state->v_posinf;
        }
        if( strncmp(buf, s_neginf, strlen(s_neginf))==0 )
        {
            *pasttheend = buf+strlen(s_neginf);
            return state->v_neginf;
        }
        ae_break(state, ERR_ASSERTION_FAILED, emsg);
    }
    
    /* 
     * General case:
     * 1. read and decode six-bit digits
     * 2. check that all 11 digits were read
     * 3. set last 12th digit to zero (needed for simplicity of conversion)
     * 4. convert to 8 bytes
     * 5. convert to big endian representation, if needed
     */
    sixbitsread = 0;
    while( *buf!=' ' && *buf!='\t' && *buf!='\n' && *buf!='\r' && *buf!=0 )
    {
        ae_int_t d;
        d = ae_char2sixbits(*buf);
        if( d<0 || sixbitsread>=AE_SER_ENTRY_LENGTH )
            ae_break(state, ERR_ASSERTION_FAILED, emsg);
        sixbits[sixbitsread] = d;
        sixbitsread++;
        buf++;
    }
    *pasttheend = buf;
    if( sixbitsread!=AE_SER_ENTRY_LENGTH )
        ae_break(state, ERR_ASSERTION_FAILED, emsg);
    sixbits[AE_SER_ENTRY_LENGTH] = 0;
    ae_foursixbits2threebytes(sixbits+0, u.bytes+0);
    ae_foursixbits2threebytes(sixbits+4, u.bytes+3);
    ae_foursixbits2threebytes(sixbits+8, u.bytes+6);
    if( state->endianness==AE_BIG_ENDIAN )
    {
        for(i=0; i<(int) sizeof(double)/2; i++)
        {
            unsigned char tc;
            tc = u.bytes[i];
            u.bytes[i] = u.bytes[sizeof(double)-1-i];
            u.bytes[sizeof(double)-1-i] = tc;
        }
    }
    return u.dval;
}            

void ae_serializer_init(ae_serializer *serializer)
{
    serializer->mode = AE_SM_DEFAULT;
    serializer->entries_needed = 0;
    serializer->bytes_asked = 0;
}

void ae_serializer_clear(ae_serializer *serializer)
{
}

void ae_serializer_alloc_start(ae_serializer *serializer)
{
    serializer->entries_needed = 0;
    serializer->bytes_asked = 0;
    serializer->mode = AE_SM_ALLOC;
}

void ae_serializer_alloc_entry(ae_serializer *serializer)
{
    serializer->entries_needed++;
}

ae_int_t ae_serializer_get_alloc_size(ae_serializer *serializer)
{
    ae_int_t rows, lastrowsize, result;
    
    serializer->mode = AE_SM_READY2S;
    
    /* if no entries needes (degenerate case) */
    if( serializer->entries_needed==0 )
    {
        serializer->bytes_asked = 1;
        return serializer->bytes_asked;
    }
    
    /* non-degenerate case */
    rows = serializer->entries_needed/AE_SER_ENTRIES_PER_ROW;
    lastrowsize = AE_SER_ENTRIES_PER_ROW;
    if( serializer->entries_needed%AE_SER_ENTRIES_PER_ROW )
    {
        lastrowsize = serializer->entries_needed%AE_SER_ENTRIES_PER_ROW;
        rows++;
    }
    
    /* calculate result size */
    result  = ((rows-1)*AE_SER_ENTRIES_PER_ROW+lastrowsize)*AE_SER_ENTRY_LENGTH;
    result +=  (rows-1)*(AE_SER_ENTRIES_PER_ROW-1)+(lastrowsize-1);
    result += rows*2;
    serializer->bytes_asked = result;
    return result;
}

#ifdef AE_USE_CPP_SERIALIZATION
void ae_serializer_sstart_str(ae_serializer *serializer, std::string *buf)
{
    serializer->mode = AE_SM_TO_CPPSTRING;
    serializer->out_cppstr = buf;
    serializer->entries_saved = 0;
    serializer->bytes_written = 0;
}
#endif

#ifdef AE_USE_CPP_SERIALIZATION
void ae_serializer_ustart_str(ae_serializer *serializer, const std::string *buf)
{
    serializer->mode = AE_SM_FROM_STRING;
    serializer->in_str = buf->c_str();
}
#endif

void ae_serializer_sstart_str(ae_serializer *serializer, char *buf)
{
    serializer->mode = AE_SM_TO_STRING;
    serializer->out_str = buf;
    serializer->out_str[0] = 0;
    serializer->entries_saved = 0;
    serializer->bytes_written = 0;
}

void ae_serializer_ustart_str(ae_serializer *serializer, const char *buf)
{
    serializer->mode = AE_SM_FROM_STRING;
    serializer->in_str = buf;
}

void ae_serializer_unserialize_int(ae_serializer *serializer, ae_int_t *v, ae_state *state)
{
    *v = ae_str2int(serializer->in_str, state, &serializer->in_str);
}

void ae_serializer_unserialize_double(ae_serializer *serializer, double *v, ae_state *state)
{
    *v = ae_str2double(serializer->in_str, state, &serializer->in_str);
}

void ae_serializer_stop(ae_serializer *serializer)
{
}

ae_int_t getrdfserializationcode(ae_state *_state)
{
    ae_int_t result;


    result = 1;
    return result;
}

void unserializerealarray(ae_serializer* s,
     /* Real    */ ae_vector* v,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t i;
    double t;

    ae_vector_clear(v);

    ae_serializer_unserialize_int(s, &n, _state);
    if( n==0 )
    {
        return;
    }
    ae_vector_set_length(v, n, _state);
    for(i=0; i<=n-1; i++)
    {
        ae_serializer_unserialize_double(s, &t, _state);
        v->ptr.p_double[i] = t;
    }
}

typedef struct
{
    ae_vector treebuf;
    ae_vector idxbuf;
    ae_vector tmpbufr;
    ae_vector tmpbufr2;
    ae_vector tmpbufi;
    ae_vector classibuf;
    ae_vector sortrbuf;
    ae_vector sortrbuf2;
    ae_vector sortibuf;
    ae_vector varpool;
    ae_vector evsbin;
    ae_vector evssplits;
} dfinternalbuffers;

typedef struct
{
    ae_int_t nvars;
    ae_int_t nclasses;
    ae_int_t ntrees;
    ae_int_t bufsize;
    ae_vector trees;
} decisionforest;

static ae_int_t dforest_innernodewidth = 3;
static ae_int_t dforest_dffirstversion = 0;
static void dforest_dfprocessinternal(decisionforest* df,
     ae_int_t offs,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state);

ae_bool _decisionforest_init(decisionforest* p, ae_state *_state, ae_bool make_automatic)
{
    if( !ae_vector_init(&p->trees, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    return ae_true;
}

ae_bool _decisionforest_init_copy(decisionforest* dst, decisionforest* src, ae_state *_state, ae_bool make_automatic)
{
    dst->nvars = src->nvars;
    dst->nclasses = src->nclasses;
    dst->ntrees = src->ntrees;
    dst->bufsize = src->bufsize;
    if( !ae_vector_init_copy(&dst->trees, &src->trees, _state, make_automatic) )
        return ae_false;
    return ae_true;
}

void _decisionforest_clear(decisionforest* p)
{
    ae_vector_clear(&p->trees);
}

void dfunserialize(ae_serializer* s,
     decisionforest* forest,
     ae_state *_state)
{
    ae_int_t i0;
    ae_int_t i1;

    _decisionforest_clear(forest);

    
    /*
     * check correctness of header
     */
    ae_serializer_unserialize_int(s, &i0, _state);
    ae_assert(i0==getrdfserializationcode(_state), "DFUnserialize: stream header corrupted", _state);
    ae_serializer_unserialize_int(s, &i1, _state);
    ae_assert(i1==dforest_dffirstversion, "DFUnserialize: stream header corrupted", _state);
    
    /*
     * Unserialize data
     */
    ae_serializer_unserialize_int(s, &forest->nvars, _state);
    ae_serializer_unserialize_int(s, &forest->nclasses, _state);
    ae_serializer_unserialize_int(s, &forest->ntrees, _state);
    ae_serializer_unserialize_int(s, &forest->bufsize, _state);
    unserializerealarray(s, &forest->trees, _state);
}
static void dforest_dfprocessinternal(decisionforest* df,
     ae_int_t offs,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state)
{
    ae_int_t k;
    ae_int_t idx;


    
    /*
     * Set pointer to the root
     */
    k = offs+1;
    
    /*
     * Navigate through the tree
     */
    for(;;)
    {
        if( ae_fp_eq(df->trees.ptr.p_double[k],-1) )
        {
            if( df->nclasses==1 )
            {
                y->ptr.p_double[0] = y->ptr.p_double[0]+df->trees.ptr.p_double[k+1];
            }
            else
            {
                idx = ae_round(df->trees.ptr.p_double[k+1], _state);
                y->ptr.p_double[idx] = y->ptr.p_double[idx]+1;
            }
            break;
        }
        if( ae_fp_less(x->ptr.p_double[ae_round(df->trees.ptr.p_double[k], _state)],df->trees.ptr.p_double[k+1]) )
        {
            k = k+dforest_innernodewidth;
        }
        else
        {
            k = offs+ae_round(df->trees.ptr.p_double[k+2], _state);
        }
    }
}
void dfprocess(decisionforest* df,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state)
{
    ae_int_t offs;
    ae_int_t i;
    double v;


    
    /*
     * Proceed
     */
    if( y->cnt<df->nclasses )
    {
        ae_vector_set_length(y, df->nclasses, _state);
    }
    offs = 0;
    for(i=0; i<=df->nclasses-1; i++)
    {
        y->ptr.p_double[i] = 0;
    }
    for(i=0; i<=df->ntrees-1; i++)
    {
        
        /*
         * Process basic tree
         */
        dforest_dfprocessinternal(df, offs, x, y, _state);
        
        /*
         * Next tree
         */
        offs = offs+ae_round(df->trees.ptr.p_double[offs], _state);
    }
    v = (double)1/(double)df->ntrees;
    ae_v_muld(&y->ptr.p_double[0], 1, ae_v_len(0,df->nclasses-1), v);
}  
}
// ================================================================ alglib
namespace alglib {
typedef alglib_impl::ae_int_t ae_int_t;

double get_aenv_nan();
double get_aenv_posinf();
double get_aenv_neginf();
ae_int_t my_stricmp(const char *s1, const char *s2);
char* filter_spaces(const char *s);

std::string arraytostring(const bool *ptr, ae_int_t n);
std::string arraytostring(const ae_int_t *ptr, ae_int_t n);
std::string arraytostring(const double *ptr, ae_int_t n, int dps);

const double machineepsilon = 5E-16;
const double maxrealnumber  = 1E300;
const double minrealnumber  = 1E-300;
const ae_int_t endianness   =  alglib_impl::ae_get_endianness();
const double fp_nan         =  alglib::get_aenv_nan();
const double fp_posinf      =  alglib::get_aenv_posinf();
const double fp_neginf      =  alglib::get_aenv_neginf();

class ap_error
{
public:
    std::string msg;
    
    ap_error();
    ap_error(const char *s);
    static void make_assertion(bool bClause);
    static void make_assertion(bool bClause, const char *msg);
private:
};
ap_error::ap_error()
{
}

ap_error::ap_error(const char *s)
{
    msg = s; 
}

void ap_error::make_assertion(bool bClause)
{
    if(!bClause) 
        throw ap_error(); 
}

void ap_error::make_assertion(bool bClause, const char *msg)
{ 
    if(!bClause) 
        throw ap_error(msg); 
}
class ae_vector_wrapper
{
public:
    ae_vector_wrapper();
    virtual ~ae_vector_wrapper();
    ae_vector_wrapper(const ae_vector_wrapper &rhs);
    const ae_vector_wrapper& operator=(const ae_vector_wrapper &rhs);

    void setlength(ae_int_t iLen);
    ae_int_t length() const;

    void attach_to(alglib_impl::ae_vector *ptr);
    void allocate_own(ae_int_t size, alglib_impl::ae_datatype datatype);
    const alglib_impl::ae_vector* c_ptr() const;
    alglib_impl::ae_vector* c_ptr();
protected:
    alglib_impl::ae_vector *p_vec;
    alglib_impl::ae_vector vec;
};  
class real_1d_array : public ae_vector_wrapper
{
public:
    real_1d_array();
    real_1d_array(alglib_impl::ae_vector *p);
    real_1d_array(const char *s);
    virtual ~real_1d_array();

    const double& operator()(ae_int_t i) const;
    double& operator()(ae_int_t i);

    const double& operator[](ae_int_t i) const;
    double& operator[](ae_int_t i);

    void setcontent(ae_int_t iLen, const double *pContent );
    double* getcontent();
    const double* getcontent() const;

    std::string tostring(int dps) const;
};
ae_vector_wrapper::ae_vector_wrapper()
{
    p_vec = NULL;
}

ae_vector_wrapper::~ae_vector_wrapper()
{
    if( p_vec==&vec )
        ae_vector_clear(p_vec);
}

ae_vector_wrapper::ae_vector_wrapper(const alglib::ae_vector_wrapper &rhs)
{
    if( rhs.p_vec!=NULL )
    {
        p_vec = &vec;
        if( !ae_vector_init_copy(p_vec, rhs.p_vec, NULL, ae_false) )
            throw alglib::ap_error("ALGLIB: malloc error!");
    }
    else
        p_vec = NULL;
}

const ae_vector_wrapper& alglib::ae_vector_wrapper::operator=(const alglib::ae_vector_wrapper &rhs)
{
    if( this==&rhs )
        return *this;
    if( p_vec==&vec )
        ae_vector_clear(p_vec);
    if( rhs.p_vec!=NULL )
    {
        p_vec = &vec;
        if( !ae_vector_init_copy(p_vec, rhs.p_vec, NULL, ae_false) )
            throw alglib::ap_error("ALGLIB: malloc error!");
    }
    else
        p_vec = NULL;
    return *this;
}

void ae_vector_wrapper::setlength(ae_int_t iLen)
{
    if( p_vec==NULL )
        throw alglib::ap_error("ALGLIB: setlength() error, p_vec==NULL (array was not correctly initialized)");
    if( p_vec!=&vec )
        throw alglib::ap_error("ALGLIB: setlength() error, p_vec!=&vec (attempt to resize frozen array)");
    if( !ae_vector_set_length(p_vec, iLen, NULL) )
        throw alglib::ap_error("ALGLIB: malloc error");
}

ae_int_t ae_vector_wrapper::length() const
{
    if( p_vec==NULL )
        return 0;
    return p_vec->cnt;
}

void ae_vector_wrapper::attach_to(alglib_impl::ae_vector *ptr)
{
    if( ptr==&vec )
        throw alglib::ap_error("ALGLIB: attempt to attach vector to itself");
    if( p_vec==&vec )
        ae_vector_clear(p_vec);
    p_vec = ptr;
}

void ae_vector_wrapper::allocate_own(ae_int_t size, alglib_impl::ae_datatype datatype)
{
    if( p_vec==&vec )
        ae_vector_clear(p_vec);
    p_vec = &vec;
    if( !ae_vector_init(p_vec, size, datatype, NULL, false) )
        throw alglib::ap_error("ALGLIB: malloc error");
}
const alglib_impl::ae_vector* ae_vector_wrapper::c_ptr() const
{
    return p_vec;
}

alglib_impl::ae_vector* ae_vector_wrapper::c_ptr()
{
    return p_vec;
}
double get_aenv_nan()
{
    double r;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    r = _alglib_env_state.v_nan;
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return r;
}

double get_aenv_posinf()
{
    double r;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    r = _alglib_env_state.v_posinf;
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return r;
}

double get_aenv_neginf()
{
    double r;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    r = _alglib_env_state.v_neginf;
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return r;
}
alglib::ae_int_t my_stricmp(const char *s1, const char *s2)
{
    int c1, c2;
    
    //
    // handle special cases
    //
    if(s1==NULL && s2!=NULL)
        return -1;
    if(s1!=NULL && s2==NULL)
        return +1;
    if(s1==NULL && s2==NULL)
        return 0;

    //
    // compare
    //
    for (;;)
    {
        c1 = *s1;
        c2 = *s2;
        s1++;
        s2++;
        if( c1==0 )
            return c2==0 ? 0 : -1;
        if( c2==0 )
            return c1==0 ? 0 : +1;
        c1 = tolower(c1);
        c2 = tolower(c2);
        if( c1<c2 )
            return -1;
        if( c1>c2 )
            return +1;
    }
}
char* filter_spaces(const char *s)
{
    size_t i, j, n;
    char *r;
    char *r0;
    n = strlen(s);
    r = (char*)alglib_impl::ae_malloc(n+1, NULL);
    if( r==NULL )
        throw ap_error("malloc error");
    for(i=0,j=0,r0=r; i<=n; i++,s++)
        if( !isspace(*s) )
        {
            *r0 = *s;
            r0++;
        }
    return r;
}
void str_vector_create(const char *src, bool match_head_only, std::vector<const char*> *p_vec)
{
    //
    // parse beginning of the string.
    // try to handle "[]" string
    //
    p_vec->clear();
    if( *src!='[' )
        throw alglib::ap_error("Incorrect initializer for vector");
    src++;
    if( *src==']' )
        return;
    p_vec->push_back(src);
    for(;;)
    {
        if( *src==0 )
            throw alglib::ap_error("Incorrect initializer for vector");
        if( *src==']' )
        {
            if( src[1]==0 || !match_head_only)
                return;
            throw alglib::ap_error("Incorrect initializer for vector");
        }
        if( *src==',' )
        {
            p_vec->push_back(src+1);
            src++;
            continue;
        }
        src++;
    }
}
bool _parse_real_delim(const char *s, const char *delim, double *result, const char **new_s)
{
    const char *p;
    char *t;
    bool has_digits;
    char buf[64];
    int  isign;
    lconv *loc;

    p = s;
    
    //
    // check string structure and decide what to do
    //
    isign = 1;
    if( *s=='-' || *s=='+' )
    {
        isign = *s=='-' ? -1 : +1;
        s++;
    }
    memset(buf, 0, sizeof(buf));
    strncpy(buf, s, 3);
    if( my_stricmp(buf,"nan")!=0 && my_stricmp(buf,"inf")!=0 )
    {
        //
        // [sign] [ddd] [.] [ddd] [e|E[sign]ddd]
        //
        has_digits = false;
        if( *s!=0 && strchr("1234567890",*s)!=NULL )
        {
            has_digits = true;
            while( *s!=0 && strchr("1234567890",*s)!=NULL )
                s++;
        }
        if( *s=='.' )
            s++;
        if( *s!=0 && strchr("1234567890",*s)!=NULL )
        {
            has_digits = true;
            while( *s!=0 && strchr("1234567890",*s)!=NULL )
                s++;
        }
        if (!has_digits )
            return false;
        if( *s=='e' || *s=='E' )
        {
            s++;
            if( *s=='-' || *s=='+' )
                s++;
            if( *s==0 || strchr("1234567890",*s)==NULL )
                return false;
            while( *s!=0 && strchr("1234567890",*s)!=NULL )
                s++;
        }   
        if( *s==0 || strchr(delim,*s)==NULL )
            return false;
        *new_s = s;

        //
        // finite value conversion
        //
        if( *new_s-p>=(int)sizeof(buf) )
            return false;
        strncpy(buf, p, (size_t)(*new_s-p));
        buf[*new_s-p] = 0;
        loc = localeconv();
        t = strchr(buf,'.');
        if( t!=NULL )
            *t = *loc->decimal_point;
        *result = atof(buf);
        return true;
    }
    else
    {
        //
        // check delimiter and update *new_s
        //
        s += 3;
        if( *s==0 || strchr(delim,*s)==NULL )
            return false;
        *new_s = s;

        //
        // NAN, INF conversion
        //
        if( my_stricmp(buf,"nan")==0 )
            *result = fp_nan;
        if( my_stricmp(buf,"inf")==0 )
            *result = isign>0 ? fp_posinf : fp_neginf;
        return true;
    }
}

double parse_real_delim(const char *s, const char *delim)
{
    double result;
    const char *new_s;
    if( !_parse_real_delim(s, delim, &result, &new_s) )
        throw alglib::ap_error("Cannot parse value");
    return result;
}

int round(double x)
{ return int(floor(x+0.5)); }

int trunc(double x)
{ return int(x>0 ? floor(x) : ceil(x)); }

int ifloor(double x)
{ return int(floor(x)); }

int iceil(double x)
{ return int(ceil(x)); }

double pi()
{ return 3.14159265358979323846; }

double sqr(double x)
{ return x*x; }

int maxint(int m1, int m2)
{
    return m1>m2 ? m1 : m2;
}

int minint(int m1, int m2)
{
    return m1>m2 ? m2 : m1;
}

double maxreal(double m1, double m2)
{
    return m1>m2 ? m1 : m2;
}

double minreal(double m1, double m2)
{
    return m1>m2 ? m2 : m1;
}

bool fp_eq(double v1, double v2)
{
    // IEEE-strict floating point comparison
    volatile double x = v1;
    volatile double y = v2;
    return x==y;
}

bool fp_neq(double v1, double v2)
{
    // IEEE-strict floating point comparison
    return !fp_eq(v1,v2);
}

bool fp_less(double v1, double v2)
{
    // IEEE-strict floating point comparison
    volatile double x = v1;
    volatile double y = v2;
    return x<y;
}

bool fp_less_eq(double v1, double v2)
{
    // IEEE-strict floating point comparison
    volatile double x = v1;
    volatile double y = v2;
    return x<=y;
}

bool fp_greater(double v1, double v2)
{
    // IEEE-strict floating point comparison
    volatile double x = v1;
    volatile double y = v2;
    return x>y;
}

bool fp_greater_eq(double v1, double v2)
{
    // IEEE-strict floating point comparison
    volatile double x = v1;
    volatile double y = v2;
    return x>=y;
}

bool fp_isnan(double x)
{
    return alglib_impl::ae_isnan_stateless(x,endianness);
}

bool fp_isposinf(double x)
{
    return alglib_impl::ae_isposinf_stateless(x,endianness);
}

bool fp_isneginf(double x)
{
    return alglib_impl::ae_isneginf_stateless(x,endianness);
}

bool fp_isinf(double x)
{
    return alglib_impl::ae_isinf_stateless(x,endianness);
}

bool fp_isfinite(double x)
{
    return alglib_impl::ae_isfinite_stateless(x,endianness);
}

std::string arraytostring(const bool *ptr, ae_int_t n)
{
    std::string result;
    ae_int_t i;
    result = "[";
    for(i=0; i<n; i++)
    {
        if( i!=0 )
            result += ",";
        result += ptr[i] ? "true" : "false";
    }
    result += "]";
    return result;
}

std::string arraytostring(const ae_int_t *ptr, ae_int_t n)
{
    std::string result;
    ae_int_t i;
    char buf[64];
    result = "[";
    for(i=0; i<n; i++)
    {
        if( sprintf(buf, i==0 ? "%ld" : ",%ld", long(ptr[i]))>=(int)sizeof(buf) )
            throw ap_error("arraytostring(): buffer overflow");
        result += buf;
    }
    result += "]";
    return result;
}

std::string arraytostring(const double *ptr, ae_int_t n, int _dps)
{
    std::string result;
    ae_int_t i;
    char buf[64];
    char mask1[64];
    char mask2[64];
    int dps = _dps>=0 ? _dps : -_dps;
    result = "[";
    if( sprintf(mask1, "%%.%d%s", dps, _dps>=0 ? "f" : "e")>=(int)sizeof(mask1) )
        throw ap_error("arraytostring(): buffer overflow");
    if( sprintf(mask2, ",%s", mask1)>=(int)sizeof(mask2) )
        throw ap_error("arraytostring(): buffer overflow");
    for(i=0; i<n; i++)
    {
        buf[0] = 0;
        if( fp_isfinite(ptr[i]) )
        {
            if( sprintf(buf, i==0 ? mask1 : mask2, double(ptr[i]))>=(int)sizeof(buf) )
                throw ap_error("arraytostring(): buffer overflow");
        }
        else if( fp_isnan(ptr[i]) )
            strcpy(buf, i==0 ?  "NAN" :  ",NAN");
        else if( fp_isposinf(ptr[i]) )
            strcpy(buf, i==0 ? "+INF" : ",+INF");
        else if( fp_isneginf(ptr[i]) )
            strcpy(buf, i==0 ? "-INF" : ",-INF");
        result += buf;
    }
    result += "]";
    return result;
}

real_1d_array::real_1d_array()  
{
    allocate_own(0, alglib_impl::DT_REAL);
}

real_1d_array::real_1d_array(alglib_impl::ae_vector *p)  
{
    p_vec = NULL;
    attach_to(p);
}

real_1d_array::real_1d_array(const char *s)  
{
    std::vector<const char*> svec;
    size_t i;
    char *p = filter_spaces(s);
    try
    {
        str_vector_create(p, true, &svec);
        allocate_own((ae_int_t)(svec.size()), alglib_impl::DT_REAL);
        for(i=0; i<svec.size(); i++)
            operator()((ae_int_t)i) = parse_real_delim(svec[i],",]");
        alglib_impl::ae_free(p);
    }
    catch(...)
    {
        alglib_impl::ae_free(p);
        throw;
    }
}

real_1d_array::~real_1d_array() 
{
}

const double& real_1d_array::operator()(ae_int_t i) const
{
    return p_vec->ptr.p_double[i];
}

double& real_1d_array::operator()(ae_int_t i)
{
    return p_vec->ptr.p_double[i];
}

const double& real_1d_array::operator[](ae_int_t i) const
{
    return p_vec->ptr.p_double[i];
}

double& real_1d_array::operator[](ae_int_t i)
{
    return p_vec->ptr.p_double[i];
}

void real_1d_array::setcontent(ae_int_t iLen, const double *pContent )
{
    ae_int_t i;
    setlength(iLen);
    for(i=0; i<iLen; i++)
        p_vec->ptr.p_double[i] = pContent[i];
}

double* real_1d_array::getcontent()
{
    return p_vec->ptr.p_double;
}

const double* real_1d_array::getcontent() const
{
    return p_vec->ptr.p_double;
}
std::string real_1d_array::tostring(int dps) const 
{
    if( length()==0 )
        return "[]";
    return arraytostring(&operator()(0), length(), dps);
}

class _decisionforest_owner
{
public:
    _decisionforest_owner();
    _decisionforest_owner(const _decisionforest_owner &rhs);
    _decisionforest_owner& operator=(const _decisionforest_owner &rhs);
    virtual ~_decisionforest_owner();
    alglib_impl::decisionforest* c_ptr();
    alglib_impl::decisionforest* c_ptr() const;
protected:
    alglib_impl::decisionforest *p_struct;
};
_decisionforest_owner::_decisionforest_owner()
{
    p_struct = (alglib_impl::decisionforest*)alglib_impl::ae_malloc(sizeof(alglib_impl::decisionforest), NULL);
    if( p_struct==NULL )
        throw ap_error("ALGLIB: malloc error");
    if( !alglib_impl::_decisionforest_init(p_struct, NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
}

_decisionforest_owner::_decisionforest_owner(const _decisionforest_owner &rhs)
{
    p_struct = (alglib_impl::decisionforest*)alglib_impl::ae_malloc(sizeof(alglib_impl::decisionforest), NULL);
    if( p_struct==NULL )
        throw ap_error("ALGLIB: malloc error");
    if( !alglib_impl::_decisionforest_init_copy(p_struct, const_cast<alglib_impl::decisionforest*>(rhs.p_struct), NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
}

_decisionforest_owner& _decisionforest_owner::operator=(const _decisionforest_owner &rhs)
{
    if( this==&rhs )
        return *this;
    alglib_impl::_decisionforest_clear(p_struct);
    if( !alglib_impl::_decisionforest_init_copy(p_struct, const_cast<alglib_impl::decisionforest*>(rhs.p_struct), NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
    return *this;
}

_decisionforest_owner::~_decisionforest_owner()
{
    alglib_impl::_decisionforest_clear(p_struct);
    ae_free(p_struct);
}

alglib_impl::decisionforest* _decisionforest_owner::c_ptr()
{
    return p_struct;
}

alglib_impl::decisionforest* _decisionforest_owner::c_ptr() const
{
    return const_cast<alglib_impl::decisionforest*>(p_struct);
}

class decisionforest : public _decisionforest_owner
{
public:
    decisionforest();
    decisionforest(const decisionforest &rhs);
    decisionforest& operator=(const decisionforest &rhs);
    virtual ~decisionforest();

};

decisionforest::decisionforest() : _decisionforest_owner() 
{
}

decisionforest::decisionforest(const decisionforest &rhs):_decisionforest_owner(rhs) 
{
}

decisionforest& decisionforest::operator=(const decisionforest &rhs)
{
    if( this==&rhs )
        return *this;
    _decisionforest_owner::operator=(rhs);
    return *this;
}

decisionforest::~decisionforest()
{
}

void dfunserialize(std::string &s_in, decisionforest &obj)
{
    alglib_impl::ae_state state;
    alglib_impl::ae_serializer serializer;

    alglib_impl::ae_state_init(&state);
    try
    {
        alglib_impl::ae_serializer_init(&serializer);
        alglib_impl::ae_serializer_ustart_str(&serializer, &s_in);
        alglib_impl::dfunserialize(&serializer, obj.c_ptr(), &state);
        alglib_impl::ae_serializer_stop(&serializer);
        alglib_impl::ae_serializer_clear(&serializer);
        alglib_impl::ae_state_clear(&state);
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(state.error_msg);
    }
    catch(...)
    {
        throw;
    }
}
void dfprocess(const decisionforest &df, const real_1d_array &x, real_1d_array &y)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::dfprocess(const_cast<alglib_impl::decisionforest*>(df.c_ptr()), const_cast<alglib_impl::ae_vector*>(x.c_ptr()), const_cast<alglib_impl::ae_vector*>(y.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
    catch(...)
    {
        throw;
    }
}
}
// ================================================================
using namespace std;
using namespace alglib;

#define all(c) (c).begin(), (c).end()
#define iter(c) __typeof((c).begin())
#define cpresent(c, e) (find(all(c), (e)) != (c).end())
#define rep(i, n) for (int i = 0; i < (int)(n); i++)
#define tr(c, i) for (iter(c) i = (c).begin(); i != (c).end(); ++i)
#define pb(e) push_back(e)
#define mp(a, b) make_pair(a, b)

extern const char *dfdump_050_1[];

void usage(const char *prog) {
  printf("%s train target\n", prog);
  exit(-1);
}

int main(int argc, char *argv[]) {
  decisionforest df; string dfs;
  for (int i=0; dfdump_050_1[i]; ++i) dfs.append(dfdump_050_1[i]);
  dfunserialize(dfs, df);
  if (argc != 3) usage(argv[0]);
  FILE *fp;
  if ((fp = fopen(argv[2], "r")) == NULL) {
    fprintf(stderr, "cannot open %s\n", argv[2]);
    return -1;
  }
  int v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17;
  while (fscanf(fp, " %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
                &v1, &v2, &v3, &v4, &v5, &v6, &v7, &v8, &v9, &v10, &v11, &v12, &v13, &v14, &v15, &v16, &v17) == 17) {
    if (v1 < 14) {
      printf("0\n");
      continue;
    }
    vector<double> sample;
    #define nominal(C,V) sample.pb((C)==(V) ? 1.0 : 0.0)
    sample.pb((double) v1);     // Age
    nominal(v2, 2);             // Make:Female
    nominal(v3, 1);             // White
    nominal(v3, 2);             // Black/Gegro
    nominal(v3, 3);             // American Indian
    nominal(v3, 4);             // Chinese
    nominal(v3, 5);             // Japanese
    nominal(v3, 6);             // Other Asian or Pacific
    nominal(v3, 7);             // Other race, nec
    nominal(v4, 1);             // Married, spouse present
    nominal(v4, 2);             // Married, spouse absent
    nominal(v4, 3);             // Separated
    nominal(v4, 4);             // Divorced
    nominal(v4, 5);             // Widowed
    nominal(v4, 6);             // Never married/single
    sample.pb((double) v5);     // # of children
    nominal(v6, 1);             // Alabama
    nominal(v6, 2);             // Alaska
    nominal(v6, 4);             // Arizona
    nominal(v6, 5);             // Arkansas
    nominal(v6, 6);             // California
    nominal(v6, 8);             // Colorado
    nominal(v6, 9);             // Conneticut
    nominal(v6, 10);            // Delaware
    nominal(v6, 11);            // District of Columbia
    nominal(v6, 12);            // Florida
    nominal(v6, 13);            // Geogia
    nominal(v6, 15);            // Hawaii
    nominal(v6, 16);            // Idaho
    nominal(v6, 17);            // Illinois
    nominal(v6, 18);            // Indiana
    nominal(v6, 19);            // Iowa
    nominal(v6, 20);            // Kansas
    nominal(v6, 21);            // Kentucky
    nominal(v6, 22);            // Louisiana
    nominal(v6, 23);            // Maine
    nominal(v6, 24);            // Maryland
    nominal(v6, 25);            // Massachusetts
    nominal(v6, 26);            // Michigan
    nominal(v6, 27);            // Minnesota
    nominal(v6, 28);            // Mississippi
    nominal(v6, 29);            // Missouri
    nominal(v6, 30);            // Montana
    nominal(v6, 31);            // Nebraska
    nominal(v6, 32);            // Nevada
    nominal(v6, 33);            // New Hampshire
    nominal(v6, 34);            // New Jersey
    nominal(v6, 35);            // New Mexico
    nominal(v6, 36);            // New York
    nominal(v6, 37);            // North Carolina
    nominal(v6, 38);            // North Dakota
    nominal(v6, 39);            // Ohio
    nominal(v6, 40);            // Oklahoma
    nominal(v6, 41);            // Oregon
    nominal(v6, 42);            // Pennsylvania
    nominal(v6, 44);            // Rhode Island
    nominal(v6, 45);            // South Carolina
    nominal(v6, 46);            // South Dakota
    nominal(v6, 47);            // Tennessee
    nominal(v6, 48);            // Texas
    nominal(v6, 49);            // Utah
    nominal(v6, 50);            // Vermont
    nominal(v6, 51);            // Virginia
    nominal(v6, 53);            // Washington
    nominal(v6, 54);            // West Virginia
    nominal(v6, 55);            // Wyoming
    nominal(v6, 90);            // Native American
    nominal(v6, 99);            // United States, ns
    nominal(v6, 100);           // American Samoa
    nominal(v6, 105);           // Guam
    nominal(v6, 110);           // Puerto Rico
    nominal(v6, 115);           // U.S. Virgin Islands
    nominal(v6, 120);           // Other US Possesions
    nominal(v6, 150);           // Canada
    nominal(v6, 160);           // Atlantic Islands
    nominal(v6, 199);           // North America, ns
    nominal(v6, 200);           // Mexico
    nominal(v6, 210);           // Central America
    nominal(v6, 250);           // Cuba
    nominal(v6, 260);           // West Indies
    nominal(v6, 300);           // South America
    nominal(v6, 400);           // Denmark
    nominal(v6, 401);           // Finland
    nominal(v6, 402);           // Iceland
    nominal(v6, 403);           // Lapland
    nominal(v6, 404);           // Norway
    nominal(v6, 405);           // Sweden
    nominal(v6, 410);           // England
    nominal(v6, 411);           // Scotland
    nominal(v6, 412);           // Wales
    nominal(v6, 413);           // United Kingdom, ns
    nominal(v6, 414);           // Ireland
    nominal(v6, 419);           // Northern Europe, ns
    nominal(v6, 420);           // Belgium
    nominal(v6, 421);           // France
    nominal(v6, 422);           // Liechtenstein
    nominal(v6, 423);           // Luxembourg
    nominal(v6, 424);           // Monaco
    nominal(v6, 425);           // Netherlands
    nominal(v6, 426);           // Switerland
    nominal(v6, 429);           // Western Europe, ns
    nominal(v6, 430);           // Albania
    nominal(v6, 431);           // Andorra
    nominal(v6, 432);           // Gibraltar
    nominal(v6, 433);           // Greece
    nominal(v6, 434);           // Italy
    nominal(v6, 435);           // Malta
    nominal(v6, 436);           // Portugal
    nominal(v6, 437);           // San Marino
    nominal(v6, 438);           // Spain
    nominal(v6, 439);           // Vatican City
    nominal(v6, 440);           // Southern Europe, ns
    nominal(v6, 450);           // Austria
    nominal(v6, 451);           // Bulgaria
    nominal(v6, 452);           // Czechoslovakia
    nominal(v6, 453);           // Germany
    nominal(v6, 454);           // Hungary
    nominal(v6, 455);           // Poland
    nominal(v6, 456);           // Romania
    nominal(v6, 457);           // Yugoslavia
    nominal(v6, 458);           // Central Europe, ns
    nominal(v6, 459);           // Eastern Europe ns
    nominal(v6, 460);           // Estonia
    nominal(v6, 461);           // Latvia
    nominal(v6, 462);           // Lithuania
    nominal(v6, 463);           // Baltic States
    nominal(v6, 465);           // USSR/Russia
    nominal(v6, 499);           // Europe, ns
    nominal(v6, 500);           // China
    nominal(v6, 501);           // Japan
    nominal(v6, 502);           // Korea
    nominal(v6, 509);           // East Asia, ns
    nominal(v6, 510);           // Brunei
    nominal(v6, 511);           // Cambodia (Kampuchea)
    nominal(v6, 512);           // Indonesia
    nominal(v6, 513);           // Laos
    nominal(v6, 514);           // Malaysia
    nominal(v6, 515);           // Phillipines
    nominal(v6, 516);           // Singapore
    nominal(v6, 517);           // Thailand
    nominal(v6, 518);           // Vietnam
    nominal(v6, 519);           // Southeast Asia, ns
    nominal(v6, 520);           // Afganistan
    nominal(v6, 521);           // India
    nominal(v6, 522);           // Iran
    nominal(v6, 523);           // Maldives
    nominal(v6, 524);           // Nepal
    nominal(v6, 530);           // Bahrain
    nominal(v6, 531);           // Cyprus
    nominal(v6, 532);           // Iraq
    nominal(v6, 533);           // Iraq/Saudi Arabia
    nominal(v6, 534);           // Israel/Palestine
    nominal(v6, 535);           // Jordan
    nominal(v6, 536);           // Kuwait
    nominal(v6, 537);           // Lebanon
    nominal(v6, 538);           // Oman
    nominal(v6, 539);           // Qatar
    nominal(v6, 540);           // Saudi Arabia
    nominal(v6, 541);           // Syria
    nominal(v6, 542);           // Turkey
    nominal(v6, 543);           // United Arab Emirates
    nominal(v6, 544);           // Yemen Arab Republic (North)
    nominal(v6, 545);           // Yemen, PDR (South)
    nominal(v6, 546);           // Persian Gulf States, ns
    nominal(v6, 547);           // Middle East, ns
    nominal(v6, 548);           // Southwest Asia, nec/ns
    nominal(v6, 549);           // Asia Minor, ns
    nominal(v6, 550);           // SouthAsia, nec
    nominal(v6, 599);           // Asia, nec/ns
    nominal(v6, 600);           // Africa
    nominal(v6, 700);           // Australia and New Zealand
    nominal(v6, 710);           // Pacific Islands
    nominal(v6, 800);           // Antarctica, ns/nec
    nominal(v6, 900);           // Abroad (unknown) or at sea
    nominal(v6, 997);           // Missing/Unknown
    nominal(v6, 999);           // Unknown value
    //    nominal(v7, 2);             // Farmer
    nominal(v8, 0);             // N/A
    nominal(v8, 1);             // Owned or being bought (loan)
    nominal(v8, 2);             // Rented
    nominal(v9, 1);             // Head/Householder
    nominal(v9, 2);             // Spouse
    nominal(v9, 3);             // Child
    nominal(v9, 4);             // Child-in-law
    nominal(v9, 5);             // Parent
    nominal(v9, 6);             // Parent-in-Law
    nominal(v9, 7);             // Sibling
    nominal(v9, 8);             // Sibling-in-Law
    nominal(v9, 9);             // Grandchild
    nominal(v9, 10);            // Other relatives
    nominal(v9, 11);            // Partner, friend, visitor
    nominal(v9, 12);            // Other non-relatives
    nominal(v9, 13);            // Institutional inmates
    nominal(v10, 0);            // N/A
    nominal(v10, 1);            // No, not in school
    nominal(v10, 2);            // Yes, in school
    nominal(v11, 0);            // N/A
    nominal(v11, 1);            // Employed
    nominal(v11, 2);            // Unemployed
    nominal(v11, 3);            // Not in labor force
    nominal(v12, 0);            // N/A
    nominal(v12, 1);            // Self-employed
    nominal(v12, 2);            // Works for wages/salary
    nominal(v12, 3);            // New worker
    nominal(v12, 4);            // Unemployed, last worked 5 years ago
    sample.pb((double) v13);    // work weeks
    sample.pb((double) v14);    // work hours
    sample.pb((double) v15);    // worked since
    nominal(v16, 0);            // N/A
    nominal(v16, 1);            // Same house
    nominal(v16, 2);            // Moved, place not reported
    nominal(v16, 3);            // Same state/county, different house
    nominal(v16, 4);            // Same state, different county
    nominal(v16, 5);            // Different state
    nominal(v16, 6);            // Abroad
    nominal(v16, 7);            // Same state, place not reported
    nominal(v16, 9);            // Unknown
    nominal(v17, 0);            // N/A
    nominal(v17, 10);           // Auto, truck, or van
    nominal(v17, 11);           // Auto
    nominal(v17, 12);           // Driver
    nominal(v17, 13);           // Passenger
    nominal(v17, 14);           // Truck
    nominal(v17, 15);           // Van
    nominal(v17, 20);           // Motorcycle
    nominal(v17, 30);           // Bus or streetcar
    nominal(v17, 31);           // Bus or trolley bus
    nominal(v17, 32);           // Streetcar or trolley car
    nominal(v17, 33);           // Subway or elevated
    nominal(v17, 34);           // Railroad
    nominal(v17, 35);           // Taxicab
    nominal(v17, 36);           // Ferryboat
    nominal(v17, 40);           // Bicycle
    nominal(v17, 50);           // Walked only
    nominal(v17, 60);           // Other
    nominal(v17, 70);           // Worked at home
//    sample.pb((double) v18);    // Annual salary
    #undef nominal
    int cols = sample.size();
    real_1d_array x, y;
    x.setcontent(cols, &sample[0]);
    dfprocess(df, x, y);
    printf("%d\n", (int) ceil(y[0]));
  }
  fclose(fp);
  return 0;
}
