#ifndef __LAYER_INTERFACE_H__
#define __LAYER_INTERFACE_H__

#include <stdio.h>

/**
 * \file    layer_interface.h
 * \author  Olgierd Humenczuk
 * \brief   Containes the layer interface implementation as long as base structure declarations
 *          required by the layer to work.
 */

// forward declaration
struct __matrix_layer;

/**
 * \brief   layer states it simplifies the between layer communication
 * \note    This is part of the standarized protocol of communication that 
 *          has been designed in order to provide the minimum restrictions, 
 *          in order to make the usage safe, with maximum capabilities.
 */
typedef enum
{
    LAYER_STATE_OK = 0,
    LAYER_STATE_FULL,
    LAYER_STATE_TIMEOUT,
    LAYER_STATE_ERROR
} layer_state_t;


/**
 * \brief   allows to seek for the next call context 
 */
typedef struct layer_connectivity
{
    struct __matrix_layer* self;
    struct __matrix_layer* next;
    struct __matrix_layer* prev;
} layer_connectivity_t;

/**
 * \struct layer_interface_t
 * 
 * \brief   The raw interface of the purly raw layer that combines both: simplicity and functionality
 *     of the 'on demand processing' idea. 
 * 
 *              The design idea is based on the assumption that the communication with the server 
 *          is bidirectional and the data is processed in packages. This interface design is 
 *          very generic and allows to implement different approaches as an extension of the 
 *          basic operations that the interface is capable of. In very basic scenario the client 
 *          wants to send some number of bytes to the server and then awaits for the response. 
 *          After receiving the response the response is passed by to the processing layer through
 *          on_data_ready function which signals the processing layer that there is new data to process.
 *
 *              Term 'on demand processing' refers to the processing managed by the connection. 
 *          With the observation that all of the decision in the software depends on the network 
 *          capabilities it is clear that only the connection has ability to give the signals about 
 *          the processing to the rest of the components. The 'demand' takes it source in the 
 *          connection and that is the layer that has command over the another layers.
 */
typedef struct layer_interface
{
    /**
     * \brief the function that is called whenever the prev layer wants more data to process/send over some kind of the connection
     */
    layer_state_t ( *on_demand )( layer_connectivity_t* context, const char* buffer, size_t size );

    /**
     * \brief the function that is called whenever there is data that is ready and it's source is the prev layer
     */
    layer_state_t ( *on_data_ready )( layer_connectivity_t* context, char* buffer, size_t size );

    /**
     * \brief whenever the processing chain suppose to be closed
     */
    layer_state_t ( *close )( layer_connectivity_t* context );

    /**
     * \brief whenver the processing chain is going to be closed it's source is in the prev layer
     */
    layer_state_t ( *on_close )( layer_connectivity_t* context );

} layer_interface_t;

#ifdef __DEBUG
/**
 * \struct  __layer_debug_info
 * \brief   The structure holds information related to the layer debugging. Here the original place of initialization
 *          or connection is being stored so it's easier to determine what layer is it.
 */
typedef struct __layer_debug_info
{
    int                     debug_line_init;
    const char*             debug_file_init;

    int                     debug_line_connect;
    const char*             debug_file_connect;

    int                     debug_line_last_call;
    const char*             debug_file_last_call;
} __layer_debug_info_t;
#endif

/**
 * \brief The mirror class that makes the access to the generated types
 *          possible
 */
typedef struct __matrix_layer
{
    layer_interface_t       layer_functions;
    layer_connectivity_t    layer_connection;
    void*                   user_data;
#ifdef __DEBUG
    __layer_debug_info_t    debug_info;
#endif
} __matrix_layer_t;


#ifdef __DEBUG
#define DECLARE_LAYER( layer_name ) typedef struct layer_name\
{\
    layer_interface_t       layer_functions;\
    layer_connectivity_t    layer_connection;\
    void*                   user_data;\
    __layer_debug_info_t    debug_info;\
} layer_name##_t
#else
#define DECLARE_LAYER( layer_name ) typedef struct layer_name\
{\
    layer_interface_t       layer_functions;\
    layer_connectivity_t    layer_connection;\
    void*                   user_data;\
} layer_name##_t
#endif

