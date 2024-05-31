.include "asm/include/battle_commands.inc"

.data

_000:
    CompareMonDataToValue OPCODE_FLAG_SET, BATTLER_CATEGORY_ATTACKER, BMON_DATA_STATUS2, STATUS2_LOCKED_INTO_MOVE, _049
    CompareVarToValue OPCODE_FLAG_SET, BSCRIPT_VAR_BATTLE_STATUS, BATTLE_STATUS_CHARGE_MOVE_HIT, _049
    CheckItemHoldEffect CHECK_OPCODE_HAVE, BATTLER_CATEGORY_ATTACKER, HOLD_EFFECT_CHARGE_SKIP, _029
    UpdateMonData OPCODE_FLAG_ON, BATTLER_CATEGORY_ATTACKER, BMON_DATA_MOVE_EFFECT, MOVE_EFFECT_FLAG_PHANTOM_FORCE
    UpdateVar OPCODE_SET, BSCRIPT_VAR_SIDE_EFFECT_FLAGS_DIRECT, MOVE_SIDE_EFFECT_TO_ATTACKER|MOVE_SUBSCRIPT_PTR_VANISH_CHARGE_TURN
    UpdateVar OPCODE_FLAG_ON, BSCRIPT_VAR_BATTLE_STATUS, BATTLE_STATUS_CHARGE_TURN|BATTLE_STATUS_CHECK_LOOP_ONLY_ONCE|BATTLE_STATUS_NO_ATTACK_MESSAGE
    End 

_029:
    UpdateMonData OPCODE_FLAG_ON, BATTLER_CATEGORY_ATTACKER, BMON_DATA_MOVE_EFFECT, MOVE_EFFECT_FLAG_PHANTOM_FORCE
    UpdateMonData OPCODE_FLAG_ON, BATTLER_CATEGORY_ATTACKER, BMON_DATA_MOVE_EFFECT_TEMP, 536870912
    Call BATTLE_SUBSCRIPT_ITEM_SKIP_CHARGE_TURN
    ToggleVanish BATTLER_CATEGORY_ATTACKER, TRUE
    CompareVarToValue OPCODE_EQU, BSCRIPT_VAR_BATTLER_TARGET, BATTLER_NONE, _055

_049:
    UpdateVar OPCODE_SET, BSCRIPT_VAR_SIDE_EFFECT_FLAGS_INDIRECT, MOVE_SIDE_EFFECT_ON_HIT|MOVE_SUBSCRIPT_PTR_FEINT
    CalcCrit 
    CalcDamage 

_055:
    UpdateMonData OPCODE_FLAG_OFF, BATTLER_CATEGORY_ATTACKER, BMON_DATA_MOVE_EFFECT, MOVE_EFFECT_FLAG_SEMI_INVULNERABLE
    Call BATTLE_SUBSCRIPT_CHARGE_MOVE_CLEANUP
    End 