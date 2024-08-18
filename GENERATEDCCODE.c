/*
This code was automatically generated using the Riverside-Irvine State machine Builder tool
Version 2.9 --- 8/18/2024 5:32:40 PST
*/

#include "rims.h"

/*Define user variables and functions for this state machine here.*/
enum SM1_States { SM1_idle, SM1_L change, SM1_default, SM1_override } SM1_State;

TickFct_Idle() {
   switch(SM1_State) { // Transitions
      case -1:
         SM1_State = SM1_idle;
         break;
         case SM1_idle: 
         if (vehicleDetected()
) {
            SM1_State = SM1_L change;
         }
         else if (emergency()) {
            SM1_State = SM1_override;
         }
         break;
      case SM1_L change: 
         if (vehicleCleared()
densityConditionsMet()
) {
            SM1_State = SM1_idle;
         }
         else if (sensorFailure()
) {
            SM1_State = SM1_default;
         }
         else if (1emergency()) {
            SM1_State = SM1_override;
         }
         break;
      case SM1_default: 
         if (emergency()) {
            SM1_State = SM1_override;
         }
         break;
      case SM1_override: 
         break;
      default:
         SM1_State = SM1_idle;
   } // Transitions

   switch(SM1_State) { // State actions
      case SM1_idle:
         break;
      case SM1_L change:
         break;
      case SM1_default:
         break;
      case SM1_override:
         break;
      default: // ADD default behaviour below
      break;
   } // State actions

}

int main() {

   
   SM1_State = -1; // Initial state
   B = 0; // Init outputs

   while(1) {
      TickFct_Idle();} // while (1)
} // Main