#ifdef __DEBUG
#define LAYER_INSTANCE( layer_name, layer_name_instance, __on_demand, __on_data_ready, __close, __on_close, user_data )\
    layer_name##_t layer_name_instance              = { { __on_demand, __on_data_ready, __close, __on_close }, { 0, 0, 0 }, 0, { 0, 0, 0, 0, 0, 0 } };\
    layer_name_instance.layer_connection.self       = ( void* ) &layer_name_instance;\
    layer_name_instance.user_data                   = ( void* ) &user_data;\
    layer_name_instance.debug_info.debug_line_init  = __LINE__;\
    layer_name_instance.debug_info.debug_file_init  = __FILE__;
#else 
#define LAYER_INSTANCE( layer_name, layer_name_instance, __on_demand, __on_data_ready, __close, __on_close, user_data )\
    layer_name##_t layer_name_instance          = { { __on_demand, __on_data_ready, __close, __on_close }, { 0, 0, 0 }, 0 };\
    layer_name_instance.layer_connection.self   = ( void* ) &layer_name_instance;\
    layer_name_instance.user_data               = ( void* ) &user_data;
#endif

#ifdef __DEBUG
#define CONNECT_LAYERS( lp_i, ln_i )\
    ln_i.layer_connection.prev  = ( void* ) &lp_i;\
    lp_i.layer_connection.next  = ( void* ) &ln_i;\
    lp_i.debug_info.debug_line_connect = __LINE__;\
    lp_i.debug_info.debug_file_connect = __FILE__;\
    ln_i.debug_info.debug_line_connect = __LINE__;\
    ln_i.debug_info.debug_file_connect = __FILE__;
#else
#define CONNECT_LAYERS( lp_i, ln_i )\
    ln_i.layer_connection.prev  = ( void* ) &lp_i;\
    lp_i.layer_connection.next  = ( void* ) &ln_i
#endif    

#define LAYER_GET_CONTEXT_PTR( instance )\
    &instance.layer_connection

#ifdef __DEBUG 
#define SET_DEBUG_INFO_ON_SELF( context )\
    context.layer_connection.self->debug_info.debug_line_last_call = __LINE__;\
    context.layer_connection.self->debug_info.debug_file_last_call = __FILE__;

#define SET_DEBUG_INFO_ON_NEXT( context )\
    context.layer_connection.next->debug_info.debug_line_last_call = __LINE__;\
    context.layer_connection.next->debug_info.debug_file_last_call = __FILE__;

#define SET_DEBUG_INFO_ON_PREV( context )\
    context.layer_connection.prev->debug_info.debug_line_last_call = __LINE__;\
    context.layer_connection.prev->debug_info.debug_file_last_call = __FILE__;
#endif 


// ON_DEMAND
#ifdef __DEBUG
#define CALL_ON_SELF_ON_DEMAND( context, buffer, size )\
    context.layer_connection.self->layer_functions.on_demand(\
        ( void* ) &context.layer_connection.self->layer_connection, buffer, size );\
    SET_DEBUG_INFO_ON_SELF( context )
#else 
#define CALL_ON_SELF_ON_DEMAND( context, buffer, size )\
    context.layer_connection.self->layer_functions.on_demand(\
        ( void* ) &context.layer_connection.self->layer_connection, buffer, size )
#endif

#ifdef __DEBUG
#define CALL_ON_NEXT_ON_DEMAND( context, buffer, size )\
    context.layer_connection.next->layer_functions.on_demand(\
        ( void* ) &context.layer_connection.next->layer_connection, buffer, size );\
    SET_DEBUG_INFO_ON_NEXT( context )
#else 
#define CALL_ON_NEXT_ON_DEMAND( context, buffer, size )\
    context.layer_connection.next->layer_functions.on_demand(\
        ( void* ) &context.layer_connection.next->layer_connection, buffer, size )
#endif

#ifdef __DEBUG
#define CALL_ON_PREV_ON_DEMAND( context, buffer, size )\
    context.layer_connection.prev->layer_functions.on_demand(\
        ( void* ) &context.layer_connection.prev->layer_connection, buffer, size );\
    SET_DEBUG_INFO_ON_PREV( context )
#else 
#define CALL_ON_PREV_ON_DEMAND( context, buffer, size )\
    context.layer_connection.prev->layer_functions.on_demand(\
        ( void* ) &context.layer_connection.prev->layer_connection, buffer, size )
#endif

// ON_DATA_READY
#ifdef __DEBUG
#define CALL_ON_SELF_ON_DATA_READY( context, buffer, size )\
    context.layer_connection.self->layer_functions.on_data_ready(\
        ( void* ) &context.layer_connection.self->layer_connection, buffer, size );\
    SET_DEBUG_INFO_ON_SELF( context )
