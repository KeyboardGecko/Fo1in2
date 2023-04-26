/*

   Military Base generic defines

*/

#pragma once

/************************************************
    Generic Defines
************************************************/
#define mbase_full_alert         (global_var(GVAR_VATS_ALERT) == 1)

#define bos_invading_cathedral   (global_var(GVAR_BROTHERHOOD_SEND_HELP) == 1)
#define bos_invading_mbase       (global_var(GVAR_BROTHERHOOD_SEND_HELP) == 2)

#define get_force_fields_on      (global_var(GVAR_MBASE_FORCE_FIELD_STATE) == 1)
#define get_force_fields_off     (global_var(GVAR_MBASE_FORCE_FIELD_STATE) == 0)

// We have to make a hard-reset on the force field MVARs. It basically acts as a toggle / reset
#define set_force_fields_on      set_global_var(GVAR_MBASE_FORCE_FIELD_STATE, 1);\
                                 set_map_var(MVAR_Field1a_Status, ENABLED);      \
                                 set_map_var(MVAR_Field2a_Status, ENABLED);      \
                                 set_map_var(MVAR_Field3a_Status, ENABLED);      \
                                 set_map_var(MVAR_Field4a_Status, ENABLED);      \
                                 set_map_var(MVAR_Field5a_Status, ENABLED);      \
                                 set_map_var(MVAR_Field6a_Status, ENABLED);      \
                                 set_map_var(MVAR_Field1b_Status, ENABLED);      \
                                 set_map_var(MVAR_Field2b_Status, ENABLED)
#define set_force_fields_off     set_global_var(GVAR_MBASE_FORCE_FIELD_STATE, 0);\
                                 set_map_var(MVAR_Field1a_Status, DISABLED);     \
                                 set_map_var(MVAR_Field2a_Status, DISABLED);     \
                                 set_map_var(MVAR_Field3a_Status, DISABLED);     \
                                 set_map_var(MVAR_Field4a_Status, DISABLED);     \
                                 set_map_var(MVAR_Field5a_Status, DISABLED);     \
                                 set_map_var(MVAR_Field6a_Status, DISABLED);     \
                                 set_map_var(MVAR_Field1b_Status, DISABLED);     \
                                 set_map_var(MVAR_Field2b_Status, DISABLED)

// Force Field status
#define DISABLED              (0)
#define ENABLED               (1)
#define FORCE_DISABLED        (2)
#define FORCE_ENABLED         (3)
#define FORCE_DISABLED_TEMP   (4)

// Destroyed emitter:
variable ff_emitter;
#define set_emitter_destroyed    set_map_var(FField_MVAR, DISABLED);                                                   \
                                 if (FField_Ptr != 0) then begin                                                       \
                                    destroy_object(FField_Ptr);                                                        \
                                    FField_Ptr := 0;                                                                   \
                                 end                                                                                   \
                                 if (self_pid == PID_EMITTER_MBASE_NS) then begin                                      \
                                    ff_emitter := create_object(PID_EMITTER_DESTROYED_NS, self_tile, self_elevation);  \
                                    destroy_object(self_obj);                                                          \
                                 end                                                                                   \
                                 else if (self_pid == PID_EMITTER_MBASE_EW) then begin                                 \
                                    ff_emitter := create_object(PID_EMITTER_DESTROYED_EW, self_tile, self_elevation);  \
                                    destroy_object(self_obj);                                                          \
                                 end

// Robobrain settings
#define robo_full_sensors     0
#define robo_min_sensors      1
#define set_robo_full_sensors       set_global_var(GVAR_ROBCTRL_SENSORS_HOWMUCH, robo_full_sensors)
#define set_robo_minimal_sensors    set_global_var(GVAR_ROBCTRL_SENSORS_HOWMUCH, robo_min_sensors)
#define get_robo_full_sensors       (global_var(GVAR_ROBCTRL_SENSORS_HOWMUCH) == robo_full_sensors)
#define get_robo_minimal_sensors    (global_var(GVAR_ROBCTRL_SENSORS_HOWMUCH) == robo_min_sensors)

#define robo_max_movement     0
#define robo_half_movement    1
#define robo_min_movement     2
#define set_robo_max_movement       set_global_var(GVAR_ROBCTRL_MOVEMENT_HOWMUCH, robo_max_movement)
#define set_robo_half_movement      set_global_var(GVAR_ROBCTRL_MOVEMENT_HOWMUCH, robo_half_movement)
#define set_robo_min_movement       set_global_var(GVAR_ROBCTRL_MOVEMENT_HOWMUCH, robo_min_movement)
#define get_robo_max_movement       (global_var(GVAR_ROBCTRL_MOVEMENT_HOWMUCH) == robo_max_movement)
#define get_robo_half_movement      (global_var(GVAR_ROBCTRL_MOVEMENT_HOWMUCH) == robo_half_movement)
#define get_robo_min_movement       (global_var(GVAR_ROBCTRL_MOVEMENT_HOWMUCH) == robo_min_movement)

#define robo_attack_unauth_pests 0
#define robo_attack_all_pests    1
#define robo_attack_large_pests  2
#define robo_attack_small_pests  3
#define robo_attack_no_pests     4
#define set_robo_attack_unauth_pests   set_global_var(GVAR_ROBCTRL_PESTS_HOWMUCH, robo_attack_unauth_pests)
#define set_robo_attack_all_pests      set_global_var(GVAR_ROBCTRL_PESTS_HOWMUCH, robo_attack_all_pests)
#define set_robo_attack_large_pests    set_global_var(GVAR_ROBCTRL_PESTS_HOWMUCH, robo_attack_large_pests)
#define set_robo_attack_small_pests    set_global_var(GVAR_ROBCTRL_PESTS_HOWMUCH, robo_attack_small_pests)
#define set_robo_attack_no_pests       set_global_var(GVAR_ROBCTRL_PESTS_HOWMUCH, robo_attack_no_pests)
#define get_robo_attack_unauth_pests   (global_var(GVAR_ROBCTRL_PESTS_HOWMUCH) == robo_attack_unauth_pests)
#define get_robo_attack_all_pests      (global_var(GVAR_ROBCTRL_PESTS_HOWMUCH) == robo_attack_all_pests)
#define get_robo_attack_large_pests    (global_var(GVAR_ROBCTRL_PESTS_HOWMUCH) == robo_attack_large_pests)
#define get_robo_attack_small_pests    (global_var(GVAR_ROBCTRL_PESTS_HOWMUCH) == robo_attack_small_pests)
#define get_robo_attack_no_pests       (global_var(GVAR_ROBCTRL_PESTS_HOWMUCH) == robo_attack_no_pests)

// Mutant combat alert check
#define alert_check \
   if (fixed_param == COMBAT_SUBTYPE_TURN) then begin \
      round_counter := round_counter + 1;             \
      if (round_counter > 2) then begin               \
         call GenSuprAlert;                           \
      end                                             \
   end                                                \
   noop

#define knock_out_player_check \
   if (fixed_param == COMBAT_SUBTYPE_HIT_SUCCEEDED) then begin \
      if (global_var(GVAR_ROBCTRL_PESTS_HOWMUCH)) then begin   \
         if (random(0, 3) == 3) then begin                     \
            critter_injure(dude_obj, DAM_KNOCKED_OUT);         \
         end                                                   \
      end                                                      \
   end                                                         \
   noop
