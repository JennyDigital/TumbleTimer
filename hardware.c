void SetBacklight( int blset )
{
    //output_bit( BL_PIN, blset );
    set_pwm1_duty( blset );
}

void SetRunning( int htr_runset )
{    
    if( htr_runset )
    {
        output_bit( RUN_PIN, 1 );
        running = 1;
    }
    
    if( !htr_runset )
    {
        output_bit( RUN_PIN, 0 );
        output_bit( HTR_PIN, 0 );
        running = 0;
        heating = 0;
    }
}

void SetHeating( int htr_heatset )
{
    if( htr_heatset && running )
    {
        output_bit( HTR_PIN, 1 );
        heating = 1;
    }
    if( !htr_heatset )
    {
        output_bit( HTR_PIN, 0 );
        heating = 0;
    }
}

short GetEncA( void )
{
    return input( ENC_A );
}

short GetEncB( void )
{
    return input( ENC_B );
}

short GetEncBtn( void )
{
    return input( ENC_BTN );
}


short GetDoorState( void )
{
//    return DOOR_CLOSED ;
    return input( DOOR_PIN );
}

void InitTimer2( void )
{
    setup_timer_2(T2_DIV_BY_4, 82, 1);
    set_timer2(20000);
}
    
 