#else 
#define CALL_ON_SELF_ON_DATA_READY( context, buffer, size )\
    context.layer_connection.self->layer_functions.on_data_ready(\
        ( void* ) &context.layer_connection.self->layer_connection, buffer, size )
#endif

#ifdef __DEBUG
#define CALL_ON_NEXT_ON_DATA_READY( context, buffer, size )\
    context.layer_connection.next->layer_functions.on_data_ready(\
        ( void* ) &context.layer_connection.next->layer_connection, buffer, size );\
    SET_DEBUG_INFO_ON_NEXT( context )
#else 
#define CALL_ON_NEXT_ON_DATA_READY( context, buffer, size )\
    context.layer_connection.next->layer_functions.on_data_ready(\
        ( void* ) &context.layer_connection.next->layer_connection, buffer, size )
#endif

#ifdef __DEBUG
#define CALL_ON_PREV_ON_DATA_READY( context, buffer, size )\
    context.layer_connection.prev->layer_functions.on_data_ready(\
        ( void* ) &context.layer_connection.prev->layer_connection, buffer, size );\
    SET_DEBUG_INFO_ON_PREV( context )
#else 
#define CALL_ON_PREV_ON_DATA_READY( context, buffer, size )\
    context.layer_connection.prev->layer_functions.on_data_ready(\
        ( void* ) &context.layer_connection.prev->layer_connection, buffer, size )
#endif

// CLOSE
#ifdef __DEBUG
#define CALL_ON_SELF_CLOSE( context )\
    context.layer_connection.self->layer_functions.close(\
        ( void* ) &context.layer_connection.self->layer_connection );\
    SET_DEBUG_INFO_ON_SELF( context )
#else 
#define CALL_ON_SELF_CLOSE( context )\
    context.layer_connection.self->layer_functions.close(\
        ( void* ) &context.layer_connection.self->layer_connection )
#endif

#ifdef __DEBUG
#define CALL_ON_NEXT_CLOSE( context )\
    context.layer_connection.next->layer_functions.close(\
        ( void* ) &context.layer_connection.next->layer_connection );\
    SET_DEBUG_INFO_ON_NEXT( context )
#else 
#define CALL_ON_NEXT_CLOSE( context )\
    context.layer_connection.next->layer_functions.close(\
        ( void* ) &context.layer_connection.next->layer_connection )
#endif

#ifdef __DEBUG
#define CALL_ON_PREV_CLOSE( context )\
    context.layer_connection.prev->layer_functions.close(\
        ( void* ) &context.layer_connection.prev->layer_connection );\
    SET_DEBUG_INFO_ON_PREV( context )
#else 
#define CALL_ON_PREV_CLOSE( context )\
    context.layer_connection.prev->layer_functions.close(\
        ( void* ) &context.layer_connection.prev->layer_connection )
#endif

// ON_CLOSE
#ifdef __DEBUG
#define CALL_ON_SELF_ON_CLOSE( context )\
    context.layer_connection.self->layer_functions.on_close(\
        ( void* ) &context.layer_connection.self->layer_connection );\
    SET_DEBUG_INFO_ON_SELF( context )
#else 
#define CALL_ON_SELF_ON_CLOSE( context )\
    context.layer_connection.self->layer_functions.on_close(\
        ( void* ) &context.layer_connection.self->layer_connection )
#endif

#ifdef __DEBUG
#define CALL_ON_NEXT_ON_CLOSE( context )\
    context.layer_connection.next->layer_functions.on_close(\
        ( void* ) &context.layer_connection.next->layer_connection );\
    SET_DEBUG_INFO_ON_NEXT( context )
#else 
#define CALL_ON_NEXT_ON_CLOSE( context )\
    context.layer_connection.next->layer_functions.on_close(\
        ( void* ) &context.layer_connection.next->layer_connection )
#endif

#ifdef __DEBUG
#define CALL_ON_PREV_ON_CLOSE( context )\
    context.layer_connection.prev->layer_functions.on_close(\
        ( void* ) &context.layer_connection.prev->layer_connection );\
    SET_DEBUG_INFO_ON_PREV( context )
#else 
#define CALL_ON_PREV_ON_CLOSE( context )\
    context.layer_connection.prev->layer_functions.on_close(\
        ( void* ) &context.layer_connection.prev->layer_connection )
#endif

#endif
