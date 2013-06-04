#ifndef __LAYER_INTERFACE_H__
#define __LAYER_INTERFACE_H__

/**
 * \file 	layer_interface.h
 * \author 	Olgierd Humenczuk
 * \brief 	Containes the layer interface implementation as long as base structure declarations
 *			required by the layer to work.
 */

// forward declaration
struct __matrix_layer;

/**
 * \brief 	layer states it simplifies the between layer communication
 * \note  	This is part of the standarized protocol of communication that 
 * 		  	has been designed in order to provide the minimum of the restriction
 *		  	with maximum capabilities.
 */
typedef enum
{
	LAYER_STATE_OK = 0,
	LAYER_STATE_FULL,
	LAYER_STATE_TIMEOUT,
	LAYER_STATE_ERROR
} layer_state_t;


/**
 * \brief 	allows to seek for the next call context 
 */
typedef struct layer_connectivity
{
	struct __matrix_layer* self;
	struct __matrix_layer* higher;
	struct __matrix_layer* lower;
} layer_connectivity_t;

/**
 * \struct layer_interface_t
 * 
 * \brief	The raw interface of the purly raw layer that combines both: simplicity and functionality
 * 	   of the 'on demand processing' idea. 
 * 
 *				The design idea is based on the assumption that the communication with the server 
 *	   		is bidirectional and the data is processed in packages. This interface design is 
 *	   		very generic and allows to implement different approaches as an extension of the 
 *     		basic operations that the interface is capable of. In very basic scenario the client 
 *     		wants to send some number of bytes to the server and then awaits for the response. 
 *     		After receiving the response the response is passed by to the processing layer through
 *	   		on_data_ready function which signals the processing layer that there is new data to process.
 *
 *          	Term 'on demand processing' refers to the processing managed by the connection. 
 *     		With the observation that all of the decision in the software depends on the network 
 *     		capabilities it is clear that only the connection has ability to give the signals about 
 *     		the processing to the rest of the components. The 'demand' takes it source in the 
 *     		connection and that is the layer that has command over the another layers.
 */
typedef struct layer_interface
{
	/**
	 * \brief the function that is called whenever the lower layer wants more data to process/send over some kind of the connection
	 */
	layer_state_t ( *on_demand )( layer_connectivity_t* context, const char* buffer, size_t size );

	/**
	 * \brief the function that is called whenever there is data that is ready and it's source is the lower layer
	 */
	layer_state_t ( *on_data_ready )( layer_connectivity_t* context, char* buffer, size_t size );

	/**
	 * \brief whenever the processing chain suppose to be closed
	 */
	layer_state_t ( *close )( layer_connectivity_t* context );

	/**
	 * \brief whenver the processing chain is going to be closed it's source is in the lower layer
	 */
	layer_state_t ( *on_close )( layer_connectivity_t* context );

} layer_interface_t;

/**
 * \brief the magic begins here
 */
typedef struct __matrix_layer
{
	layer_interface_t 		layer_functions;
	layer_connectivity_t	layer_connection;
} __matrix_layer_t;

#define DECLARE_LAYER( layer_name ) typedef struct layer_name\
{\
	layer_interface_t 		layer_functions;\
	layer_connectivity_t	layer_connection;\
} layer_name##_t;\


#define LAYER_INSTANCE( layer_name, layer_name_instance, __on_demand, __on_data_ready, __close, __on_close )\
	layer_name##_t layer_name_instance = { { __on_demand, __on_data_ready, __close, __on_close }, { 0, 0, 0 } };\
	void* layer_name_instance##__ptr = &layer_name_instance;\
	layer_name_instance.layer_connection.self = layer_name_instance##__ptr;

#define CONNECT_LAYERS( lh_i, ll_i )\
	lh_i.layer_connection.lower 	= ( void* ) &ll_i;\
	ll_i.layer_connection.higher 	= ( void* ) &lh_i

#define LAYER_GET_CONTEXT_PTR( instance )\
	&instance.layer_connection

#define CALL_ON_SELF_ON_DEMAND( context, buffer, size )\
	context.layer_connection.self->layer_functions.on_demand( ( void* ) context.layer_connection.self, buffer, size )

#define CALL_ON_HIGHER_ON_DEMAND( context, buffer, size )\
	context.layer_connection.higher->layer_functions.on_demand( ( void* ) context.layer_connection.higher, buffer, size )

#define CALL_ON_LOWER_ON_DEMAND( context, buffer, size )\
	context.layer_connection.lower->layer_functions.on_demand( ( void* ) context.layer_connection.lower, buffer, size )

#endif
