#include "../include/battle.h"
#include "../include/pokemon.h"
#include "../include/types.h"
#include "../include/constants/ability.h"
#include "../include/constants/battle_script_constants.h"
#include "../include/constants/battle_message_constants.h"
#include "../include/constants/hold_item_effects.h"
#include "../include/constants/item.h"
#include "../include/constants/move_effects.h"
#include "../include/constants/moves.h"
#include "../include/constants/species.h"
#include "../include/constants/weather_numbers.h"

BOOL btl_scr_cmd_33_statbuffchange(void *bw, struct BattleStruct *sp)
{
    int address1;
    int address2;
    int address3;
    int stattochange;
    int statchange;
    int flag;
    struct BattlePokemon *battlemon = &sp->battlemon[sp->state_client];

    IncrementBattleScriptPtr(sp, 1);

    address1 = read_battle_script_param(sp);
    address2 = read_battle_script_param(sp);
    address3 = read_battle_script_param(sp);

    flag = 0;

    sp->server_status_flag &= !(SERVER_STATUS_FLAG_STAT_CHANGE);

        //2 steps down
    if (sp->addeffect_param >= ADD_STATE_ATTACK_DOWN_2)
    {
        stattochange = sp->addeffect_param - ADD_STATE_ATTACK_DOWN_2;
        statchange = -2;
        sp->temp_work = STATUS_EFF_DOWN;
    }
        //2 steps up
    else if (sp->addeffect_param >= ADD_STATE_ATTACK_UP_2)
    {
        stattochange = sp->addeffect_param - ADD_STATE_ATTACK_UP_2;
        statchange = 2;
        sp->temp_work = STATUS_EFF_UP;
    }
        //1 step down
    else if (sp->addeffect_param >= ADD_STATE_ATTACK_DOWN)
    {
        stattochange = sp->addeffect_param - ADD_STATE_ATTACK_DOWN;
        statchange = -1;
        sp->temp_work = STATUS_EFF_DOWN;
    }
        //1 step up
    else
    {
        stattochange = sp->addeffect_param - ADD_STATE_ATTACK_UP;
        statchange = 1;
        sp->temp_work = STATUS_EFF_UP;
    }

    if (battlemon->ability == ABILITY_CONTRARY)
    {
        //statchange
        statchange = -statchange;

        //sp->temp_work
        if(sp->temp_work == STATUS_EFF_UP)
        {
            sp->temp_work= STATUS_EFF_DOWN;
        }
        else if(sp->temp_work == STATUS_EFF_DOWN)
        {
            sp->temp_work= STATUS_EFF_UP;
        }
    }


    if (statchange > 0)
    {
        if (battlemon->states[STAT_ATTACK + stattochange] == 12)
        {
            sp->server_status_flag |= SERVER_STATUS_FLAG_STAT_CHANGE;
            
            if ((sp->addeffect_type == ADD_EFFECT_INDIRECT)
             || (sp->addeffect_type == ADD_EFFECT_ABILITY))
            {
                IncrementBattleScriptPtr(sp, address2);
            }
            else
            {
                sp->mp.msg_id = BATTLE_MSG_STAT_WONT_GO_HIGHER;
                sp->mp.msg_tag = TAG_NICK_STAT;
                sp->mp.msg_para[0] = TagNickParaMake(sp, sp->state_client);
                sp->mp.msg_para[1] = STAT_ATTACK + stattochange;
                IncrementBattleScriptPtr(sp, address1);
            }
        }
        else
        {
            if (sp->addeffect_type == ADD_EFFECT_ABILITY)
            {
                switch (statchange)
                {
                case 1:
                    sp->mp.msg_id = BATTLE_MSG_ABILITY_RAISED_STAT;
                    break;
                case 2:
                    sp->mp.msg_id = BATTLE_MSG_ABILITY_RAISED_STAT_SHARPLY;
                    break;
                default:
                    sp->mp.msg_id = BATTLE_MSG_ABILITY_RAISED_STAT_DRASTICALLY;
                    break;
                }
                sp->mp.msg_tag = TAG_NICK_TOKU_STAT;
                sp->mp.msg_para[0] = TagNickParaMake(sp, sp->state_client);
                sp->mp.msg_para[1] = sp->battlemon[sp->state_client].ability;
                sp->mp.msg_para[2] = STAT_ATTACK + stattochange;
            }
            else if (sp->addeffect_type == ADD_EFFECT_HELD_ITEM)
            {
                sp->mp.msg_id = BATTLE_MSG_ITEM_RAISED_STAT;
                sp->mp.msg_tag = TAG_NICK_ITEM_STAT;
                sp->mp.msg_para[0] = TagNickParaMake(sp, sp->state_client);
                sp->mp.msg_para[1] = sp->item_work;
                sp->mp.msg_para[2] = STAT_ATTACK + stattochange;
            }
            else
            {
                switch (statchange)
                {
                case 1:
                    sp->mp.msg_id = BATTLE_MSG_STAT_RAISED;
                    break;
                case 2:
                    sp->mp.msg_id = BATTLE_MSG_STAT_RAISED_SHARPLY;
                    break;
                default:
                    sp->mp.msg_id = BATTLE_MSG_STAT_RAISED_DRASTICALLY;
                    break;
                }
                sp->mp.msg_tag = TAG_NICK_STAT;
                sp->mp.msg_para[0] = TagNickParaMake(sp, sp->state_client);
                sp->mp.msg_para[1] = STAT_ATTACK + stattochange;
            }
            battlemon->states[STAT_ATTACK + stattochange] += statchange;
            if (battlemon->states[STAT_ATTACK + stattochange] > 12)
            {
                battlemon->states[STAT_ATTACK + stattochange] = 12;
            }
        }
    }
    else
    {
        if ((sp->addeffect_flag & ADD_STATUS_NO_ABILITY) == 0)
        {
            if (sp->attack_client != sp->state_client)
            {
                if (sp->scw[IsClientEnemy(bw,sp->state_client)].mist_count)
                {
                    sp->mp.msg_id = BATTLE_MSG_PROTECTED_BY_MIST;
                    sp->mp.msg_tag = TAG_NICK;
                    sp->mp.msg_para[0] = TagNickParaMake(sp,sp->state_client);
                    flag = 1;
                }
                else if ((MoldBreakerAbilityCheck(sp, sp->attack_client, sp->state_client, ABILITY_CLEAR_BODY) == TRUE)
                      || (MoldBreakerAbilityCheck(sp, sp->attack_client, sp->state_client, ABILITY_WHITE_SMOKE) == TRUE))
                    {
                    if (sp->addeffect_type == ADD_EFFECT_ABILITY)
                    {
                        sp->mp.msg_id = BATTLE_MSG_ABILITY_SUPPRESSES_STAT_LOSS;
                        sp->mp.msg_tag = TAG_NICK_TOKU_NICK_TOKU;
                        sp->mp.msg_para[0] = TagNickParaMake(sp, sp->state_client);
                        sp->mp.msg_para[1] = sp->battlemon[sp->state_client].ability;
                        sp->mp.msg_para[2] = TagNickParaMake(sp, sp->attack_client);
                        sp->mp.msg_para[3] = sp->battlemon[sp->attack_client].ability;
                    }
                    else
                    {
                        sp->mp.msg_id = BATTLE_MSG_PREVENTS_STAT_LOSS;
                        sp->mp.msg_tag = TAG_NICK_TOKU;
                        sp->mp.msg_para[0] = TagNickParaMake(sp, sp->state_client);
                        sp->mp.msg_para[1] = sp->battlemon[sp->state_client].ability;
                    }
                    flag = 1;
                }
                else if (((MoldBreakerAbilityCheck(sp, sp->attack_client, sp->state_client, ABILITY_KEEN_EYE) == TRUE)
                       && ((STAT_ATTACK + stattochange)==STAT_ACCURACY))
                      || ((MoldBreakerAbilityCheck(sp, sp->attack_client, sp->state_client, ABILITY_HYPER_CUTTER) == TRUE)
                       && ((STAT_ATTACK + stattochange) == STAT_ATTACK))
                       || ((MoldBreakerAbilityCheck(sp, sp->attack_client, sp->state_client, ABILITY_BIG_PECKS) == TRUE)
                       && ((STAT_ATTACK + stattochange) == STAT_DEFENSE)))
                    {
                    if (sp->addeffect_type == ADD_EFFECT_ABILITY)
                    {
                        sp->mp.msg_id = BATTLE_MSG_ABILITY_SUPPRESSES_STAT_LOSS;
                        sp->mp.msg_tag = TAG_NICK_TOKU_NICK_TOKU;
                        sp->mp.msg_para[0] = TagNickParaMake(sp, sp->state_client);
                        sp->mp.msg_para[1] = sp->battlemon[sp->state_client].ability;
                        sp->mp.msg_para[2] = TagNickParaMake(sp, sp->attack_client);
                        sp->mp.msg_para[3] = sp->battlemon[sp->attack_client].ability;
                    }
                    else
                    {
                        sp->mp.msg_id = BATTLE_MSG_ITEM_PREVENTS_STAT_LOSS;
                        sp->mp.msg_tag = TAG_NICK_TOKU_STAT;
                        sp->mp.msg_para[0] = TagNickParaMake(sp, sp->state_client);
                        sp->mp.msg_para[1] = sp->battlemon[sp->state_client].ability;
                        sp->mp.msg_para[2] = STAT_ATTACK + stattochange;
                    }
                    flag = 1;
                }
                else if (battlemon->states[STAT_ATTACK + stattochange] == 0)
                {
                    sp->server_status_flag |= SERVER_STATUS_FLAG_STAT_CHANGE;
                    if ((sp->addeffect_type == ADD_EFFECT_INDIRECT)
                     || (sp->addeffect_type == ADD_EFFECT_ABILITY))
                    {
                        IncrementBattleScriptPtr(sp, address2);
                        return FALSE;
                    }
                    else
                    {
                        sp->mp.msg_id = BATTLE_MSG_STAT_WONT_GO_LOWER;
                        sp->mp.msg_tag = TAG_NICK_STAT;
                        sp->mp.msg_para[0] = TagNickParaMake(sp, sp->state_client);
                        sp->mp.msg_para[1] = STAT_ATTACK + stattochange;
                        IncrementBattleScriptPtr(sp, address1);
                        return FALSE;
                    }
                }   
                else if ((MoldBreakerAbilityCheck(sp, sp->attack_client, sp->state_client, ABILITY_SHIELD_DUST) == TRUE)
                      && (sp->addeffect_type == ADD_EFFECT_INDIRECT))
                {
                    flag = 1;
                }
                else if (sp->battlemon[sp->state_client].condition2 & STATUS2_FLAG_SUBSTITUTE)
                {
                    flag = 2;
                }
            }
            else if (battlemon->states[STAT_ATTACK + stattochange] == 0)
            {
                sp->server_status_flag |= SERVER_STATUS_FLAG_STAT_CHANGE;
                if ((sp->addeffect_type == ADD_EFFECT_INDIRECT)
                 || (sp->addeffect_type == ADD_EFFECT_ABILITY))
                {
                    IncrementBattleScriptPtr(sp, address2);
                    return FALSE;
                }
                else
                {
                    sp->mp.msg_id = BATTLE_MSG_STAT_WONT_GO_LOWER;
                    sp->mp.msg_tag = TAG_NICK_STAT;
                    sp->mp.msg_para[0] = TagNickParaMake(sp, sp->state_client);
                    sp->mp.msg_para[1] = STAT_ATTACK + stattochange;
                    IncrementBattleScriptPtr(sp, address1);
                    return FALSE;
                }
            }
            if ((flag == 2) && (sp->addeffect_type == ADD_STATUS_DIRECT))
            {
                IncrementBattleScriptPtr(sp, address3);
                return FALSE;
            }
            else if ((flag) && (sp->addeffect_type == ADD_EFFECT_INDIRECT))
            {
                IncrementBattleScriptPtr(sp, address2);
                return FALSE;
            }
            else if (flag)
            {
                IncrementBattleScriptPtr(sp, address1);
                return FALSE;
            }
        }
        if (sp->addeffect_type == ADD_EFFECT_ABILITY && sp->client_work == sp->state_client)
        {
            sp->mp.msg_id = BATTLE_MSG_ABILITY_LOWERED_ITS_OWN_STAT;
            sp->mp.msg_tag = TAG_NICK_TOKU_STAT;
            sp->mp.msg_para[0] = TagNickParaMake(sp, sp->client_work);
            sp->mp.msg_para[1] = sp->battlemon[sp->client_work].ability;
            sp->mp.msg_para[2] = STAT_ATTACK + stattochange;
        }
        else if (sp->addeffect_type == ADD_EFFECT_ABILITY)
        {
            sp->mp.msg_id = BATTLE_MSG_ATK_ABILITY_CUTS_MON_STAT;
            sp->mp.msg_tag = TAG_NICK_TOKU_NICK_STAT;
            sp->mp.msg_para[0] = TagNickParaMake(sp, sp->attack_client);
            sp->mp.msg_para[1] = sp->battlemon[sp->attack_client].ability;
            sp->mp.msg_para[2] = TagNickParaMake(sp, sp->state_client);
            sp->mp.msg_para[3] = STAT_ATTACK + stattochange;
        }
        // certain abilities fuck it up.  this fixes them
        else if (sp->addeffect_type == ADD_EFFECT_PRINT_WORK_ABILITY)
        {
            sp->mp.msg_id = BATTLE_MSG_ATK_ABILITY_CUTS_MON_STAT;
            sp->mp.msg_tag = TAG_NICK_TOKU_NICK_STAT;
            sp->mp.msg_para[0] = TagNickParaMake(sp, sp->client_work);
            sp->mp.msg_para[1] = sp->battlemon[sp->client_work].ability;
            sp->mp.msg_para[2] = TagNickParaMake(sp, sp->state_client);
            sp->mp.msg_para[3] = STAT_ATTACK+stattochange;
        }
        else
        {
            sp->mp.msg_id = ((statchange == -1) ? BATTLE_MSG_STAT_FELL : BATTLE_MSG_STAT_HARSHLY_FELL);
            sp->mp.msg_tag = TAG_NICK_STAT;
            sp->mp.msg_para[0] = TagNickParaMake(sp, sp->state_client);
            sp->mp.msg_para[1] = STAT_ATTACK + stattochange;
        }
        
        battlemon->states[STAT_ATTACK + stattochange] += statchange;
        if (battlemon->states[STAT_ATTACK + stattochange] < 0)
        {
            battlemon->states[STAT_ATTACK + stattochange] = 0;
        }
    }

    return	FALSE;
}
