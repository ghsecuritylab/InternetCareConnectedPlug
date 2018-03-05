/*
 * global.c
 *
 *  Created on: Feb 21, 2018
 *      
 */

#include "global.h"

void gstateSet(SemaphoreHandle_t s, uint16_t v)
{
  if( xSemaphoreTake(s, ( TickType_t ) 10 ) == pdTRUE ) {
    gstate = v | (gstate & STATE_RESTART_INTERNAL) | (gstate & STATE_RESTART_EXTERNAL);
    xSemaphoreGive( s );
  }
}

void gstateAdd(SemaphoreHandle_t s, uint16_t v)
{
  if( xSemaphoreTake(s, ( TickType_t ) 10 ) == pdTRUE ) {
    gstate |= v;
    xSemaphoreGive( s );
  }
}

void gstateRm(SemaphoreHandle_t s, uint16_t v)
{
  if( xSemaphoreTake(s, ( TickType_t ) 10 ) == pdTRUE ) {
    gstate &= ~v;
    xSemaphoreGive( s );
  }
}

