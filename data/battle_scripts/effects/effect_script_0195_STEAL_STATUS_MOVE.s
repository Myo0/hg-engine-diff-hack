.include "asm/include/battle_commands.inc"

.data

_000:
    TrySnatch _010
    // {0} waits for a target to make a move!
    BufferMessage 577, TAG_NICKNAME, BATTLER_CATEGORY_ATTACKER
    UpdateVar OPCODE_SET, BSCRIPT_VAR_SIDE_EFFECT_FLAGS_INDIRECT, MOVE_SIDE_EFFECT_ON_HIT|MOVE_SUBSCRIPT_PTR_PRINT_MESSAGE_AND_PLAY_ANIMATION
    End 

_010:
    UpdateVar OPCODE_SET, BSCRIPT_VAR_MOVE_STATUS_FLAGS, MOVE_STATUS_FAILED
    End 
