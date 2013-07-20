#include <stdio.h>

#include "layer_interface.h"


DECLARE_LAYER( dummy_layer1 );

layer_state_t dummy_layer1_on_demand( layer_connectivity_t* context, const char* buffer, size_t size )
{
	printf( "dummy_layer1_on_demand %p\n", context->self->user_data );
	return LAYER_STATE_OK;
}

layer_state_t dummy_layer1_on_data_ready( layer_connectivity_t* context, char* buffer, size_t size )
{
	printf( "dummy_layer1_on_data_ready %p\n", context->self->user_data );
	return LAYER_STATE_OK;
}

layer_state_t dummy_layer1_close( layer_connectivity_t* context )
{
	printf( "dummy_layer1_close %p\n", context->self->user_data );
	return LAYER_STATE_OK;
}

layer_state_t dummy_layer1_on_close( layer_connectivity_t* context )
{
	printf( "dummy_layer1_on_close %p\n", context->self->user_data );
	return LAYER_STATE_OK;
}

DECLARE_LAYER( dummy_layer2 );

layer_state_t dummy_layer2_on_demand( layer_connectivity_t* context, const char* buffer, size_t size )
{
	printf( "dummy_layer2_on_demand %p\n", context->self->user_data );
	return LAYER_STATE_OK;
}

layer_state_t dummy_layer2_on_data_ready( layer_connectivity_t* context, char* buffer, size_t size )
{
	printf( "dummy_layer2_on_data_ready %p\n", context->self->user_data );
	return LAYER_STATE_OK;
}

layer_state_t dummy_layer2_close( layer_connectivity_t* context )
{
	printf( "dummy_layer2_close %p\n", context->self->user_data );
	return LAYER_STATE_OK;
}

layer_state_t dummy_layer2_on_close( layer_connectivity_t* context )
{
	printf( "dummy_layer2_on_close %p\n", context->self->user_data );
	return LAYER_STATE_OK;
}

int main( int argc, const char* argv[] )
{

    int user_data = 0;
    
	LAYER_INSTANCE( dummy_layer1, dummy_layer1_instance, &dummy_layer1_on_demand, &dummy_layer1_on_data_ready, &dummy_layer1_close, &dummy_layer1_on_close, user_data );
	LAYER_INSTANCE( dummy_layer2, dummy_layer2_instance, &dummy_layer2_on_demand, &dummy_layer2_on_data_ready, &dummy_layer2_close, &dummy_layer2_on_close, user_data );

	CONNECT_LAYERS( dummy_layer1_instance, dummy_layer2_instance );

	CALL_ON_NEXT_ON_DEMAND( dummy_layer1_instance, 0, 0 );
    CALL_ON_PREV_ON_DEMAND( dummy_layer2_instance, 0, 0 );
    CALL_ON_SELF_ON_DEMAND( dummy_layer1_instance, 0, 0 );
    CALL_ON_SELF_ON_DEMAND( dummy_layer2_instance, 0, 0 );

    CALL_ON_NEXT_ON_DATA_READY( dummy_layer1_instance, 0, 0 );
    CALL_ON_PREV_ON_DATA_READY( dummy_layer2_instance, 0, 0 );
    CALL_ON_SELF_ON_DATA_READY( dummy_layer1_instance, 0, 0 );
    CALL_ON_SELF_ON_DATA_READY( dummy_layer2_instance, 0, 0 );

    CALL_ON_NEXT_CLOSE( dummy_layer1_instance );
    CALL_ON_PREV_CLOSE( dummy_layer2_instance );
    CALL_ON_SELF_CLOSE( dummy_layer1_instance );
    CALL_ON_SELF_CLOSE( dummy_layer2_instance );

    CALL_ON_NEXT_ON_CLOSE( dummy_layer1_instance );
    CALL_ON_PREV_ON_CLOSE( dummy_layer2_instance );
    CALL_ON_SELF_ON_CLOSE( dummy_layer1_instance );
    CALL_ON_SELF_ON_CLOSE( dummy_layer2_instance );

	return 0;
}
